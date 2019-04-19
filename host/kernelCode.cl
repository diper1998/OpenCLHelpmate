kernel void sumVectors(global const float* A, global const float* B,
                       global float* C) {
  int i = get_global_id(0);
  C[i] = 0;
  C[i] = A[i] + B[i];
}

kernel void blur(global const float* A, global float* B, constant int* ptrSize, constant int* ptrH) {
  int i = get_global_id(0);
  int j = get_global_id(1);
  int size = *ptrSize;
  int h = *ptrH;
  
 int count = 0;

  B[i*size+j] = A[i*size+j];
  count++;
  
  for(int x = 1; x <= h; x++){ 
	for(int y = 1; y <= h; y++){ 

	if(i > 0 && i < size -1 && j > 0 && j < size -1) { 

		if(j + x < size && i + y < size){ 
			B[i*size+j] += A[(i+y)*size+(j+x)]; 
			count++;
		}
		
		if(j + x < size && i - y >= 0){ 
			B[i*size+j] += A[(i-y)*size+(j+x)]; 
			count++;
		}

		if(j - x >= 0 && i + y < size){ 
			B[i*size+j] += A[(i+y)*size+(j-x)]; 
			count++;
		}

		if(j - x >= 0 && i - y >= 0){ 
			B[i*size+j] += A[(i-y)*size+(j-x)]; 
			count++;
		}
		
		}
	}
	
  }
  
  B[i*size+j] /= count;

}

kernel void mulMatrix(global const float* A, global const float* B,
                      global float* C, constant int* ptrSize) {
  int j = get_global_id(0);
  int i = get_global_id(1);
  int size = *ptrSize;
  C[i * size + j] = 0;
  for (int k = 0; k < size; k++)
    C[i * size + j] += A[i * size + k] * B[k * size + j];
}

kernel void mulMatrixOpt(global const float* a, global const float* b,
                         global float* c, constant int* size_ptr,
                         local float* a_local, local float* b_local,
                         constant int* blocksize_ptr) {
  int j = get_global_id(0);
  int i = get_global_id(1);

  int size = *size_ptr;
  int BLOCK_SIZE = *blocksize_ptr;

  int localJ = get_local_id(0);
  int localI = get_local_id(1);

  float sum = 0.0f;

  for (int p = 0; p < size / BLOCK_SIZE; ++p) {
    a_local[localI * BLOCK_SIZE + localJ] =
        a[i * size + p * BLOCK_SIZE + localJ];
    b_local[localI * BLOCK_SIZE + localJ] =
        b[(p * BLOCK_SIZE + localI) * size + j];
    barrier(CLK_LOCAL_MEM_FENCE);

    for (int l = 0; l < BLOCK_SIZE; ++l) {
      sum +=
          a_local[localI * BLOCK_SIZE + l] * b_local[l * BLOCK_SIZE + localJ];
    }
    barrier(CLK_LOCAL_MEM_FENCE);
  }
  c[i * size + j] = sum;
}