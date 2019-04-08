#pragma once
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <CL/cl.hpp>
#include <fstream>
#include <iostream>
#include <string>

class device {
 protected:
  // Platforms
  std::vector<cl::Platform> allPlatforms;
  std::vector<cl::Platform> allPlatformCPU;
  std::vector<cl::Platform> allPlatformGPU;

  cl::Platform defaultPlatformCPU;
  cl::Platform defaultPlatformGPU;

  // Devices
  std::vector<cl::Device> allDevices;
  std::vector<cl::Device> allDevicesCPU;
  std::vector<cl::Device> allDevicesGPU;

  cl::Device defaultDeviceCPU;
  cl::Device defaultDeviceGPU;

  cl::Context clContext;

 public:
  void SetDevices();
  void GetDevicesInfo();

  void SetContext();
  cl::Context GetContext();

  cl::Device GetCPU();
  cl::Device GetGPU();

  device();
  ~device();
};

device::device() {
  SetDevices();
  SetContext();
}

device::~device() {}

void device::SetDevices() {
  cl::Platform::get(&allPlatforms);

  for (int i = 0; i < allPlatforms.size(); i++) {
    allPlatforms[i].getDevices(CL_DEVICE_TYPE_GPU, &allDevicesGPU);
    allPlatforms[i].getDevices(CL_DEVICE_TYPE_CPU, &allDevicesCPU);

    if (allDevicesGPU.size() != 0 && allDevicesCPU.size() != 0) {
      if (i < allDevicesCPU.size() && i < allDevicesGPU.size()) {
        defaultDeviceGPU = allDevicesGPU[i];
        defaultDeviceCPU = allDevicesCPU[i];
        defaultPlatformGPU = allPlatforms[i];
        defaultPlatformCPU = allPlatforms[i];
        break;
      }
    }
  }
}

void device::GetDevicesInfo() {
  std::string myInfo;

  std::cout << std::endl << "CPU: " << std::endl;
  myInfo = defaultPlatformCPU.getInfo<CL_PLATFORM_NAME>();
  std::cout << std::endl << myInfo << std::endl;
  myInfo = defaultDeviceCPU.getInfo<CL_DEVICE_NAME>();
  std::cout << std::endl << myInfo << std::endl;

  std::cout << std::endl << "GPU: " << std::endl;
  myInfo = defaultPlatformGPU.getInfo<CL_PLATFORM_NAME>();
  std::cout << std::endl << myInfo << std::endl;
  myInfo = defaultDeviceGPU.getInfo<CL_DEVICE_NAME>();
  std::cout << std::endl << myInfo << std::endl;
}

void device::SetContext() {
  cl::Context myContext({defaultDeviceCPU, defaultDeviceGPU});
  clContext = myContext;
}

cl::Context device::GetContext() { return clContext; }

cl::Device device::GetCPU() { return defaultDeviceCPU; }
cl::Device device::GetGPU() { return defaultDeviceGPU; }
