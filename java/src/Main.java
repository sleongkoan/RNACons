
public class Main {

    public static void main(String[] args) {

        String db1 = "(((...)).(.))";
        String db2 = "((..)).(.)..((()))";
        OrderedRootedLabeledTree t1 = new OrderedRootedLabeledTree(db1, '(', ')', '.');
        OrderedRootedLabeledTree t2 = new OrderedRootedLabeledTree(db2, '(', ')', '.');

        for (char c : t1.getPostOrderLabels())
        {
            System.out.print(c + " ");
        }
        System.out.println();

        System.out.println(t1.toString() + System.lineSeparator() + t2.toString());
        System.out.println(new TreeDistance().distance(t1, t2));

        System.out.println(new StringEditDistance().distance(db1, db2));
    }
}
