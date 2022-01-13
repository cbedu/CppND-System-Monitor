#include <string>
#include <iostream>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
    
    //Previous less efficient maths
    //int minutes_ = (seconds - (hours * 3600)) / 60;
    //int seconds_ = (seconds - ((hours *3600) + (minutes *60))) % 60;

    int hours_ = seconds / 3600;
    int minutes_ = (seconds % 3600) / 60;
    int seconds_ = (seconds % 60);
    
    std::string outputStr;

    if(hours_ < 10){ outputStr.append("0"); }
    outputStr.append(std::to_string(hours_));

    outputStr.append(":");

    if(minutes_ < 10){ outputStr.append("0"); }
    outputStr.append(std::to_string(minutes_));

    outputStr.append(":");

    if(seconds_ < 10){ outputStr.append("0"); }
    outputStr.append(std::to_string(seconds_));

    return outputStr;
}