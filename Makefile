TARGET = invaders

CC = gcc
CFLAGS = -Wall -Wextra -Werror

LINKER = gcc
LFLAGS = -Wall -Wextra -Werror
LFLAGS += -L /usr/local/lib/ -L lib/ -l SDL2 -l SDL2_mixer -l SDL2_image

SRCDIR   = src
OBJDIR   = obj
BINDIR   = bin

SOURCES  := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJ := $(wildcard $(OBJDIR)/*.o)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

default: release
all: $(BINDIR)/$(TARGET)

debug: CFLAGS += -O0 -g -I include/
debug: all
release: CFLAGS += -O3 -I include/
release: all

$(BINDIR)/$(TARGET): $(OBJECTS)
	$(LINKER) $(OBJECTS) $(LFLAGS) -o $@

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm $(OBJ)
