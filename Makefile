# Makefile
# tool marcros
CC := g++
CCFLAG := -std=c++11 -w -O3 -D_PTHREADS
DEBGFLAG := -g
CCOBJFLAG := $(CCFLAG) -c
LIBS    := -lpthread
INCLUDE:= -I./usr/local/lib

# path marcros
BIN_PATH := bin
OBJ_PATH := obj
SRC_PATH := src
DBG_PATH := debug

# compile marcors
TARGET_NAME := WebServer
TARGET := $(BIN_PATH)/$(TARGET_NAME)
TARGET_DEBUG := $(DBG_PATH)/$(TARGET_NAME)
MAIN_SRC := src/main.cpp

# src files & obj files
# 将源文件src/路径下的所有后缀是.cc的文件添加文件名前缀"src/"
SRC := $(foreach x, $(SRC_PATH), $(wildcard $(addprefix $(x)/*, .cc*)))
# 将所有的源文件转换为后缀是.o的目标文件名
OBJ := $(addprefix $(OBJ_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRC)))))
# 同理生成debug的目标文件名
OBJ_DEBUG := $(addprefix $(DBG_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRC)))))

# clean files list
DISTCLEAN_LIST := $(OBJ) \
				  $(OBJ_DEBUG)
CLEAN_LIST := $(TARGET) \
				  $(TARGET_DEBUG) \
				  $(DISTCLEAN_LIST)

# default rule
default: all

# non-phony targets
$(TARGET):$(OBJ)
	$(CC) $(CCFLAG) -o $@ $? 2>errorlog.txt

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.cc*
	$(CC) $(CCOBJFLAG) -o $@ $< $(LIBS) 2>errorlog.txt

$(DBG_PATH)/%.o: $(SRC_PATH)/%.cc*
	$(CC) $(CCOBJFLAG) $(DBGFLAG) -o $@ $< $(LIBS) 2>errorlog.txt

$(TARGET_DEBUG): $(OBJ_DEBUG)
	$(CC) $(CCFLAG) $(DBGFLAG) $? -o $@ 2>errorlog.txt

# phony rules
.PHONY: all debug clean distclean
all: $(TARGET)

debug: $(TARGET_DEBUG)

clean:
	@echo CLAEN $(CLEAN_LIST)
	@rm -f $(CLEAN_LIST)

distclean:
	@echo CLAEN $(DISTCLEAN_LIST)
	@rm -f $(DISTCLEAN_LIST)