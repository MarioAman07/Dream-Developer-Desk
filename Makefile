# Makefile for the Final Project "Dream Developer Desk" (topic 28)
#
#   make         build the program (default target)
#   make clean   remove the program and the object files
#
# Supported: Windows (MinGW-w64 g++ or MSVC cl), Linux (g++), macOS (clang++).
# The packages required on each OS are listed in README.md.

PROGRAM := DreamDesk

INCLUDES  := -Isrc -Iexternal/include -Iexternal/stb
DEFINES   := -D_CRT_SECURE_NO_WARNINGS
GLAD_SRC  := external/glad/glad.c
GLFW_DIR  := external/glfw
OBJDIR    := obj

# Our code: every .cpp in src/ and its subfolders.
SRC_DIRS  := src src/core src/gfx src/render src/scene src/ui src/util
APP_SRC   := $(foreach d,$(SRC_DIRS),$(wildcard $(d)/*.cpp))
HEADERS   := $(foreach d,$(SRC_DIRS),$(wildcard $(d)/*.h))

vpath %.cpp $(SRC_DIRS)
vpath %.c   external/glad

# ---------------------------------------------------------------------------
# Operating system and compiler detection
# ---------------------------------------------------------------------------
ifeq ($(OS),Windows_NT)
    PLATFORM := windows
else ifeq ($(shell uname -s),Darwin)
    PLATFORM := macos
else
    PLATFORM := linux
endif

# On Windows MSVC is used when cl.exe is in PATH (Developer Command Prompt),
# otherwise MinGW-w64 g++. Can be forced with: make TOOLCHAIN=mingw / TOOLCHAIN=msvc
ifeq ($(PLATFORM),windows)
    ifndef TOOLCHAIN
        ifneq ($(findstring cl.exe,$(shell where cl.exe 2>&1)),)
            TOOLCHAIN := msvc
        else
            TOOLCHAIN := mingw
        endif
    endif
else
    TOOLCHAIN := gcc
endif

ifeq ($(TOOLCHAIN),msvc)
    # Options start with "-" instead of "/" so that MSYS2 / Git Bash
    # do not mistake them for file paths.
    EXE      := .exe
    OBJ      := obj
    CFLAGS   := -nologo -O2 -MD $(INCLUDES)
    CXXFLAGS := -nologo -O2 -MD -EHsc -W3 -std:c++17 -utf-8 $(INCLUDES) $(DEFINES) -I$(GLFW_DIR)/include
    LDFLAGS  := -link -nologo -LIBPATH:$(GLFW_DIR)/lib-vc
    LDLIBS   := glfw3.lib opengl32.lib user32.lib gdi32.lib shell32.lib

    COMPILE_C   = cl $(CFLAGS) -c $< -Fo$@
    COMPILE_CXX = cl $(CXXFLAGS) -c $< -Fo$@
    LINK_CXX    = cl -nologo $(ALL_OBJ) -Fe$@ $(LDFLAGS) $(LDLIBS)
else
    EXE      :=
    OBJ      := o
    CFLAGS   := -O2 $(INCLUDES)
    CXXFLAGS := -std=c++17 -O2 -Wall $(INCLUDES) $(DEFINES)

    ifeq ($(TOOLCHAIN),mingw)
        ifeq ($(origin CC),default)
            CC := gcc
        endif
        EXE      := .exe
        CXXFLAGS += -I$(GLFW_DIR)/include
        # -static: the program runs without the MinGW DLLs in PATH
        LDFLAGS  := -static -L$(GLFW_DIR)/lib-mingw
        LDLIBS   := -lglfw3 -lopengl32 -lgdi32 -luser32 -lshell32 -limm32
    else
        # Linux and macOS: GLFW installed by the system package manager
        GLFW_CFLAGS := $(shell pkg-config --cflags glfw3 2>/dev/null)
        GLFW_LIBS   := $(shell pkg-config --libs glfw3 2>/dev/null)

        ifeq ($(PLATFORM),macos)
            ifeq ($(origin CXX),default)
                CXX := clang++
            endif
            ifeq ($(GLFW_LIBS),)
                BREW_PREFIX := $(shell brew --prefix 2>/dev/null || echo /usr/local)
                GLFW_CFLAGS := -I$(BREW_PREFIX)/include
                GLFW_LIBS   := -L$(BREW_PREFIX)/lib -lglfw
            endif
            LDLIBS := $(GLFW_LIBS) -framework OpenGL
        else
            ifeq ($(GLFW_LIBS),)
                GLFW_LIBS := -lglfw
            endif
            LDLIBS := $(GLFW_LIBS) -ldl -lpthread
        endif

        CXXFLAGS += $(GLFW_CFLAGS)
    endif

    COMPILE_C   = $(CC) $(CFLAGS) -c $< -o $@
    COMPILE_CXX = $(CXX) $(CXXFLAGS) -c $< -o $@
    LINK_CXX    = $(CXX) $(ALL_OBJ) -o $@ $(LDFLAGS) $(LDLIBS)
endif

TARGET    := $(PROGRAM)$(EXE)
APP_OBJ   := $(addprefix $(OBJDIR)/,$(notdir $(APP_SRC:.cpp=.$(OBJ))))
GLAD_OBJ  := $(OBJDIR)/glad.$(OBJ)
ALL_OBJ   := $(APP_OBJ) $(GLAD_OBJ)

# ---------------------------------------------------------------------------
# Targets
# ---------------------------------------------------------------------------
.PHONY: all clean

all: $(TARGET)
	@echo "Build finished ($(PLATFORM), $(TOOLCHAIN)). Program: $(TARGET)"

$(TARGET): $(ALL_OBJ)
	$(LINK_CXX)

# Our files are rebuilt when any of our headers changes.
$(APP_OBJ): $(HEADERS)

$(OBJDIR)/%.$(OBJ): %.cpp | $(OBJDIR)
	$(COMPILE_CXX)

$(OBJDIR)/%.$(OBJ): %.c | $(OBJDIR)
	$(COMPILE_C)

$(OBJDIR):
	mkdir $(OBJDIR)

clean:
	rm -rf $(OBJDIR) $(TARGET) $(PROGRAM).exp $(PROGRAM).lib
