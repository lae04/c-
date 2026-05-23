/*
 * gate.h
 * ---------------------------------------------------------------
 * Snake Game의 Gate 기능을 관리하는 헤더 파일.
 *
 * [담당 역할 - D단계]
 * - Gate 한 쌍 생성
 * - Gate 위치 저장
 * - Gate 진입 여부 판단
 * - Gate 사용 횟수 관리
 * - Gate 진출 방향 계산 함수 선언
 *
 * [구현자]
 * 추가_민석 : GateManager 클래스 설계 및 선언
 * ---------------------------------------------------------------
 */

#pragma once

#include "globals.h"
#include "map.h"

// ---------------------------------------------------------------
// GateManager
// ---------------------------------------------------------------
// Gate는 항상 두 개가 한 쌍으로 존재한다.
// Wall 위치에만 생성되며, Immune Wall에는 생성되지 않는다.
// Snake가 한 Gate에 진입하면 다른 Gate로 이동한다.
// ---------------------------------------------------------------

class GateManager {
public:
    GateManager();

    // 추가_민석: 스테이지 시작 시 Gate 상태 초기화
    void init(Map& map);

    // 추가_민석: 일정 시간마다 Gate 생성/갱신
    void update(Map& map);

    // 추가_민석: 현재 좌표가 Gate인지 확인
    bool isGateCell(const Pos& pos) const;

    // 추가_민석: Snake가 Gate에 들어갔을 때 반대편 Gate로 이동
    bool enterGate(Map& map, const Pos& entrance, Direction currentDir);

    // 추가_민석: Gate 사용 횟수 반환
    int getGateCount() const;

    // 추가_민석: Gate 활성 상태 반환
    bool isActive() const;

    // 추가_민석: Gate A, B 위치 반환
    Pos getGateA() const;
    Pos getGateB() const;

private:
    Pos gateA_;
    Pos gateB_;

    bool active_;
    int spawnTick_;
    int gateCount_;

    // 추가_민석: 기존 Gate를 Wall로 되돌림
    void removeGate(Map& map);

    // 추가_민석: 새로운 Gate 한 쌍 생성
    void spawnGate(Map& map);

    // 추가_민석: 임의의 Wall 위치 선택
    Pos randomWallPos(const Map& map) const;

    // 추가_민석: entrance가 gateA_면 gateB_, gateB_면 gateA_ 반환
    Pos getExitGate(const Pos& entrance) const;

    // 추가_민석: Gate 진출 방향 계산
    Direction getExitDirection(const Map& map, const Pos& exitGate, Direction currentDir) const;

    // 추가_민석: 방향에 따른 다음 좌표 계산
    Pos nextPos(const Pos& pos, Direction dir) const;

    // 추가_민석: 해당 방향으로 Snake가 나갈 수 있는지 확인
    bool canExitTo(const Map& map, const Pos& gate, Direction dir) const;

    // 추가_민석: 방향 회전 계산
    Direction clockwise(Direction dir) const;
    Direction counterClockwise(Direction dir) const;
    Direction opposite(Direction dir) const;
};