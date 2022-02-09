#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
    // Process(int _pid, std::string _user, std::string _command, float _cpuUtilization,
    //     std::string _ram, long int _uptime): pid_{_pid}, user_{_user}, command_{_command},
    //     cpuUtilization_{_cpuUtilization}, ram_{_ram}, uptime_{_uptime}
    //     {
    //         ;
    //     }
 public:
  Process(int _pid): pid_{_pid} { ; }
  int Pid();
  std::string User();
  std::string Command();
  float CpuUtilization();
  std::string Ram();
  long int UpTime();
  bool operator<(Process & a);

 private:
    int pid_;
    long prevActiveJiffies_{0};
    long prevTotalJiffies_{0};
    long prevUptime_{0};
    // std::string user_;
    // std::string command_;
    // float cpuUtilization_;
    // std::string ram_;
    // long int uptime_;
};

#endif