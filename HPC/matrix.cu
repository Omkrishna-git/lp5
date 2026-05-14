#include <cuda_runtime.h>
#include <bits/stdc++.h>

using namespace std;
using namespace std::chrono;

// CUDA Kernel Function
// Each thread computes one element of result matrix C
__global__ void multiply(int* A, int* B, int* C, int M, int N, int K) {

    // Calculate global row index
    int row = blockIdx.y * blockDim.y + threadIdx.y;

    // Calculate global column index
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    // Ensure thread works only within valid matrix range
    if (row < M && col < K) {

        int sum = 0;

        // Perform multiplication of row of A and column of B
        for (int i = 0; i < N; i++) {

            sum += A[row * N + i] * B[i * K + col];
        }

        // Store computed value in result matrix
        C[row * K + col] = sum;
    }
}

// Initialize matrix with random values between 0-9
void initialize(int* matrix, int size) {

    for (int i = 0; i < size; i++) {

        matrix[i] = rand() % 10;
    }
}

// Print matrix contents
void print(int* matrix, int rows, int cols) {

    for (int i = 0; i < rows; i++) {

        for (int j = 0; j < cols; j++) {

            cout << matrix[i * cols + j] << " ";
        }

        cout << endl;
    }

    cout << endl;
}

// Sequential CPU Matrix Multiplication
void sequentialMultiply(int* A, int* B, int* C,
                        int M, int N, int K) {

    // Traverse rows of matrix A
    for (int i = 0; i < M; i++) {

        // Traverse columns of matrix B
        for (int j = 0; j < K; j++) {

            int sum = 0;

            // Compute dot product
            for (int k = 0; k < N; k++) {

                sum += A[i * N + k] * B[k * K + j];
            }

            // Store result in matrix C
            C[i * K + j] = sum;
        }
    }
}

int main() {

    srand(time(0));

    // Matrix dimensions
    // A = M x N
    // B = N x K
    // C = M x K
    int M = 20, N = 20, K = 20;

    // Allocate host memory (CPU memory)
    int* A = new int[M * N];
    int* B = new int[N * K];
    int* C_cpu = new int[M * K];
    int* C_gpu = new int[M * K];

    // Initialize matrices
    initialize(A, M * N);
    initialize(B, N * K);

    // Display input matrices
    cout << "\nMatrix A:\n";
    print(A, M, N);

    cout << "Matrix B:\n";
    print(B, N, K);

    // Device pointers (GPU memory)
    int *X, *Y, *Z;

    // Allocate memory on GPU
    cudaMalloc(&X, M * N * sizeof(int));
    cudaMalloc(&Y, N * K * sizeof(int));
    cudaMalloc(&Z, M * K * sizeof(int));

    // Copy matrices from CPU memory to GPU memory
    cudaMemcpy(X, A,
               M * N * sizeof(int),
               cudaMemcpyHostToDevice);

    cudaMemcpy(Y, B,
               N * K * sizeof(int),
               cudaMemcpyHostToDevice);

    // Number of threads per block
    int THREADS = 16;

    // Define 2D thread block
    dim3 threads(THREADS, THREADS);

    // Define number of blocks required
    dim3 blocks(
        (K + THREADS - 1) / THREADS,
        (M + THREADS - 1) / THREADS
    );

    // ---------------- CPU Execution ----------------

    auto start = high_resolution_clock::now();

    // Sequential matrix multiplication
    sequentialMultiply(A, B, C_cpu, M, N, K);

    auto stop = high_resolution_clock::now();

    // Calculate CPU execution time
    auto cpu_time =
        duration_cast<microseconds>(stop - start);

    // ---------------- GPU Execution ----------------

    start = high_resolution_clock::now();

    // Launch CUDA kernel
    multiply<<<blocks, threads>>>(X, Y, Z, M, N, K);

    // Wait until all GPU threads complete execution
    cudaDeviceSynchronize();

    stop = high_resolution_clock::now();

    // Calculate GPU execution time
    auto gpu_time =
        duration_cast<microseconds>(stop - start);

    // Copy result matrix from GPU back to CPU
    cudaMemcpy(
        C_gpu,
        Z,
        M * K * sizeof(int),
        cudaMemcpyDeviceToHost
    );

    // Check for CUDA runtime errors
    cudaError_t err = cudaGetLastError();

    if (err != cudaSuccess) {

        cout << "CUDA Error: "
             << cudaGetErrorString(err)
             << endl;
    }

    // Display CPU result matrix
    cout << "CPU Result Matrix:\n";
    print(C_cpu, M, K);

    // Display GPU result matrix
    cout << "GPU Result Matrix:\n";
    print(C_gpu, M, K);

    // Display execution times
    cout << "CPU Time: "
         << cpu_time.count()
         << " microseconds\n";

    cout << "GPU Time: "
         << gpu_time.count()
         << " microseconds\n";

    // Free CPU memory
    delete[] A;
    delete[] B;
    delete[] C_cpu;
    delete[] C_gpu;

    // Free GPU memory
    cudaFree(X);
    cudaFree(Y);
    cudaFree(Z);

    return 0;
}