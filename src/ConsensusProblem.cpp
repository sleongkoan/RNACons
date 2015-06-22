#include "../include/ConsensusProblem.h"

using std::vector;
using std::map;


template <class T>
ConsensusProblem<T>::ConsensusProblem(std::vector< std::vector<T> > data,
                                      int (*distance_function)(T a, T b))
{
    // constructor
    objects_ = vector<T>();
    ranges_ = vector<Range>();
    int cumul = 0;

    for (int x = 0; x < data.size(); ++x)
    {
        for (int y = 0; y < data[x].size(); ++y)
        {
            objects_.push_back(data[x][y]);
        }
        Range r = {cumul, (int) (cumul + data[x].size())};
        ranges_.push_back( r );
        cumul += data[x].size();
    }

    // fill the distance matrix
    distance_matrix_ = vector< vector<double> >();
    for(int i = 0; i < objects_.size(); ++i)
    {
        distance_matrix_.push_back(vector<double>(objects_.size(), std::numeric_limits<double>::infinity()));
    }

    // memoize distance calculations
    // for it to work, objects must be able to be put inside a map and the distance
    // function must be metric!
    // create dict and prefill it
    map < T, map <T, double> > memoized_distances = map < T, map <T, double> >();
    for (int i = 0; i < objects_.size(); ++i)
    {
        memoized_distances[objects_[i]] = map<T, double>();

    }

    // only compare those not of the same group
    // compute distances only once for each pair of objects
    // to do so, objects must be hashable and have equality operators (in this implementation at least)
    for(int i = 0; i < ranges_.size()-1; ++i)
    {
        int low1 = ranges_[i].low;
        int high1 = ranges_[i].high;

        for(int j = i+1; j < ranges_.size(); ++j)
        {
            int low2 = ranges_[j].low;
            int high2 = ranges_[j].high;

            // compare every objects in the range
            for (int index_obj1 = low1; index_obj1 < high1; ++index_obj1)
            {
                for(int index_obj2 = low2; index_obj2 < high2; ++index_obj2)
                {
                    //
                    T obj1 = objects_[index_obj1];
                    T obj2 = objects_[index_obj2];

                    // check if they already have been compared
                    if(memoized_distances[obj1].find(obj2) == memoized_distances[obj1].end())
                    {
                        // haven't been compared
                        double distance = (*distance_function)(obj1, obj2);
                        memoized_distances[obj1][obj2] = distance;
                        memoized_distances[obj2][obj1] = distance;
                    }
                    // assign to the matrix
                    distance_matrix_[index_obj1][index_obj2] = memoized_distances[obj1][obj2];
                    distance_matrix_[index_obj2][index_obj1] = memoized_distances[obj2][obj1];
                }
            }
        }
    }


    // fill the diagonal with zeros
    for (int i = 0; i < distance_matrix_.size(); ++i)
    {
        distance_matrix_[i][i] = 0.;
    }

    // check that the distance used is symmetric
    assert(distance_is_symmetric(distance_matrix_) && "Distance Matrix is not symmetric");
    return;
}

bool distance_is_symmetric(const std::vector< std::vector<double> > & distance_matrix)
{
    // used to assert that a distance matrix does not violate
    // the first two metric requirements
    // d(x, x) = 0, d(x, y) >= 0
    // d(x, y) >= 0

    // but not d(x, y) + d(y, z) >= d(x, z): this one would be costly to check
    for(size_t i = 0; i < distance_matrix.size(); ++i)
    {
      for(size_t j = 0; j < distance_matrix[i].size(); ++j)
      {
          if ( (distance_matrix[i][j] < 0)  ||
               (distance_matrix[i][i] != 0) ||
               (distance_matrix[i][j] != distance_matrix[j][i]) )
          {
              return false;
          }
      }
    }
    return true;
}


template <class T> ConsensusProblem<T>::~ConsensusProblem() { }



template <class T>
const std::vector<T> & ConsensusProblem<T>::get_objects()
{
    return objects_;
}


template<class T>
const std::vector< Range > & ConsensusProblem<T>::get_ranges()
{
    return ranges_;

}


template<class T>
const std::vector< std::vector<double> > & ConsensusProblem<T>::get_distance_matrix()
{
    return distance_matrix_;

}


// definitions for later, from
// http://stackoverflow.com/questions/8752837/undefined-reference-to-template-class-constructor
template class ConsensusProblem<std::string>;
template class ConsensusProblem<Tree>;

