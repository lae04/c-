/*
 * item.h
 * ---------------------------------------------------------------
 * Growth Item, Poison Item, Reverse Item의 생성·소멸·상호작용을
 * 관리하는 ItemManager 클래스의 선언 파일.
 *
 * [담당 역할]
 *  - 맵 위에 아이템을 랜덤 배치하고 수명(ITEM_LIFE 틱)이 지나면 제거
 *  - 뱀 머리가 아이템 셀을 밟았을 때 grow / shrink / reverse 효과 적용
 *  - Reverse Item: 일정 시간(REVERSE_DURATION 틱) 동안 방향키 입력 반전
 *
 * [아이템 종류 및 맵 셀값]
 *  Growth Item  : GROWTH_ITEM  = 5  → 뱀 길이 +1
 *  Poison Item  : POISON_ITEM  = 6  → 뱀 길이 -1 (최소 3 미만이면 사망)
 *  Reverse Item : REVERSE_ITEM = 8  → REVERSE_DURATION 틱 동안 방향 반전
 *
 * [사용 방법 (main.cpp)]
 *  ItemManager itemMgr;
 *  itemMgr.init(map);           // 스테이지 시작 시 1회
 *  itemMgr.update(map);         // 매 틱 호출
 *  int result = itemMgr.eat(map, head, movedCell); // 아이템 셀 밟았을 때
 * ---------------------------------------------------------------
 */

#pragma once
#include "globals.h"
#include "map.h"
#include <vector>

// ────────────────────────────────────────────
//  Reverse Item 맵 셀값 (globals.h의 0~7과 겹치지 않음)
// ────────────────────────────────────────────
constexpr int REVERSE_ITEM = 8;

// ────────────────────────────────────────────
//  아이템 종류 열거형
// ────────────────────────────────────────────
enum class ItemType {
    GROWTH,   // 길이 증가
    POISON,   // 길이 감소
    REVERSE   // 방향키 반전
};

// ────────────────────────────────────────────
//  아이템 1개를 나타내는 구조체
//  (ItemManager 내부에서만 사용)
// ────────────────────────────────────────────
struct ItemData {
    Pos      pos;        // 맵 상의 위치
    ItemType type;       // 아이템 종류
    int      spawnTick;  // 생성된 틱 (수명 계산 기준)
    bool     active;     // 맵에 존재하는지 여부
};

// ────────────────────────────────────────────
//  ItemManager 클래스
//  아이템 전체 생명주기(생성~소멸~상호작용)를 담당
// ────────────────────────────────────────────
class ItemManager {
public:
    // ── 생성자: 모든 카운터·상태를 0/false로 초기화
    ItemManager();

    // ── 스테이지 시작 시 1회 호출
    //    맵 위 기존 아이템 제거 후 Growth·Poison 각 1개 초기 배치
    void init(Map& map);

    // ── 매 틱 호출
    //    1) 반전 효과 만료 체크
    //    2) 수명 만료 아이템 제거
    //    3) Growth·Poison 부족분 보충
    //    4) Reverse 아이템 주기적 출현
    void update(Map& map);

    // ── 뱀 머리가 아이템 셀을 밟았을 때 호출
    //    pos      : 뱀 머리가 이동한 맵 좌표
    //    cellVal  : moveSnake()가 반환한 셀값
    //    반환값   : GROWTH_ITEM  → 호출자가 growSnake() 처리
    //               POISON_ITEM  → 호출자가 shrinkSnake() 처리
    //               REVERSE_ITEM → 반전 효과 시작 (내부 처리 완료)
    //               EMPTY        → 해당 위치에 활성 아이템 없음
    int eat(Map& map, const Pos& pos, const int cellVal);

    // ── 현재 반전 효과 중인지 반환 (main.cpp의 handleInput에서 사용)
    bool isReversed() const;

    // ── 누적 획득 수 반환 (ScoreData 구성용)
    int getGrowthCount()  const;
    int getPoisonCount()  const;
    int getReverseCount() const;

private:
    // ── 아이템 목록
    std::vector<ItemData> items_;

    // ── 누적 획득 카운터
    int growthEaten_;
    int poisonEaten_;
    int reverseEaten_;

    // ── 반전 효과 상태
    bool isReversed_;      // 현재 반전 중인지
    int  reverseEndTick_;  // 반전 효과 종료 틱

    // ── 동시 출현 최대 개수
    static constexpr int MAX_GROWTH  = 1;
    static constexpr int MAX_POISON  = 1;
    static constexpr int MAX_REVERSE = 1;
    static constexpr int MAX_TOTAL   = 3;

    // ── Reverse 아이템 관련 상수
    static constexpr int REVERSE_DURATION       = 10; // 반전 지속 틱
    static constexpr int REVERSE_SPAWN_INTERVAL = 20; // 출현 주기 (틱)

    // ── 내부 헬퍼 함수 (외부에 노출할 필요 없음)
    int  countActive(ItemType type) const;  // 특정 타입 활성 개수
    int  countAllActive()           const;  // 전체 활성 개수
    void removeFromMap(Map& map, const ItemData& item) const; // 맵에서 아이템 제거
    void spawnOne(Map& map, ItemType type);                   // 아이템 1개 배치
    Pos  randomEmptyPos(const Map& map) const;                // 빈 셀 무작위 선택
};
