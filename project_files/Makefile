CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
TARGET   = snake

# ─── 소스 파일 목록 ───────────────────────────
# 팀원 파일 완성되면 주석 해제
SRCS = main.cpp \
       globals.cpp \
       map.cpp \
       snake.cpp \
       renderer.cpp \  # 추가_하윤
       # item.cpp         ← C 완성 후 추가
       # gate.cpp         ← D 완성 후 추가
       # scoreboard.cpp   ← D 완성 후 추가

OBJS = $(SRCS:.cpp=.o)

# ─── OS 자동 감지 (Linux vs Windows) ─────────
# Linux/Mac  → ncurses 사용
# Windows    → PDCurses 사용 (MinGW 환경 기준)
#
# 사용법:
#   Linux  : make
#   Windows: make OS=windows  (또는 MinGW에서 자동 감지)

ifeq ($(OS), Windows_NT)
    LDFLAGS = -lpdcurses
    EXE     = $(TARGET).exe
    RM      = del /Q
else
    LDFLAGS = -lncurses
    EXE     = $(TARGET)
    RM      = rm -f
endif

# ─── 기본 타겟 ───────────────────────────────
all: $(EXE)

$(EXE): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ─── 의존성 ──────────────────────────────────
main.o:         main.cpp globals.h map.h
globals.o:      globals.cpp globals.h
map.o:          map.cpp map.h globals.h
# snake.o:      snake.cpp snake.h globals.h map.h
# item.o:       item.cpp item.h globals.h map.h
# gate.o:       gate.cpp gate.h globals.h map.h
# scoreboard.o: scoreboard.cpp scoreboard.h globals.h

# ─── 유틸리티 ────────────────────────────────
clean:
	$(RM) $(OBJS) $(EXE)

rebuild: clean all

run: all
	./$(EXE)

# ─── 참고: 수동 컴파일 (강의자료 기준) ──────────
# Linux  : g++ -o snake main.cpp globals.cpp map.cpp -lncurses -std=c++17
# Windows: g++ -o snake main.cpp globals.cpp map.cpp -lpdcurses -std=c++17

.PHONY: all clean rebuild run
