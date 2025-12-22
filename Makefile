CXX = g++
BUILD = ./build
DLL_FLAGS   = -shared -fPIC
CXX_FLAGS   = -static
DEBUG_FLAGS = -g -Wall -Wextra 


SRCS        = test.cpp slots.cpp
OBJS        = $(SRCS:%=$(BUILD)/%.o)
TARGETS     = $(BUILD)/test.exe $(BUILD)/slots.dll

HEADERS     = json.hpp ret_struct.hpp


all: mkdir $(TARGETS)
	@echo "Build complete"

$(BUILD)/%.cpp.o: %.cpp $(HEADERS)
	$(CXX) $(CXX_FLAGS) -c $< -o $@

$(BUILD)/slots.dll: $(BUILD)/slots.cpp.o
	$(CXX) $(DLL_FLAGS) -o $@ $^ $(CXX_FLAGS)

$(BUILD)/test.exe: $(BUILD)/test.cpp.o
	$(CXX) -o $@ $^ $(CXX_FLAGS)

mkdir:
	mkdir -p $(BUILD)

clean:
	rm -rf $(BUILD)

.PHONY: clean debug all mkdir run drun

run:
	$(BUILD)/test

drun:
	gdb $(BUILD)/test

debug:
	@$(MAKE) CXX_FLAGS="$(CXX_FLAGS) $(DEBUG_FLAGS)"
