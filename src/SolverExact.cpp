

// constructors and destructors
SolverExact::SolverExact(bool silent) : Solver(silent)
{
    threshold_ = std::numeric_limits<double>::infinity();
}


SolverExact::~SolverExact() { }


SolverExact::set_threshold(double new_threshold)
{
    threshold_ = new_threshold;
}


std::vector<Solution> SolverExact::solve(std::vector< std::vector<double> > distance_matrix,
                                         std::vector<Range> ranges,
                                         unsigned long seeds[6]) const
{
    
}