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
  int Pid();                               // TODO: See src/process.cpp
  std::string User();                      // TODO: See src/process.cpp
  std::string Command();                   // TODO: See src/process.cpp
  float CpuUtilization();                  // TODO: See src/process.cpp
  std::string Ram();                       // TODO: See src/process.cpp
  long int UpTime();                       // TODO: See src/process.cpp
  bool operator<(Process const& a) const;  // TODO: See src/process.cpp

  // TODO: Declare any necessary private members
 private:
    int pid_;
    // std::string user_;
    // std::string command_;
    // float cpuUtilization_;
    // std::string ram_;
    // long int uptime_;
};

#endif