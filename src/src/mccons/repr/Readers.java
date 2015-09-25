package mccons.repr;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;


public final class Readers {

    public static int getSuboptLength(String line) {
        char[] chars = line.toCharArray();
        int i = 0;
        for (char c : chars) {
            if (c != '(' && c != ')' && c != '.') {
                return i;
            } else {
                i += 1;
            }
        }
        return line.length();
    }


    public static ArrayList<ArrayList<String>> readMarnaFile(String file) throws IOException {
        // read marna file format with suboptimal structures
        // we only take suboptimal structures as data, rest is ignored
        ArrayList<ArrayList<String>> data = new ArrayList<>();
        ArrayList<String> subopts = new ArrayList<>();

        try (BufferedReader br = new BufferedReader(new FileReader(file))) {
            for (String line; (line = br.readLine()) != null; ) {
                // process the line

                // figure out where we are
                if (line.startsWith(">") && (subopts.size() != 0)) {
                    // new subopts are coming, add old ones to the data
                    data.add(subopts);
                    subopts = new ArrayList<>();
                } else if (line.startsWith("(") || line.startsWith(")") || line.startsWith(".")) {
                    // read the new suboptimal
                    // must keep only the structure (first part)
                    subopts.add(line.substring(0, getSuboptLength(line)));
                } else {
                    continue;
                }
            }
            // line is not visible here.
            if (subopts.size() != 0) {
                data.add(subopts);
            }
        }
        return data;
    }
}


