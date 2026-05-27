/*
 * main.cpp
 * ---------------------------------------------------------------
 * Snake 게임의 진입점(entry point) 및 게임 루프 관리 파일.
 *
 * [담당 역할]
 * - ncurses 초기화 및 색상 쌍 등록
 * - 키 입력 처리 (반전 효과 중에는 방향키를 반대로 변환)
 * - 스테이지 단위 게임 루프 실행 (runStage)
 * - 게임 오버 / 스테이지 클리어 화면 표시
 *
 * [수정 이력]
 * 추가_하윤 : snake.h, renderer.h include / initSnake, moveSnake 연동
 * 추가_진원 : item.h include / ItemManager 연동 / 반전 입력 처리
 * 추가_민석 : gate.h, scoreboard.h include / GateManager, ScoreboardManager 연동
 * ---------------------------------------------------------------
 */

#include "globals.h"
#include "map.h"
#include "snake.h"        // 추가_하윤
#include "renderer.h"     // 추가_하윤
#include "item.h"         // 추가_진원: ItemManager 사용
#include "gate.h"         // 추가_민석: GateManager 사용
#include "scoreboard.h"   // 추가_민석: ScoreboardManager 사용

#include <ncurses.h>
#include <unistd.h>       // usleep

// ────────────────────────────────────────────
// ncurses 초기화
// ────────────────────────────────────────────

void initNcurses() {
    initscr();              // curses 모드 시작, stdscr 생성
    noecho();               // 입력 문자 화면에 표시 안 함
    cbreak();               // 버퍼링 없이 즉시 입력 처리
    curs_set(0);            // 커서 숨김
    keypad(stdscr, TRUE);   // 방향키 등 특수키 활성화
    nodelay(stdscr, TRUE);  // getch() 논블로킹

    // 컬러 초기화
    if (has_colors()) {
        start_color();

        init_pair(1, COLOR_WHITE, COLOR_WHITE);   // Immune Wall
        init_pair(2, COLOR_YELLOW, COLOR_BLACK);  // Wall
        init_pair(3, COLOR_GREEN, COLOR_BLACK);   // Snake
        init_pair(4, COLOR_CYAN, COLOR_BLACK);    // Growth Item
        init_pair(5, COLOR_RED, COLOR_BLACK);     // Poison Item
        init_pair(6, COLOR_MAGENTA, COLOR_BLACK); // Gate

        // 추가_진원: Reverse Item 색상
        init_pair(9, COLOR_BLACK, COLOR_YELLOW);
    }
}

// ────────────────────────────────────────────
// 키 입력 처리
// 추가_진원: Reverse Item 효과 중이면 방향키 반전
// ────────────────────────────────────────────

Direction handleInput(const ItemManager& itemMgr) {
    const int ch = getch();

    if (itemMgr.isReversed()) {
        switch (ch) {
        case KEY_UP:    return DOWN;
        case KEY_DOWN:  return UP;
        case KEY_LEFT:  return RIGHT;
        case KEY_RIGHT: return LEFT;
        default:        return NONE;
        }
    }

    switch (ch) {
    case KEY_UP:    return UP;
    case KEY_DOWN:  return DOWN;
    case KEY_LEFT:  return LEFT;
    case KEY_RIGHT: return RIGHT;
    default:        return NONE;
    }
}

// ────────────────────────────────────────────
// 게임 오버 화면
// ────────────────────────────────────────────

void showGameOver() {
    clear();
    mvprintw(MAP_SIZE / 2, MAP_SIZE - 4, "GAME OVER");
    mvprintw(MAP_SIZE / 2 + 1, MAP_SIZE - 7, "Press any key to exit");

    nodelay(stdscr, FALSE);
    getch();
}

// ────────────────────────────────────────────
// 스테이지 클리어 화면
// ────────────────────────────────────────────

void showStageClear(const int stage) {
    clear();
    mvprintw(MAP_SIZE / 2, MAP_SIZE - 6, "STAGE %d CLEAR!", stage);
    mvprintw(MAP_SIZE / 2 + 1, MAP_SIZE - 8, "Press any key to continue");

    nodelay(stdscr, FALSE);
    getch();
    nodelay(stdscr, TRUE);
}

// ────────────────────────────────────────────
// 한 스테이지 실행
// ────────────────────────────────────────────

void runStage(Map& map, const int stage) {
    map.load(stage);

    initSnake(map); // map 추가_하윤

    g_gameState = RUNNING;
    g_tick = 0;

    ItemManager itemMgr;    // 추가_진원: 스테이지마다 새 인스턴스 생성
    itemMgr.init(map);      // 추가_진원: 기존 spawnItem() 대체

    // 추가_민석: D단계 Gate와 Scoreboard 관리자 생성
    GateManager gateMgr;
    gateMgr.init(map);

    ScoreboardManager scoreMgr;
    scoreMgr.init(stage);

    while (g_gameState == RUNNING) {
        // 1) 입력 처리
        const Direction dir = handleInput(itemMgr);

        // 2) 틱 처리
        usleep(TICK_MS * 1000);
        g_tick++;

        // 3) Snake 이동
        setSnakeDir(dir);
        const int movedCell = moveSnake(map);

        // 4) 아이템 수명 갱신
        itemMgr.update(map);

        // 5) 아이템 상호작용 처리
        {
            const Pos head = getSnakeHead();
            const int result = itemMgr.eat(map, head, movedCell);

            if (result == GROWTH_ITEM) {
                growSnake();
            } else if (result == POISON_ITEM) {
                if (!shrinkSnake()) {
                    g_gameState = GAME_OVER;
                    break;
                }
            }

            // REVERSE_ITEM은 itemMgr.eat() 내부에서 처리
        }

        // 6) 게이트 상호작용 및 갱신
        if (movedCell == GATE) {
            const Pos head = getSnakeHead();
            gateMgr.enterGate(map, head, g_snakeDir);
        }

        gateMgr.update(map);

        // 7) 충돌 체크
        if (checkDead(map)) {
            g_gameState = GAME_OVER;
            break;
        }

        // 8) 점수판 갱신 및 미션 달성 체크
        scoreMgr.update(
            getSnakeLength(),
            itemMgr.getGrowthCount(),
            itemMgr.getPoisonCount(),
            gateMgr.getGateCount(),
            itemMgr.getReverseCount(),
            itemMgr.isReversed()
        );

        if (scoreMgr.isMissionClear()) {
            g_gameState = STAGE_CLEAR;
            break;
        }

        // 9) 화면 출력
        werase(stdscr); //수정_하윤 clear() -> werase(stdscr);
        map.render();
        refresh(); //수정_하윤 순서조정
        // 수정_민석: ScoreboardManager가 만든 ScoreData를 renderer.cpp에 전달
        renderScoreboard(scoreMgr.makeScoreData());

        
    }
}

// ────────────────────────────────────────────
// main
// ────────────────────────────────────────────

int main() {
    initNcurses();

    initRenderer(); // 추가_하윤

    Map map;

    for (g_currentStage = 1; g_currentStage <= 4; ++g_currentStage) {
        runStage(map, g_currentStage);

        if (g_gameState == GAME_OVER) {
            showGameOver();
            break;
        } else if (g_gameState == STAGE_CLEAR) {
            showStageClear(g_currentStage);

            // 마지막 스테이지까지 클리어한 경우
            if (g_currentStage == 4) {
                clear();
                mvprintw(MAP_SIZE / 2, MAP_SIZE - 5, "ALL CLEAR!");
                nodelay(stdscr, FALSE);
                getch();
                break;
            }
        }
    }

    quitRenderer(); // 추가_하윤

    endwin();
    return 0;
}
