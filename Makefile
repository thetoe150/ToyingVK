CC=g++
 
CFLAGS=-Wall -std=c++17 -IC:\VulkanSDK\1.3.268.0\Include -Iinc -Iinc/imgui -Itracy/public/tracy
LFLAGS=-Wall -std=c++17 -Llib -lglfw3dll -LC:/VulkanSDK/1.3.268.0/Lib -lvulkan-1

SRC_FILES=$(wildcard src/*.cpp)
OBJ_FILES=$(patsubst src/%, obj/%, $(patsubst %.cpp, %.o, $(SRC_FILES)))
IMGUI_SRC_FILES=$(wildcard src/imgui/*.cpp)
IMGUI_OBJ_FILES=$(patsubst src/imgui/%, obj/imgui/%, $(patsubst %.cpp, %.o, $(IMGUI_SRC_FILES)))
TRACY_OBJ=obj/tracy/TracyClient.o
TRACY_SRC=tracy/public/TracyClient.cpp
# HEADER_ONLY_FILES
#
EXE=bin/main

debug: CFLAGS += -DDEBUG -DTRACY_ENABLE -O0 -ggdb -D_WIN32_WINNT=0x0602 -DWINVER=0x0602 -DTRACY_VK_USE_SYMBOL_TABLE
debug: LFLAGS += -DDEBUG -DTRACY_ENABLE -O0 -ggdb -lws2_32 -limagehlp
debug: OBJ_FILES += $(IMGUI_OBJ_FILES) 
debug: $(EXE)
	./$(EXE)

release: $(EXE)
	./$(EXE)

$(EXE): $(OBJ_FILES) $(IMGUI_OBJ_FILES) $(TRACY_OBJ)
	$(CC) $^ -o $@ $(LFLAGS)

obj/%.o: src/%.cpp inc/%.hpp
	$(CC) -c $< -o $@ $(CFLAGS)

obj/imgui/%.o: src/imgui/%.cpp
	$(CC) -c $< -o $@ $(CFLAGS)

$(TRACY_OBJ): $(TRACY_SRC) tracy/public/tracy/Tracy.hpp
	$(CC) -c $< -o $@ $(CFLAGS)
