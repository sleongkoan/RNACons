#include "../include/ConsensusProblem.h"

using std::vector;
using std::map;


template <class T>
ConsensusProblem<T>::ConsensusProblem(std::vector< std::vector<T> > data,
                                      int (*distance_function)(T a, T b))
{
    // constructor
    objects = vector<T>();
    ranges = vector<Range>();
    int cumul = 0;

    for (int x = 0; x < data.size(); ++x)
    {
        for (int y = 0; y < data[x].size(); ++y)
        {
            objects.push_back(data[x][y]);
        }
        Range r = {cumul, (int) (cumul + data[x].size())};
        ranges.push_back( r );
        cumul += data[x].size();
    }

    // fill the distance matrix
    this->distance_matrix = vector< vector<double> >();
    for(int i = 0; i < objects.size(); ++i)
    {
        this->distance_matrix.push_back(vector<double>(objects.size(), std::numeric_limits<double>::infinity()));
    }

    // memoize distance calculations
    // for it to work, objects must be able to be put inside a map and the distance
    // function must be metric!
    // create dict and prefill it
    map < T, map <T, double> > memoized_distances = map < T, map <T, double> >();
    for (int i = 0; i < objects.size(); ++i)
    {
        memoized_distances[objects[i]] = map<T, double>();

    }

    // only compare those not of the same group
    // compute distances only once for each pair of objects
    // to do so, objects must be hashable and have equality operators (in this implementation at least)
    for(int i = 0; i < ranges.size()-1; ++i)
    {
        int low1 = ranges[i].low;
        int high1 = ranges[i].high;

        for(int j = i+1; j < ranges.size(); ++j)
        {
            int low2 = ranges[j].low;
            int high2 = ranges[j].high;

            // compare every objects in the range
            for (int index_obj1 = low1; index_obj1 < high1; ++index_obj1)
            {
                for(int index_obj2 = low2; index_obj2 < high2; ++index_obj2)
                {
                    //
                    T obj1 = objects[index_obj1];
                    T obj2 = objects[index_obj2];

                    // check if they already have been compared
                    if(memoized_distances[obj1].find(obj2) == memoized_distances[obj1].end())
                    {
                        // haven't been compared
                        double distance = (*distance_function)(obj1, obj2);
                        memoized_distances[obj1][obj2] = distance;
                        memoized_distances[obj2][obj1] = distance;
                    }
                    // assign to the matrix
                    this->distance_matrix[index_obj1][index_obj2] = memoized_distances[obj1][obj2];
                    this->distance_matrix[index_obj2][index_obj1] = memoized_distances[obj2][obj1];
                }
            }
        }
    }


    // fill the diagonal with zeros
    for (int i = 0; i < distance_matrix.size(); ++i)
    {
        distance_matrix[i][i] = 0.;
    }

    return;
}

template <class T> ConsensusProblem<T>::~ConsensusProblem() { }



template <class T>
std::vector<T>& ConsensusProblem<T>::get_objects()
{
    return this->objects;
}


template<class T>
std::vector< Range >& ConsensusProblem<T>::get_ranges()
{
    return this->ranges;

}


template<class T>
std::vector< std::vector<double> >& ConsensusProblem<T>::get_distance_matrix()
{
    return this->distance_matrix;

}


// definitions for later, from
// http://stackoverflow.com/questions/8752837/undefined-reference-to-template-class-constructor
template class ConsensusProblem<std::string>;
template class ConsensusProblem<Tree>;

