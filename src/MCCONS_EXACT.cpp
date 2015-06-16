
#include "../include/MCCONS.h"
#include "../include/SolverExact.h"
#include "../include/OptionParser.h"


using optparse::OptionParser;


int main(int argc, char *argv[])
{
    // GLOBAL SETTINGS (MODIFY AT YOUR OWN RISKS (WHICH ARE MINIMAL))
    unsigned long SEEDS[6] = {42, 42, 42, 42, 42, 42};


    // create the command line parser
    OptionParser parser = OptionParser().description("MC-Cons Consensus Optimizer Using an Exact Branch and Bound Solver");
    parser.add_option("-f", "--data").dest("data_file").help("path to MARNA-like input file");
    parser.add_option("-s", "--silent").action("store_true").dest("silent").help("don't display status to stderr");
    optparse::Values options = parser.parse_args(argc, argv);
    std::vector<std::string> args = parser.args();


    if (options.is_set("data_file"))
    {
      // extract command line parameters
      bool silent = false;
      if (options["silent"] == "1")
      {
          silent = true;
      }

      std::string path = options["data_file"];

      // instantiate the genetic algorithm solver
      Solver* solver = new SolverExact(silent);

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
