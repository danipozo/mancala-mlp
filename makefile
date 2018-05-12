
all: format
	g++ -oAlumnoBot *.cpp -I./ -lm -std=c++11


format: *.h *.cpp *.hpp utils/*.cpp
	astyle --style=kr --indent=spaces --indent-modifiers --indent-switches --indent-col1-comments --break-blocks --pad-oper --pad-comma --pad-header --align-reference=type --add-braces --attach-return-type --attach-return-type-decl  --max-code-length=80 --break-one-line-headers $^

self_play_parallel: self_play_parallel.cpp MancalaMCTSPlayer.cpp GameState_mod.cpp nn.cpp
	g++ $^  -O2 -o $@ -std=c++17 -pthread

train_on_database: utils/train_on_database.cpp nn.cpp
	g++ $^ -I.. -o $@ -O2 -std=c++17 -lsqlite3

clean:
	rm AlumnoBot

