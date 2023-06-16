OPENCV=1
CPP=g++ -std=c++11
OPTS=-O0
CFLAGS := -Wall -Wno-unknown-pragmas -Wfatal-errors

RUN_DIR := ./obj/run
DEBUG_DIR := ./obj/debug
OBJ_DIR := $(RUN_DIR)

TARGET := mocv
DEBUG_TARGET := debug_mocv

ifeq ($(firstword $(MAKECMDGOALS)), debug)
	CFLAGS += "-g"
	OBJ_DIR := $(DEBUG_DIR)
	TARGET := $(DEBUG_TARGET)
endif
SRC_DIR := ./src

SRCS := $(shell find $(SRC_DIR) -name '*.cpp' -or -name '*.c')
OBJS := $(SRCS:%=$(OBJ_DIR)/%.o)
IFLAGS = -Isrc/



LDFLAGS := $(shell pkg-config --libs opencv4) -lstdc++ -lm
IFLAGS += $(shell pkg-config --cflags opencv4)

ifeq ($(OPENCV), 1)
LDFLAGS += -DOPENCV
IFLAGS += -DOPENCV
endif


VPATH=src/

all: obj $(TARGET)

run: all
	./$(TARGET)

debug: all
	gdb $(TARGET)

$(TARGET): $(OBJS)
	$(CPP) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CPP) $(CFLAGS)  $(IFLAGS) -c $< -o $@

$(OBJ_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS)  $(IFLAGS) -c $< -o $@

obj:
	mkdir -p obj

clean:
	rm -rf $(TARGET) $(DEBUG_TARGET) $(DEBUG_DIR) $(RUN_DIR)
