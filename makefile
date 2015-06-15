
CPP       = g++
# compiling flags here
FLAGS   = -O2

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
OBJECTS  := obj/ConsensusProblem.o obj/Node.o obj/ProgressBar.o obj/RNA.o\
obj/RNAConsensus.o obj/RngStream.o obj/Solution.o obj/SolutionNode.o\
obj/SolverGA.o obj/SolverMC.o obj/Tree.o obj/MCCONS.o obj/OptionParser.o\
obj/Solver.o

# executable specific objects
MCOBJECTS := obj/MCCONS_MC.o
GAOBJECTS := obj/MCCONS_GA.o
#EXACTOBJECTS := obj/MCCONS_EXACT.o

rm       = rm -f

all: $(BINDIR)/mccons_mc $(BINDIR)/mccons_ga

$(BINDIR)/mccons_mc: $(OBJECTS) $(MCOBJECTS)
	@$(LINKER) $@ $(LFLAGS) $(OBJECTS) $(MCOBJECTS)
# 	@echo "Monte Carlo version complete!"

$(BINDIR)/mccons_ga: $(OBJECTS) $(GAOBJECTS)
	@$(LINKER) $@ $(LFLAGS) $(OBJECTS) $(GAOBJECTS)
# 	@echo "Genetic Algorithm version complete!"

# $(BINDIR)/mccons_exact: $(OBJECTS) $(EXACTOBJECTS)
# 	@$(LINKER) $@ $(LFLAGS) $(OBJECTS) $(EXACTOBJECTS)
#	@echo "Exact Algorithm version complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@$(CPP) $(FLAGS) -c $< -o $@
# 	@echo "Compiled "$<" successfully!"

$(MCOBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@$(CPP) $(FLAGS) -c $< -o $@
# 	@echo "Compiled "$<" successfully!"

$(GAOBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@$(CPP) $(FLAGS) -c $< -o $@
# 	@echo "Compiled "$<" successfully!"


.PHONEY: clean
clean:
	@$(rm) $(OBJECTS) $(MCOBJECTS) $(GAOBJECTS)
	@$(rm) $(BINDIR)/*

