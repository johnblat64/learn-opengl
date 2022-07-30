CXX = clang++
CC = clang


BUILD_DIR := .build

DEPDIR := .deps
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.cpp.d

CXXFLAGS = -std=c++11 -Wno-expansion-to-defined -D_GLIBCXX_DEBUG -fno-limit-debug-info
CFLAGS = -std=gnu99

GAME_CXX_SOURCES = $(wildcard src/*.cpp)
GAME_CXX_OBJ_FILES := $(GAME_CXX_SOURCES:src/%.cpp=$(BUILD_DIR)/%.cpp.o)
$(info GAME FILES)
$(info =============)
$(info game src files: $(GAME_CXX_SOURCES))
$(info )
$(info game obj files: $(GAME_CXX_OBJ_FILES))
$(info =============)

GAME_C_SOURCES = $(wildcard src/*.c)
GAME_C_OBJ_FILES := $(GAME_C_SOURCES:src/%.c=$(BUILD_DIR)/%.c.o)
$(info GAME FILES)
$(info =============)
$(info game src files: $(GAME_C_SOURCES))
$(info )
$(info game obj files: $(GAME_C_OBJ_FILES))
$(info =============)



LFLAGS = -Llib
IFLAGS = -Iinclude -Iinclude/SDL2 -Iinclude/glad

LIBS = -lSDL2 -lSDL2main -Lm -ldl -Wl,-rpath,lib





game: $(GAME_CXX_OBJ_FILES) $(GAME_C_OBJ_FILES)
	$(CXX) $(CXXFLAGS) $(LFLAGS) $(LIBS) -g $^ -o $@



$(BUILD_DIR)/%.c.o: src/%.c 
$(BUILD_DIR)/%.c.o: src/%.c $(DEPDIR)/%.c.d | $(DEPDIR) $(BUILD_DIR)
	$(CC) $(DEPFLAGS) $(IFLAGS) -c -g $< -o $@

$(BUILD_DIR)/%.cpp.o: src/%.cpp 
$(BUILD_DIR)/%.cpp.o: src/%.cpp $(DEPDIR)/%.cpp.d | $(DEPDIR) $(BUILD_DIR)
	$(CXX) $(DEPFLAGS) $(CXXFLAGS) $(IFLAGS) -c -g $< -o $@

# needed directories for build process
$(DEPDIR):
	mkdir -p $@

$(BUILD_DIR):
	mkdir -p $@



DEPFILES := $(GAME_CXX_SOURCES:src/%.cpp=$(DEPDIR)/%.cpp.d)

DEPFILES += $(GAME_C_SOURCES:src/%.c=$(DEPDIR)/%.c.d)

$(DEPFILES):

include $(wildcard $(DEPFILES))

# #.PHONY: clean 
clean:
	rm -f src/game/*.o
	rm -f src/shared/*.o
	rm -f src/levelEditor/*.o 
	rm -f build/game/*.o
	rm -f build/shared/*.o
	rm -f build/levelEditor/*.o