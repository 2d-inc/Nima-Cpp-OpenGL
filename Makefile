# Specify extensions of files to delete when cleaning
CPP_COMPILER	= clang++
CPP_FLAGS		= -Wall -g -std=c++11
# Specify the target file and the install directory
OUTPUTFILE 		= Nima-Testing
BUILD_DIR 		= Build
OBJ_DIR			= $(BUILD_DIR)/obj
CPP_SOURCES		= Main.cpp
CPP_OBJECTS		= $(CPP_SOURCES:%.cpp=$(OBJ_DIR)/%.o)
CPP_INCLUDES	= -I/usr/local/include -INima-Cpp/Build/include 
LFLAGS			= -Wl,-rpath,./ -L/usr/local/lib -LNima-Cpp/Build/lib
LIBS			= -lnima -lglfw3 -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo

# Default target
all: dirs $(OUTPUTFILE)

clean:
	$(RM) -fR $(OBJ_DIR)/Main.o

dirs:
	mkdir -p $(OBJ_DIR)
	cd Nima-Math-Cpp && make
	cd Nima-Cpp && make

# Build sources
$(OUTPUTFILE): $(CPP_OBJECTS)
	$(CPP_COMPILER) $(CPP_FLAGS) $(CPP_INCLUDES) -o $(BUILD_DIR)/$(OUTPUTFILE) $(CPP_OBJECTS) $(LFLAGS) $(LIBS)

$(OBJ_DIR)/%.o: Source/%.cpp
	$(CPP_COMPILER) $(CPP_FLAGS) $(CPP_INCLUDES) -c $< -o $@