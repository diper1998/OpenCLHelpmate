#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <iostream>
#include "lib/helpmate.h"

void clSumVectors(const int size, int sizeOffset);
void clMulMatrix(const int size, int sizeOffset);

int main() {
 // clSumVectors(10000, 6000);
    clMulMatrix(1001, 500);
}

void clMulMatrix(const int size, int sizeOffset) {
  std::vector<std::string> functionsNames;
  functionsNames.push_back("mulMatrix");

  helpmate helpMate("kernelCode.cl", functionsNames);

  std::vector<size_t> mySizes;

  mySizes.push_back(size * size * sizeof(float));
  mySizes.push_back(size * size * sizeof(float));
  mySizes.push_back(size * size * sizeof(float));
  mySizes.push_back(sizeof(const int*));

  float* A = new float[size * size];
  float* B = new float[size * size];
  float* C = new float[size * size];

  for (int i = 0; i < size * size; i++) {
    A[i] = 1.0;
    B[i] = 2.0;
    C[i] = 0;
  }

  int* ptrSize = new int;
  *ptrSize = size;

  helpMate.SetArgument(0, 0, mySizes[0], A);
  helpMate.SetArgument(0, 1, mySizes[1], B);
  helpMate.SetArgument(0, 2, mySizes[2], C);
  helpMate.SetArgument(0, 3, mySizes[3], ptrSize);
  
 /* helpMate.Start("GPU", CL_FALSE, 0, 0, 0, sizeOffset, size, NULL, NULL);
  helpMate.Start("CPU", CL_FALSE, 0, sizeOffset, 0, size - sizeOffset, size, NULL, NULL);
 
  helpMate.Wait("GPU");
  helpMate.Wait("CPU");
  */

  helpMate.Execute(0, size, size, 0, 0, "DEF");



 // helpMate.ReadBuffer("GPU", 2, CL_FALSE, 0, mySizes[2], C);
 // helpMate.Wait("GPU");

  for (int i = 0; i < size * size; i++) {
    if (C[i] != size * 2){ 
      std::cout << "Error";
      break;
    }
  }
  /*
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
        std::cout << C[i*size+j] << " ";
    }
    std::cout << std::endl;
  }
  */
}

void clSumVectors(int size, int sizeOffset) {
  std::vector<std::string> functionsNames;
  functionsNames.push_back("sumVectors");

  helpmate helpMate("kernelCode.cl", functionsNames);

  std::vector<size_t> mySizes;

  mySizes.push_back(size * sizeof(float));
  mySizes.push_back(size * sizeof(float));
  mySizes.push_back(size * sizeof(float));

  float* A = new float[size];
  float* B = new float[size];
  float* C = new float[size];

  for (int i = 0; i < size; i++) {
    A[i] = 1.5;
    B[i] = 1.5;
  }

  helpMate.SetBuffers(mySizes);

  helpMate.WriteBuffer("GPU", 0, CL_FALSE, 0, mySizes[0], A);
  helpMate.WriteBuffer("GPU", 1, CL_FALSE, 0, mySizes[1], B);
  helpMate.WriteBuffer("GPU", 2, CL_FALSE, 0, mySizes[2], C);
  helpMate.Wait("GPU");

  helpMate.SetArgument(0, 0, 0);
  helpMate.SetArgument(0, 1, 1);
  helpMate.SetArgument(0, 2, 2);

  helpMate.Start("GPU", CL_FALSE, 0, 0, sizeOffset, NULL);
  helpMate.Start("CPU", CL_FALSE, 0, sizeOffset, size - sizeOffset, NULL);
  helpMate.Wait("GPU");
  helpMate.Wait("CPU");
  

  //helpMate.Execute();

  helpMate.ReadBuffer("GPU", 2, CL_TRUE, 0, mySizes[2], C);

  for (int i = 0; i < size; i++) {
    if (C[i] != 3) {
    std::cout << "Error";
      }
  }
}