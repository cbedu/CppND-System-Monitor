#include "processor.h"
#include "linux_parser.h"

// DONE : Return the aggregate CPU utilization
float Processor::Utilization()
{
    // grab from functions once
    long tempActive = LinuxParser::ActiveJiffiesDelta();
    //long tempIdle = LinuxParser::IdleJiffiesDelta();
    long tempTotal = LinuxParser::JiffiesDelta();

    //return 1.00f;
    return (float)tempActive / (float)tempTotal;

    // compare against our last run
    //long idleJiffiesDelta = tempIdle - previousIdleJiffies_;
    long totalJiffiesDelta = tempTotal - previousTotalJiffies_;
    long activeJiffiesDelta = tempActive - previousActiveJiffies_;

    // store latest values for next comparison
    //previousIdleJiffies_ = tempIdle;
    previousTotalJiffies_ = tempTotal;
    previousActiveJiffies_ = tempActive;

    return (float)activeJiffiesDelta / (float)totalJiffiesDelta;
}