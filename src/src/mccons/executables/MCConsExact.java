package mccons.executables;

import com.martiansoftware.jsap.FlaggedOption;
import com.martiansoftware.jsap.JSAP;
import com.martiansoftware.jsap.JSAPResult;
import com.martiansoftware.jsap.Switch;
import mccons.solvers.MCCons;
import mccons.solvers.SolverExact;


public final class MCConsExact {

    public static void main(String[] args) throws Exception {
        JSAP parser = new JSAP();

        // I/O settings
        parser.registerParameter(new FlaggedOption("input", JSAP.STRING_PARSER, null, true,
                'i', "input", "input file path"));

        // verbosity
        parser.registerParameter(new Switch("verbose", 'v', "verbose", "verbosity of the solver"));

        // suboptimality tolerance
        parser.registerParameter(new FlaggedOption("tolerance", JSAP.DOUBLE_PARSER, "0.", false,
                't', "tolerance", "Permitted gap between kept solution and best solution, normalized"));


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

        SolverExact solver = new SolverExact(config.getDouble("tolerance"), config.getBoolean("verbose"));


        // execute MC-Cons
        MCCons.MCCONS(config.getString("input"), solver, solver);



    }
}

