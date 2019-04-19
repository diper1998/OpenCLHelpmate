#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <CL/cl.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include "device.h"
#include "kernel.h"

class program {

 protected:
  
     cl::Program clProgram;

 public:

  void SetProgram(cl::Context myContext, cl::Program::Sources mySources,
                  cl::Device myCPU, cl::Device myGPU);

  void BuildProgram(cl::Device myCPU, cl::Device myGPU);
  void GetBuildInfo(cl::Device myCPU, cl::Device myGPU);

  cl::Program* GetProgramPoint();

  program();
  ~program();
};

program::program() {}
program::~program() {}

void program::SetProgram(cl::Context myContext, cl::Program::Sources mySources,
                         cl::Device myCPU, cl::Device myGPU) {
  cl::Program myProgram(myContext, mySources);
  clProgram = myProgram;
}

void program::BuildProgram(cl::Device myCPU, cl::Device myGPU) {
  if (clProgram.build({myCPU, myGPU}) != CL_SUCCESS) {
    std::cout << "ERROR SetProgram(): "
              << clProgram.getBuildInfo<CL_PROGRAM_BUILD_LOG>(myCPU)
              << std::endl
              << clProgram.getBuildInfo<CL_PROGRAM_BUILD_LOG>(myGPU)
              << std::endl;
    exit(1);
  }
}

 void program::GetBuildInfo(cl::Device myCPU, cl::Device myGPU) {
    std::cout << "CPU:" << std::endl
              << clProgram.getBuildInfo<CL_PROGRAM_BUILD_LOG>(myCPU)
              << std::endl
              << "GPU:" << std::endl
              << clProgram.getBuildInfo<CL_PROGRAM_BUILD_LOG>(myGPU)
              << std::endl;
  }

cl::Program* program::GetProgramPoint(){ return &clProgram;}

