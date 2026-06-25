# Compiler and flags
CXX      := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -Wpedantic -g
LDFLAGS  := -pthread

# Target executable name
TARGET   := main 

# Find all .cpp files in the current directory
SRCS     := main.cpp NetworkListener.cpp
# Generate .o object file names for each .cpp file
OBJS     := $(SRCS:.cpp=.o)
# Generate .d dependency file names
DEPS     := $(SRCS:.cpp=.d)

# Default rule (runs when you just type 'make')
all: $(TARGET)

# Link object files to create the final executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compile .cpp files into .o files
# Also generates .d files to track header dependencies automatically
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

# Include the dependency files if they exist
-include $(DEPS)

# Clean up build artifacts
.PHONY: clean
clean:
	rm -f $(OBJS) $(DEPS) $(TARGET)
