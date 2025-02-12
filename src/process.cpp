//DONE
#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(const int& pid) : pid_(pid) { }


// DONE: Return this process's ID
int Process::Pid() { return pid_; }

// DONE: Return this process's CPU utilization
float Process::CpuUtilization() const { 
    /*const float clkSysHertz = sysconf(_SC_CLK_TCK);
    const float activeTotalTimePIDSeconds = static_cast<float>(LinuxParser::ActiveJiffies(pid_))/clkSysHertz;
    const float upTimeSeconds = static_cast<float>(LinuxParser::UpTime());
    const float upTimePIDSeconds = static_cast<float>(LinuxParser::UpTime(pid_));
    return (activeTotalTimePIDSeconds / (upTimeSeconds - upTimePIDSeconds));*/
    const float tck = sysconf(_SC_CLK_TCK);
    const float tot = static_cast<float>(LinuxParser::ActiveJiffies(pid_)) / tck;
    const auto up = LinuxParser::UpTime();
    const auto pUp = LinuxParser::UpTime(pid_);
    return tot / static_cast<float>(up - pUp);
}


// DONE: Return the command that generated this process
string Process::Command() { 
    return LinuxParser::Command(pid_); 
}

// DONE: Return this process's memory utilization
string Process::Ram() { 
    return LinuxParser::Ram(pid_); 
}

// DONE: Return the user (name) that generated this process
string Process::User() { 
    return LinuxParser::User(pid_); 
}

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() { 
    return LinuxParser::UpTime(pid_); 
}

// DONE: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
    return (CpuUtilization() > a.CpuUtilization());
}