#include "../include/ProgressBar.h"


ProgressBar::ProgressBar(std::string processName, size_t barLength)
{
    process_name = processName;
    bar_length = barLength;
}


ProgressBar::~ProgressBar() { }


void ProgressBar::update(float percentage)
{
    // displays the current progress of the process
    assert (percentage >= 0.);
    assert (percentage <= 1.);

    size_t progress_length = this->bar_length * percentage;
    std::cerr << "[";
    for (size_t i = 0; i < this->bar_length; ++i)
    {
        if (i <= progress_length)
            std::cerr << "#";
        else
            std::cerr << " ";
    }
    std::cerr << "] " << int(percentage * 100) << " %\r";
    std::cerr.flush();
}


void ProgressBar::clean()
{
    this->update(1.);
    std::cerr << std::endl;
    std::cerr.flush();
}
