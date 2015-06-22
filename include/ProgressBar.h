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

    // displays 100% status, should be called immediately after last iteration
    void clean();

private:
    size_t bar_length_;
    std::string process_name_;
};

#endif // PROGRESSBAR_H
