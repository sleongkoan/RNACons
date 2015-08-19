
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
obj/SolverHeuristic.o obj/SolverExact.o obj/Tree.o obj/MCCONS.o\
obj/OptionParser.o obj/Solver.o obj/Readers.o

# executable specific objects
EXACTOBJECTS := obj/mcconsExact.o
HEURISTICOBJECTS := obj/mcconsHeuristic.o
SHAPEEXPLORER := obj/shapeExplorer.o
rm       = rm -f

all: $(BINDIR)/mcconsHeuristic $(BINDIR)/mcconsExact $(BINDIR)/shapeExplorer


# compiling
$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@$(CPP) $(FLAGS) -c $< -o $@
$(HEURISTICOBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@$(CPP) $(FLAGS) -c $< -o $@

$(EXACTOBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@$(CPP) $(FLAGS) -c $< -o $@

$(SHAPEEXPLORER): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@$(CPP) $(FLAGS) -c $< -o $@


# linking
$(BINDIR)/mcconsHeuristic: $(OBJECTS) $(HEURISTICOBJECTS)
	@$(LINKER) $@ $(LFLAGS) $(OBJECTS) $(HEURISTICOBJECTS)

$(BINDIR)/mcconsExact: $(OBJECTS) $(EXACTOBJECTS)
	@$(LINKER) $@ $(LFLAGS) $(OBJECTS) $(EXACTOBJECTS)

$(BINDIR)/shapeExplorer: $(OBJECTS) $(SHAPEEXPLORER)
	@$(LINKER) $@ $(LFLAGS) $(OBJECTS) $(SHAPEEXPLORER)

.PHONEY: clean
clean:
	@$(rm) $(OBJECTS) $(MCOBJECTS) $(HEURISTICOBJECTS) $(EXACTOBJECTS) $(SHAPEEXPLORER)
	@$(rm) $(BINDIR)/*

