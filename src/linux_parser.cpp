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

// DONE : Read and return the system memory utilization
float LinuxParser::MemoryUtilization()
{
  // Don't use MemAvailable
  // (https://superuser.com/questions/980820/what-is-the-difference-between-memfree-and-memavailable-in-proc-meminfo)
  float memFree = LinuxParser::KeyValLookup<float>(kProcDirectory + kMeminfoFilename, "MemFree:");
  float memTotal = LinuxParser::KeyValLookup<float>(kProcDirectory + kMeminfoFilename, "MemTotal:");
  float memUsed = memTotal - memFree;

  // return system memory in use (including cached)
  return memUsed / memTotal;
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

/// Info from 'man proc 5'
//  Jiffies in /proc/stat:
//  cpu [1] [2] ... [9] [10]
//
// where:
//  [1]  user       - user mode active
//  [2]  nice       - user mode active in low priority
//  [3]  system     - system mode active
//  [4]  idle       - idle task, USER_HZ * /proc/uptime
//  [5]  iowait     - time waiting for i/o, unreliable
//  [6]  irq        - time servicing interrupts
//  [7]  softirq    - time servicing softirqs
//  [8]  steal      - time in other o/s when running in virtual envs
//  [9]  guest      - running a vCPU for vGuest under kernel control
//  [10] guest_nice - running a vCPU for a 'niced' vGuest under kernel control
// DONE : Read and return the number of jiffies for the system
long LinuxParser::Jiffies()
{
  LinuxParser::UpdateGlobalJiffies();

  return globalJiffies->total;
}

long LinuxParser::JiffiesDelta()
{
  LinuxParser::UpdateGlobalJiffies();

  return globalJiffies->totalDelta;
}

// DONE: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]])
{
  // man proc 5  (see proc/[pid]/stat  (14) ... (17))
  // return active jiffies for a given process
  // single line
  return CumulativeCPUStat(kProcDirectory+to_string(pid)+kStatFilename,14,17);
}

// This helper function is called by all LinuxParser functions related to global Jiffies.
// This function will mask a time based gate on getting updated jiffy information.
// This will make percentage use calcs more stable and make process operation more
// predicatble.
void LinuxParser::UpdateGlobalJiffies()
{
  long tempUTime = LinuxParser::UpTime();
  long tempUTimeDelta = tempUTime - globalJiffies->lastUTime;
  
  // bail if we don't want to waste the time
  if(tempUTimeDelta < globalJiffies->minUTimeDelta)
    return;

  // update our last update time
  globalJiffies->lastUTime = tempUTime;
  globalJiffies->lastUTimeDelta = tempUTimeDelta;

  long tempActive = LinuxParser::GetRawJiffies_Active();
  globalJiffies->activeDelta = tempActive - globalJiffies->active;
  globalJiffies->active = tempActive;

  long tempIdle = LinuxParser::GetRawJiffies_Idle();
  globalJiffies->idleDelta = tempIdle - globalJiffies->idle;
  globalJiffies->idle = tempIdle;

  long tempTotal = tempActive + tempIdle;
  globalJiffies->totalDelta = tempTotal - globalJiffies->total;
  globalJiffies->total = tempTotal;
}

//  Directly Active Jiffies = [1] user + [2] nice + [3] system
//  Passively Active Jiffies = [6] irq + [7] softirq + [8] steal
//                              + [9] guest + [10] guest_nice
//
//    *[8] steal  is included because it is time that the CPU cannot be idle and indicates 
//      physical CPU use
long LinuxParser::GetRawJiffies_Active()
{
  int part1_IndexFirst = 1;
  int part1_IndexLast = 3;
  int part2_IndexFirst = 6;
  int part2_IndexLast = 10;

  long total = CumulativeCPUStat(kProcDirectory+kStatFilename,part1_IndexFirst,part1_IndexLast);
  total += CumulativeCPUStat(kProcDirectory+kStatFilename,part2_IndexFirst,part2_IndexLast);
  
  return total;
}

//  Idle Jiffies = [4] idle + [5] iowait
long LinuxParser::GetRawJiffies_Idle()
{
  int indexFirst = 4;
  int indexLast = 5;
  
  return CumulativeCPUStat(kProcDirectory+kStatFilename,indexFirst,indexLast);
}

// DONE : Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies()
{
  LinuxParser::UpdateGlobalJiffies();
  return globalJiffies->active;
}

// DONE : Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies()
{
  LinuxParser::UpdateGlobalJiffies();
  
  return globalJiffies->idle;
}

long LinuxParser::ActiveJiffiesDelta()
{
  LinuxParser::UpdateGlobalJiffies();
  return globalJiffies->activeDelta;
}

long LinuxParser::IdleJiffiesDelta()
{
  LinuxParser::UpdateGlobalJiffies();
  
  return globalJiffies->idleDelta;
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization()
{
  // vector version of LinuxParser::UpTime()
  vector<string> cpus;
  std::string line;

  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open())
  {
    std::getline(stream, line);
    std::istringstream linestream(line);
    string target;
    while (linestream >> target)
    {
      cpus.push_back(target);
    }
  }

  // return a vector of cpu utilisation values
  return cpus;
}

// DONE : Read and return the total number of processes
int LinuxParser::TotalProcesses()
{
  // Processes vs threads:
  // LinuxParser::Pids().size() would give literal process id count.
  // Does not include threads (E.g. forks)

  // /proc/stat has "processes" field
  // return total of all processes
  return LinuxParser::KeyValLookup<int>(kProcDirectory + kStatFilename, "processes");
}

// DONE : Read and return the number of running processes
int LinuxParser::RunningProcesses()
{
  // /proc/status has running procs as a field
  return LinuxParser::KeyValLookup<int>(kProcDirectory + kStatFilename, "procs_running");
}

// DONE : Read and return the command associated with a process
string LinuxParser::Command(int pid)
{
  // easy: /proc/[pid]/cmdline
  string line = "";
  std::ifstream stream(kProcDirectory+to_string(pid)+kCmdlineFilename);

  if (stream.is_open())
  {
    std::getline(stream, line);
    return line;
  }

  // return the command of a given process
  return string();
}

// DONE: Read and return the memory used by a process
string LinuxParser::Ram(int pid)
{
  // https://man7.org/linux/man-pages/man5/proc.5.html
  // /proc/[pid]/status has VmXXX for memory mappings for fast values
  // statm values match status for what we need
  // smaps is more accurate but much slower.
  long ram = KeyValLookup<long>(kProcDirectory + to_string(pid) + kStatusFilename,"VmRSS:");
  double ram_MB = (double)ram / 1000.00;

  // prettify : https://stackoverflow.com/questions/14520309/the-precision-of-stdto-stringdouble#14520388
  std::string trimmedString = std::to_string(ram_MB).substr(0, std::to_string(ram_MB).find(".") + 3);

  // return string of memory used by a given process
  return trimmedString;
}

// DONE: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid)
{
  // from /proc/[pid]/status   grab string starting "Uid:"
  //  just take first field (real uid) for now
  // return the string representation of the userID of a given process
  std::string uid = LinuxParser::KeyValLookup<std::string>(kProcDirectory + to_string(pid) + kStatusFilename , "Uid:");
  return uid;
}

// DONE : Read and return the user associated with a process
string LinuxParser::User(int pid)
{
  // scan /etc/passwd for the uid, extract name
  // return the user string of a given process

  // could change the ctype delimiter to use ':' but that's a bit heavy here.
  // repurpose the OperatingSystem() code to replace ':' with ws.
  string line;
  string nameStr;
  string throwAway;
  string uidStr;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> nameStr >> throwAway >> uidStr)
      {
          if(LinuxParser::Uid(pid) == uidStr)
            return nameStr;
      }
    }
  }
  return "n/a";
}

// DONE : Read and return the uptime of a process
long LinuxParser::UpTime(int pid)
{
  // https://man7.org/linux/man-pages/man5/proc.5.html
  // Field 22 of /proc/[pid]/stat is utime
  // Used LinuxParser::Kernel() code refactored for long int.
  int targetField = 22;
  long uptime;
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
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
}

//// Helper functions
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

// targetRow optional.
// global CPU is first row, so default value
// if CPU[1...n] wanted then offset row by CPU number (1 based)
long LinuxParser::CumulativeCPUStat(string path, int firstEntry, int lastEntry, int targetRow)
{
  string line;
  string key;
  int rowIndex = 0;
  int entryIndex = 0;
  string value;
  long acc = 0;
  std::ifstream filestream(path);

  if (filestream.is_open())
  {
    while (std::getline(filestream, line))
    {
      if(rowIndex == targetRow) // get our target row (E.g. specific CPU entry)
      {
        std::istringstream linestream(line); // got whole line as stream
        linestream >> key; // discard non numeric entry
        entryIndex = 1;
        while (linestream >> value)
        {
          if(entryIndex >= firstEntry) //valid entry to accumulate
          {
            acc += strtol(value.c_str(), nullptr, 10);
          }

          if(entryIndex >= lastEntry) // used '>=' to optimise over '==' comparison
          {
            return acc;
          }

          entryIndex++;
        }

        rowIndex++;
      }
    }
  }
  return acc; // 0 if no entry was found
}