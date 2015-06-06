#include "../include/ProgressBar.h"


ProgressBar::ProgressBar(std::string process_name, size_t bar_length)
{
    _process_name = process_name;
    _bar_length = bar_length;
}


ProgressBar::~ProgressBar() { }


void ProgressBar::update(float percentage)
{
    // displays the current progress of the process
    assert (percentage >= 0.);
    assert (percentage <= 1.);

    size_t progress_length = _bar_length * percentage;
    std::cerr << "[";
    for (size_t i = 0; i < _bar_length; ++i)
    {
        if (i <= progress_length)
            std::cerr << "#";
        else
            std::cerr << " ";
    }
    std::cerr << "] " << int(percentage * 100) << "% " << _process_name << "\r";// ;
    std::cerr.flush();
}


void ProgressBar::clean()
{
    this->update(1.);
    std::cerr << std::endl;
    std::cerr.flush();
}
