#include "renderer.h"

// 점수판 창 위치: 맵 오른쪽 (셀 2칸 너비 * MAP_SIZE + 여백)
static constexpr int SCORE_WIN_X = MAP_SIZE * 2 + 2;
static constexpr int SCORE_WIN_W = 24;
static constexpr int SCORE_WIN_H = 20;

static WINDOW* g_scoreWin = nullptr;

// main.cpp이 이미 1~6번 init_pair를 정의했으므로
// 점수판 전용 색상은 7번부터 사용
static constexpr int CP_MISSION_OK = 7;  // 미션 달성 (초록)
static constexpr int CP_MISSION_NO = 8;  // 미션 미달성 (흰색)

// ────────────────────────────────────────────
//  초기화
// ────────────────────────────────────────────
void initRenderer() {
    if (has_colors()) {
        init_pair(CP_MISSION_OK, COLOR_GREEN, COLOR_BLACK);
        init_pair(CP_MISSION_NO, COLOR_WHITE, COLOR_BLACK);
    }
    g_scoreWin = newwin(SCORE_WIN_H, SCORE_WIN_W, 0, SCORE_WIN_X);
    scrollok(g_scoreWin, FALSE); //수정_하윤 추가
}

void quitRenderer() {
    if (g_scoreWin) {
        delwin(g_scoreWin);
        g_scoreWin = nullptr;
    }
}

// ────────────────────────────────────────────
//  점수판 출력 (main.cpp의 renderScoreboard() stub 교체)
// ────────────────────────────────────────────
void renderScoreboard(const ScoreData& score) {
    if (!g_scoreWin) return;

    touchwin(g_scoreWin); //수정_하윤 추가
    
    werase(g_scoreWin);
    box(g_scoreWin, 0, 0);

    mvwprintw(g_scoreWin, 1, 2, "=== Score Board ===");

    // 현재 상태
    mvwprintw(g_scoreWin, 3, 2, "B : %d", score.length);
    mvwprintw(g_scoreWin, 4, 2, "+  : %d", score.growthCount);
    mvwprintw(g_scoreWin, 5, 2, "-  : %d", score.poisonCount);
    mvwprintw(g_scoreWin, 6, 2, "G : %d", score.gateCount);

    // 미션
    mvwprintw(g_scoreWin, 8, 2, "=== Mission ===");

    // B (목표 길이)
    wattron(g_scoreWin, COLOR_PAIR(score.lengthOk ? CP_MISSION_OK : CP_MISSION_NO));
    mvwprintw(g_scoreWin, 10, 2, "B : %d  %s", score.targetLength, score.lengthOk ? "v" : " ");
    wattroff(g_scoreWin, COLOR_PAIR(score.lengthOk ? CP_MISSION_OK : CP_MISSION_NO));

    // + (Growth)
    wattron(g_scoreWin, COLOR_PAIR(score.growthOk ? CP_MISSION_OK : CP_MISSION_NO));
    mvwprintw(g_scoreWin, 11, 2, "+  : %d  %s", score.targetGrowth, score.growthOk ? "v" : " ");
    wattroff(g_scoreWin, COLOR_PAIR(score.growthOk ? CP_MISSION_OK : CP_MISSION_NO));

    // - (Poison)
    wattron(g_scoreWin, COLOR_PAIR(score.poisonOk ? CP_MISSION_OK : CP_MISSION_NO));
    mvwprintw(g_scoreWin, 12, 2, "-  : %d  %s", score.targetPoison, score.poisonOk ? "v" : " ");
    wattroff(g_scoreWin, COLOR_PAIR(score.poisonOk ? CP_MISSION_OK : CP_MISSION_NO));

    // G (Gate)
    wattron(g_scoreWin, COLOR_PAIR(score.gateOk ? CP_MISSION_OK : CP_MISSION_NO));
    mvwprintw(g_scoreWin, 13, 2, "G : %d  %s", score.targetGate, score.gateOk ? "v" : " ");
    wattroff(g_scoreWin, COLOR_PAIR(score.gateOk ? CP_MISSION_OK : CP_MISSION_NO));

    mvwprintw(g_scoreWin, 15, 2, "Stage : %d", g_currentStage);

    wrefresh(g_scoreWin);
}
