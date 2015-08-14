// MCCONS using a genetic algorithm solver

#include "../include/MCCONS.h"
#include "../include/SolverGA.h"
#include "../include/OptionParser.h"


using optparse::OptionParser;


int main(int argc, char *argv[])
{

    // create the command line parser
    OptionParser parser = OptionParser().description("MC-Cons Consensus Optimizer Using a Genetic Algorithm");

    // I/O settings
    parser.add_option("-i", "--data").dest("dataFile").help("path to MARNA-like input file");
    parser.add_option("-s", "--silent").action("store_true").dest("silent").help("don't display status to stderr");

    // heuristics parameters
    parser.add_option("-p", "--popsize").dest("popSize").help("genetic algorithm population size").type("size_t");
    parser.add_option("-n", "--numgen").dest("numGenerations").type("size_t").help("genetic algorithm number of generations");

    // threshold on tree consensus
    parser.add_option("-t", "--threshold").dest("threshold").type("size_t").help("consider up to t additional score for tree consensus (vs optiomal one)");

    // prng seeds
    parser.add_option("--seed0").dest("seed0").type("long").help("seed 0 used by the prng");
    parser.add_option("--seed1").dest("seed1").type("long").help("seed 1 used by the prng");
    parser.add_option("--seed2").dest("seed2").type("long").help("seed 2 used by the prng");
    parser.add_option("--seed3").dest("seed3").type("long").help("seed 3 used by the prng");
    parser.add_option("--seed4").dest("seed4").type("long").help("seed 4 used by the prng");
    parser.add_option("--seed5").dest("seed5").type("long").help("seed 5 used by the prng");


//     parser.add_option("--sedd2").dest("seed2").type("unsigned)
    optparse::Values options = parser.parse_args(argc, argv);


    // GLOBAL SETTINGS (MODIFY AT YOUR OWN RISKS (WHICH ARE MINIMAL))
    unsigned long SEEDS[6] = {42, 42, 42, 42, 42, 42};

    if (options.is_set("seed0"))
    {
        SEEDS[0] = atoi(options["seed0"].c_str());
    }
    if (options.is_set("seed1"))
    {
        SEEDS[1] = atoi(options["seed1"].c_str());
    }
    if (options.is_set("seed2"))
    {
        SEEDS[2] = atoi(options["seed2"].c_str());
    }
    if (options.is_set("seed3"))
    {
        SEEDS[3] = atoi(options["seed3"].c_str());
    }
    if (options.is_set("seed4"))
    {
        SEEDS[4] = atoi(options["seed4"].c_str());
    }
    if (options.is_set("seed5"))
    {
        SEEDS[5] = atoi(options["seed5"].c_str());
    }


    // check that the seeds aren't all zeros, because the generator stays in the same
    // state if that happens
    int num_zeros = 0.;
    for (size_t i = 0; i != 6; ++i)
    {
        if (SEEDS[i] == 0)
        {
            num_zeros += 1;
        }
    }
    if (num_zeros == 6)
    {
        std::cerr << "The seeds of the pseudorandom number generator must not be all zeros." << std::endl;
        std::exit(0);
    }

    int IMPROVEMENT_DEPTH= 2;
    int ELITE_SIZE = 30;
    double CROSSOVER_PROBABILITY = 0.5;
    double MUTATION_PROBABILITY = 0.05;
    double IMPROVEMENT_PROBABILITY = 0.05;
    double SUBOPTIMAL_THRESHOLD = 0;
    size_t POPULATION_SIZE = 250;
    size_t NUM_GENERATIONS = 250;
    bool silent = false;


    if (options.is_set("dataFile"))
    {
      // extract command line parameters
      if (options["silent"] == "1")
      {
          silent = true;
      }

      if (options.is_set("popSize"))
      {
          POPULATION_SIZE = atoi(options["popSize"].c_str());
      }

      if (options.is_set("numGenerations"))
      {
          NUM_GENERATIONS = atoi(options["numGenerations"].c_str());
      }

      if (options.is_set("threshold"))
      {
          SUBOPTIMAL_THRESHOLD = atof(options["threshold"].c_str());
      }


      std::string path = options["dataFile"];

      // instantiate the genetic algorithm solver
      // tree solver, will consider suboptimal consensus
      Solver* tree_solver = new SolverGA(silent,
                                         POPULATION_SIZE,
                                         NUM_GENERATIONS,
                                         IMPROVEMENT_DEPTH,
                                         ELITE_SIZE,
                                         SUBOPTIMAL_THRESHOLD, // specified subopt threshold for trees

                                         CROSSOVER_PROBABILITY,
                                         MUTATION_PROBABILITY,
                                         IMPROVEMENT_PROBABILITY);

      Solver* dot_bracket_solver = new SolverGA(silent,
                                                POPULATION_SIZE,
                                                NUM_GENERATIONS,
                                                IMPROVEMENT_DEPTH,
                                                ELITE_SIZE,
                                                0.,  // no threshold for this one

                                                CROSSOVER_PROBABILITY,
                                                MUTATION_PROBABILITY,
                                                IMPROVEMENT_PROBABILITY);

      // execute and cleanup
      MCCONS(path, tree_solver, dot_bracket_solver, SEEDS);
      delete tree_solver;
      delete dot_bracket_solver;

    } else
    {
      // something went wrong with the arguemnts, print error message and exit
      std::cerr << "Error: something went wrong, please check usage -h or --help" << std::endl;
      std::exit(0);
    }
}
