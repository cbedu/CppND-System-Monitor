#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
    long previousActiveJiffies_{0};
    long previousIdleJiffies_{0};
    long previousTotalJiffies_{0};
};

#endif