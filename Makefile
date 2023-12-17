# Makefile

CC := g++
CFLAGS := -std=c++11 -Wall -Iincludes -lSDL2 -lSDL2_ttf -lSDL2_mixer

SRCDIR := src
INCLUDEDIR := includes
BUILDDIR := build

SRCEXT := cpp

SOURCES := $(wildcard $(SRCDIR)/*.$(SRCEXT))
OBJECTS := $(SOURCES:$(SRCDIR)/%.$(SRCEXT)=$(BUILDDIR)/%.o)

EXECUTABLE := build/mygame

$(EXECUTABLE): $(OBJECTS)
	@mkdir -p $(BUILDDIR)
	@$(CC) $^ -o $(EXECUTABLE) $(CFLAGS)
	@echo "Executable created in $(EXECUTABLE)"

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@$(CC) -c -o $@ $< $(CFLAGS)

clean:
	@rm -rf $(BUILDDIR)
