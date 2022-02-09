#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <iostream> //<<DELETE>> Once debug complete

#include "linux_parser.h" // Access to kFile parsing
#include "process.h"
#include "processor.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;
/*You need to complete the mentioned TODOs in order to satisfy the rubric criteria "The student will be able to extract and display basic data about the system."

You need to properly format the uptime. Refer to the comments mentioned in format. cpp for formatting the uptime.*/

// NOTHING-TODO: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// FIND LEAKING ITERATOR : Return a container composed of the system's processes
vector<Process>& System::Processes()
{
    // Get list of process ids
    vector<int> pids = LinuxParser::Pids();
    
    // Generate new object for each and populate info
    for (int pid : pids)
    {
        bool found = false;
        for (Process tempProcess : processes_)
        {
            if( tempProcess.Pid() == pid)
            {
                //goto FOUND_VALID_PID;
                found = true;
            }
                
        }
        if(found == false)
            processes_.emplace_back(pid);
    }

//FOUND_VALID_PID:
    try
    {
        // Need to sort (by CPU use)
        std::sort(processes_.begin(),processes_.end(),[ ]( Process left, Process right)
        {
            return left.CpuUtilization() > right.CpuUtilization();
        });
        processes_.shrink_to_fit();
    }
    catch(std::bad_alloc & ba)
    {
        std::cerr << "I caught " << ba.what() << '\n';
    }

    return processes_;
}

// DONE : Return the system's kernel identifier (string)
std::string System::Kernel()
{
    return LinuxParser::Kernel();
}

// DONE : Return the system's memory utilization
float System::MemoryUtilization()
{
    return LinuxParser::MemoryUtilization();
}

// DONE : Return the operating system name
std::string System::OperatingSystem()
{
    return LinuxParser::OperatingSystem();
}

// DONE : Return the number of processes actively running on the system
int System::RunningProcesses()
{
    return LinuxParser::RunningProcesses();
}

// DONE : Return the total number of processes on the system
int System::TotalProcesses()
{
    return LinuxParser::TotalProcesses();
}

// DONE : Return the number of seconds since the system started running
long int System::UpTime()
{
    return LinuxParser::UpTime();
}
