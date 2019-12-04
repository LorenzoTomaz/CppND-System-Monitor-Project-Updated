#include <string>
#include "format.h"
using std::string;

// DONE: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) { 
    int h = (int)(seconds / 3600);
    int m = (int)(seconds % 3600 / 60);
    int s = (int)(seconds % 3600 % 60);
    std::string hh, mm, ss;
    hh = (h  < 10) ? ("0" + std::to_string(h)) : std::to_string(h);
    mm = (m  < 10) ? ("0" + std::to_string(m)) : std::to_string(m);
    ss = (s  < 10) ? ("0" +std::to_string(s)) : std::to_string(s);
    return hh +  ":" + mm + ":" + ss;

 }