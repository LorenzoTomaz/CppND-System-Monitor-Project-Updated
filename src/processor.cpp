#include "processor.h"
#include "linux_parser.h"

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() { 
    float total = static_cast<float>(LinuxParser::Jiffies());
    float active = static_cast<float>(LinuxParser::ActiveJiffies());
    float cpu = 0.0;
    if (total != total_) {cpu = ((active - active_) / (total - total_));}
    active_ = active;
    total_ = total; 
    return cpu;

}