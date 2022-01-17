#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

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
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization()
{
  // return system memory use (percentage?)
  return 0.0;
}

// DONE : Read and return the system uptime
long LinuxParser::UpTime()
{
  // https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/6/html/deployment_guide/s2-proc-uptime
  // Can disregard the decimal of the string in /proc/uptime file.
  // Used LinuxParser::Kernel() code refactored for long int.
  long uptime;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return uptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies()
{
  // return total jiffies for system (since boot?)
  return 0;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]])
{
  // return active jiffies for a given process
  return 0;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies()
{
  // return active jiffies for system (since boot?)
  return 0;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies()
{
  // return idle jiffies for system (since boot?)
  return 0;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization()
{
  // return a vector of cpu utilisation values
  return {};
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses()
{
  // /proc/stat has "processes" field
  // return total of all processes
  return 0;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses()
{
  // /proc/status has running procs as a field
  return LinuxParser::KeyValLookup<int>(kProcDirectory + kStatFilename, "procs_running");
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid[[maybe_unused]])
{
  // easy: /proc/[pid]/cmdline
  // return the command of a given process
  return string();
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid[[maybe_unused]])
{
  // https://man7.org/linux/man-pages/man5/proc.5.html
  // /proc/[pid]/status has VmXXX for memory mappings for fast values
  // statm values match status for what we need
  // smaps is more accurate but much slower.

  // return string of memory used by a given process
  return string();
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid[[maybe_unused]])
{
  // from /proc/[pid]/status   grab string starting "Uid:"
  //  just take first field (real uid) for now
  // return the string representation of the userID of a given process
  return string();
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid[[maybe_unused]])
{
  // scan /etc/passwd for the uid, extract name
  // return the user string of a given process
  return string();
}

// TEST : Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid)
{
  // https://man7.org/linux/man-pages/man5/proc.5.html
  // Field 22 of /proc/[pid]/stat is utime
  // Used LinuxParser::Kernel() code refactored for long int.
  int targetField = 22;
  long uptime;
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    for(int i = 0; i < targetField; i++)
    {
      linestream >> uptime;
    }
    
  }
  return uptime;
  // return the UpTime of a given process.
  return 0;
}

// Helper function
template <typename T> T LinuxParser::KeyValLookup(string _filePath, string _key)
{
  string line, key;
  T val; // istream arbitrarily takes characters until ws, it doesn't care what.
  T returnVal;

  // repurposed from LinuxParser::OperatingSystem
  std::ifstream filestream(_filePath);
  if (filestream.is_open())
  {
    while (std::getline(filestream, line))
    {
      std::istringstream linestream(line);
      while (linestream >> key >> val)
      {
        if (key == _key)
        {
          returnVal = val;
          break;
        }
      }
    }
  }

  return returnVal;
}