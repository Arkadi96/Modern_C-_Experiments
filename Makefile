SOURCE_FILES := $(wildcard src/*.cpp)
BASE_NAMES := $(patsubst %.cpp, %, $(SOURCE_FILES))
PROGRAM_NAMES := $(patsubst src/%, %, $(BASE_NAMES))

$(PROGRAM_NAMES):
	g++ src/$@.cpp -o bin/$@.out

clean:
	rm -rf bin/*
