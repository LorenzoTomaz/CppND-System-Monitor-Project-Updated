//DONE
#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <fstream>
#include <regex>
#include <string>
#include "linux_parser.h"

#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;


using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel;
  }
  return kernel;
}

// DONE BONUS: Update this to use std::filesystem
std::vector<int> LinuxParser::Pids() {
    std::vector<int> pids;
    for (const auto & entry : fs::directory_iterator(kProcDirectory)){
        std::string str_str = entry.path().string();
        std::regex patternMatch("(/proc/)");
        std::string pattern = std::regex_replace(str_str, patternMatch, "$2");
        if (std::all_of(pattern.begin(), pattern.end(), ::isdigit)){
                int pid = std::stoi(pattern);
                pids.push_back(pid);
        }
    }
    return pids;
}

/*  
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}
*/

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {   
  std::string line;
  std::string key;
  bool hadTotalMemory = false;
  float total;
  bool hadFreeMemory = false;
  float free;
  float value;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "MemTotal:") {
        total = value;
        hadTotalMemory = true;
      } else if (key == "MemFree:") {
        free = value;
        hadFreeMemory = true;
        }

        if (hadTotalMemory && hadFreeMemory) {
          return (total - free) / total;
        }
      }
    }
  return 0.0;
     
}



// DONE: Read and return the system uptime
long LinuxParser::UpTime() { 
  std::string line;
  long uptime;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
    return uptime;
  }
 }

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  //convert seconds to jiffies: jiffies = seconds * HZ
  return UpTime() * sysconf(_SC_CLK_TCK); }

// DONE: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {     
  //check https://linux.die.net/man/5/proc
  long jiffiesForPID = 0;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
    if (filestream.is_open()) {
        std::string line;
        std::string str;
        std::getline(filestream, line);
        std::istringstream linestream(line);
        vector<string> values;
        while (linestream >> str) {
            values.push_back(str);
        }
        if (values.size() > 21) {
                jiffiesForPID = std::stol(values[13]) +
                          std::stol(values[14]) +
                          std::stol(values[15]) +
                          std::stol(values[16]);
        }
    }
  return jiffiesForPID;
}


// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
    long activeJiffies = 0;
    vector<std::string> cpuUtil = LinuxParser::CpuUtilization();
    for (int i = kUser_; i <= kGuestNice_; i++) {
        activeJiffies += std::stol(cpuUtil[i]);
    }
    activeJiffies -= std::stol(cpuUtil[kIdle_]) + std::stol(cpuUtil[kIOwait_]);
    return activeJiffies;
}


// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
    vector<string> cpuUtil = LinuxParser::CpuUtilization();
    return std::stol(cpuUtil[kIdle_]) + std::stol(cpuUtil[kIOwait_]);
 }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
    vector<string> values;
    std::ifstream filestream(kProcDirectory + kStatFilename);
    if (filestream.is_open()) {
        std::string line;
        getline(filestream, line);
        std::istringstream linestream(line);
        std::string value;
        //read the content from first line only
        while (linestream >> value) {
            if ("cpu" != value) {
                values.push_back(value);
            }
        }
    }
    return values;

 }

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  std::ifstream filestream(kProcDirectory + kStatFilename);
    if (filestream.is_open()) {
        std::string key;
        std::string line;
        while (std::getline(filestream, line)) {
            std::istringstream linestream(line);
            int value;
            linestream >> key >> value;
            if ("processes" == key) {
                return value;
            }
        }
    }
    return 0;
 }

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
    std::ifstream filestream(kProcDirectory + kStatFilename);
    if (filestream.is_open()) {
        std::string key;
        std::string line;
        while (std::getline(filestream, line)) {
            std::istringstream linestream(line);
            int value;
            linestream >> key >> value;
            if ("procs_running" == key) {
                return value;
            }
        }
    }
    return 0;

 }

// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
   std::ifstream filestream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
    if (filestream.is_open()) {
        std::string line;
        std::getline(filestream, line);
        return line;
    }
    return string();
 }

// DONE: Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
        std::string key;
        std::string line;
        while (std::getline(filestream, line)) {
            std::istringstream linestream(line);
            std::string value;
            linestream >> key >> value;
            if ("VmSize:" == key) {
                return std::to_string(std::stol(value) / 1024);
            }
        }
    }
    return std::string();

}

// DONE: Read and return the user ID associated with a process
std::string LinuxParser::Uid(int pid) { 
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
    if (filestream.is_open()) {
        string key, line;
        while (std::getline(filestream, line)) {
            std::istringstream linestream(line);
            string key, value;
            while (linestream >> key >> value) {
                if ("Uid:" == key) {
                    return value;
                }
            }
        }
    }
    return std::string();
}

// DONE: Read and return the user associated with a process
string LinuxParser::User(int pid) { 
      std::ifstream filestream(kPasswordPath);
    if (filestream.is_open()) {
        std::string line;
        while (std::getline(filestream, line)) {
            std::replace(line.begin(), line.end(), ':', ' ');
            std::istringstream linestream(line);
            std::string value;
            std::string name;
            std::string x;
            while (linestream >> name >> x >> value) {
                if (value == Uid(pid)) {
                    return name;
                }
            }
        }
    }
    return std::string();
 }

// DONE: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) { 
      std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
    if (filestream.is_open()) {
        std::string line;
        while (std::getline(filestream, line)) {
            std::string value;
            int counter = 0;
            std::istringstream linestream(line);
            while (linestream >> value) {
                //see https://linux.die.net/man/5/proc for details
                if (23 == counter) {
                    //convert jiffies to seconds: seconds = jiffies/HZ
                    long up = std::stol(value)/sysconf(_SC_CLK_TCK);
                    return up;
                }
                counter++;
            }
        }
    }
    return 0;
 }