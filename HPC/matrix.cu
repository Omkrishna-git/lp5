#include <cuda_runtime.h>
#include <bits/stdc++.h>

using namespace std;
using namespace std::chrono;

// CUDA kernel
__global__ void multiply(int* A, int* B, int* C, int M, int N, int K) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if (row < M && col < K) {
        int sum = 0;
        for (int i = 0; i < N; i++) {
            sum += A[row * N + i] * B[i * K + col];
        }
        C[row * K + col] = sum;
    }
}

// Initialize matrix with random values
void initialize(int* matrix, int size) {
    for (int i = 0; i < size; i++) {
        matrix[i] = rand() % 10;
    }
}

// Print matrix (only for small sizes)
void print(int* matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            cout << matrix[i * cols + j] << " ";
        }
        cout << "\n";
    }
    cout << "\n";
}

// Sequential multiplication
void sequentialMultiply(int* A, int* B, int* C, int M, int N, int K) {
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < K; j++) {
            int sum = 0;
            for (int k = 0; k < N; k++) {
                sum += A[i * N + k] * B[k * K + j];
            }
            C[i * K + j] = sum;
        }
    }
}

int main() {
    // Matrix dimensions
    int M = 512, N = 512, K = 512;

    int* A = new int[M * N];
    int* B = new int[N * K];
    int* C = new int[M * K];

    initialize(A, M * N);
    initialize(B, N * K);

    // Device memory
    int *X, *Y, *Z;
    cudaMalloc(&X, M * N * sizeof(int));
    cudaMalloc(&Y, N * K * sizeof(int));
    cudaMalloc(&Z, M * K * sizeof(int));

    cudaMemcpy(X, A, M * N * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(Y, B, N * K * sizeof(int), cudaMemcpyHostToDevice);

    int THREADS = 16;

    dim3 threads(THREADS, THREADS);

    // Correct grid dimensions
    dim3 blocks(
        (K + THREADS - 1) / THREADS,
        (M + THREADS - 1) / THREADS
    );

    // ---------------- CPU ----------------
    auto start = high_resolution_clock::now();

    sequentialMultiply(A, B, C, M, N, K);

    auto stop = high_resolution_clock::now();
    auto cpu_time = duration_cast<microseconds>(stop - start);

    // ---------------- GPU ----------------
    start = high_resolution_clock::now();

    multiply<<<blocks, threads>>>(X, Y, Z, M, N, K);
    cudaDeviceSynchronize();  //  important

    stop = high_resolution_clock::now();
    auto gpu_time = duration_cast<microseconds>(stop - start);

    // Copy result back
    cudaMemcpy(C, Z, M * K * sizeof(int), cudaMemcpyDeviceToHost);

    // CUDA error check
    cudaError_t err = cudaGetLastError();
    if (err != cudaSuccess) {
        cout << "CUDA Error: " << cudaGetErrorString(err) << endl;
    }

    // Print (only if small)
    if (M <= 10) {
        cout << "Result Matrix:\n";
        print(C, M, K);
    }

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