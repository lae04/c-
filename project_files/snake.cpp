#include "snake.h"

// ────────────────────────────────────────────
//  전역 변수 정의
// ────────────────────────────────────────────
std::deque<Pos> g_snakeBody;
Direction       g_snakeDir     = RIGHT;
Direction       g_snakeNextDir = RIGHT;

static bool     g_isDead       = false;  // 사망 플래그
static constexpr int MIN_LENGTH = 3;

// ────────────────────────────────────────────
//  내부 헬퍼 (이 파일 안에서만 사용)
// ────────────────────────────────────────────

// 반대 방향인지 체크
static bool isOpposite(Direction a, Direction b) {
    return (a == UP    && b == DOWN)  ||
           (a == DOWN  && b == UP)    ||
           (a == LEFT  && b == RIGHT) ||
           (a == RIGHT && b == LEFT);
}

// 현재 방향 기준으로 다음 머리 좌표 계산
static Pos calcNextHead() {
    Pos head = g_snakeBody.front();
    switch (g_snakeDir) {
        case UP:    head.y -= 1; break;
        case DOWN:  head.y += 1; break;
        case LEFT:  head.x -= 1; break;
        case RIGHT: head.x += 1; break;
        default: break;
    }
    return head;
}

// 맵에 뱀 몸통 다시 그리기
static void applySnakeToMap(Map& map) {
    // 기존 뱀 위치 전부 지우기
    for (int y = 0; y < MAP_SIZE; ++y)
        for (int x = 0; x < MAP_SIZE; ++x)
            if (map.getCell(y, x) == SNAKE_HEAD ||
                map.getCell(y, x) == SNAKE_BODY)
                map.setCell(y, x, EMPTY);

    // 새 위치로 다시 표시
    map.setCell(g_snakeBody.front().y, g_snakeBody.front().x, SNAKE_HEAD);
    for (int i = 1; i < (int)g_snakeBody.size(); ++i)
        map.setCell(g_snakeBody[i].y, g_snakeBody[i].x, SNAKE_BODY);
}

// ────────────────────────────────────────────
//  초기화
// ────────────────────────────────────────────
void initSnake(Map& map) {
    g_snakeBody.clear();
    g_snakeDir     = RIGHT;
    g_snakeNextDir = RIGHT;
    g_isDead       = false;

    // 맵 중앙에서 시작, 오른쪽 방향으로 길이 3
    int cy = map.getHeight() / 2;
    int cx = map.getWidth()  / 2;

    g_snakeBody.push_back({cy, cx});      // 머리
    g_snakeBody.push_back({cy, cx - 1}); // 몸통
    g_snakeBody.push_back({cy, cx - 2}); // 꼬리

    applySnakeToMap(map);
}

// ────────────────────────────────────────────
//  방향 입력
// ────────────────────────────────────────────
void setSnakeDir(Direction dir) {
    if (dir == NONE) return;
    if (!isOpposite(dir, g_snakeDir))
        g_snakeNextDir = dir;
}

// ────────────────────────────────────────────
//  이동 (매 틱 호출)
// ────────────────────────────────────────────
int moveSnake(Map& map) {
    // 입력 버퍼 적용
    g_snakeDir = g_snakeNextDir;

    Pos next = calcNextHead();

    // 범위 밖 → 벽 충돌로 처리
    if (!map.inBounds(next.y, next.x)) {
        g_isDead = true;
        return WALL;
    }

    int cell = map.getCell(next.y, next.x);

    // 벽 충돌
    if (cell == WALL || cell == IMMUNE_WALL) {
        g_isDead = true;
        return cell;
    }

    // 자기 몸 충돌 (꼬리는 이번 틱에 빠지므로 제외)
    for (int i = 0; i < (int)g_snakeBody.size() - 1; ++i) {
        if (g_snakeBody[i] == next) {
            g_isDead = true;
            return SNAKE_BODY;
        }
    }

    // 정상 이동: 꼬리 제거, 머리 추가
    g_snakeBody.pop_back();
    g_snakeBody.push_front(next);

    applySnakeToMap(map);

    return cell;  // EMPTY / GROWTH_ITEM / POISON_ITEM / GATE
}

// ────────────────────────────────────────────
//  사망 체크 (main.cpp의 checkDead()에서 호출)
// ────────────────────────────────────────────
bool checkDead(Map& map) {
    return g_isDead;
}

// ────────────────────────────────────────────
//  성장 / 축소
// ────────────────────────────────────────────
void growSnake() {
    // 꼬리 뒤에 같은 위치 추가 → 다음 틱에 자연스럽게 분리
    g_snakeBody.push_back(g_snakeBody.back());
}

bool shrinkSnake() {
    if ((int)g_snakeBody.size() <= MIN_LENGTH) {
        g_isDead = true;
        return false;  // 사망
    }
    g_snakeBody.pop_back();
    return true;
}

// ────────────────────────────────────────────
//  게이트 텔레포트 (gate.cpp에서 호출)
// ────────────────────────────────────────────
void teleportSnake(Pos newHead, Direction newDir) {
    g_snakeDir     = newDir;
    g_snakeNextDir = newDir;
    g_snakeBody.pop_front();
    g_snakeBody.push_front(newHead);
}

// ────────────────────────────────────────────
//  Getter
// ────────────────────────────────────────────
Pos getSnakeHead() {
    return g_snakeBody.front();
}

int getSnakeLength() {
    return (int)g_snakeBody.size();
}
