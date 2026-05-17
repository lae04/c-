#pragma once

#include "globals.h"
#include <string>

// ────────────────────────────────────────────
//  Map 클래스
//  - 2D 배열로 맵 상태 관리
//  - Wall / ImmuneWall / Gate 구분
// ────────────────────────────────────────────
class Map {
public:
    // 생성 / 초기화
    Map();
    void load(int stage);           // 스테이지 번호로 맵 로딩
    void reset();                   // 맵 초기화 (EMPTY로)

    // 셀 접근
    int  getCell(int y, int x) const;
    void setCell(int y, int x, int val);

    // 셀 타입 판별
    bool isWall(int y, int x) const;
    bool isImmuneWall(int y, int x) const;
    bool isGate(int y, int x) const;
    bool isEmpty(int y, int x) const;
    bool inBounds(int y, int x) const;

    // 렌더링
    void render() const;            // ncurses로 맵 출력

    // 맵 크기
    int getHeight() const { return height_; }
    int getWidth()  const { return width_;  }

private:
    int grid_[MAP_SIZE][MAP_SIZE];
    int height_;
    int width_;

    // 스테이지별 하드코딩 맵 로더
    void loadStage1();
    void loadStage2();
    void loadStage3();
    void loadStage4();

    // 공통 테두리 생성 (Immune Wall 외곽, Wall 내곽)
    void buildBorder();
};
