// MCCONS using a genetic algorithm solver

#include "../include/MCCONS.h"
#include "../include/SolverGA.h"
#include "../include/OptionParser.h"


using optparse::OptionParser;


int main(int argc, char *argv[])
{

    // create the command line parser
    OptionParser parser = OptionParser().description("MC-Cons Consensus Optimizer Using a Genetic Algorithm");
    parser.add_option("-f", "--data").dest("dataFile").help("path to MARNA-like input file");
    parser.add_option("-p", "--popsize").dest("popSize").help("genetic algorithm population size").type("size_t");
    parser.add_option("-n", "--numgen").dest("numGenerations").type("size_t").help("genetic algorithm number of generations");
    parser.add_option("-s", "--silent").action("store_true").dest("silent").help("don't display status to stderr");
    optparse::Values options = parser.parse_args(argc, argv);
    std::vector<std::string> args = parser.args();


    // GLOBAL SETTINGS (MODIFY AT YOUR OWN RISKS (WHICH ARE MINIMAL))
    unsigned long SEEDS[6] = {42, 42, 42, 42, 42, 42};
//     assert((SEEDS != {0, 0, 0, 0, 0, 0}) &&
//            "Invalid values for the seeds of the Pseudo-Random Number Generator");

    int IMPROVEMENT_DEPTH= 2;
    int ELITE_SIZE = 30;
    double CROSSOVER_PROBABILITY = 0.5;
    double MUTATION_PROBABILITY = 0.05;
    double IMPROVEMENT_PROBABILITY = 0.05;


    if (options.is_set("dataFile") && options.is_set("popSize") && options.is_set("numGenerations"))
    {
      // extract command line parameters
      bool silent = false;
      if (options["silent"] == "1")
      {
          silent = true;
      }

      std::string path = options["dataFile"];
      size_t population_size = atoi(options["popSize"].c_str());
      size_t num_generations = atoi(options["numGenerations"].c_str());

      // instantiate the genetic algorithm solver
      Solver* solver = new SolverGA(silent,
                                    population_size,
                                    num_generations,
                                    IMPROVEMENT_DEPTH,
                                    ELITE_SIZE,

                                    CROSSOVER_PROBABILITY,
                                    MUTATION_PROBABILITY,
                                    IMPROVEMENT_PROBABILITY);

      // execute and cleanup
      MCCONS(path, solver, SEEDS);
      delete solver;
    } else
    {
      // something went wrong with the arguemnts, print error message and exit
      std::cerr << "Error: something went wrong, please check usage -h or --help" << std::endl;
      std::exit(0);
    }
}
