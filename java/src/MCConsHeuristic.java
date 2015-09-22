import com.martiansoftware.jsap.FlaggedOption;
import com.martiansoftware.jsap.JSAP;
import com.martiansoftware.jsap.JSAPResult;
import com.martiansoftware.jsap.Switch;


public final class MCConsHeuristic {

    public static void main(String[] args) throws Exception {
        JSAP parser = new JSAP("MC-Cons Consensus Optimizer Using A Heuristic Approach");

        // I/O settings
        FlaggedOption input = new FlaggedOption("input", JSAP.STRING_PARSER, null, true,
                'i', "input", "input file path");
        parser.registerParameter(input);

        FlaggedOption output = new FlaggedOption("output", JSAP.STRING_PARSER, "stdout", false,
                'o', "output", "output file destination (default is stdout)");
        parser.registerParameter(output);


        // verbosity
        Switch verbose = new Switch("verbose", 'v', "verbose", "verbosity of the solver");
        parser.registerParameter(verbose);

        // suboptimality tolerance
        FlaggedOption tolerance = new FlaggedOption("tolerance", JSAP.DOUBLE_PARSER, "0.", false,
                't', "tolerance", "Permitted gap between kept solution and best solution, normalized");
        parser.registerParameter(tolerance);

        // random number generator seeds
        FlaggedOption seed0 = new FlaggedOption("seed0", JSAP.LONG_PARSER, "42", false,
                JSAP.NO_SHORTFLAG, "seed0", "First seed of the random stream");
        parser.registerParameter(seed0);
        FlaggedOption seed1 = new FlaggedOption("seed1", JSAP.LONG_PARSER, "42", false,
                JSAP.NO_SHORTFLAG, "seed1", "Second seed of the random stream");
        parser.registerParameter(seed1);
        FlaggedOption seed2 = new FlaggedOption("seed2", JSAP.LONG_PARSER, "42", false,
                JSAP.NO_SHORTFLAG, "seed2", "Third seed of the random stream");
        parser.registerParameter(seed2);
        FlaggedOption seed3 = new FlaggedOption("seed3", JSAP.LONG_PARSER, "42", false,
                JSAP.NO_SHORTFLAG, "seed3", "Fourth seed of the random stream");
        parser.registerParameter(seed3);
        FlaggedOption seed4 = new FlaggedOption("seed4", JSAP.LONG_PARSER, "42", false,
                JSAP.NO_SHORTFLAG, "seed4", "Fifth seed of the random stream");
        parser.registerParameter(seed4);
        FlaggedOption seed5 = new FlaggedOption("seed5", JSAP.LONG_PARSER, "42", false,
                JSAP.NO_SHORTFLAG, "seed5", "Sixth seed of the random stream");
        parser.registerParameter(seed5);


        // heuristic parameters
        FlaggedOption popSize = new FlaggedOption("popSize", JSAP.INTEGER_PARSER, "150", false,
                'p', "popSize", "Genetic algorithm population size");
        parser.registerParameter(popSize);

        FlaggedOption numGenerations = new FlaggedOption("numGenerations", JSAP.INTEGER_PARSER, "150", false,
                'n', "numGenerations", "Number of generations to evaluate");
        parser.registerParameter(numGenerations);

        FlaggedOption eliteSize = new FlaggedOption("eliteSize", JSAP.INTEGER_PARSER, "30", false,
                JSAP.NO_SHORTFLAG, "eliteSize", "Size of the elite in the genetic algorithm");
        parser.registerParameter(eliteSize);

        FlaggedOption improvementProbability = new FlaggedOption("improvementProbability", JSAP.DOUBLE_PARSER, "0.05",
                false, JSAP.NO_SHORTFLAG, "improvementProbability",
                "Probability of applying steepest descent on a solution");
        parser.registerParameter(improvementProbability);

        FlaggedOption improvementDepth = new FlaggedOption("improvementDepth", JSAP.INTEGER_PARSER, "2",
                false, JSAP.NO_SHORTFLAG, "improvementDepth",
                "Number of iterations applied each time the steepest descent is called");
        parser.registerParameter(improvementDepth);

        FlaggedOption crossoverProbability = new FlaggedOption("crossoverProbability", JSAP.DOUBLE_PARSER, "0.5",
                false, JSAP.NO_SHORTFLAG, "crossoverProbability",
                "Probability of applying uniform crossover on a child solution");
        parser.registerParameter(crossoverProbability);

        FlaggedOption crossoverMixingRatio = new FlaggedOption("crossoverMixingRatio", JSAP.DOUBLE_PARSER, "0.5",
                false, JSAP.NO_SHORTFLAG, "crossoverMixingRatio",
                "Mixing ratio used for uniform crossover between two parent solutions");
        parser.registerParameter(crossoverMixingRatio);


        FlaggedOption mutationProbability = new FlaggedOption("mutationProbability", JSAP.DOUBLE_PARSER, "0.05",
                false, JSAP.NO_SHORTFLAG, "mutationProbability",
                "Probability of applying mutation on a child solution");
        parser.registerParameter(mutationProbability);

        FlaggedOption mutationStrength = new FlaggedOption("mutationStrength", JSAP.DOUBLE_PARSER, "0.3",
                false, JSAP.NO_SHORTFLAG, "mutationStrength",
                "Probability that a gene is mutated");
        parser.registerParameter(mutationStrength);


        // parse the arguments
        JSAPResult config = parser.parse(args);


        if (!config.success()) {

            System.err.println();

            // print out specific error messages describing the problems
            // with the command line, THEN print usage, THEN print full
            // help.  This is called "beating the user with a clue stick... lol
            for (java.util.Iterator errs = config.getErrorMessageIterator();
                 errs.hasNext(); ) {
                System.err.println("Error: " + errs.next());
            }

            System.err.println("                "
                    + parser.getUsage());
            System.err.println();
            System.err.println(parser.getHelp());
            System.exit(1);
        }

        long[] SEEDS = {config.getLong("seed0"), config.getLong("seed1"), config.getLong("seed2"),
                config.getLong("seed3"), config.getLong("seed4"), config.getLong("seed5")};


        SolverHeuristic tree_solver = new SolverHeuristic(
                config.getBoolean("verbose"),
                config.getDouble("tolerance"),
                SEEDS,

                config.getInt("popSize"),
                config.getInt("numGenerations"),
                config.getInt("eliteSize"),

                config.getDouble("improvementProbability"),
                config.getInt("improvementDepth"),

                config.getDouble("crossoverProbability"),
                config.getDouble("crossoverMixingRatio"),

                config.getDouble("mutationProbability"),
                config.getDouble("mutationStrength"));


        SolverHeuristic dot_bracket_solver = new SolverHeuristic(
                config.getBoolean("verbose"),
                0.,
                SEEDS,

                config.getInt("popSize"),
                config.getInt("numGenerations"),
                config.getInt("eliteSize"),

                config.getDouble("improvementProbability"),
                config.getInt("improvementDepth"),

                config.getDouble("crossoverProbability"),
                config.getDouble("crossoverMixingRatio"),

                config.getDouble("mutationProbability"),
                config.getDouble("mutationStrength"));

        // execute MC-Cons
        MCCons.MCCONS(config.getString("input"), tree_solver, dot_bracket_solver);
    }
}

