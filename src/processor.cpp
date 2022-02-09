#include "processor.h"
#include "linux_parser.h"

// DONE : Return the aggregate CPU utilization
float Processor::Utilization()
{
    return (float)LinuxParser::ActiveJiffiesDelta() / (float)LinuxParser::JiffiesDelta();
}