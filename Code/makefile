CC                := gcc
OBJDIR            := obj/
DEPDIR            := deps/
SRCDIR            := src/
STD               := gnu11
INCLUDEDIRS       := -I./src/front_end -I./src/back_end -I./src/objects -I./src/objects/material -I./src/common/ -I./src/back_end/intersections/ -I ./src/back_end/photon/
CFLAGS            := --std=$(STD) -Wall -D__STDC_FORMAT_MACROS -D_USE_MATH_DEFINES=1 $(INCLUDEDIRS) $(shell sdl2-config --cflags)
LIBS              := -lm -lpthread -lGL $(shell sdl2-config --libs)
GUI               := 1
PMAP              := 1
OUTPUT_PHOTON_MAP := 1
VERBOSE           := 1
SANITIZE          := 0

CFLAGS += -DSINGLE_THREAD

ifeq ($(SANITIZE), 1)
	CFLAGS  += -fsanitize=address -fno-omit-frame-pointer
endif

ifeq ($(ASM), 1)
	CFLAGS += -S
endif

###################################
#     Photon mapping defines.     #
###################################
FAST_DIFFUSE      := 1
CAUSTICS          := 0

ifeq ($(FAST_DIFFUSE), 1)
	CFLAGS += -DFAST_DIFFUSE
endif

ifeq ($(CAUSTICS), 1)
	CFLAGS += -DCAUSTICS
endif

#Set up the BUILD_MODE variable so that if the mode changes then
#it is picked up and a clean build is performed
ifeq ($(DEBUG), 1)
	CFLAGS += -DDEBUG -O0 -g
	CFLAGS += -Wno-unused-variable
	CFLAGS += -Wno-unused-function
	BUILD_MODE = debug_mode
else ifeq ($(REL), 1)
	CFLAGS += -DNDEBUG -O3 -Werror
	BUILD_MODE = release_mode
else ifeq ($(RELDEBUG), 1)
	CFLAGS += -O3 -Werror -g
else
	CFLAGS += -g -DDEBUG
	CFLAGS += -Wno-unused-variable
	CFLAGS += -Wno-unused-function
	BUILD_MODE = normal_mode
endif

ifeq ($(VERBOSE), 1)
	CFLAGS  += -DOUTPUT_PROGRESS
	CFLAGS  += -DVERBOSE_OUTPUT
endif

ifeq ($(GUI), 1)
	CFLAGS += -DGUI
endif

ifeq ($(BRUTEFORCE), 1)
	CFLAGS += -DBRUTEFORCE
endif

ifeq ($(PROFILE), 1)
	CFLAGS += -pg -O3
endif

ifeq ($(PMAP), 1)
	CFLAGS += -DPMAP
endif

ifeq ($(OUTPUT_PHOTON_MAP), 1)
	CFLAGS += -DOUTPUT_PMAP
endif

SRC   := $(shell find $(SRCDIR) -name *.c)
OBJS  := $(patsubst $(SRCDIR)%.c, $(OBJDIR)%.o, $(SRC))
DEPS  := $(patsubst $(SRCDIR)%.c, $(DEPDIR)%.d, $(SRC))

all: raytracer

raytracer: $(OBJS)
	@$(CC) $^ $(CFLAGS) $(LIBS) -o $@

mkdirs: $(BUILD_MODE)
	@mkdir -p $(OBJDIR) $(DEPDIR)



-include $(DEPS)

$(OBJDIR)%.o: $(SRCDIR)%.c
	@echo "Compiling $<"
	@mkdir -p $(OBJDIR) $(DEPDIR) $(@D) $(patsubst $(OBJDIR)%, $(DEPDIR)%, $(@D))
	@$(CC) $< $(CFLAGS) -c -o $@ -MMD -MF $(DEPDIR)$*.d

%_mode: %.mode
	@:

%_mode: clean
	@touch $*.mode

run: all
	./raytracer

time: all
	time ./raytracer

clean:
	@echo "Cleaning up."
	@rm -rf *.bmp *.pmap *.mode $(OBJDIR) $(DEPDIR) raytracer

log: all
	./raytracer > log.txt

val: all
	valgrind ./raytracer

kview: all
	./raytracer
	./kdview.py
