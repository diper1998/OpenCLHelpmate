// host.cpp : Этот файл содержит функцию "main". Здесь начинается и
// заканчивается выполнение программы.
//

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <CL/cl.h>
#include <iostream>
#include "helpmate.h"

int main() {
 
  std::vector<std::string> funNames;

  funNames.push_back("sumVectors");

  helpmate helpMate("kernelCode.cl", funNames);

  

  std::vector<size_t> mySizes;
  const int mySize = 10000;
  mySizes.push_back(mySize * sizeof(float));
  mySizes.push_back(mySize * sizeof(float));
  mySizes.push_back(mySize * sizeof(float));

  float* A = new float[mySize];
  float* B = new float[mySize];
  float* C = new float[mySize];

  for (int i = 0; i < mySize; i++) {
    A[i] = 1.5;
    B[i] = 1.5;
  }

  helpMate.SetBuffers(mySizes);

  helpMate.WriteBuffer("CPU", 0, CL_TRUE, 0, mySizes[0], A);
  helpMate.WriteBuffer("CPU", 1, CL_TRUE, 0, mySizes[1], B);
  helpMate.WriteBuffer("CPU", 2, CL_TRUE, 0, mySizes[2], C);

  helpMate.SetArgument(0, 0, 0);
  helpMate.SetArgument(0, 1, 1);
  helpMate.SetArgument(0, 2, 2);

  helpMate.Start("GPU", CL_FALSE, 0, 0, mySize/2, NULL);
  helpMate.Start("CPU", CL_FALSE, 0, 5000, mySize/2, NULL);
  helpMate.Wait("GPU");
  helpMate.Wait("CPU");

  helpMate.ReadBuffer("CPU", 2, CL_TRUE, 0, mySizes[2], C);

  for (int i = 0; i < mySize; i++) {
    std::cout << C[i] << " ";
  }
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и
//   другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый
//   элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий
//   элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" >
//   "Открыть" > "Проект" и выберите SLN-файл.
