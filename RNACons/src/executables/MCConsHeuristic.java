package executables;

import com.martiansoftware.jsap.FlaggedOption;
import com.martiansoftware.jsap.JSAP;
import com.martiansoftware.jsap.JSAPResult;
import com.martiansoftware.jsap.Switch;
import problemSolving.StrategyTemplate;
import problemSolving.HeuristicStrategy;


public final class MCConsHeuristic {

    public static void main(String[] args) throws Exception {

        //region ARGUMENT PARSING
        JSAP parser = new JSAP();

        // I/O settings
        parser.registerParameter(new FlaggedOption("input", JSAP.STRING_PARSER, null, true,
                'i', "input", "input file path"));

        // verbosity
        parser.registerParameter(new Switch("verbose", 'v', "verbose", "verbosity of the solver"));

        // suboptimal tolerance
        parser.registerParameter(new FlaggedOption("tolerance", JSAP.DOUBLE_PARSER, "0.", false,
                't', "tolerance", "Permitted gap between kept solution and best solution, normalized"));

        // random number generator seeds
        parser.registerParameter(new FlaggedOption("seed0", JSAP.LONG_PARSER, "42", false,
                JSAP.NO_SHORTFLAG, "seed0", "First seed of the random stream"));

        parser.registerParameter(new FlaggedOption("seed1", JSAP.LONG_PARSER, "42", false,
                JSAP.NO_SHORTFLAG, "seed1", "Second seed of the random stream"));

        parser.registerParameter(new FlaggedOption("seed2", JSAP.LONG_PARSER, "42", false,
                JSAP.NO_SHORTFLAG, "seed2", "Third seed of the random stream"));

        parser.registerParameter(new FlaggedOption("seed3", JSAP.LONG_PARSER, "42", false,
                JSAP.NO_SHORTFLAG, "seed3", "Fourth seed of the random stream"));

        parser.registerParameter(new FlaggedOption("seed4", JSAP.LONG_PARSER, "42", false,
                JSAP.NO_SHORTFLAG, "seed4", "Fifth seed of the random stream"));

        parser.registerParameter(new FlaggedOption("seed5", JSAP.LONG_PARSER, "42", false,
                JSAP.NO_SHORTFLAG, "seed5", "Sixth seed of the random stream"));

        // heuristic parameters
        parser.registerParameter(new FlaggedOption("popSize", JSAP.INTEGER_PARSER, "250", false,
                'p', "popSize", "Genetic algorithm population size"));


        parser.registerParameter(new FlaggedOption("numGenerations", JSAP.INTEGER_PARSER, "250", false,
                'n', "numGenerations", "Number of generations to evaluate"));


        parser.registerParameter(new FlaggedOption("eliteRatio", JSAP.DOUBLE_PARSER, "0.1", false,
                JSAP.NO_SHORTFLAG, "eliteRatio", "Size of the elite as percentage of population size"));


        parser.registerParameter(new FlaggedOption("improvementProbability", JSAP.DOUBLE_PARSER, "0.1",
                false, JSAP.NO_SHORTFLAG, "improvementProbability",
                "Probability of applying steepest descent on a solution"));


        parser.registerParameter(new FlaggedOption("improvementDepth", JSAP.INTEGER_PARSER, "4",
                false, JSAP.NO_SHORTFLAG, "improvementDepth",
                "Number of iterations applied each time the steepest descent is called"));


        parser.registerParameter(new FlaggedOption("crossoverProbability", JSAP.DOUBLE_PARSER, "0.5",
                false, JSAP.NO_SHORTFLAG, "crossoverProbability",
                "Probability of applying uniform crossover on a child solution"));

        parser.registerParameter(new FlaggedOption("crossoverMixingRatio", JSAP.DOUBLE_PARSER, "0.1",
                false, JSAP.NO_SHORTFLAG, "crossoverMixingRatio",
                "Mixing ratio used for uniform crossover between two parent solutions"));


        parser.registerParameter(new FlaggedOption("mutationProbability", JSAP.DOUBLE_PARSER, "0.05",
                false, JSAP.NO_SHORTFLAG, "mutationProbability",
                "Probability of applying mutation on a child solution"));

        parser.registerParameter(new FlaggedOption("mutationStrength", JSAP.DOUBLE_PARSER, "0.2",
                false, JSAP.NO_SHORTFLAG, "mutationStrength",
                "Probability that a gene is mutated"));


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
        //endregion


        //region INSTANTIATION
        long[] SEEDS = {config.getLong("seed0"), config.getLong("seed1"), config.getLong("seed2"),
                config.getLong("seed3"), config.getLong("seed4"), config.getLong("seed5")};


        HeuristicStrategy strategy1 = new HeuristicStrategy(
                config.getBoolean("verbose"),
                config.getDouble("tolerance"),
                SEEDS,

                config.getInt("popSize"),
                config.getInt("numGenerations"),
                config.getDouble("eliteRatio"),

                config.getDouble("crossoverProbability"),
                config.getDouble("crossoverMixingRatio"),
                config.getDouble("mutationProbability"),
                config.getDouble("mutationStrength"),
                config.getDouble("improvementProbability"),
                config.getInt("improvementDepth"));

        HeuristicStrategy strategy2 = new HeuristicStrategy(
                config.getBoolean("verbose"),
                0,
                SEEDS,

                config.getInt("popSize"),
                config.getInt("numGenerations"),
                config.getDouble("eliteRatio"),

                config.getDouble("crossoverProbability"),
                config.getDouble("crossoverMixingRatio"),
                config.getDouble("mutationProbability"),
                config.getDouble("mutationStrength"),
                config.getDouble("improvementProbability"),
                config.getInt("improvementDepth"));
        //endregion


        //region SOLVING
        StrategyTemplate.applyStrategy(config.getString("input"), strategy1, strategy2);
        //endregion

    }
}

