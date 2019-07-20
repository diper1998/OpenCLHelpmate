#pragma once
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <CL/cl.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include "device.h"
#include "kernel.h"
#include "program.h"
#include "windows.h"

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
                   size_t sizeArgument, void* ptrData,
                   std::string myDevice = "GPU");
  void Start(std::string myDevice, unsigned int blockingStart,
             unsigned int indexFunction, unsigned int offset,
             size_t globalRange, size_t localRange);

  void Start(std::string myDevice, unsigned int blockingStart,
             unsigned int indexFunction, unsigned int myOffsetX,
             unsigned int myOffsetY, size_t myGlobalRangeX,
             size_t myGlobalRangeY, size_t myLocalRangeX, size_t myLocalRangeY);

  void Start(std::string myDevice, unsigned int blockingStart,
             unsigned int indexFunction, unsigned int myOffsetX,
             unsigned int myOffsetY, unsigned int myOffsetZ,
             size_t myGlobalRangeX, size_t myGlobalRangeY,
             size_t myGlobalRangeZ, size_t myLocalRangeX, size_t myLocalRangeY,
             size_t myLocalRangeZ);

  void Execute(unsigned int indexFunction, size_t globalRange,
               size_t localRange, unsigned int percentGPU);
  void Execute(unsigned int indexFunction, size_t globalRangeX,
               size_t globalRangeY, size_t localRangeX, size_t localRangeY,
               unsigned int percentGPU, std::string parallelParametrXY);
  void Execute(unsigned int indexFunction, size_t globalRangeX,
               size_t globalRangeY, size_t globalRangeZ, size_t localRangeX,
               size_t localRangeY, size_t localRangeZ,
               unsigned int percentGPU, std::string parallelParametrXYZ);

  void Test(unsigned int indexFunction, size_t globalRangeX,
            size_t globalRangeY, size_t localRangeX, size_t localRangeY,
            std::string parsllelParametrXY);

  void Test(unsigned int indexFunction, size_t globalRange, size_t localRange);

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
  // clKernel.GetKernelCodeInfo();
}

void helpmate::SetProgram() {
  clProgram.SetProgram(clDevice.GetContext(), clKernel.GetSources(),
                       clDevice.GetCPU(), clDevice.GetGPU());
  clProgram.BuildProgram(clDevice.GetCPU(), clDevice.GetGPU());
  std::cout << std::endl;
  clProgram.GetBuildInfo(clDevice.GetCPU(), clDevice.GetGPU());
  std::cout << std::endl;
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
  std::cout << std::endl;
  clDevice.GetDevicesInfo();
  std::cout << std::endl;
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

void helpmate::SetArgument(unsigned int indexFunction, unsigned int indexArg,
                           size_t sizeArgument, void* ptrData,
                           std::string myDevice) {
  sizesData.push_back(sizeArgument);

  ptrsData.push_back(ptrData);

  unsigned int indexBuffer = clKernel.GetSizeBuffers();

  clKernel.SetBuffer(clDevice.GetContext(), sizeArgument);

  if (ptrData != NULL) {
    WriteBuffer(myDevice, indexArg, CL_TRUE, 0, sizeArgument, ptrData);

    clKernel.SetArg(*clKernel.GetFunctionPoint(indexFunction), indexArg,
                    clKernel.GetBuffer(indexBuffer));
  } else {
    clKernel.SetArg(*clKernel.GetFunctionPoint(indexFunction), indexArg,
                    sizeArgument, ptrData);
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

void helpmate::Start(std::string myDevice, unsigned int blockingStart,
                     unsigned int indexFunction, unsigned int myOffsetX,
                     size_t myGlobalRangeX, size_t myLocalRangeX) {
  cl::NDRange globalRangeX = myGlobalRangeX;
  cl::NDRange localRangeX = myLocalRangeX;
  cl::NDRange offsetX = myOffsetX;

  if (myLocalRangeX == 0) localRangeX = cl::NullRange;

  int myStatus;

  if (myDevice == "CPU" && myGlobalRangeX != 0) {
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

  if (myDevice == "GPU" && myGlobalRangeX != 0) {
    myStatus = commandQueueGPU.enqueueNDRangeKernel(
        *clKernel.GetFunctionPoint(indexFunction), offsetX, globalRangeX,
        localRangeX);

    if (myStatus != CL_SUCCESS) {
      std::cout << "ERROR: startGPU " << myStatus << std::endl;
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

  if (myDevice == "CPU" && (myGlobalRangeX != 0 && myGlobalRangeY != 0)) {
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

  if (myDevice == "GPU" && (myGlobalRangeX != 0 && myGlobalRangeY != 0)) {
    myStatus = commandQueueGPU.enqueueNDRangeKernel(
        *clKernel.GetFunctionPoint(indexFunction), offset, globalRange,
        localRange);

    if (myStatus != CL_SUCCESS) {
      std::cout << "ERROR: startGPU " << myStatus << std::endl;
    }

    if (blockingStart) {
      commandQueueGPU.finish();
    }
  }
}

inline void helpmate::Start(std::string myDevice, unsigned int blockingStart,
                            unsigned int indexFunction, unsigned int myOffsetX,
                            unsigned int myOffsetY, unsigned int myOffsetZ,
                            size_t myGlobalRangeX, size_t myGlobalRangeY,
                            size_t myGlobalRangeZ, size_t myLocalRangeX,
                            size_t myLocalRangeY, size_t myLocalRangeZ) {

  cl::NDRange offset = cl::NDRange(myOffsetX, myOffsetY, myOffsetZ);
  cl::NDRange globalRange = cl::NDRange(myGlobalRangeX, myGlobalRangeY, myGlobalRangeZ);
  cl::NDRange localRange = cl::NDRange(myLocalRangeX, myLocalRangeY, myLocalRangeZ);

  if (myLocalRangeX == 0 && myLocalRangeY == 0 && myLocalRangeZ == 0) {
    localRange = cl::NullRange;
  }

  int myStatus;

  if (myDevice == "CPU" && (myGlobalRangeX != 0 && myGlobalRangeY != 0 && myGlobalRangeZ !=0)) {
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

  if (myDevice == "GPU" &&
      (myGlobalRangeX != 0 && myGlobalRangeY != 0 && myGlobalRangeZ != 0)) {
    myStatus = commandQueueGPU.enqueueNDRangeKernel(
        *clKernel.GetFunctionPoint(indexFunction), offset, globalRange,
        localRange);

    if (myStatus != CL_SUCCESS) {
      std::cout << "ERROR: startGPU " << myStatus << std::endl;
    }

    if (blockingStart) {
      commandQueueGPU.finish();
    }
  }
}

void helpmate::Execute(unsigned int indexFunction, size_t globalRange,
                       size_t localRange, unsigned int percentGPU) {
  size_t cpuSizeOffset = 0;
  size_t gpuSizeOffset = 0;
  size_t cpuSize = 0;
  size_t gpuSize = 0;

  gpuSize = int((globalRange * percentGPU) / 100) +
            ceil((globalRange * percentGPU) % 100);

  if (localRange != 0) {
    while (gpuSize % localRange != 0) {
      gpuSize++;
    }
  }

  cpuSizeOffset = gpuSize;

  cpuSize = globalRange - cpuSizeOffset;

  Start("GPU", CL_FALSE, indexFunction, gpuSizeOffset, gpuSize, localRange);
  Start("CPU", CL_FALSE, indexFunction, cpuSizeOffset, cpuSize, localRange);
  Wait("GPU");
  Wait("CPU");

  for (int i = 0; i < ptrsData.size(); i++) {
    ReadBuffer("GPU", i, CL_TRUE, 0, sizesData[i], ptrsData[i]);
  }
}

void helpmate::Execute(unsigned int indexFunction, size_t globalRangeX,
                       size_t globalRangeY, size_t localRangeX,
                       size_t localRangeY, unsigned int percentGPU,
                       std::string parallelParameterXY) {
  size_t gpuSizeOffsetX = 0;
  size_t gpuSizeOffsetY = 0;
  size_t gpuSizeX = 0;
  size_t gpuSizeY = 0;
  size_t cpuSizeOffsetX = 0;
  size_t cpuSizeOffsetY = 0;
  size_t cpuSizeX = 0;
  size_t cpuSizeY = 0;

  if (parallelParameterXY == "X") {
    gpuSizeX = int((globalRangeX * percentGPU) / 100) +
               ceil((globalRangeX * percentGPU) % 100);

    if (gpuSizeX > globalRangeX) gpuSizeX = globalRangeX;

    gpuSizeY = globalRangeY;

    if (localRangeX != 0) {
      while (gpuSizeX % localRangeX != 0) gpuSizeX++;
    }

    cpuSizeOffsetX = gpuSizeX;
    cpuSizeOffsetY = 0;

    cpuSizeX = globalRangeX - cpuSizeOffsetX;
    cpuSizeY = globalRangeY;
  }

  if (parallelParameterXY == "Y") {
    gpuSizeY = int((globalRangeY * percentGPU) / 100) +
               ceil((globalRangeY * percentGPU) % 100);
    if (gpuSizeY > globalRangeY) gpuSizeY = globalRangeY;

    gpuSizeX = globalRangeX;


    if (localRangeY != 0) {
      while (gpuSizeY % localRangeY != 0) gpuSizeY++;
    }

    cpuSizeOffsetY = gpuSizeY;
    cpuSizeOffsetX = 0;

    cpuSizeY = globalRangeY - cpuSizeOffsetY;
    cpuSizeX = globalRangeX;
  }

  if (parallelParameterXY == "XY") {
    gpuSizeY = int((globalRangeY * percentGPU) / 100) +
               ceil((globalRangeY * percentGPU) % 100);
    if (gpuSizeY > globalRangeY) gpuSizeY = globalRangeY;

    gpuSizeX = int((globalRangeX * percentGPU) / 100) +
               ceil((globalRangeX * percentGPU) % 100);
    if (gpuSizeX > globalRangeX) gpuSizeX = globalRangeX;

    if (localRangeX != 0) {
      while (gpuSizeX % localRangeX != 0) gpuSizeX++;
    }

    if (localRangeY != 0) {
      while (gpuSizeY % localRangeY != 0) gpuSizeY++;
    }

    cpuSizeOffsetY = gpuSizeY;
    cpuSizeOffsetX = gpuSizeX;

    cpuSizeY = globalRangeY - cpuSizeOffsetY;
    cpuSizeX = globalRangeX - cpuSizeOffsetX;
  }

  Start("GPU", CL_FALSE, indexFunction, gpuSizeOffsetX, gpuSizeOffsetY,
        gpuSizeX, gpuSizeY, localRangeX, localRangeY);
  Start("CPU", CL_FALSE, indexFunction, cpuSizeOffsetX, cpuSizeOffsetY,
        cpuSizeX, cpuSizeY, localRangeX, localRangeY);
  Wait("GPU");
  Wait("CPU");

  for (int i = 0; i < ptrsData.size(); i++) {
    ReadBuffer("GPU", i, CL_TRUE, 0, sizesData[i], ptrsData[i]);
  }
}

inline void helpmate::Execute(unsigned int indexFunction, size_t globalRangeX,
                              size_t globalRangeY, size_t globalRangeZ,
                              size_t localRangeX, size_t localRangeY,
                              size_t localRangeZ, unsigned int percentGPU,
                              std::string parallelParameterXYZ) {

  size_t gpuSizeOffsetX = 0;
  size_t gpuSizeOffsetY = 0;
  size_t gpuSizeOffsetZ = 0;
  size_t gpuSizeX = 0;
  size_t gpuSizeY = 0;
  size_t gpuSizeZ = 0;
  size_t cpuSizeOffsetX = 0;
  size_t cpuSizeOffsetY = 0;
  size_t cpuSizeOffsetZ = 0;
  size_t cpuSizeX = 0;
  size_t cpuSizeY = 0;
  size_t cpuSizeZ = 0;

  if (parallelParameterXYZ == "X") {
    gpuSizeX = int((globalRangeX * percentGPU) / 100) +
               ceil((globalRangeX * percentGPU) % 100);

    if (gpuSizeX > globalRangeX) gpuSizeX = globalRangeX;

    gpuSizeY = globalRangeY;
    gpuSizeZ = globalRangeZ;

    if (localRangeX != 0) {
      while (gpuSizeX % localRangeX != 0) gpuSizeX++;
    }

    cpuSizeOffsetX = gpuSizeX;
    cpuSizeOffsetY = 0;
    cpuSizeOffsetZ = 0;

    cpuSizeX = globalRangeX - cpuSizeOffsetX;
    cpuSizeY = globalRangeY;
    cpuSizeZ = globalRangeZ;
  }

  if (parallelParameterXYZ == "Y") {
    gpuSizeY = int((globalRangeY * percentGPU) / 100) +
               ceil((globalRangeY * percentGPU) % 100);
    if (gpuSizeY > globalRangeY) gpuSizeY = globalRangeY;

    gpuSizeX = globalRangeX;
    gpuSizeZ = globalRangeZ;

    if (localRangeY != 0) {
      while (gpuSizeY % localRangeY != 0) gpuSizeY++;
    }

    cpuSizeOffsetY = gpuSizeY;
    cpuSizeOffsetX = 0;

    cpuSizeY = globalRangeY - cpuSizeOffsetY;
    cpuSizeX = globalRangeX;
    cpuSizeZ = globalRangeZ;
  }

    if (parallelParameterXYZ == "Z") {
    gpuSizeZ = int((globalRangeY * percentGPU) / 100) +
               ceil((globalRangeY * percentGPU) % 100);
    if (gpuSizeZ > globalRangeY) gpuSizeZ = globalRangeY;

    gpuSizeX = globalRangeX;
    gpuSizeY = globalRangeY;


    if (localRangeZ != 0) {
      while (gpuSizeZ % localRangeZ != 0) gpuSizeY++;
    }

    cpuSizeOffsetZ = gpuSizeZ;
    cpuSizeOffsetX = 0;
    cpuSizeOffsetY = 0;

    cpuSizeZ = globalRangeZ - cpuSizeOffsetZ;
    cpuSizeX = globalRangeX;
    cpuSizeY = globalRangeY;
  }


 
  Start("GPU", CL_FALSE, indexFunction, gpuSizeOffsetX, gpuSizeOffsetY, gpuSizeOffsetZ,
        gpuSizeX, gpuSizeY, gpuSizeZ, localRangeX, localRangeY, localRangeZ);
  Start("CPU", CL_FALSE, indexFunction, cpuSizeOffsetX, cpuSizeOffsetY, cpuSizeOffsetZ,
        cpuSizeX, cpuSizeY, cpuSizeZ, localRangeX, localRangeY, localRangeZ);
  Wait("GPU");
  Wait("CPU");

  for (int i = 0; i < ptrsData.size(); i++) {
    ReadBuffer("GPU", i, CL_TRUE, 0, sizesData[i], ptrsData[i]);
  }

}

void helpmate::Test(unsigned int indexFunction, size_t globalRangeX,
                    size_t globalRangeY, size_t localRangeX, size_t localRangeY,
                    std::string parallelParametrXY) {
  LARGE_INTEGER frequency;
  LARGE_INTEGER t1, t2;
  double time[11];
  QueryPerformanceFrequency(&frequency);

  std::cout << std::endl;
  std::cout << "Work time for parallelParameter = " << parallelParametrXY << ":"
            << std::endl;

  for (int i = 0; i < 11; i++) {
    QueryPerformanceCounter(&t1);
    Execute(indexFunction, globalRangeX, globalRangeY, localRangeX, localRangeY,
            i * 10, parallelParametrXY);
    QueryPerformanceCounter(&t2);
    time[i] = (t2.QuadPart - t1.QuadPart) / double(frequency.QuadPart);

    std::cout << "GPU: " << i * 10 << "% ";

    std::cout << "CPU: " << (10 - i) * 10 << "%";

    std::cout << " = " << time[i] << " sec" << std::endl;
  }

  double bestTime = time[0];
  int bestIndex = 0;
  for (int j = 0; j < 11; j++) {
    if (time[j] < bestTime) {
      bestTime = time[j];
      bestIndex = j;
    }
  }

  std::cout << std::endl;

  std::cout << "BEST TIME:" << std::endl;

  std::cout << "Work time for parallelParameter = " << parallelParametrXY << ":"
            << std::endl;

  std::cout << "GPU: " << bestIndex * 10 << "% ";

  std::cout << "CPU: " << (10 - bestIndex) * 10 << "%";

  std::cout << " = " << bestTime << " sec" << std::endl;
}

void helpmate::Test(unsigned int indexFunction, size_t globalRange,
                    size_t localRange) {
  LARGE_INTEGER frequency;
  LARGE_INTEGER t1, t2;
  double time[11];
  QueryPerformanceFrequency(&frequency);

  std::cout << "Work time:" << std::endl;
  for (int i = 0; i < 11; i++) {
    QueryPerformanceCounter(&t1);
    Execute(indexFunction, globalRange, localRange, i * 10);
    QueryPerformanceCounter(&t2);
    time[i] = (t2.QuadPart - t1.QuadPart) / double(frequency.QuadPart);

    std::cout << "GPU: " << i * 10 << "% ";
    std::cout << "CPU: " << (10 - i) * 10 << "%";
    std::cout << " = " << time[i] << " sec" << std::endl;
  }

  double bestTime = time[0];
  int bestIndex = 0;
  for (int j = 0; j < 11; j++) {
    if (time[j] < bestTime) {
      bestTime = time[j];
      bestIndex = j;
    }
  }

  std::cout << std::endl;
  std::cout << "BEST TIME:" << std::endl;
  std::cout << "GPU: " << bestIndex * 10 << "% ";
  std::cout << "CPU: " << (10 - bestIndex) * 10 << "%";
  std::cout << " = " << bestTime << " sec" << std::endl;
}
