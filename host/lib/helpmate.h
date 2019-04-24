#pragma once
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <CL/cl.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include "device.h"
#include "kernel.h"
#include "program.h"

class helpmate {
 protected:

  kernel clKernel;
  device clDevice;
  program clProgram;

  cl::CommandQueue commandQueueCPU;
  cl::CommandQueue commandQueueGPU;

  std::vector<size_t> sizesData;
  std::vector<void*> ptrsData;

 public:

  void SetKernel(std::string myFileName);
  void SetKernelFunctions(std::vector<std::string> myFunctionsNames);
  void SetProgram();
  void SetDevices();
  void SetBuffers(std::vector<size_t> mySizes);
  void SetCommandQueues();
  void WriteBuffer(std::string myDevice, unsigned int bufferIndex,
                   unsigned int blockingWrite, unsigned int myOffset,
                   size_t bufferSize, const void* ptrOrigin);
  void SetArgument(unsigned int indexFunction, unsigned int indexArg,
                   unsigned int indexBuffer);

  void SetArgument(unsigned int indexFunction, unsigned int indexArg,
                   size_t sizeArgument, void* ptrData, std::string myDevice = "GPU");
  void Start(std::string myDevice, unsigned int blockingStart,
             unsigned int indexFunction, unsigned int offset,
             size_t globalRange, size_t localRange);

  void Start(std::string myDevice, unsigned int blockingStart,
             unsigned int indexFunction, unsigned int myOffsetX,
             unsigned int myOffsetY, size_t myGlobalRangeX,
             size_t myGlobalRangeY, size_t myLocalRangeX, size_t myLocalRangeY);

  void Execute(unsigned int indexFunction, size_t globalRange,
               size_t localRange, std::string myMode);
  void Execute(unsigned int indexFunction, size_t globalRangeX,
               size_t globalRangeY, size_t localRangeX, size_t localRangeY,
               std::string myMode);


  void ReadBuffer(std::string myDevice, unsigned int bufferIndex,
                  unsigned int blockingRead, unsigned int myOffset,
                  size_t bufferSize, void* ptrResult);

  void Wait(std::string myDevice);


  helpmate(std::string myFileName, std::vector<std::string> myFunctionsNames);
  helpmate();
  ~helpmate();
};

helpmate::helpmate(std::string myFileName,
                   std::vector<std::string> myFunctionsNames) {
  SetDevices();
  SetKernel(myFileName);
  SetProgram();
  SetKernelFunctions(myFunctionsNames);
  SetCommandQueues();
}

helpmate::helpmate() {}

helpmate::~helpmate() {}

void helpmate::SetKernel(std::string myFileName) {
  clKernel.SetSources(clKernel.ReadKernelCode(myFileName));
  clKernel.GetKernelCodeInfo();
}

void helpmate::SetProgram() {
  clProgram.SetProgram(clDevice.GetContext(), clKernel.GetSources(),
                       clDevice.GetCPU(), clDevice.GetGPU());
  clProgram.BuildProgram(clDevice.GetCPU(), clDevice.GetGPU());
  clProgram.GetBuildInfo(clDevice.GetCPU(), clDevice.GetGPU());
}

void helpmate::SetKernelFunctions(std::vector<std::string> myFunctionsNames) {
  for (int i = 0; i < myFunctionsNames.size(); i++) {
    clKernel.SetKernelFunction(myFunctionsNames[i],
                               *clProgram.GetProgramPoint());
  }
};

void helpmate::SetDevices() {
  clDevice.SetDevices();
  clDevice.SetContext();
  clDevice.GetDevicesInfo();
}

void helpmate::SetBuffers(std::vector<size_t> mySizes) {
  for (int i = 0; i < mySizes.size(); i++) {
    clKernel.SetBuffer(clDevice.GetContext(), mySizes[i]);
  }
}

void helpmate::SetCommandQueues() {
  cl::CommandQueue myCommandQueueCPU(clDevice.GetContext(), clDevice.GetCPU());
  cl::CommandQueue myCommandQueueGPU(clDevice.GetContext(), clDevice.GetGPU());

  commandQueueCPU = myCommandQueueCPU;
  commandQueueGPU = myCommandQueueGPU;
}

void helpmate::WriteBuffer(std::string myDevice, unsigned int bufferIndex,
                           unsigned int blockingWrite, unsigned int myOffset,
                           size_t bufferSize, const void* pointOrigin) {
  int myStatus;
  if (myDevice == "CPU") {
    myStatus = commandQueueCPU.enqueueWriteBuffer(
        clKernel.GetBuffer(bufferIndex), blockingWrite, myOffset, bufferSize,
        pointOrigin);
  }
  if (myDevice == "GPU") {
    myStatus = commandQueueGPU.enqueueWriteBuffer(
        clKernel.GetBuffer(bufferIndex), blockingWrite, myOffset, bufferSize,
        pointOrigin);
  }

  if (myStatus != CL_SUCCESS) {
    std::cout << "ERROR: writing " << myStatus << std::endl;
  }
}

void helpmate::SetArgument(unsigned int indexFunction, unsigned int indexArg,
                           unsigned int indexBuffer) {
  clKernel.SetArg(*clKernel.GetFunctionPoint(indexFunction), indexArg,
                  clKernel.GetBuffer(indexBuffer));
}

void helpmate::SetArgument(unsigned int indexFunction, unsigned int indexArg, size_t sizeArgument,
                                  void* ptrData, std::string myDevice) {

    sizesData.push_back(sizeArgument);
  
    ptrsData.push_back(ptrData);

    unsigned int indexBuffer = clKernel.GetSizeBuffers();

    clKernel.SetBuffer(clDevice.GetContext(), sizeArgument);

    WriteBuffer(myDevice, indexArg, CL_TRUE, 0, sizeArgument, ptrData);
    
    clKernel.SetArg(*clKernel.GetFunctionPoint(indexFunction), indexArg,
                     clKernel.GetBuffer(indexBuffer));
}

void helpmate::Start(std::string myDevice, unsigned int blockingStart,
                     unsigned int indexFunction, unsigned int myOffsetX,
                     size_t myGlobalRangeX, size_t myLocalRangeX) {
  cl::NDRange globalRangeX = myGlobalRangeX;
  cl::NDRange localRangeX = myLocalRangeX;
  cl::NDRange offsetX = myOffsetX;

  if (myLocalRangeX == 0) localRangeX = cl::NullRange;

  int myStatus;

  if (myDevice == "CPU") {
    myStatus = commandQueueCPU.enqueueNDRangeKernel(
        *clKernel.GetFunctionPoint(indexFunction), offsetX, globalRangeX,
        localRangeX);

    if (myStatus != CL_SUCCESS) {
      std::cout << "ERROR: startCPU " << myStatus << std::endl;
    }

    if (blockingStart) {
      commandQueueCPU.finish();
    }
  }

  if (myDevice == "GPU") {
    myStatus = commandQueueGPU.enqueueNDRangeKernel(
        *clKernel.GetFunctionPoint(indexFunction), offsetX, globalRangeX,
        localRangeX);

    if (myStatus != CL_SUCCESS) {
      std::cout << "ERROR: start GPU " << myStatus << std::endl;
    }

    if (blockingStart) {
      commandQueueGPU.finish();
    }
  }
}

void helpmate::Start(std::string myDevice, unsigned int blockingStart,
                     unsigned int indexFunction, unsigned int myOffsetX,
                     unsigned int myOffsetY, size_t myGlobalRangeX,
                     size_t myGlobalRangeY, size_t myLocalRangeX,
                     size_t myLocalRangeY) {
  cl::NDRange offset = cl::NDRange(myOffsetX, myOffsetY);
  cl::NDRange globalRange = cl::NDRange(myGlobalRangeX, myGlobalRangeY);
  cl::NDRange localRange = cl::NDRange(myLocalRangeX, myLocalRangeY);

  if (myLocalRangeX == 0 && myLocalRangeY == 0) {
    localRange = cl::NullRange;
  }

  int myStatus;

  if (myDevice == "CPU") {
    myStatus = commandQueueCPU.enqueueNDRangeKernel(
        *clKernel.GetFunctionPoint(indexFunction), offset, globalRange,
        localRange);

    if (myStatus != CL_SUCCESS) {
      std::cout << "ERROR: startCPU " << myStatus << std::endl;
    }

    if (blockingStart) {
      commandQueueCPU.finish();
    }
  }

  if (myDevice == "GPU") {
    myStatus = commandQueueGPU.enqueueNDRangeKernel(
        *clKernel.GetFunctionPoint(indexFunction), offset, globalRange,
        localRange);

    if (myStatus != CL_SUCCESS) {
      std::cout << "ERROR: start GPU " << myStatus << std::endl;
    }

    if (blockingStart) {
      commandQueueGPU.finish();
    }
  }
}

inline void helpmate::Execute(unsigned int indexFunction, size_t globalRangeX,
                              size_t globalRangeY, size_t localRangeX,
                              size_t localRangeY, std::string myMode) {
  
    size_t cpuSizeOffsetX = 0;
    size_t cpuSizeOffsetY = 0;
    size_t gpuSizeOffsetX = 0;
    size_t gpuSizeOffsetY = 0;
    size_t cpuSizeX = 0;
    size_t cpuSizeY = 0;
    size_t gpuSizeX = 0;
    size_t gpuSizeY = 0;

  if (myMode == "DEF") {

    cpuSizeOffsetX = int(globalRangeX / 2) + globalRangeX % 2;
    cpuSizeOffsetY = 0;
    gpuSizeOffsetX = 0;
    gpuSizeOffsetY = 0;
    cpuSizeX = globalRangeX - cpuSizeOffsetX;
    cpuSizeY = globalRangeY;
    gpuSizeX = int(globalRangeX / 2) + globalRangeX % 2;
    gpuSizeY = globalRangeY;

    Start("GPU", CL_FALSE, indexFunction, gpuSizeOffsetX, gpuSizeOffsetY,
          gpuSizeX, gpuSizeY, localRangeX, localRangeY);
    Start("CPU", CL_FALSE, indexFunction, cpuSizeOffsetX, cpuSizeOffsetY,
          cpuSizeX, cpuSizeY, localRangeX, localRangeY);
      Wait("GPU");
      Wait("CPU");
  }

  for (int i = 0; i < ptrsData.size(); i++) {
    ReadBuffer("GPU", i, CL_TRUE, 0, sizesData[i], ptrsData[i]);
  }
  

}

void helpmate::ReadBuffer(std::string myDevice, unsigned int bufferIndex,
                          unsigned int blockingRead, unsigned int myOffset,
                          size_t bufferSize, void* ptrResult) {
  if (myDevice == "CPU") {
    commandQueueCPU.enqueueReadBuffer(clKernel.GetBuffer(bufferIndex),
                                      blockingRead, myOffset, bufferSize,
                                      ptrResult);

    if (blockingRead) {
      commandQueueCPU.finish();
    }
  }

  if (myDevice == "GPU") {
    commandQueueGPU.enqueueReadBuffer(clKernel.GetBuffer(bufferIndex),
                                      blockingRead, myOffset, bufferSize,
                                      ptrResult);
    if (blockingRead) {
      commandQueueGPU.finish();
    }
  }
}

void helpmate::Wait(std::string myDevice) {
  
    if (myDevice == "CPU") {
    commandQueueCPU.finish();
  }

    if (myDevice == "GPU") {
    commandQueueGPU.finish();
  }
}
