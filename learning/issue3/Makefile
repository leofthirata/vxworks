VX_TARGET_TYPE=RTP

#BuildTool flags
ifeq ($(DEBUG_MODE),1)
DEBUGFLAGS = -gdwarf-3
else
DEBUGFLAGS =  -O2
endif

ifeq ($(CODE_COVERAGE),1)
    CPPFLAGS += -fprofile-arcs  -ftest-coverage -mllvm -gcov-const-priority=50
    CFLAGS   += -fprofile-arcs  -ftest-coverage -mllvm -gcov-const-priority=50
    LDFLAGS  += -lllvm_cov
endif

TARGET = issue3.vxe
C_SRC = $(wildcard src/*.c)                             # list of c source files
C_OBJS = $(patsubst src/%.c, obj/%.o, $(C_SRC))       # list of c object files
CXX_SRC = $(wildcard src/*.cpp)                         # list of c++ source files
CXX_OBJS = $(patsubst src/%.cpp, obj/%.o, $(CXX_SRC)) # list of c++ object files
ASM_SRC = $(wildcard src/*.s)                           # list of assemble source files
ASM_OBJS = $(patsubst src/%.s, obj/%.o, $(ASM_SRC))   # list of assemble object files

DEPS = $(wildcard ./obj/*.d)                              # dependency files if any
-include $(DEPS)

AS = wr-cc
CC = wr-cc
CXX = wr-c++
CFLAGS   += -MD
# CFLAGS   += -I$(WIND_SDK_CC_SYSROOT)/usr/3pp/develop/usr/include/opencv4
CPPFLAGS += -MD
LDFLAGS  += -static

.DEFAULT: all

all :: mkobjdir $(TARGET)
	@echo "Done"

mkobjdir :
	mkdir -p ./obj/c ./obj/cxx ./obj/asm

$(TARGET) : $(C_OBJS) $(CXX_OBJS) $(ASM_OBJS)
	$(CXX) $^ $(LDFLAGS) -o $@

./obj/%.o: ./src/%.c
	@echo "- Compiling c source files --- $<"
	$(CC) -c $(CFLAGS) $(DEBUGFLAGS) $(INCLUDES) -o $@ -c $<

./obj/%.o: ./src/%.cpp
	@echo "- Compiling cpp source files --- $<"
	$(CXX) -c $(CPPFLAGS) $(DEBUGFLAGS) $(INCLUDES) -o $@ -c $<

./obj/%.o: ./src/%.s
	@echo "- Assembling source file --- $<"
	$(CC) $(ASFLAGS) -o $@ -c $<

.PHONY: clean
clean :
	rm -rf ./obj $(TARGET)