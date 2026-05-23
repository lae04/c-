/*
 * scoreboard.h
 * ---------------------------------------------------------------
 * Snake Game의 점수판과 미션 상태를 관리하는 헤더 파일.
 *
 * [담당 역할 - D단계]
 * - 현재 Snake 길이, 최대 길이 관리
 * - Growth / Poison / Gate / Reverse Item 횟수 관리
 * - 스테이지별 Mission 목표 관리
 * - Mission 달성 여부 판단
 * - renderer.cpp에 넘길 ScoreData 생성
 *
 * [구현자]
 * 추가_민석 : ScoreboardManager 클래스 설계 및 선언
 * ---------------------------------------------------------------
 */

#pragma once

#include "globals.h"
#include "renderer.h"

// ---------------------------------------------------------------
// ScoreboardManager
// ---------------------------------------------------------------
// 실제 화면 출력은 renderer.cpp의 renderScoreboard()가 담당한다.
// 이 클래스는 점수와 미션 상태를 계산해서 ScoreData 형태로 만든다.
// ---------------------------------------------------------------

class ScoreboardManager {
public:
    ScoreboardManager();

    // 추가_민석: 스테이지 시작 시 점수판 상태 초기화
    void init(int stage);

    // 추가_민석: 매 틱마다 현재 점수 정보 갱신
    void update(int currentLength,
                int growthCount,
                int poisonCount,
                int gateCount,
                int reverseCount,
                bool reverseActive);

    // 추가_민석: Mission이 모두 달성되었는지 확인
    bool isMissionClear() const;

    // 추가_민석: renderer.cpp에 넘길 ScoreData 생성
    ScoreData makeScoreData() const;

    // 추가_민석: 추가 아이템 정보 반환
    int getReverseCount() const;
    bool isReverseActive() const;

    // 추가_민석: 게임 시간 반환
    int getElapsedSeconds() const;

private:
    Mission mission_;

    int currentLength_;
    int maxLength_;

    int growthCount_;
    int poisonCount_;
    int gateCount_;

    // 추가_민석: C단계 추가 기능 Reverse Item 표시용
    int reverseCount_;
    bool reverseActive_;

    int elapsedSeconds_;

    // 추가_민석: 스테이지별 미션 목표 설정
    Mission getMissionByStage(int stage) const;

    // 추가_민석: 개별 미션 달성 여부
    bool isLengthMissionClear() const;
    bool isGrowthMissionClear() const;
    bool isPoisonMissionClear() const;
    bool isGateMissionClear() const;
};