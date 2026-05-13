#include <cuda_runtime.h>
#include <bits/stdc++.h>

using namespace std;
using namespace std::chrono;

// CUDA Kernel
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

// Initialize Matrix
void initialize(int* matrix, int size) {

    for (int i = 0; i < size; i++) {
        matrix[i] = rand() % 10;
    }
}

// Print Matrix
void print(int* matrix, int rows, int cols) {

    for (int i = 0; i < rows; i++) {

        for (int j = 0; j < cols; j++) {
            cout << matrix[i * cols + j] << " ";
        }

        cout << endl;
    }

    cout << endl;
}

// Sequential Multiplication
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

    srand(time(0));

    // Small size for visible output
    int M = 20, N = 20, K = 20;

    int* A = new int[M * N];
    int* B = new int[N * K];
    int* C_cpu = new int[M * K];
    int* C_gpu = new int[M * K];

    initialize(A, M * N);
    initialize(B, N * K);

    cout << "\nMatrix A:\n";
    print(A, M, N);

    cout << "Matrix B:\n";
    print(B, N, K);

    // Device Memory
    int *X, *Y, *Z;

    cudaMalloc(&X, M * N * sizeof(int));
    cudaMalloc(&Y, N * K * sizeof(int));
    cudaMalloc(&Z, M * K * sizeof(int));

    cudaMemcpy(X, A, M * N * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(Y, B, N * K * sizeof(int), cudaMemcpyHostToDevice);

    int THREADS = 16;

    dim3 threads(THREADS, THREADS);

    dim3 blocks(
        (K + THREADS - 1) / THREADS,
        (M + THREADS - 1) / THREADS
    );

    // ---------------- CPU ----------------
    auto start = high_resolution_clock::now();

    sequentialMultiply(A, B, C_cpu, M, N, K);

    auto stop = high_resolution_clock::now();

    auto cpu_time =
        duration_cast<microseconds>(stop - start);

    // ---------------- GPU ----------------
    start = high_resolution_clock::now();

    multiply<<<blocks, threads>>>(X, Y, Z, M, N, K);

    cudaDeviceSynchronize();

    stop = high_resolution_clock::now();

    auto gpu_time =
        duration_cast<microseconds>(stop - start);

    cudaMemcpy(
        C_gpu,
        Z,
        M * K * sizeof(int),
        cudaMemcpyDeviceToHost
    );

    // CUDA Error Check
    cudaError_t err = cudaGetLastError();

    if (err != cudaSuccess) {
        cout << "CUDA Error: "
             << cudaGetErrorString(err)
             << endl;
    }

    // Output Results
    cout << "CPU Result Matrix:\n";
    print(C_cpu, M, K);

    cout << "GPU Result Matrix:\n";
    print(C_gpu, M, K);

    cout << "CPU Time: "
         << cpu_time.count()
         << " microseconds\n";

    cout << "GPU Time: "
         << gpu_time.count()
         << " microseconds\n";

    // Cleanup
    delete[] A;
    delete[] B;
    delete[] C_cpu;
    delete[] C_gpu;

    cudaFree(X);
    cudaFree(Y);
    cudaFree(Z);

    return 0;
}
