#include "include/progressbarlistener.h"

#include <iostream>
#include <string>

ProgressBarListener::ProgressBarListener(const int tick_max) :
tick_max_(tick_max)
{
}

void ProgressBarListener::update(const int i) {
    int percent = static_cast<int>((i*100)/tick_max_);

    if ((i*100/5)%tick_max_ == 0) {
        std::string bar;
        for (int i = 0; i < 50; i++) {
            if ( i < (percent/2)) {
                bar.replace(i, 1, "=");
            } else if ( i == (percent/2)) {
                bar.replace(i, 1, ">");
            } else {
                bar.replace(i, 1, " ");
            }
        }
        std::cout << '\r' << "[" << bar << "] ";
        std::cout.width(3);
        std::cout<< percent << "%     " << std::flush;
    }
}

void ProgressBarListener::finished() {
    update(tick_max_);
    std::cout << std::endl;
}
