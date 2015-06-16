#include "../include/MCCONS.h"
#include "../include/OptionParser.h"
#include "../include/SolverMC.h"

using optparse::OptionParser;


int main(int argc, char *argv[])
{
    OptionParser parser = OptionParser().description("MC-Cons Consensus Optimizer Using a Monte Carlo and Steepest Descent");
    parser.add_option("-f", "--data").dest("data_file").help("path to MARNA-like input file");
    parser.add_option("-n", "--num_trials").dest("num_trials").help("number of trials").type("size_t");
    parser.add_option("-s", "--silent").action("store_true").dest("silent").help("don't display status to stderr");
    optparse::Values options = parser.parse_args(argc, argv);


    // GLOBAL SETTINGS (MODIFY AT YOUR OWN RISKS (WHICH ARE MINIMAL))
    unsigned long SEEDS[6] = {42, 42, 42, 42, 42, 42};

    // extract parameters and lauch the genetic algorithm (either silent or not)
    if (options.is_set("data_file") && options.is_set("num_trials"))
    {
      // extract command line parameters
      bool silent = false;
      if (options["silent"] == "1")
      {
          silent = true;
      }
      std::string path = options["data_file"];
      size_t num_trials = atoi(options["num_trials"].c_str());

      // instantiate the genetic algorithm solver
      Solver* solver = new SolverMC(num_trials, silent);

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