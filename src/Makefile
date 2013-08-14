program_NAME := ../bin/linux/libccmd.a 
program_C_SRCS := $(wildcard *.c)
program_CXX_SRCS := $(wildcard *.cpp)
program_CU_SRCS := $(wildcard *.cu)
program_C_OBJS := ${program_C_SRCS:.c=.o}
program_CXX_OBJS := ${program_CXX_SRCS:.cpp=.o}
program_CU_OBJS := ${program_CU_SRCS:.cu=.o}
program_OBJS := $(program_C_OBJS) $(program_CXX_OBJS)

BUILD_DIR := ../build/linux/
program_OBJS := $(addprefix $(BUILD_DIR), $(program_OBJS))
VPATH = ./ 

NVCC_PATH = /usr/local/cuda/bin
CUTIL_LIB = /export/home/tps24/mtb/NVIDIA_GPU_Computing_SDK/C/common/inc
CUDA_INCLUDE = /usr/local/cuda/include
CUDA_LIB = /usr/local/cuda/lib
BOOST_INCLUDE = /home/tps24/mtb/boost_1_49_0

program_INCLUDE_DIRS := $(CUDA_INCLUDE) $(BOOST_INCLUDE) ./include
program_LIBRARY_DIRS := $(CUDA_LIB) 
program_LIBRARIES := cuda cudart

CPPFLAGS += $(foreach includedir,$(program_INCLUDE_DIRS),-I$(includedir))
CPPFLAGS += -I$(CUTIL_LIB)
#CPPFLAGS += -O3 -Wall -fPIC -c
CPPFLAGS += -O3 -fPIC -c

#LDFLAGS += $(foreach librarydir,$(program_LIBRARY_DIRS),-L$(librarydir))
#LDFLAGS += $(foreach library,$(program_LIBRARIES),-l$(library))

.PHONY: all clean distclean

all: $(program_NAME)

$(program_NAME): $(program_OBJS)
	ar -rcs $@ $^

$(BUILD_DIR)%.o : %.cpp
	$(CXX) $< -o $@ $(CPPFLAGS) $(LDFLAGS)

$(BUILD_DIR)bodysystemcuda.o: bodysystemcuda.cu bodysystemcuda.cpp include/bodysystemcuda.h
	$(NVCC_PATH)/nvcc -O3 --include-path $(CUTIL_LIB) -c bodysystemcuda.cu -o $(BUILD_DIR)bodysystemcuda.o 

clean:
	@- $(RM) $(program_NAME)
	@- $(RM) $(program_OBJS)

distclean: clean
