#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <fstream>
#include <regex>
#include <string>

namespace LinuxParser {

  struct SystemJiffies{
    public:
    long active{0};
    long activeDelta{0};
    long total{0};
    long totalDelta{0};
    long idle{0};
    long idleDelta{0};
    long lastUTime{0};
    long lastUTimeDelta{0};
    const long minUTimeDelta{1};
  };

  static SystemJiffies* globalJiffies = new SystemJiffies();

// Paths
const std::string kProcDirectory{"/proc/"};
const std::string kCmdlineFilename{"/cmdline"};
const std::string kCpuinfoFilename{"/cpuinfo"};
const std::string kStatusFilename{"/status"};
const std::string kStatFilename{"/stat"};
const std::string kUptimeFilename{"/uptime"};
const std::string kMeminfoFilename{"/meminfo"};
const std::string kVersionFilename{"/version"};
const std::string kOSPath{"/etc/os-release"};
const std::string kPasswordPath{"/etc/passwd"};

// System
float MemoryUtilization();
long UpTime();
std::vector<int> Pids();
int TotalProcesses();
int RunningProcesses();
std::string OperatingSystem();
std::string Kernel();

// CPU
enum CPUStates {
  kUser_ = 0,
  kNice_,
  kSystem_,
  kIdle_,
  kIOwait_,
  kIRQ_,
  kSoftIRQ_,
  kSteal_,
  kGuest_,
  kGuestNice_
};
std::vector<std::string> CpuUtilization();
long Jiffies();
long ActiveJiffies(int pid);
long ActiveJiffies();
long IdleJiffies();
long JiffiesDelta();
long ActiveJiffiesDelta();
long IdleJiffiesDelta();

// Processes
std::string Command(int pid);
std::string Ram(int pid);
std::string Uid(int pid);
std::string User(int pid);
long int UpTime(int pid);

// Helper functions
template <typename T> T KeyValLookup(std::string filePath, std::string key);
long CumulativeCPUStat(std::string path, int firstEntry, int lastEntry, int targetRow = 0);
void UpdateGlobalJiffies();
long GetRawJiffies_Active();
long GetRawJiffies_Idle();

};  // namespace LinuxParser

#endif