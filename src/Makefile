EXECUTABLE:= ../bin/linux/ccmd 

intel: CC = icpc
intel: LD = icpc

gnu: CC = g++
gnu: LD = g++

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


intel: INCLUDE_DIRS := $(BOOST_INCLUDE) ./include
gnu: INCLUDE_DIRS :=  ./include

intel: LIBRARY_DIR := -L/u/tps/crennick/boost_1_55_0/stage/lib/
LIBRARIES := -lboost_system-mt -lboost_thread-mt -lpthread -lpng -lz

CPPFLAGS += $(foreach includedir,$(INCLUDE_DIRS),-I$(includedir))

intel: CPPFLAGS += -O3 -std=c++11
gnu: CPPFLAGS += -Ofast -std=c++11 -march=native -ffast-math -flto
gnu: LDFLAGS += -Ofast -std=c++11 -march=native -ffast-math -flto

#LIBRARY_PATH = $(foreach librarydir,$(LIBRARIES),$(librarydir))

.PHONY: all clean distclean

gnu: $(EXECUTABLE)
intel: $(EXECUTABLE)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
#	ar rc $@ $^  
	$(CC) -o $@ $^ $(LIBRARY_DIR) $(LIBRARIES)

$(BUILD_DIR)%.o : %.cpp
	$(CC) -c $< -o $@ $(CPPFLAGS)

# $(BUILD_DIR)bodysystemcuda.o: bodysystemcuda.cu bodysystemcuda.cpp include/bodysystemcuda.h
# 	$(NVCC_PATH)/nvcc -O3 --include-path $(CUTIL_LIB) -c bodysystemcuda.cu -o $(BUILD_DIR)bodysystemcuda.o 

clean:
	@- $(RM) $(EXECUTABLE)
	@- $(RM) $(OBJS)


distclean: clean
