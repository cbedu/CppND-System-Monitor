#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// DONE : Return this process's ID
int Process::Pid() { return pid_; }

// TESTING : Return this process's CPU utilization
float Process::CpuUtilization()
{
    long tempActive = LinuxParser::ActiveJiffies(pid_);
    long tempActiveDelta = tempActive - prevActiveJiffies_;

    long tempTotal = LinuxParser::Jiffies();
    long tempTotalDelta = tempTotal - prevTotalJiffies_;

    prevActiveJiffies_ = tempActive;
    prevTotalJiffies_ = tempTotal;

    return (float)tempActiveDelta / (float)tempTotalDelta;
}

// DONE : Return the command that generated this process
string Process::Command()
{
    return LinuxParser::Command(pid_);
}

// TODO: Return this process's memory utilization
string Process::Ram() { return string(); }

// TODO: Return the user (name) that generated this process
string Process::User() { return string(); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return 0; }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a[[maybe_unused]]) const { return true; }