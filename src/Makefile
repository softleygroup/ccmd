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

BUILD_DIR := ../build/
OBJS := $(addprefix $(BUILD_DIR), $(OBJS))
VPATH = ./ 
EXECUTABLE:= ../bin/ccmd 

BOOST_INCLUDE = /system/software/linux-x86_64/lib/boost/1_58_0/include

intel: INCLUDE_DIRS := $(BOOST_INCLUDE) ./include
gnu: INCLUDE_DIRS :=  ./include /opt/local/include C:\MinGW\boost_1_58_0 C:\MinGW\lib\gcc\mingw32\4.8.1\include C:\mingw\include\libpng C:\mingw\dcmt0.6.2\include /system/software/linux-x86_64/lib/boost/1_58_0/include


intel: LIBRARY_DIR := -LC:\MinGW\mingw32\lib\gcc\mingw32\4.8.1 C:\mingw\include\libpng
gnu: LIBRARY_DIR := -LC:\mingw\include\libpng -LC:\mingw\dcmt0.6.2\lib
LIBRARIES := -lpng -lz

CPPFLAGS += $(foreach includedir,$(INCLUDE_DIRS),-I$(includedir))

intel: CPPFLAGS += -O3 -std=c++11
gnu: CPPFLAGS +=  -Ofast -std=c++11 -ffast-math -m64 -funroll-loops
gnu: LDFLAGS +=  -Ofast -std=c++11 -ffast-math -m64 -funroll-loops
gnu: CFLAGS += -fopenmp

#LIBRARY_PATH = $(foreach librarydir,$(LIBRARIES),$(librarydir))

.PHONY: all clean distclean

gnu: $(EXECUTABLE)
intel: $(EXECUTABLE)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
#	ar rc $@ $^  
	$(CC) $(CFLAGS) -o $@ $^ $(LIBRARY_DIR) $(LIBRARIES)

$(BUILD_DIR)%.o : %.cpp 
	$(CC) $(CFLAGS) -c $< -o $@ $(CPPFLAGS)

# $(BUILD_DIR)bodysystemcuda.o: bodysystemcuda.cu bodysystemcuda.cpp include/bodysystemcuda.h
# 	$(NVCC_PATH)/nvcc -O3 --include-path $(CUTIL_LIB) -c bodysystemcuda.cu -o $(BUILD_DIR)bodysystemcuda.o 

#clean:
#    $(info $$EXECUTABLE is [${EXECUTABLE}])#
#	@- $(RM) $(EXECUTABLE)
#	@- $(RM) $(OBJS)

#distclean: clean
