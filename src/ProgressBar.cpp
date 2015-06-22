#include "../include/ProgressBar.h"


ProgressBar::ProgressBar(std::string process_name, size_t bar_length)
{
    process_name_ = process_name;
    bar_length_ = bar_length;
}


ProgressBar::~ProgressBar() { }


void ProgressBar::update(float percentage)
{
    // displays the current progress of the process
    assert (percentage >= 0.);
    assert (percentage <= 1.);

    size_t progress_length = bar_length_ * percentage;
    std::cerr << "[";
    for (size_t i = 0; i < bar_length_; ++i)
    {
        if (i <= progress_length)
            std::cerr << "#";
        else
            std::cerr << " ";
    }
    std::cerr << "] " << int(percentage * 100) << "% " << process_name_ << "\r";// ;
    std::cerr.flush();
}


void ProgressBar::clean()
{
    // called after last iteration, displays 100% success
    // because I seem to constantly mess up (usually ends 99%)
    this->update(1.);
    std::cerr << std::endl;
    std::cerr.flush();
}
