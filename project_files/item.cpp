/*
 * item.cpp
 * ---------------------------------------------------------------
 * item.h에 선언된 ItemManager 클래스의 멤버 함수 구현 파일.
 *
 * [핵심 설계 결정]
 *  - 전역변수 없음: 모든 상태(아이템 목록, 카운터, 반전 상태)를
 *    ItemManager 인스턴스의 멤버변수로 관리
 *  - 아이템 수명: ITEM_LIFE 틱(globals.h) 경과 시 자동 제거
 *  - Reverse 출현: REVERSE_SPAWN_INTERVAL 틱마다 1회 시도,
 *    이미 맵에 있거나 전체 한도(MAX_TOTAL) 초과 시 스킵
 *  - eat() 반환값으로 grow/shrink 결정권을 호출자(main.cpp)에 위임:
 *    Reverse 효과만 eat() 내부에서 즉시 처리
 * ---------------------------------------------------------------
 */

#include "item.h"
#include <cstdlib>   // rand(), srand()
#include <ctime>     // time()
#include <algorithm> // std::remove_if

// ────────────────────────────────────────────
//  생성자: 모든 멤버 초기화
// ────────────────────────────────────────────
ItemManager::ItemManager()
    : growthEaten_(0)
    , poisonEaten_(0)
    , reverseEaten_(0)
    , isReversed_(false)
    , reverseEndTick_(0)
{
    srand(static_cast<unsigned>(time(nullptr)));
}

// ────────────────────────────────────────────
//  init: 스테이지 시작 시 1회 호출
//  기존 아이템을 모두 맵에서 지운 뒤
//  Growth 1개, Poison 1개를 초기 배치
// ────────────────────────────────────────────
void ItemManager::init(Map& map) {
    // 맵에 남은 아이템 셀 제거
    for (const auto& item : items_) {
        if (item.active) removeFromMap(map, item);
    }
    items_.clear();

    // 카운터·반전 상태 초기화
    growthEaten_  = 0;
    poisonEaten_  = 0;
    reverseEaten_ = 0;
    isReversed_   = false;
    reverseEndTick_ = 0;

    // 초기 아이템 배치 (Reverse는 주기적으로만 출현)
    spawnOne(map, ItemType::GROWTH);
    spawnOne(map, ItemType::POISON);
}

// ────────────────────────────────────────────
//  update: 매 틱 호출
// ────────────────────────────────────────────
void ItemManager::update(Map& map) {

    // 1) 반전 효과 만료 체크
    //    g_tick이 종료 틱에 도달하면 반전 해제
    if (isReversed_ && g_tick >= reverseEndTick_) {
        isReversed_ = false;
    }

    // 2) 수명(ITEM_LIFE 틱) 만료 아이템 맵에서 제거 후 비활성화
    for (auto& item : items_) {
        if (!item.active) continue;
        if ((g_tick - item.spawnTick) >= ITEM_LIFE) {
            removeFromMap(map, item);
            item.active = false;
        }
    }

    // 비활성 슬롯 정리 (벡터 크기가 무한히 커지는 것을 방지)
    items_.erase(
        std::remove_if(items_.begin(), items_.end(),
                       [](const ItemData& i){ return !i.active; }),
        items_.end()
    );

    // 3) Growth / Poison 부족분 보충
    //    각 타입이 0개이면 1개 새로 배치 (전체 합계 MAX_TOTAL 이하 유지)
    if (countActive(ItemType::GROWTH) < MAX_GROWTH &&
        countAllActive() < MAX_TOTAL)
    {
        spawnOne(map, ItemType::GROWTH);
    }

    if (countActive(ItemType::POISON) < MAX_POISON &&
        countAllActive() < MAX_TOTAL)
    {
        spawnOne(map, ItemType::POISON);
    }

    // 4) Reverse 아이템 주기적 출현
    //    REVERSE_SPAWN_INTERVAL 틱마다 1회 시도
    if (g_tick > 0 && (g_tick % REVERSE_SPAWN_INTERVAL) == 0) {
        if (countActive(ItemType::REVERSE) < MAX_REVERSE &&
            countAllActive() < MAX_TOTAL)
        {
            spawnOne(map, ItemType::REVERSE);
        }
    }
}

// ────────────────────────────────────────────
//  eat: 뱀 머리가 아이템 셀을 밟았을 때 호출
//  해당 위치의 활성 아이템을 비활성화하고
//  효과 종류를 반환값으로 알림
// ────────────────────────────────────────────
int ItemManager::eat(Map& map, const Pos& pos, const int cellVal) {
    // 아이템 셀이 아니면 즉시 반환
    if (cellVal != GROWTH_ITEM &&
        cellVal != POISON_ITEM &&
        cellVal != REVERSE_ITEM)
    {
        return EMPTY;
    }

    // 해당 위치의 활성 아이템 탐색
    for (auto& item : items_) {
        if (!item.active || !(item.pos == pos)) continue;

        // 아이템 비활성화
        // (맵 셀은 applySnakeToMap()이 SNAKE_HEAD로 덮으므로 별도 제거 불필요)
        item.active = false;

        if (cellVal == GROWTH_ITEM) {
            ++growthEaten_;
            return GROWTH_ITEM;   // 호출자가 growSnake() 처리

        } else if (cellVal == POISON_ITEM) {
            ++poisonEaten_;
            return POISON_ITEM;   // 호출자가 shrinkSnake() 처리

        } else {
            // Reverse Item: 반전 효과 내부에서 즉시 적용
            ++reverseEaten_;
            isReversed_     = true;
            reverseEndTick_ = g_tick + REVERSE_DURATION;
            return REVERSE_ITEM;  // 호출자는 grow/shrink 처리 불필요
        }
    }

    // 목록에서 찾지 못한 경우 (이미 만료된 아이템 위치)
    return EMPTY;
}

// ────────────────────────────────────────────
//  isReversed: 현재 반전 효과 중인지 반환
// ────────────────────────────────────────────
bool ItemManager::isReversed() const {
    return isReversed_;
}

// ────────────────────────────────────────────
//  Getter: 누적 획득 수 반환
// ────────────────────────────────────────────
int ItemManager::getGrowthCount()  const { return growthEaten_;  }
int ItemManager::getPoisonCount()  const { return poisonEaten_;  }
int ItemManager::getReverseCount() const { return reverseEaten_; }

// ════════════════════════════════════════════
//  private 내부 헬퍼 구현
// ════════════════════════════════════════════

// ── 특정 타입의 활성 아이템 수 반환
int ItemManager::countActive(ItemType type) const {
    int cnt = 0;
    for (const auto& item : items_) {
        if (item.active && item.type == type) ++cnt;
    }
    return cnt;
}

// ── 전체 활성 아이템 수 반환
int ItemManager::countAllActive() const {
    int cnt = 0;
    for (const auto& item : items_) {
        if (item.active) ++cnt;
    }
    return cnt;
}

// ── 맵에서 아이템 셀 제거 (뱀·벽 등 다른 셀은 건드리지 않음)
void ItemManager::removeFromMap(Map& map, const ItemData& item) const {
    const int cell = map.getCell(item.pos.y, item.pos.x);
    if (cell == GROWTH_ITEM ||
        cell == POISON_ITEM ||
        cell == REVERSE_ITEM)
    {
        map.setCell(item.pos.y, item.pos.x, EMPTY);
    }
}

// ── EMPTY 셀 중 무작위 위치 반환
//    후보가 없으면 {-1, -1} 반환
Pos ItemManager::randomEmptyPos(const Map& map) const {
    std::vector<Pos> candidates;
    for (int y = 0; y < map.getHeight(); ++y) {
        for (int x = 0; x < map.getWidth(); ++x) {
            if (map.getCell(y, x) == EMPTY)
                candidates.push_back({y, x});
        }
    }
    if (candidates.empty()) return {-1, -1};
    return candidates[rand() % static_cast<int>(candidates.size())];
}

// ── 아이템 1개를 빈 셀에 배치
//    빈 셀이 없으면 아무것도 하지 않음
void ItemManager::spawnOne(Map& map, ItemType type) {
    const Pos pos = randomEmptyPos(map);
    if (pos.y == -1) return;

    // 타입별 맵 셀값 결정
    int cellVal;
    switch (type) {
        case ItemType::GROWTH:  cellVal = GROWTH_ITEM;  break;
        case ItemType::POISON:  cellVal = POISON_ITEM;  break;
        case ItemType::REVERSE: cellVal = REVERSE_ITEM; break;
        default:                cellVal = GROWTH_ITEM;  break;
    }

    map.setCell(pos.y, pos.x, cellVal);

    // 아이템 목록에 추가
    ItemData newItem;
    newItem.pos       = pos;
    newItem.type      = type;
    newItem.spawnTick = g_tick;
    newItem.active    = true;
    items_.push_back(newItem);
}
