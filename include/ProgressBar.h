#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <string>
#include <iostream>

#include <assert.h>

class ProgressBar
{
public:
    // constructors and destructor
    ProgressBar(std::string process_name="", size_t bar_length=40);
    ~ProgressBar();

    // updates the progress displayed
    void update(float percentage);
    // displays 100% status
    void clean();
private:
    size_t _bar_length;
    std::string _process_name;
};

#endif // PROGRESSBAR_H
