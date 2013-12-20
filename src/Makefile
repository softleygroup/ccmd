CC = icpc
# EXECUTABLE:= ../bin/linux/libccmd.a 
EXECUTABLE:= ../bin/linux/ccmd 

C_SRCS := $(wildcard *.c)
CXX_SRCS := $(wildcard *.cpp)
# program_CU_SRCS := $(wildcard *.cu)
C_OBJS := ${C_SRCS:.c=.o}
CXX_OBJS := ${CXX_SRCS:.cpp=.o}
# program_CU_OBJS := ${program_CU_SRCS:.cu=.o}
OBJS := $(C_OBJS) $(CXX_OBJS)

BUILD_DIR := ../build/linux/
OBJS := $(addprefix $(BUILD_DIR), $(OBJS))
VPATH = ./ 

BOOST_INCLUDE = /u/tps/crennick/boost_1_55_0

INCLUDE_DIRS := $(BOOST_INCLUDE) ./include
LIBRARY_DIR := -L/u/tps/crennick/boost_1_55_0/stage/lib/
LIBRARIES := -lboost_system -lboost_thread -lpthread -lrt -lpng -lz

CPPFLAGS += $(foreach includedir,$(INCLUDE_DIRS),-I$(includedir))
# CPPFLAGS += -I$(CUTIL_LIB)
#CPPFLAGS += -O3 -Wall -fPIC -c
CPPFLAGS += -O3 -std=gnu++98

LIBRARY_PATH = $(foreach librarydir,$(LIBRARIES),$(librarydir))

.PHONY: all clean distclean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
#	ar rc $@ $^  
	$(CC) -o $@ $^ -static $(LIBRARY_DIR) $(LIBRARIES)

$(BUILD_DIR)%.o : %.cpp
	$(CC) -c $< -o $@ $(CPPFLAGS)

# $(BUILD_DIR)bodysystemcuda.o: bodysystemcuda.cu bodysystemcuda.cpp include/bodysystemcuda.h
# 	$(NVCC_PATH)/nvcc -O3 --include-path $(CUTIL_LIB) -c bodysystemcuda.cu -o $(BUILD_DIR)bodysystemcuda.o 

clean:
	@- $(RM) $(program_NAME)
	@- $(RM) $(program_OBJS)

distclean: clean
