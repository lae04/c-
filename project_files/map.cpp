#include "map.h"
#include <stdexcept>

// ────────────────────────────────────────────
//  생성자
// ────────────────────────────────────────────
Map::Map() : height_(MAP_SIZE), width_(MAP_SIZE) {
    reset();
}

void Map::reset() {
    for (int y = 0; y < MAP_SIZE; y++)
        for (int x = 0; x < MAP_SIZE; x++)
            grid_[y][x] = EMPTY;
}

// ────────────────────────────────────────────
//  스테이지 로딩
// ────────────────────────────────────────────
void Map::load(int stage) {
    reset();
    switch (stage) {
        case 1: loadStage1(); break;
        case 2: loadStage2(); break;
        case 3: loadStage3(); break;
        case 4: loadStage4(); break;
        default: loadStage1(); break;
    }
}

// ────────────────────────────────────────────
//  공통 테두리 생성
//  가장 외곽: Immune Wall (2)
//  바로 안쪽: Wall (1)
// ────────────────────────────────────────────
void Map::buildBorder() {
    for (int y = 0; y < height_; y++) {
        for (int x = 0; x < width_; x++) {
            // 가장 외곽 한 줄 → Immune Wall
            if (y == 0 || y == height_-1 || x == 0 || x == width_-1) {
                grid_[y][x] = IMMUNE_WALL;
            }
            // 바로 안쪽 한 줄 → Wall
            else if (y == 1 || y == height_-2 || x == 1 || x == width_-2) {
                grid_[y][x] = WALL;
            }
        }
    }
}

// ────────────────────────────────────────────
//  Stage 1 맵
//  과제 예시 배열 그대로 반영
//  2 1 1 1 ... 1 2  (외곽 ImmuneWall)
//  1 0 0 0 ... 0 1  (Wall 안쪽)
//  Snake 초기 위치: (10, 10) 방향으로 body 3칸
// ────────────────────────────────────────────
void Map::loadStage1() {
    buildBorder();
    // 내부 장애물 없음 (기본 맵)
    // Snake 초기 배치는 Snake 클래스에서 처리
}

// ────────────────────────────────────────────
//  Stage 2 맵 (내부 벽 추가 예시)
// ────────────────────────────────────────────
void Map::loadStage2() {
    buildBorder();
    // 중앙 수평 벽
    for (int x = 5; x <= 10; x++)
        grid_[10][x] = WALL;
    // 중앙 수직 벽
    for (int y = 5; y <= 10; y++)
        grid_[y][15] = WALL;
}

// ────────────────────────────────────────────
//  Stage 3 맵
// ────────────────────────────────────────────
void Map::loadStage3() {
    buildBorder();
    // ㄱ자 벽
    for (int x = 5; x <= 15; x++)
        grid_[5][x] = WALL;
    for (int y = 5; y <= 15; y++)
        grid_[y][5] = WALL;
}

// ────────────────────────────────────────────
//  Stage 4 맵
// ────────────────────────────────────────────
void Map::loadStage4() {
    buildBorder();
    // 십자 벽
    for (int x = 5; x <= 15; x++)
        grid_[10][x] = WALL;
    for (int y = 5; y <= 15; y++)
        grid_[y][10] = WALL;
    // 단, 중앙(10,10)은 비워둠
    grid_[10][10] = EMPTY;
}

// ────────────────────────────────────────────
//  셀 접근
// ────────────────────────────────────────────
int Map::getCell(int y, int x) const {
    if (!inBounds(y, x)) return IMMUNE_WALL;
    return grid_[y][x];
}

void Map::setCell(int y, int x, int val) {
    if (inBounds(y, x))
        grid_[y][x] = val;
}

// ────────────────────────────────────────────
//  타입 판별
// ────────────────────────────────────────────
bool Map::isWall(int y, int x)       const { return getCell(y,x) == WALL; }
bool Map::isImmuneWall(int y, int x) const { return getCell(y,x) == IMMUNE_WALL; }
bool Map::isGate(int y, int x)       const { return getCell(y,x) == GATE; }
bool Map::isEmpty(int y, int x)      const { return getCell(y,x) == EMPTY; }
bool Map::inBounds(int y, int x)     const {
    return y >= 0 && y < height_ && x >= 0 && x < width_;
}

// ────────────────────────────────────────────
//  렌더링 (ncurses)
//  셀 값에 따라 문자/색상 출력
// ────────────────────────────────────────────
void Map::render() const {
    for (int y = 0; y < height_; y++) {
        for (int x = 0; x < width_; x++) {
            int cell  = grid_[y][x];
            int screenX = x * 2;   // 셀 하나를 2칸 너비로 표현

            switch (cell) {
                case IMMUNE_WALL:
                    attron(COLOR_PAIR(1) | A_BOLD);
                    mvprintw(y, screenX, "##");
                    attroff(COLOR_PAIR(1) | A_BOLD);
                    break;
                case WALL:
                    attron(COLOR_PAIR(2));
                    mvprintw(y, screenX, "[]");
                    attroff(COLOR_PAIR(2));
                    break;
                case SNAKE_HEAD:
                    attron(COLOR_PAIR(3) | A_BOLD);
                    mvprintw(y, screenX, "@ ");
                    attroff(COLOR_PAIR(3) | A_BOLD);
                    break;
                case SNAKE_BODY:
                    attron(COLOR_PAIR(3));
                    mvprintw(y, screenX, "o ");
                    attroff(COLOR_PAIR(3));
                    break;
                case GROWTH_ITEM:
                    attron(COLOR_PAIR(4) | A_BOLD);
                    mvprintw(y, screenX, "+ ");
                    attroff(COLOR_PAIR(4) | A_BOLD);
                    break;
                case POISON_ITEM:
                    attron(COLOR_PAIR(5) | A_BOLD);
                    mvprintw(y, screenX, "- ");
                    attroff(COLOR_PAIR(5) | A_BOLD);
                    break;
                case GATE:
                    attron(COLOR_PAIR(6) | A_BOLD);
                    mvprintw(y, screenX, "G ");
                    attroff(COLOR_PAIR(6) | A_BOLD);
                    break;
                default:
                    mvprintw(y, screenX, "  ");
                    break;
            }
        }
    }
}
