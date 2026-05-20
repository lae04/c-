#pragma once
#include "globals.h"
#include "map.h"
#include <deque>

// ────────────────────────────────────────────
//  전역 Snake 상태 (snake.cpp에서 정의)
// ────────────────────────────────────────────
extern std::deque<Pos> g_snakeBody;  // front = 머리, back = 꼬리
extern Direction       g_snakeDir;   // 현재 방향
extern Direction       g_snakeNextDir; // 입력 버퍼

// ────────────────────────────────────────────
//  main.cpp에서 호출하는 함수들
// ────────────────────────────────────────────

// 초기화 (runStage 시작 시 호출)
void initSnake(Map& map);

// 방향 입력 적용 (handleInput() 결과를 넘겨줌)
// main.cpp의 handleInput()이 NONE을 반환하면 무시됨
void setSnakeDir(Direction dir);

// 한 틱 이동 + 맵 반영
// 반환값: 머리가 이동한 셀값
//   EMPTY        → 정상 이동
//   WALL / IMMUNE_WALL / SNAKE_BODY → 사망
//   GROWTH_ITEM  → 성장 아이템 먹음
//   POISON_ITEM  → 독 아이템 먹음
//   GATE         → 게이트 진입
int  moveSnake(Map& map);

// 충돌로 죽었는지 체크 (main.cpp의 checkDead() 안에서 호출)
bool checkDead(Map& map);

// 성장 (Growth Item 먹었을 때)
void growSnake();

// 축소 (Poison Item 먹었을 때)
// 반환값: false → 길이 부족으로 사망
bool shrinkSnake();

// 게이트 통과 후 위치·방향 강제 설정 (gate.cpp에서 호출)
void teleportSnake(Pos newHead, Direction newDir);

// 현재 머리 위치 반환
Pos  getSnakeHead();

// 현재 길이 반환
int  getSnakeLength();
