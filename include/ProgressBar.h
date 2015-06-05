#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <string>
#include <iostream>

#include <assert.h>

class ProgressBar
{
public:
    // constructors and destructor
    ProgressBar(std::string processName="", size_t barLength=50);
    ~ProgressBar();

    // updates the progress displayed
    void update(float percentage);
    // displays 100% status
    void clean();
private:
    size_t bar_length;
    std::string process_name;
};

#endif // PROGRESSBAR_H
