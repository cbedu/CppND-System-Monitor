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

// DONE : Return this process's CPU utilization
float Process::CpuUtilization()
{
    //return (float)LinuxParser::ActiveJiffies(pid_)/(float)LinuxParser::Jiffies();

    // <<POSSIBLE-TODO>> << REWRITE >> This section (or lower) seems to be annoying valgrind.
    long tempActive = LinuxParser::ActiveJiffies(pid_);
    long tempActiveDelta = tempActive - prevActiveJiffies_;

    long tempTotal = LinuxParser::Jiffies();  // Lazy, should be exposing the System var.
    long tempTotalDelta = tempTotal - prevTotalJiffies_;

    prevActiveJiffies_ = tempActive;
    prevTotalJiffies_ = tempTotal;

    float val = (float)tempActiveDelta / (float)tempTotalDelta;
    if(val < 0.00)
        val = 0.00f;

    return val;
}

// DONE : Return the command that generated this process
string Process::Command()
{
    return LinuxParser::Command(pid_);
}

// DONE: Return this process's memory utilization
string Process::Ram()
{
    return LinuxParser::Ram(pid_);
}

// DONE: Return the user (name) that generated this process
string Process::User()
{
    return LinuxParser::User(pid_);
}

// DONE: Return the age of this process (in seconds)
long int Process::UpTime()
{
    // just Uptime(pid_) is huge, needs to be taken from total up.
    return LinuxParser::UpTime() - LinuxParser::UpTime(pid_);
}

// NOTHING-TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a[[maybe_unused]]) const { return true; }