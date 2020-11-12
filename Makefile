.PHONY: clean

PROJECT = StaticWalker
PROJECT_DIR = static_walker_std11
EXE_FILE = $(PROJECT).exe
INCLUDE_DIR = -I ../$(PROJECT_DIR)
				
LIB_DIR =
LDFLAGS =
OUTPUT_DIR = ../$(PROJECT_DIR)/bin
OUTPUT = $(OUTPUT_DIR)/$(EXE_FILE)
CPPFLAGS = -ggdb -std=c++11 -Wextra -Wall -pedantic
SRC_DIR := ../$(PROJECT_DIR)
OBJ_DIR := $(OUTPUT_DIR)
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
cxx := g++ -Wno-deprecated
#cxx := clang++ --verbose -nostdinc -stdlib=libstdc++ --gcc-toolchain=C:\\Program\ Files\\mingw64 -target i686-w64-mingw32 -I C:\\mingw-w64\\i686-8.1.0-posix-dwarf-rt_v6-rev0\\mingw32\\lib\\gcc\\i686-w64-mingw32\\8.1.0\\include\c++\tr1

$(OUTPUT): $(OBJ_FILES)
	$(cxx) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(cxx) $(INCLUDE_DIR) $(CPPFLAGS) -c -o $@ $<

clean:
	rm -f $(OUTPUT_DIR)/*.o
	rm -f $(OUTPUT_DIR)/*.exe