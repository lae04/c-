#pragma once
#include "globals.h"
#include "map.h"

// ────────────────────────────────────────────
//  점수판 데이터 (scoreboard.cpp이 채워서 넘겨줌)
// ────────────────────────────────────────────
struct ScoreData {
    int length;         // 현재 뱀 길이
    int growthCount;    // 먹은 Growth Item 수
    int poisonCount;    // 먹은 Poison Item 수
    int gateCount;      // 통과한 Gate 횟수

    // 미션 목표값
    int targetLength;
    int targetGrowth;
    int targetPoison;
    int targetGate;

    // 미션 달성 여부
    bool lengthOk;
    bool growthOk;
    bool poisonOk;
    bool gateOk;
};

// ────────────────────────────────────────────
//  main.cpp에서 호출하는 함수들
// ────────────────────────────────────────────

// 점수판 창 초기화 (main의 initNcurses() 다음에 호출)
void initRenderer();

// 점수판 출력 (main.cpp의 renderScoreboard() stub 교체)
void renderScoreboard(const ScoreData& score);

// 점수판 창 종료 (main의 endwin() 전에 호출)
void quitRenderer();
