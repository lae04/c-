/*
 * scoreboard.cpp
 * ---------------------------------------------------------------
 * Snake Game의 점수판과 미션 상태를 구현하는 파일.
 *
 * [담당 역할 - D단계]
 * - 현재 점수 정보 갱신
 * - 최대 길이 계산
 * - 스테이지별 미션 목표 설정
 * - 미션 달성 여부 판단
 * - renderer.cpp의 renderScoreboard()에 넘길 ScoreData 생성
 *
 * [구현자]
 * 추가_민석 : ScoreboardManager 클래스 구현
 * ---------------------------------------------------------------
 */

#include "scoreboard.h"

// ---------------------------------------------------------------
// 생성자
// ---------------------------------------------------------------

ScoreboardManager::ScoreboardManager()
    : mission_{0, 0, 0, 0},
      currentLength_(0),
      maxLength_(0),
      growthCount_(0),
      poisonCount_(0),
      gateCount_(0),
      reverseCount_(0),
      reverseActive_(false),
      elapsedSeconds_(0) {
}

// ---------------------------------------------------------------
// 스테이지 시작 시 점수판 상태 초기화
// ---------------------------------------------------------------

void ScoreboardManager::init(int stage) {
    mission_ = getMissionByStage(stage);

    currentLength_ = 0;
    maxLength_ = 0;

    growthCount_ = 0;
    poisonCount_ = 0;
    gateCount_ = 0;

    // 추가_민석: C단계 추가 아이템 Reverse Item 표시용 데이터 초기화
    reverseCount_ = 0;
    reverseActive_ = false;

    elapsedSeconds_ = 0;
}

// ---------------------------------------------------------------
// 매 틱마다 현재 점수 정보 갱신
// ---------------------------------------------------------------

void ScoreboardManager::update(int currentLength,
                               int growthCount,
                               int poisonCount,
                               int gateCount,
                               int reverseCount,
                               bool reverseActive) {
    currentLength_ = currentLength;

    if (currentLength_ > maxLength_) {
        maxLength_ = currentLength_;
    }

    growthCount_ = growthCount;
    poisonCount_ = poisonCount;
    gateCount_ = gateCount;

    // 추가_민석: C단계 Reverse Item 정보 저장
    reverseCount_ = reverseCount;
    reverseActive_ = reverseActive;

    // 추가_민석: g_tick과 TICK_MS를 이용해 초 단위 시간 계산
    elapsedSeconds_ = (g_tick * TICK_MS) / 1000;
}

// ---------------------------------------------------------------
// 모든 Mission 달성 여부 확인
// ---------------------------------------------------------------

bool ScoreboardManager::isMissionClear() const {
    return isLengthMissionClear() &&
           isGrowthMissionClear() &&
           isPoisonMissionClear() &&
           isGateMissionClear();
}

// ---------------------------------------------------------------
// renderer.cpp에 넘길 ScoreData 생성
// ---------------------------------------------------------------

ScoreData ScoreboardManager::makeScoreData() const {
    ScoreData score{};

    score.length = currentLength_;
    score.growthCount = growthCount_;
    score.poisonCount = poisonCount_;
    score.gateCount = gateCount_;

    score.targetLength = mission_.targetLength;
    score.targetGrowth = mission_.targetGrowth;
    score.targetPoison = mission_.targetPoison;
    score.targetGate = mission_.targetGate;

    score.lengthOk = isLengthMissionClear();
    score.growthOk = isGrowthMissionClear();
    score.poisonOk = isPoisonMissionClear();
    score.gateOk = isGateMissionClear();

    return score;
}

// ---------------------------------------------------------------
// Reverse Item 획득 횟수 반환
// ---------------------------------------------------------------

int ScoreboardManager::getReverseCount() const {
    return reverseCount_;
}

// ---------------------------------------------------------------
// Reverse Item 효과 활성 여부 반환
// ---------------------------------------------------------------

bool ScoreboardManager::isReverseActive() const {
    return reverseActive_;
}

// ---------------------------------------------------------------
// 게임 시간 반환
// ---------------------------------------------------------------

int ScoreboardManager::getElapsedSeconds() const {
    return elapsedSeconds_;
}

// ---------------------------------------------------------------
// 스테이지별 Mission 목표 설정
//
// B : 목표 Snake 길이
// + : Growth Item 획득 목표
// - : Poison Item 획득 목표
// G : Gate 사용 목표
// ---------------------------------------------------------------

Mission ScoreboardManager::getMissionByStage(int stage) const {
    switch (stage) {
    case 1:
        return Mission{5, 1, 1, 1};

    case 2:
        return Mission{6, 2, 1, 1};

    case 3:
        return Mission{7, 2, 2, 2};

    case 4:
        return Mission{8, 3, 2, 2};

    default:
        return Mission{5, 1, 1, 1};
    }
}

// ---------------------------------------------------------------
// 길이 Mission 달성 여부
// ---------------------------------------------------------------

bool ScoreboardManager::isLengthMissionClear() const {
    return maxLength_ >= mission_.targetLength;
}

// ---------------------------------------------------------------
// Growth Item Mission 달성 여부
// ---------------------------------------------------------------

bool ScoreboardManager::isGrowthMissionClear() const {
    return growthCount_ >= mission_.targetGrowth;
}

// ---------------------------------------------------------------
// Poison Item Mission 달성 여부
// ---------------------------------------------------------------

bool ScoreboardManager::isPoisonMissionClear() const {
    return poisonCount_ >= mission_.targetPoison;
}

// ---------------------------------------------------------------
// Gate Mission 달성 여부
// ---------------------------------------------------------------

bool ScoreboardManager::isGateMissionClear() const {
    return gateCount_ >= mission_.targetGate;
}