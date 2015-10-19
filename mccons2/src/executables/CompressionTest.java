package executables;


import com.martiansoftware.jsap.FlaggedOption;
import com.martiansoftware.jsap.JSAP;
import com.martiansoftware.jsap.JSAPException;
import com.martiansoftware.jsap.JSAPResult;
import rna.GranularBasePairTree;
import rna.GranularRepr;
import rna.RNAConverter;
import rna.RNAShapeRepr;
import util.Readers;

import java.io.IOException;
import java.util.*;

/**
 * used for testing the compression induced by different representations of RNA secondary structure
 */
public class CompressionTest {


    public static void main(String[] args) throws JSAPException, IOException {
        JSAP parser = new JSAP();

        // I/O settings
        parser.registerParameter(new FlaggedOption("input", JSAP.STRING_PARSER, null, true,
                'i', "input", "input file path"));


        //parser.registerParameter(new FlaggedOption("output", JSAP.STRING_PARSER, null, true,
         //       'o', "output", "output file path"));



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



        ArrayList<ArrayList<String>> dotBrackets = Readers.readMarnaFile(config.getString("input"));

        // group them all together and keep only uniques
        HashSet<String> uniqueInputs = new HashSet<>();

        for (ArrayList<String> list : dotBrackets) {
            for (String dotBracket : list) {
                uniqueInputs.add(dotBracket);
            }
        }



        ArrayList<RNAConverter> converters = new ArrayList<>();
        converters.add(new RNAShapeRepr("shapes 1", 1));
        converters.add(new RNAShapeRepr("shapes 3", 3));
        converters.add(new RNAShapeRepr("shapes 5", 5));
        converters.add(new GranularRepr("granular 1 (equivalent to brackets)", 1));
        converters.add(new GranularRepr("granular 2", 2));
        converters.add(new GranularRepr("granular 3", 3));
        converters.add(new GranularRepr("granular 5", 5));
        converters.add(new GranularRepr("granular 8", 9));
        converters.add(new GranularRepr("granular 13", 13));
        converters.add(new GranularRepr("granular 21", 21));
        converters.add(new GranularRepr("granular 34", 44));


        // call them all
        for (String dotBracket : uniqueInputs) {
            for (RNAConverter converter : converters) {
                converter.call(dotBracket);
            }
        }

        // extract the reverse mappings
        ArrayList<Hashtable<String, HashSet<String>>> reverseMappings = new ArrayList<>();
        for (RNAConverter converter : converters)
        {
            reverseMappings.add(converter.getReverseMapping());
        }

        // get the names
        ArrayList<String> names = new ArrayList<>();
        for (RNAConverter converter : converters)
        {
            names.add(converter.getName());
        }

        // STATS

        // number of classes, the size of the keyset each reverse mapping
        ArrayList<Integer> numberOfClasses = new ArrayList<>();

        // distribution : number of elements for each class, sorted
        ArrayList<Hashtable<Integer, Integer>> distributions = new ArrayList<>();

        for (Hashtable<String, HashSet<String>> mapping : reverseMappings)
        {
            numberOfClasses.add(mapping.keySet().size());
            Hashtable<Integer, Integer> sizes = new Hashtable<>();
            for (HashSet<String> input : mapping.values())
            {
                int size = input.size();
                if (!sizes.containsKey(size))
                {
                    sizes.put(size, 0);
                }
                sizes.put(size, sizes.get(size)+1);
            }

            distributions.add(sizes);
        }


        // output results to stdout
        System.out.println("#################### Compression results ####################");
        System.out.println("Input = " + uniqueInputs.size() + " unique objects");
        System.out.println("#############################################################" + System.lineSeparator());
        for (int i = 0; i != names.size(); ++i)
        {
            System.out.println(names.get(i)+ " : " + numberOfClasses.get(i) + " classes");
            ArrayList<Integer> keys = new ArrayList<>(distributions.get(i).keySet());
            Collections.sort(keys, Collections.reverseOrder());
            for (Integer key : keys)
            {
                int repetitions = distributions.get(i).get(key);
                if ( repetitions > 1)
                {
                    System.out.print(key+"x"+ repetitions +"; ");
                }
                else
                {
                    System.out.print(key + "; ");
                }
            }
            System.out.println(System.lineSeparator());
        }








    }
}

