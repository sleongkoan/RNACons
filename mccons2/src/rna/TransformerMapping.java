package rna;

import util.Pair;

import java.util.ArrayList;

public class TransformerMapping<A extends Comparable<A>, B extends Comparable<B>> {

    private ArrayList<ArrayList<Pair<A, B>>> mapping;

    public TransformerMapping(int numClasses)
    {
        mapping = new ArrayList<>();
        assert numClasses > 0;
        for (int i = 0; i != numClasses; ++i)
        {
            mapping.add(new ArrayList<Pair<A, B>>());
        }
    }




    public void addMapping(int classIndex, A from, B to)
    {
        mapping.get(classIndex).add(new Pair<A, B>(from, to));
    }


    public ArrayList<A> getReverseMap(int index, B query)
    {
        assert (0 <= index && index < mapping.size());
        ArrayList<A> corresponding = new ArrayList<>();
        for (Pair<A, B> map : mapping.get(index))
        {
            if (map.getSecond().compareTo(query) == 0)
            {
                corresponding.add(map.getFirst());
            }
        }
        return corresponding;
    }


    private static <T extends Comparable<T>> boolean contains(ArrayList<T> list, T toFind)
    {
        boolean contains = false;
        for (T element : list)
        {
            if (element.compareTo(toFind) == 0)
            {
                contains = true;
                break;
            }
        }
        return contains;
    }

    private static <T extends Comparable<T>> ArrayList<T> getUniques(ArrayList<T> list)
    {
        ArrayList<T> uniques = new ArrayList<>();
        for (T obj : list)
        {
            if (!contains(uniques, obj))
            {
                uniques.add(obj);
            }
        }
        return uniques;

    }


    public ArrayList<A> getReverseMapUniques(int index, B query)
    {
        return getUniques(getReverseMap(index, query));

    }

    public ArrayList<A> getDomain()
    {
        ArrayList<A> domain = new ArrayList<>();
        for (int i = 0; i != mapping.size(); ++i)
        {
            for (Pair<A, B> map : mapping.get(i))
            {
                domain.add(map.getFirst());
            }
        }
        return domain;
    }


    public ArrayList<A> getDomainUniques()
    {
        return getUniques(getDomain());
    }

    public ArrayList<B> getCoDomain()
    {
        ArrayList<B> coDomain = new ArrayList<>();
        for (int i = 0; i != mapping.size(); ++i)
        {
            for (Pair<A, B> map : mapping.get(i))
            {
                coDomain.add(map.getSecond());
            }
        }
        return coDomain;
    }

    public ArrayList<B> getCoDomainUniques()
    {
        return getUniques(getCoDomain());
    }

}
