#include "processor.h"
#include "linux_parser.h"

// DONE : Return the aggregate CPU utilization
float Processor::Utilization()
{
    //return (float)LinuxParser::ActiveJiffies() / (float)LinuxParser::IdleJiffies(); // fail at runtime
    //return (float)LinuxParser::ActiveJiffies(); // ran, even if no good output
    //return (float)LinuxParser::IdleJiffies(); // ran, even if no good output
    return (float)LinuxParser::ActiveJiffies() / (float)LinuxParser::Jiffies(); // fail at runtime
    
    //return (float)LinuxParser::IdleJiffies() / (float)(LinuxParser::IdleJiffies() * 2.00f);
}