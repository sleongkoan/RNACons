
CPP       = g++
# compiling flags here
FLAGS   = -O2 -Wunused -Wunreachable-code -std=c++98

LINKER   = g++ -o
# linking flags here
LFLAGS   = -Wall -I.

# change these to set the proper directories where each files shoould be
SRCDIR   = src
INCLUDEDIR = include
OBJDIR   = obj
BINDIR   = bin

SOURCES  := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(INCLUDEDIR)/*.h)

# the objects
OBJECTS  := obj/RNA2D.o obj/ConsensusProblem.o obj/Distances.o obj/ProgressBar.o\
obj/RngStream.o obj/Solution.o obj/SubSolution.o\
obj/SolverGA.o obj/SolverExact.o obj/Tree.o obj/MCCONS.o\
obj/OptionParser.o obj/Solver.o obj/Readers.o

# executable specific objects
GAOBJECTS := obj/MCCONS_GA.o
EXACTOBJECTS := obj/MCCONS_EXACT.o

rm       = rm -f

all: $(BINDIR)/mccons_ga $(BINDIR)/mccons_exact


# compiling
$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@$(CPP) $(FLAGS) -c $< -o $@

$(GAOBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@$(CPP) $(FLAGS) -c $< -o $@

$(EXACTOBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@$(CPP) $(FLAGS) -c $< -o $@


# linking
$(BINDIR)/mccons_ga: $(OBJECTS) $(GAOBJECTS)
	@$(LINKER) $@ $(LFLAGS) $(OBJECTS) $(GAOBJECTS)

$(BINDIR)/mccons_exact: $(OBJECTS) $(EXACTOBJECTS)
	@$(LINKER) $@ $(LFLAGS) $(OBJECTS) $(EXACTOBJECTS)


.PHONEY: clean
clean:
	@$(rm) $(OBJECTS) $(MCOBJECTS) $(GAOBJECTS) $(EXACTOBJECTS)
	@$(rm) $(BINDIR)/*

