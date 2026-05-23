/*
 * main.cpp
 * ---------------------------------------------------------------
 * Snake 게임의 진입점(entry point) 및 게임 루프 관리 파일.
 *
 * [담당 역할]
 *  - ncurses 초기화 및 색상 쌍 등록
 *  - 키 입력 처리 (반전 효과 중에는 방향키를 반대로 변환)
 *  - 스테이지 단위 게임 루프 실행 (runStage)
 *  - 게임 오버 / 스테이지 클리어 화면 표시
 *
 * [수정 이력]
 *  추가_하윤  : snake.h, renderer.h include / initSnake, moveSnake 연동
 *  추가_진원     : item.h include / ItemManager 연동 / 반전 입력 처리
 * ---------------------------------------------------------------
 */
 
#include "globals.h"
#include "map.h"
#include "snake.h"  //추가_하윤
#include "renderer.h"  //추가_하윤
#include "item.h"        // 추가_진원: ItemManager 사용
// #include "gate.h"
// #include "scoreboard.h"
 
#include <ncurses.h>
#include <unistd.h>      // usleep
 
// ────────────────────────────────────────────
//  전방 선언 (각 팀원 구현 전 임시 stub)
// ────────────────────────────────────────────
// TODO: 아래 stub들은 각 파일 완성 후 삭제하고 헤더로 교체
// void initSnake()   { /* B 구현 */ }
// void moveSnake()   { /* B 구현 */ }
// bool checkDead()   { return false; } // 파일 삭제_하윤
// spawnItem() / updateItems() stub 제거         ← 추가_진원: ItemManager로 대체
void spawnGate()        { /* D 구현 */ }
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
        init_pair(1, COLOR_WHITE,   COLOR_WHITE);   // Immune Wall : 흰색 블록
        init_pair(2, COLOR_YELLOW,  COLOR_BLACK);   // Wall        : 노란색
        init_pair(3, COLOR_GREEN,   COLOR_BLACK);   // Snake       : 초록색
        init_pair(4, COLOR_CYAN,    COLOR_BLACK);   // Growth Item : 하늘색
        init_pair(5, COLOR_RED,     COLOR_BLACK);   // Poison Item : 빨간색
        init_pair(6, COLOR_MAGENTA, COLOR_BLACK);   // Gate        : 보라색
        // 추가_진원: Reverse Item 색상 (검정 글자 + 노란 배경)
        init_pair(9, COLOR_BLACK,   COLOR_YELLOW);  // Reverse Item: 노란 배경
    }
}
 
// ────────────────────────────────────────────
//  키 입력 처리
//  추가_진원: itemMgr.isReversed()가 true이면
//          방향키 입력을 반대로 바꿔서 반환
//          (UP↔DOWN, LEFT↔RIGHT)
// ────────────────────────────────────────────
Direction handleInput(const ItemManager& itemMgr) {  // 추가_진원: itemMgr 인자 추가
    const int ch = getch();
 
    // 추가_진원: 반전 효과 중일 때 방향 반전 처리 ──────────────────
    if (itemMgr.isReversed()) {
        switch (ch) {
            case KEY_UP:    return DOWN;
            case KEY_DOWN:  return UP;
            case KEY_LEFT:  return RIGHT;
            case KEY_RIGHT: return LEFT;
            default:        return NONE;
        }
    }
    // ── 추가_진원 끝 ──────────────────────────────────────────────
 
    // 반전 효과 없을 때 (기존 로직 그대로)
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
    mvprintw(MAP_SIZE / 2,     MAP_SIZE - 4, "GAME OVER");
    mvprintw(MAP_SIZE / 2 + 1, MAP_SIZE - 7, "Press any key to exit");
    nodelay(stdscr, FALSE);
    getch();
}
 
// ────────────────────────────────────────────
//  스테이지 클리어 화면
// ────────────────────────────────────────────
void showStageClear(const int stage) {
    clear();
    mvprintw(MAP_SIZE / 2,     MAP_SIZE - 6, "STAGE %d CLEAR!", stage);
    mvprintw(MAP_SIZE / 2 + 1, MAP_SIZE - 8, "Press any key to continue");
    nodelay(stdscr, FALSE);
    getch();
    nodelay(stdscr, TRUE);
}
 
// ────────────────────────────────────────────
//  한 스테이지 실행
//  추가_진원: ItemManager 인스턴스를 스테이지마다 생성하여
//          아이템 상태를 스테이지 단위로 격리
// ────────────────────────────────────────────
void runStage(Map& map, const int stage) {
    map.load(stage);
    initSnake(map);  //map 추가_하윤
 
    ItemManager itemMgr;          // 추가_진원: 스테이지마다 새 인스턴스 생성
    itemMgr.init(map);            // 추가_진원: 기존 spawnItem() 대체
 
    g_gameState = RUNNING;
    g_tick = 0;
 
    while (g_gameState == RUNNING) {
 
        // 1) 입력 처리
        //    추가_진원: itemMgr 전달 → 반전 효과 중이면 방향 반전
        const Direction dir = handleInput(itemMgr);  // 추가_진원: 인자 추가
 
        // 2) 틱 처리 (TICK_MS마다 한 틱 진행)
        usleep(TICK_MS * 1000);
        g_tick++;
 
        // 3) Snake 이동
        setSnakeDir(dir);
        const int movedCell = moveSnake(map);  // 추가_진원: 반환값 저장 (아이템 판정에 사용)
        //moveSnake(map); // 추가_하윤   //기존코드
 
        // 4) 아이템 수명 갱신 (만료 제거 + 부족분 보충 + Reverse 주기 출현)
        itemMgr.update(map);                   // 추가_진원: 기존 updateItems() 대체
 
        // 5) 아이템 상호작용 처리                 추가_진원: 블록 전체 신규
        {
            const Pos head   = getSnakeHead();
            const int result = itemMgr.eat(map, head, movedCell);
 
            if (result == GROWTH_ITEM) {
                // Growth Item 획득: 뱀 길이 +1
                growSnake();
 
            } else if (result == POISON_ITEM) {
                // Poison Item 획득: 뱀 길이 -1 (최소 길이 미만이면 사망)
                if (!shrinkSnake()) {
                    g_gameState = GAME_OVER;
                    break;
                }
            }
            // REVERSE_ITEM은 itemMgr.eat() 내부에서 isReversed_ 플래그를
            // 설정하므로 여기서 추가 처리 없음
        }
        // ── 추가_진원 끝 ──────────────────────────────────────────
 
        // 6) 게이트 갱신 (D 구현 후 활성화)
        // spawnGate();
 
        // 7) 충돌 체크 (벽·자기몸 충돌 시 GAME_OVER)
        if (checkDead(map)) {
            g_gameState = GAME_OVER;
            break;
        }
 
        // 8) 미션 달성 체크 (모든 미션 완료 시 STAGE_CLEAR)
        if (checkMission()) {
            g_gameState = STAGE_CLEAR;
            break;
        }
 
        // 9) 화면 출력
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
    initRenderer();   // 추가_하윤
 
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
                mvprintw(MAP_SIZE / 2, MAP_SIZE - 5, "ALL CLEAR!");
                nodelay(stdscr, FALSE);
                getch();
                break;
            }
        }
    }
 
    quitRenderer();   // 추가_하윤
    endwin();
    return 0;
}
 
