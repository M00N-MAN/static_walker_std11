.PHONY: clean

PROJECT = StaticWalker
PROJECT_DIR = static_walker_std11
EXE_FILE = $(PROJECT).exe
INCLUDE_DIR = -I ../$(PROJECT_DIR)
				
LIB_DIR =
LDFLAGS =
OUTPUT_DIR = ../$(PROJECT_DIR)/bin
OUTPUT = $(OUTPUT_DIR)/$(EXE_FILE)
CPPFLAGS = -ggdb -std=c++11 -Wextra -Wall
SRC_DIR := ../$(PROJECT_DIR)
OBJ_DIR := $(OUTPUT_DIR)
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
cxx := g++
#cxx := clang++

$(OUTPUT): $(OBJ_FILES)
	$(cxx) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(cxx) $(INCLUDE_DIR) $(CPPFLAGS) -c -o $@ $<

clean:
	rm -f $(OUTPUT_DIR)/*.o
	rm -f $(OUTPUT_DIR)/*.exe