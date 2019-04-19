#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <iostream>
#include "lib/helpmate.h"

void clSumVectors(const int size);
void clMulMatrix(const int size);
void clMulMatrixOpt(const int size, const int blockSize);
void clBlur(int size);

int main(int argc, char* argv[]) {
  std::string myFunction = "";
  size_t mySize = 0;

  
  myFunction = argv[1];
  mySize = atoi(argv[2]);

  LARGE_INTEGER frequency;
  LARGE_INTEGER t1, t2;
  double time;
  QueryPerformanceFrequency(&frequency);
  QueryPerformanceCounter(&t1);

  if (myFunction == "clMulMatrixOpt") clMulMatrixOpt(mySize, 16);

  if (myFunction == "clMulMatrix") clMulMatrix(mySize);

  if (myFunction == "clSumVectors") clSumVectors(mySize);

  if (myFunction == "clBlur") clBlur(mySize);

  QueryPerformanceCounter(&t2);
  time = (t2.QuadPart - t1.QuadPart) / double(frequency.QuadPart);
  std::cout << "Time of session: " << time << std::endl;
}

void clMulMatrix(const int size) {
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

  // helpMate.Execute(0, size, size, 0, 0, 50, "XY");
  helpMate.Test(0, size, size, 0, 0, "X");
  helpMate.Test(0, size, size, 0, 0, "Y");

  /*
  LARGE_INTEGER frequency;
  LARGE_INTEGER t1, t2;
  double time;
  QueryPerformanceFrequency(&frequency);

  for (int i = 0; i < 11; i++) {
    QueryPerformanceCounter(&t1);
    helpMate.Execute(0, size, size, 0, 0,
            i * 10, "Y");
    QueryPerformanceCounter(&t2);
    time = (t2.QuadPart - t1.QuadPart) / double(frequency.QuadPart);
    std::cout << time << std::endl;
  
     for (int i = 0; i < size * size; i++) {
      if (C[i] != size * 2) {
        std::cout << "Error";
        break;
      }
    }
    
  }
  */

  /*
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
        std::cout << C[i*size+j] << " ";
    }
    std::cout << std::endl;
  }
  */
}

void clMulMatrixOpt(const int size, const int blockSize) {
  std::vector<std::string> functionsNames;
  functionsNames.push_back("mulMatrixOpt");

  helpmate helpMate("kernelCode.cl", functionsNames);

  std::vector<size_t> mySizes;

  mySizes.push_back(size * size * sizeof(float));
  mySizes.push_back(size * size * sizeof(float));
  mySizes.push_back(size * size * sizeof(float));
  mySizes.push_back(sizeof(const int*));
  mySizes.push_back(blockSize * blockSize * sizeof(float));
  mySizes.push_back(blockSize * blockSize * sizeof(float));
  mySizes.push_back(sizeof(const int*));

  float* A = new float[size * size];
  float* B = new float[size * size];
  float* C = new float[size * size];
  float* localA = NULL;
  float* localB = NULL;

  for (int i = 0; i < size * size; i++) {
    A[i] = 1.0;
    B[i] = 2.0;
    C[i] = 0;
  }

  int* ptrSize = new int;
  *ptrSize = size;

  int* ptrBlockSize = new int;
  *ptrBlockSize = blockSize;

  helpMate.SetArgument(0, 0, mySizes[0], A);
  helpMate.SetArgument(0, 1, mySizes[1], B);
  helpMate.SetArgument(0, 2, mySizes[2], C);
  helpMate.SetArgument(0, 3, mySizes[3], ptrSize);
  helpMate.SetArgument(0, 4, mySizes[4], localA);
  helpMate.SetArgument(0, 5, mySizes[5], localB);
  helpMate.SetArgument(0, 6, mySizes[6], ptrBlockSize);

  // helpMate.Execute(0, size, size, blockSize, blockSize, 50, "X");
  helpMate.Test(0, size, size, blockSize, blockSize, "X");
  helpMate.Test(0, size, size, blockSize, blockSize, "Y");

  /*
   LARGE_INTEGER frequency;
   LARGE_INTEGER t1, t2;
   double time;
   QueryPerformanceFrequency(&frequency);

   for (int i = 0; i < 11; i++) {
     QueryPerformanceCounter(&t1);
     helpMate.Execute(0, size, size, blockSize, blockSize,
             i * 10, "Y");
     QueryPerformanceCounter(&t2);
     time = (t2.QuadPart - t1.QuadPart) / double(frequency.QuadPart);
     std::cout << time << std::endl;
   
      for (int i = 0; i < size * size; i++) {
       if (C[i] != size * 2) {
         std::cout << "Error";
         break;
       }
     }
     
   }
   */

  /*
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
        std::cout << C[i*size+j] << " ";
    }
    std::cout << std::endl;
  }
  


  for (int i = 0; i < size * size; i++) {
    if (C[i] != size * 2) {
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

void clBlur(int size) {
  std::vector<std::string> functionsNames;
  functionsNames.push_back("blur");

  helpmate helpMate("kernelCode.cl", functionsNames);

  std::vector<size_t> mySizes;

  mySizes.push_back(size * size * sizeof(float));
  mySizes.push_back(size * size * sizeof(float));
  mySizes.push_back(sizeof(const int*));
  mySizes.push_back(sizeof(const int*));

  float* A = new float[size * size];
  float* B = new float[size * size];

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      A[i * size + j] = i * size + j;
    }
   }

  int h = 1;

  helpMate.SetArgument(0, 0, mySizes[0], A);
  helpMate.SetArgument(0, 1, mySizes[1], B);
  helpMate.SetArgument(0, 2, mySizes[2], &size);
  helpMate.SetArgument(0, 3, mySizes[3], &h);

  //helpMate.Execute(0, size, size, 0, 0, 20, "Y");
    helpMate.Test(0, size, size, 0, 0, "X");
    helpMate.Test(0, size, size, 0, 0, "Y");

  /*
   for (int i = 0; i < size; i++) {
     for (int j = 0; j < size; j++) {
       std::cout << B[i * size + j] << " ";
     }
     std::cout << std::endl;
   }
   */
  
}

void clSumVectors(int size) {
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

  helpMate.SetArgument(0, 0, mySizes[0], A);
  helpMate.SetArgument(0, 1, mySizes[1], B);
  helpMate.SetArgument(0, 2, mySizes[2], C);

  helpMate.Test(0, size, 0);

  for (int i = 0; i < size; i++) {
    if (C[i] != 3) {
      std::cout << "Error";
    }
  }
}
