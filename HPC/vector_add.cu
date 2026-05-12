#include <cuda_runtime.h>
#include <bits/stdc++.h>

using namespace std;
using namespace std::chrono;

// CUDA kernel
__global__ void add(int* A, int* B, int* C, int size) {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;

    if (tid < size) {
        C[tid] = A[tid] + B[tid];
    }
}

// Initialize with random values
void initialize(int* arr, int size) {
    for (int i = 0; i < size; i++) {
        arr[i] = rand() % 100;
    }
}

// Print first few elements (for verification)
void printSample(int* arr, int size, string name) {
    cout << name << " (first 10 elements): ";
    for (int i = 0; i < min(size, 10); i++) {
        cout << arr[i] << " ";
    }
    cout << "\n";
}

// Sequential addition
void sequentialAddition(int* A, int* B, int* C, int size) {
    for (int i = 0; i < size; i++) {
        C[i] = A[i] + B[i];
    }
}

int main() {
    // Large input
    int N = 1000000;  // 1 million elements

    int* A = new int[N];
    int* B = new int[N];
    int* C = new int[N];

    initialize(A, N);
    initialize(B, N);

    printSample(A, N, "Vector A");
    printSample(B, N, "Vector B");

    // Device memory
    int *X, *Y, *Z;
    cudaMalloc(&X, N * sizeof(int));
    cudaMalloc(&Y, N * sizeof(int));
    cudaMalloc(&Z, N * sizeof(int));

    cudaMemcpy(X, A, N * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(Y, B, N * sizeof(int), cudaMemcpyHostToDevice);

    int threadsPerBlock = 256;
    int blocksPerGrid = (N + threadsPerBlock - 1) / threadsPerBlock;

    // ---------------- CPU ----------------
    auto start = high_resolution_clock::now();

    sequentialAddition(A, B, C, N);

    auto stop = high_resolution_clock::now();
    auto cpu_time = duration_cast<microseconds>(stop - start);

    printSample(C, N, "C (CPU)");

    // ---------------- GPU ----------------
    start = high_resolution_clock::now();

    add<<<blocksPerGrid, threadsPerBlock>>>(X, Y, Z, N);
    cudaDeviceSynchronize();  // important

    stop = high_resolution_clock::now();
    auto gpu_time = duration_cast<microseconds>(stop - start);

    // Copy result back
    cudaMemcpy(C, Z, N * sizeof(int), cudaMemcpyDeviceToHost);

    // Error check
    cudaError_t err = cudaGetLastError();
    if (err != cudaSuccess) {
        cout << "CUDA Error: " << cudaGetErrorString(err) << endl;
    }

    printSample(C, N, "C (GPU)");

    cout << "CPU Time: " << cpu_time.count() << " ms\n";
    cout << "GPU Time: " << gpu_time.count() << " ms\n";

    // Cleanup
    delete[] A;
    delete[] B;
    delete[] C;

    cudaFree(X);
    cudaFree(Y);
    cudaFree(Z);

    return 0;
}