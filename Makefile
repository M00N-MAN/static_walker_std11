.PHONY: clean all release debug

PROJECT = StaticWalker
PROJECT_DIR = static_walker_std11
EXE_FILE = $(PROJECT).exe
INCLUDE_DIR = -I ../$(PROJECT_DIR)
				
LIB_DIR =
LDFLAGS =
OUTPUT_DIR = ../$(PROJECT_DIR)/bin
OUTPUT = $(OUTPUT_DIR)/$(EXE_FILE)

SRC_DIR := ../$(PROJECT_DIR)
OBJ_DIR := $(OUTPUT_DIR)
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

CXXFLAGS = -std=c++11 -Wextra -Wall -pedantic 
CXX := g++

all: $(OUTPUT)

debug: CXXFLAGS += -O0 -DDEBUG -ggdb
debug: $(OUTPUT)

release: CXXFLAGS += -Os -s -fno-rtti
release: $(OUTPUT)

$(OUTPUT): $(OBJ_FILES)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(INCLUDE_DIR) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -vf $(OUTPUT_DIR)/*.o $(OUTPUT)