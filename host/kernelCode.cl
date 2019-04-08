kernel void sumVectors(global float* A,  global float* B, global float*C )
{ 
	int i = get_global_id(0);
	
	C[i] = A[i]+B[i];
	

}


