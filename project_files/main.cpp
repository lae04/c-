#include "globals.h"
#include "map.h"
// 아래는 다른 팀원이 구현 후 include
// #include "snake.h"
// #include "item.h"
// #include "gate.h"
// #include "scoreboard.h"

#include <ncurses.h>
#include <unistd.h>   // usleep

// ────────────────────────────────────────────
//  전방 선언 (각 팀원 구현 전 임시 stub)
// ────────────────────────────────────────────
// TODO: 아래 stub들은 각 파일 완성 후 삭제하고 헤더로 교체
void initSnake()   { /* B 구현 */ }
void moveSnake()   { /* B 구현 */ }
bool checkDead()   { return false; }

void spawnItem()   { /* C 구현 */ }
void updateItems() { /* C 구현 */ }

void spawnGate()   { /* D 구현 */ }

void renderScoreboard() { /* D 구현 */ }
bool checkMission()     { return false; }

// ────────────────────────────────────────────
//  ncurses 초기화
// ────────────────────────────────────────────
void initNcurses() {
    initscr();               // curses 모드 시작, stdscr 생성
    noecho();                // 입력 문자 화면에 표시 안 함
    cbreak();                // 버퍼링 없이 즉시 입력 처리
    curs_set(0);             // 커서 숨김
    keypad(stdscr, TRUE);    // 방향키 등 특수키 활성화
    nodelay(stdscr, TRUE);   // getch() 논블로킹 (게임 루프용)

    // 컬러 초기화 (강의자료: start_color → init_pair → attron 순서)
    if (has_colors()) {
        start_color();
        // init_pair(번호, 폰트색, 배경색)
        init_pair(1, COLOR_WHITE,   COLOR_WHITE);    // Immune Wall  : 흰색 블록
        init_pair(2, COLOR_YELLOW,  COLOR_BLACK);    // Wall         : 노란색
        init_pair(3, COLOR_GREEN,   COLOR_BLACK);    // Snake        : 초록색
        init_pair(4, COLOR_CYAN,    COLOR_BLACK);    // Growth Item  : 하늘색
        init_pair(5, COLOR_RED,     COLOR_BLACK);    // Poison Item  : 빨간색
        init_pair(6, COLOR_MAGENTA, COLOR_BLACK);    // Gate         : 보라색
    }
}

// ────────────────────────────────────────────
//  키 입력 처리
// ────────────────────────────────────────────
Direction handleInput() {
    int ch = getch();
    switch (ch) {
        case KEY_UP:    return UP;
        case KEY_DOWN:  return DOWN;
        case KEY_LEFT:  return LEFT;
        case KEY_RIGHT: return RIGHT;
        default:        return NONE;
    }
}

// ────────────────────────────────────────────
//  게임 오버 화면
// ────────────────────────────────────────────
void showGameOver() {
    clear();
    mvprintw(MAP_SIZE/2,   MAP_SIZE - 4, "GAME OVER");
    mvprintw(MAP_SIZE/2+1, MAP_SIZE - 7, "Press any key to exit");
    nodelay(stdscr, FALSE);
    getch();
}

// ────────────────────────────────────────────
//  스테이지 클리어 화면
// ────────────────────────────────────────────
void showStageClear(int stage) {
    clear();
    mvprintw(MAP_SIZE/2,   MAP_SIZE - 6, "STAGE %d CLEAR!", stage);
    mvprintw(MAP_SIZE/2+1, MAP_SIZE - 8, "Press any key to continue");
    nodelay(stdscr, FALSE);
    getch();
    nodelay(stdscr, TRUE);
}

// ────────────────────────────────────────────
//  한 스테이지 실행
// ────────────────────────────────────────────
void runStage(Map& map, int stage) {
    map.load(stage);
    initSnake();
    spawnItem();

    g_gameState = RUNNING;
    g_tick = 0;

    while (g_gameState == RUNNING) {
        // 1) 입력 처리
        Direction dir = handleInput();

        // 2) 틱 처리 (TICK_MS마다)
        usleep(TICK_MS * 1000);
        g_tick++;

        // 3) Snake 이동
        // moveSnake(dir);   // B 구현 후 활성화

        // 4) 아이템 갱신
        // updateItems();    // C 구현 후 활성화

        // 5) 게이트 갱신
        // spawnGate();      // D 구현 후 활성화

        // 6) 충돌 체크
        if (checkDead()) {
            g_gameState = GAME_OVER;
            break;
        }

        // 7) 미션 달성 체크
        if (checkMission()) {
            g_gameState = STAGE_CLEAR;
            break;
        }

        // 8) 화면 출력
        clear();
        map.render();
        renderScoreboard();
        refresh();
    }
}

// ────────────────────────────────────────────
//  main
// ────────────────────────────────────────────
int main() {
    initNcurses();

    Map map;

    for (g_currentStage = 1; g_currentStage <= 4; g_currentStage++) {
        runStage(map, g_currentStage);

        if (g_gameState == GAME_OVER) {
            showGameOver();
            break;
        } else if (g_gameState == STAGE_CLEAR) {
            showStageClear(g_currentStage);
            // 마지막 스테이지 클리어 시
            if (g_currentStage == 4) {
                mvprintw(MAP_SIZE/2, MAP_SIZE - 5, "ALL CLEAR!");
                nodelay(stdscr, FALSE);
                getch();
                break;
            }
        }
    }

    endwin();
    return 0;
}
