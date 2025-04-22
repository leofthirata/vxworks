
VX_TARGET_TYPE=LKM

#BuildTool flags
ifeq ($(DEBUG_MODE),1)
DEBUGFLAGS_C-Compiler = -gdwarf-3
else
DEBUGFLAGS_C-Compiler =  -O2
endif

all :

	wr-c++ -lkm ./src/dkm.cpp $(DEBUGFLAGS_C-Compiler) -o "app.o"
# wr-cc -lkm ./src/dkm.c $(DEBUGFLAGS_C-Compiler) -o "app.o"

clean :
	rm -f vxworks-work.o
