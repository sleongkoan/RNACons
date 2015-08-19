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
    std::vector<T> unique_objects = std::vector<T>();
    map<T, int> obj_to_index = map<T, int>();
    for (size_t i = 0; i != objects_.size(); ++i)
    {
        T obj = objects_[i]; 
        if (find(unique_objects.begin(), unique_objects.end(), obj) == unique_objects.end())
        {
            unique_objects.push_back(obj);
            obj_to_index[obj] = unique_objects.size()-1;
        }
    }
    std::vector< std::vector<double> > condensed_distances = std::vector< std::vector<double> >();
    for (size_t i = 0; i < unique_objects.size(); ++i)
    {
        condensed_distances.push_back(vector<double>(unique_objects.size(), 0.));
    }
    /*
    map < T, map <T, double> > memoized_distances = map < T, map <T, double> >();
    for (int i = 0; i < objects_.size(); ++i)
    {
        memoized_distances[objects_[i]] = map<T, double>();

    }*/
    double distance;
    for (size_t i = 0; i != unique_objects.size() - 1; ++i)
    {
        T obj1 = unique_objects[i];
        for (size_t j = i + 1; j != unique_objects.size(); ++j)
        {
            T obj2 = unique_objects[j];
            // calculate and assign the distance
            distance = (*distance_function)(obj1, obj2);
            condensed_distances[i][j] = distance;
            condensed_distances[j][i] = distance;
        }
    }

    // fill the distance matrix
    int index1, index2;
    for (size_t i = 0; i != objects_.size() - 1; ++i)
    {
        index1 = obj_to_index[objects_[i]];
        for (size_t j = i + 1; j != objects_.size(); ++j)
        {
            index2 = obj_to_index[objects_[j]];
            distance = condensed_distances[index1][index2];
            distance_matrix_[i][j] = distance;
            distance_matrix_[j][i] = distance;
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

