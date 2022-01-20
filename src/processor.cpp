#include "processor.h"
#include "linux_parser.h"

// DONE : Return the aggregate CPU utilization
float Processor::Utilization()
{
    // grab from functions once
    long tempActive = LinuxParser::ActiveJiffies();
    long tempIdle = LinuxParser::IdleJiffies();
    long tempTotal = LinuxParser::Jiffies();

    // compare against our last run
    long idleJiffiesDelta = tempIdle - previousIdleJiffies_;
    long totalJiffiesDelta = tempTotal - previousTotalJiffies_;
    long activeJiffiesDelta = tempActive - previousActiveJiffies_;

    // store latest values for next comparison
    previousIdleJiffies_ = tempIdle;
    previousTotalJiffies_ = tempTotal;
    previousActiveJiffies_ = tempActive;

//   long deltaT;
//   deltaT = total - CPUUsageVars::previousIdleJiffies_;
//   CPUUsageVars::previousIdleJiffies_ = total;
//   //return total;
//   return deltaT;



    return (float)activeJiffiesDelta / (float)totalJiffiesDelta;
}