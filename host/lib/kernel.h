#pragma once
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <CL/cl.hpp>
#include <iostream>
#include <Windows.h>
#include <fstream>
#include <string>

class kernel {

 protected:
  
  std::string clKernelCode;
  std::vector<cl::Kernel> clKernelFunctions;
  cl::Program::Sources clSources;
  std::vector<cl::Buffer> clBuffers;

 public:
  
  std::string ReadKernelCode(std::string myFileName);
  void SetSources(std::string myKernelCode);
  void GetKernelCodeInfo();
  void SetKernelFunction(std::string myFunctionName, cl::Program& myProgram);
  cl::Kernel* GetFunctionPoint(unsigned int index);
  void SetBuffer(cl::Context myContext, size_t mySize);
  cl::Buffer GetBuffer(unsigned int index);
  void SetArg(cl::Kernel& myKernelFunction, unsigned int numbArg,
              cl::Buffer myBuffer);
  cl::Program::Sources GetSources();
  
  kernel();
  ~kernel();
};

kernel::kernel() {}
kernel::~kernel() {}

std::string kernel::ReadKernelCode(std::string myFileName) {
  std::ifstream myFile(myFileName);
  if (!myFile.is_open()) {
    std::cout << "ERROR:";
    std::cout << "ReadKernelCode():";
    std::cout << myFileName << " isn't open.";
    exit(1);
  }
  myFile.seekg(0, std::ios::end);
  size_t size = myFile.tellg();
  std::string myKernelCode(size, ' ');
  myFile.seekg(0);
  myFile.read(&myKernelCode[0], size);
  myFile.close();

  return myKernelCode;
}

void kernel::SetSources(std::string myKernelCode) {
  clKernelCode = myKernelCode;
  clSources.push_back({clKernelCode.c_str(), clKernelCode.length()});
}

void kernel::GetKernelCodeInfo() { std::cout << clKernelCode << std::endl; }

void kernel::SetKernelFunction(std::string myFunctionName,
                               cl::Program& myProgram) {
  cl::Kernel kernelFunction(myProgram, myFunctionName.c_str());
  clKernelFunctions.push_back(kernelFunction);
}

cl::Program::Sources kernel::GetSources() { return clSources; }

void kernel::SetBuffer(cl::Context myContext, size_t mySize) {
  cl::Buffer myBuffer(myContext, CL_MEM_READ_WRITE, mySize);
  clBuffers.push_back(myBuffer);
}

cl::Buffer kernel::GetBuffer(unsigned int index) { return clBuffers[index]; }

cl::Kernel* kernel::GetFunctionPoint(unsigned int index) {
  return &clKernelFunctions[index];
}

void kernel::SetArg(cl::Kernel& myKernelFunction, unsigned int indexArg,
                    cl::Buffer myBuffer) {
  myKernelFunction.setArg(indexArg, myBuffer);
}
