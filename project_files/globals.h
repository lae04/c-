#pragma once

#include <ncurses.h>
#include <vector>
#include <utility>

// ────────────────────────────────────────────
//  Map 셀 값 상수
// ────────────────────────────────────────────
constexpr int EMPTY        = 0;
constexpr int WALL         = 1;
constexpr int IMMUNE_WALL  = 2;
constexpr int SNAKE_HEAD   = 3;
constexpr int SNAKE_BODY   = 4;
constexpr int GROWTH_ITEM  = 5;
constexpr int POISON_ITEM  = 6;
constexpr int GATE         = 7;

// ────────────────────────────────────────────
//  Map 크기
// ────────────────────────────────────────────
constexpr int MAP_SIZE     = 21;   // 최소 21x21

// ────────────────────────────────────────────
//  게임 타이밍
// ────────────────────────────────────────────
constexpr int TICK_MS      = 400;  // 1틱 = 200ms (조정 가능)
constexpr int ITEM_LIFE    = 15;   // 아이템 생존 틱 수
constexpr int GATE_LIFE    = 30;   // 게이트 생존 틱 수

// ────────────────────────────────────────────
//  방향
// ────────────────────────────────────────────
enum Direction { UP, DOWN, LEFT, RIGHT, NONE };

// ────────────────────────────────────────────
//  게임 상태
// ────────────────────────────────────────────
enum GameState {
    RUNNING,
    STAGE_CLEAR,
    GAME_OVER
};

// ────────────────────────────────────────────
//  좌표 구조체
// ────────────────────────────────────────────
struct Pos {
    int y, x;
    bool operator==(const Pos& o) const { return y == o.y && x == o.x; }
};

// ────────────────────────────────────────────
//  미션 구조체
// ────────────────────────────────────────────
struct Mission {
    int targetLength;   // B: 목표 몸 길이
    int targetGrowth;   // +: 획득해야 할 Growth Item 수
    int targetPoison;   // -: 획득해야 할 Poison Item 수
    int targetGate;     // G: 사용해야 할 Gate 횟수
};

// ────────────────────────────────────────────
//  전역 게임 상태 (globals.cpp에서 정의)
// ────────────────────────────────────────────
extern GameState  g_gameState;
extern int        g_currentStage;   // 현재 스테이지 (1~4)
extern int        g_tick;           // 현재 틱 카운터
