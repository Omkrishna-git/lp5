#include <cuda_runtime.h>
#include <bits/stdc++.h>

using namespace std;
using namespace std::chrono;

// CUDA Kernel Function
// Each GPU thread adds one element from A and B
__global__ void add(int* A, int* B, int* C, int size) {

    // Calculate unique thread index
    int tid = blockIdx.x * blockDim.x + threadIdx.x;

    // Boundary check to avoid invalid memory access
    if (tid < size) {
        C[tid] = A[tid] + B[tid];
    }
}

// Initialize array with random values
void initialize(int* arr, int size) {

    for (int i = 0; i < size; i++) {
        arr[i] = rand() % 100;
    }
}

// Print first few elements for verification
void printSample(int* arr, int size, string name) {

    cout << name << " (first 10 elements): ";

    for (int i = 0; i < min(size, 10); i++) {
        cout << arr[i] << " ";
    }

    cout << "\n";
}

// Sequential Vector Addition using CPU
void sequentialAddition(int* A, int* B, int* C, int size) {

    for (int i = 0; i < size; i++) {
        C[i] = A[i] + B[i];
    }
}

int main() {

    srand(time(0));

    // Large vector size
    int N = 1000000;

    // Host memory allocation (CPU)
    int* A = new int[N];
    int* B = new int[N];
    int* C = new int[N];

    // Initialize vectors
    initialize(A, N);
    initialize(B, N);

    // Display sample input values
    printSample(A, N, "Vector A");
    printSample(B, N, "Vector B");

    // Device memory pointers (GPU)
    int *X, *Y, *Z;

    // Allocate memory on GPU
    cudaMalloc(&X, N * sizeof(int));
    cudaMalloc(&Y, N * sizeof(int));
    cudaMalloc(&Z, N * sizeof(int));

    // Copy input data from CPU to GPU
    cudaMemcpy(
        X,
        A,
        N * sizeof(int),
        cudaMemcpyHostToDevice
    );

    cudaMemcpy(
        Y,
        B,
        N * sizeof(int),
        cudaMemcpyHostToDevice
    );

    // CUDA execution configuration
    int threadsPerBlock = 256;

    // Calculate total number of blocks required
    int blocksPerGrid =
        (N + threadsPerBlock - 1) / threadsPerBlock;

    // ---------------- CPU EXECUTION ----------------
    auto start = high_resolution_clock::now();

    sequentialAddition(A, B, C, N);

    auto stop = high_resolution_clock::now();

    auto cpu_time =
        duration_cast<microseconds>(stop - start);

    // Print CPU result sample
    printSample(C, N, "C (CPU)");

    // ---------------- GPU EXECUTION ----------------
    start = high_resolution_clock::now();

    // Launch CUDA kernel
    add<<<blocksPerGrid, threadsPerBlock>>>(
        X, Y, Z, N
    );

    // Wait until all GPU threads complete execution
    cudaDeviceSynchronize();

    stop = high_resolution_clock::now();

    auto gpu_time =
        duration_cast<microseconds>(stop - start);

    // Copy output from GPU back to CPU
    cudaMemcpy(
        C,
        Z,
        N * sizeof(int),
        cudaMemcpyDeviceToHost
    );

    // Check CUDA runtime errors
    cudaError_t err = cudaGetLastError();

    if (err != cudaSuccess) {

        cout << "CUDA Error: "
             << cudaGetErrorString(err)
             << endl;
    }

    // Print GPU result sample
    printSample(C, N, "C (GPU)");

    // Display execution times
    cout << "\nCPU Time: "
         << cpu_time.count()
         << " microseconds\n";

    cout << "GPU Time: "
         << gpu_time.count()
         << " microseconds\n";

    // Calculate speedup
    if (gpu_time.count() > 0) {

        cout << "Speedup: "
             << (float)cpu_time.count() /
                gpu_time.count()
             << endl;
    }

    // Free CPU memory
    delete[] A;
    delete[] B;
    delete[] C;

    // Free GPU memory
    cudaFree(X);
    cudaFree(Y);
    cudaFree(Z);

    return 0;
}