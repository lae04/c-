/*
 * gate.cpp
 * ---------------------------------------------------------------
 * Snake Game의 Gate 기능을 구현하는 파일.
 *
 * [담당 역할 - D단계]
 * - Wall 위치 중 임의의 두 곳에 Gate 생성
 * - Gate 수명 관리
 * - Snake가 Gate에 진입하면 반대쪽 Gate로 이동
 * - Gate 진출 방향 계산
 *
 * [구현자]
 * 추가_민석 : GateManager 클래스 구현
 * ---------------------------------------------------------------
 */

#include "gate.h"
#include "snake.h"

#include <cstdlib>
#include <ctime>
#include <vector>

// ---------------------------------------------------------------
// 생성자
// ---------------------------------------------------------------

GateManager::GateManager()
    : gateA_{-1, -1},
      gateB_{-1, -1},
      active_(false),
      spawnTick_(0),
      gateCount_(0) {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

// ---------------------------------------------------------------
// 스테이지 시작 시 Gate 상태 초기화
// ---------------------------------------------------------------

void GateManager::init(Map& map) {
    active_ = false;
    spawnTick_ = g_tick;
    gateCount_ = 0;
    gateA_ = {-1, -1};
    gateB_ = {-1, -1};

    // 추가_민석: 스테이지 시작 직후 바로 Gate 한 쌍 생성
    spawnGate(map);
}

// ---------------------------------------------------------------
// Gate 갱신
// - Gate가 없으면 생성
// - Gate가 있고 수명이 지나면 제거 후 새 위치에 생성
// ---------------------------------------------------------------

void GateManager::update(Map& map) {
    if (!active_) {
        spawnGate(map);
        return;
    }

    if ((g_tick - spawnTick_) >= GATE_LIFE) {
        removeGate(map);
        spawnGate(map);
    }
}

// ---------------------------------------------------------------
// 현재 좌표가 Gate인지 확인
// ---------------------------------------------------------------

bool GateManager::isGateCell(const Pos& pos) const {
    if (!active_) return false;

    return (pos == gateA_) || (pos == gateB_);
}

// ---------------------------------------------------------------
// Snake가 Gate에 진입했을 때 반대편 Gate로 이동
// ---------------------------------------------------------------

bool GateManager::enterGate(Map& map, const Pos& entrance, Direction currentDir) {
    if (!isGateCell(entrance)) {
        return false;
    }

    const Pos exitGate = getExitGate(entrance);
    const Direction exitDir = getExitDirection(map, exitGate, currentDir);
    const Pos newHead = nextPos(exitGate, exitDir);

    if (!map.inBounds(newHead.y, newHead.x)) {
        g_gameState = GAME_OVER;
        return false;
    }

    const int exitCell = map.getCell(newHead.y, newHead.x);

    if (exitCell == WALL || exitCell == IMMUNE_WALL || exitCell == SNAKE_BODY) {
        g_gameState = GAME_OVER;
        return false;
    }

    // 추가_민석: Snake를 반대쪽 Gate 바깥 칸으로 이동시킴
    teleportSnake(newHead, exitDir);

    ++gateCount_;

    return true;
}

// ---------------------------------------------------------------
// Gate 사용 횟수 반환
// ---------------------------------------------------------------

int GateManager::getGateCount() const {
    return gateCount_;
}

// ---------------------------------------------------------------
// Gate 활성 상태 반환
// ---------------------------------------------------------------

bool GateManager::isActive() const {
    return active_;
}

// ---------------------------------------------------------------
// Gate A 위치 반환
// ---------------------------------------------------------------

Pos GateManager::getGateA() const {
    return gateA_;
}

// ---------------------------------------------------------------
// Gate B 위치 반환
// ---------------------------------------------------------------

Pos GateManager::getGateB() const {
    return gateB_;
}

// ---------------------------------------------------------------
// 기존 Gate 제거
// Gate는 Wall이 변한 것이므로 다시 WALL로 되돌린다.
// ---------------------------------------------------------------

void GateManager::removeGate(Map& map) {
    if (!active_) return;

    if (map.inBounds(gateA_.y, gateA_.x) && map.getCell(gateA_.y, gateA_.x) == GATE) {
        map.setCell(gateA_.y, gateA_.x, WALL);
    }

    if (map.inBounds(gateB_.y, gateB_.x) && map.getCell(gateB_.y, gateB_.x) == GATE) {
        map.setCell(gateB_.y, gateB_.x, WALL);
    }

    active_ = false;
    gateA_ = {-1, -1};
    gateB_ = {-1, -1};
}

// ---------------------------------------------------------------
// 새로운 Gate 한 쌍 생성
// ---------------------------------------------------------------

void GateManager::spawnGate(Map& map) {
    const Pos first = randomWallPos(map);
    Pos second = randomWallPos(map);

    int retry = 0;
    while (second == first && retry < 100) {
        second = randomWallPos(map);
        ++retry;
    }

    if (first.y == -1 || second.y == -1 || first == second) {
        active_ = false;
        return;
    }

    gateA_ = first;
    gateB_ = second;

    map.setCell(gateA_.y, gateA_.x, GATE);
    map.setCell(gateB_.y, gateB_.x, GATE);

    active_ = true;
    spawnTick_ = g_tick;
}

// ---------------------------------------------------------------
// 임의의 Wall 위치 선택
// Immune Wall에는 Gate를 만들 수 없으므로 WALL만 후보로 사용한다.
// ---------------------------------------------------------------

Pos GateManager::randomWallPos(const Map& map) const {
    std::vector<Pos> candidates;

    for (int y = 0; y < map.getHeight(); ++y) {
        for (int x = 0; x < map.getWidth(); ++x) {
            if (map.getCell(y, x) == WALL) {
                candidates.push_back({y, x});
            }
        }
    }

    if (candidates.empty()) {
        return {-1, -1};
    }

    const int index = std::rand() % candidates.size();
    return candidates[index];
}

// ---------------------------------------------------------------
// entrance가 gateA_면 gateB_, gateB_면 gateA_ 반환
// ---------------------------------------------------------------

Pos GateManager::getExitGate(const Pos& entrance) const {
    if (entrance == gateA_) {
        return gateB_;
    }

    return gateA_;
}

// ---------------------------------------------------------------
// Gate 진출 방향 계산
//
// 1. 가장자리 벽에 있는 Gate:
//    항상 Map 안쪽 방향으로 진출
//
// 2. 내부 벽에 있는 Gate:
//    현재 진행 방향 → 시계방향 → 반시계방향 → 반대방향 순서로 검사
// ---------------------------------------------------------------

Direction GateManager::getExitDirection(const Map& map,
                                        const Pos& exitGate,
                                        Direction currentDir) const {
    const int top = 1;
    const int bottom = map.getHeight() - 2;
    const int left = 1;
    const int right = map.getWidth() - 2;

    if (exitGate.y == top) {
        return DOWN;
    }

    if (exitGate.y == bottom) {
        return UP;
    }

    if (exitGate.x == left) {
        return RIGHT;
    }

    if (exitGate.x == right) {
        return LEFT;
    }

    const Direction candidates[4] = {
        currentDir,
        clockwise(currentDir),
        counterClockwise(currentDir),
        opposite(currentDir)
    };

    for (Direction dir : candidates) {
        if (canExitTo(map, exitGate, dir)) {
            return dir;
        }
    }

    return opposite(currentDir);
}

// ---------------------------------------------------------------
// 방향에 따른 다음 좌표 계산
// ---------------------------------------------------------------

Pos GateManager::nextPos(const Pos& pos, Direction dir) const {
    Pos next = pos;

    switch (dir) {
    case UP:
        --next.y;
        break;
    case DOWN:
        ++next.y;
        break;
    case LEFT:
        --next.x;
        break;
    case RIGHT:
        ++next.x;
        break;
    case NONE:
        break;
    }

    return next;
}

// ---------------------------------------------------------------
// 해당 방향으로 Snake가 나갈 수 있는지 확인
// ---------------------------------------------------------------

bool GateManager::canExitTo(const Map& map, const Pos& gate, Direction dir) const {
    const Pos next = nextPos(gate, dir);

    if (!map.inBounds(next.y, next.x)) {
        return false;
    }

    const int cell = map.getCell(next.y, next.x);

    return cell != WALL &&
           cell != IMMUNE_WALL &&
           cell != SNAKE_BODY &&
           cell != GATE;
}

// ---------------------------------------------------------------
// 시계방향 회전
// UP → RIGHT → DOWN → LEFT
// ---------------------------------------------------------------

Direction GateManager::clockwise(Direction dir) const {
    switch (dir) {
    case UP:    return RIGHT;
    case RIGHT: return DOWN;
    case DOWN:  return LEFT;
    case LEFT:  return UP;
    case NONE:  return NONE;
    }

    return NONE;
}

// ---------------------------------------------------------------
// 반시계방향 회전
// UP → LEFT → DOWN → RIGHT
// ---------------------------------------------------------------

Direction GateManager::counterClockwise(Direction dir) const {
    switch (dir) {
    case UP:    return LEFT;
    case LEFT:  return DOWN;
    case DOWN:  return RIGHT;
    case RIGHT: return UP;
    case NONE:  return NONE;
    }

    return NONE;
}

// ---------------------------------------------------------------
// 반대 방향
// ---------------------------------------------------------------

Direction GateManager::opposite(Direction dir) const {
    switch (dir) {
    case UP:    return DOWN;
    case DOWN:  return UP;
    case LEFT:  return RIGHT;
    case RIGHT: return LEFT;
    case NONE:  return NONE;
    }

    return NONE;
}