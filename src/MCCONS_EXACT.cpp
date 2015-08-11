
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
    parser.add_option("-t", "--threshold").dest("threshold").type("size_t").help("consider up to t additional score for trees");
    parser.add_option("-i", "--interrupt").dest("interrupt").type("bool").help("do only tree consensus");
    optparse::Values options = parser.parse_args(argc, argv);


    if (options.is_set("data_file"))
    {
      // extract command line parameters
      std::string path = options["data_file"];
      double SUBOPTIMAL_THRESHOLD = 0;
      if (options.is_set("threshold"))
      {
          SUBOPTIMAL_THRESHOLD = atof(options["threshold"].c_str());
      }

      // instantiate the solver
      Solver* tree_solver = new SolverExact(SUBOPTIMAL_THRESHOLD, false);
      Solver* dot_bracket_solver = new SolverExact(0., false);

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
