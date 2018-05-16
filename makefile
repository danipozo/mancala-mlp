
main.o: main.cpp
	g++ -c $< -o $@ -O2 -lm -I./ -std=c++17

format: *.h *.cpp *.hpp utils/*.cpp
	astyle --style=kr --indent=spaces --indent-modifiers --indent-switches --indent-col1-comments --break-blocks --pad-oper --pad-comma --pad-header --align-reference=type --add-braces --attach-return-type --attach-return-type-decl  --max-code-length=80 --break-one-line-headers $^

GameState.cpp: GameState.h
GameState.o: GameState.cpp
	g++ -c $< -o $@ -O2 -lm -I./ -std=c++17

Bot.cpp: Bot.h
Bot.o: Bot.cpp
	g++ -c $< -o $@ -O2 -lm -I./ -std=c++17

MancalaTrainingPlayer.cpp: MancalaTrainingPlayer.hpp alpha_beta.cpp alpha_beta.hpp
MancalaTrainingPlayer.o: MancalaTrainingPlayer.cpp
	g++ -c $< -o $@ -O2 -lm -I./ -std=c++17

AlumnoBot.cpp: AlumnoBot.h
AlumnoBot.o: AlumnoBot.cpp
	g++ -c $< -o $@ -O2 -lm -I./ -std=c++17

SimulatorLink.cpp: SimulatorLink.h
SimulatorLink.o: SimulatorLink.cpp
	g++ -c $< -o $@ -O2 -lm -I./ -std=c++17


all: main.cpp AlumnoBot.o Bot.o GameState.o MancalaTrainingPlayer.o heuristic.o nn.cpp SimulatorLink.o
	g++ -oAlumnoBot $^ -I./ -O2 -lm -std=c++17

heuristic.cpp: heuristic.hpp
heuristic.o: heuristic.cpp
	g++ -c $< -o $@ -O2 -lm -I./ -std=c++17

self_play_parallel.o: self_play_parallel.cpp
	g++ -c $< -o $@ -O2 -lm -I./ -std=c++17

self_play_parallel: self_play_parallel.o MancalaTrainingPlayer.o heuristic.o GameState_mod.o nn.cpp
	g++ $^  -O2 -o $@ -std=c++17 -pthread

train_on_database: utils/train_on_database.cpp nn.cpp
	g++ $^ -I.. -o $@ -O2 -std=c++17 -lsqlite3

clean:
	rm AlumnoBot

