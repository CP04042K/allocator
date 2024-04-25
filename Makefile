WORKING_DIRECTORY = /home/shin24/project/allocator_and_gc
INC = -I$(WORKING_DIRECTORY)/includes
CXX := g++ 
CXXFLAGS := -Wall -Wextra -std=c++11 $(INC)
SRCDIR := $(WORKING_DIRECTORY)/src
OBJDIR := $(WORKING_DIRECTORY)/objs
SRCS := $(wildcard $(SRCDIR)/*.cpp)
OBJS := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))

# Default rule
all: $(OBJS)

# Rule to compile object files

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

build:
	$(CXX) $(OBJS) $(CXXFLAGS) main.cpp

# Clean rule
clean:
	rm -f $(OBJDIR)/*.o
