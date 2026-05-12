#include <iostream>
#include <omp.h>
#include <chrono>
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace chrono;

// -------- SEQUENTIAL --------
int seq_min(int arr[], int n) {
    int mn = arr[0];

    for (int i = 1; i < n; i++) {
        if (arr[i] < mn)
            mn = arr[i];
    }

    return mn;
}

int seq_max(int arr[], int n) {
    int mx = arr[0];

    for (int i = 1; i < n; i++) {
        if (arr[i] > mx)
            mx = arr[i];
    }

    return mx;
}

long long seq_sum(int arr[], int n) {
    long long s = 0;

    for (int i = 0; i < n; i++) {
        s += arr[i];
    }

    return s;
}

double seq_avg(int arr[], int n) {
    return (double)seq_sum(arr, n) / n;
}

// -------- PARALLEL --------
int par_min(int arr[], int n) {
    int mn = arr[0];

    #pragma omp parallel for reduction(min:mn)
    for (int i = 0; i < n; i++) {
        if (arr[i] < mn)
            mn = arr[i];
    }

    return mn;
}

int par_max(int arr[], int n) {
    int mx = arr[0];

    #pragma omp parallel for reduction(max:mx)
    for (int i = 0; i < n; i++) {
        if (arr[i] > mx)
            mx = arr[i];
    }

    return mx;
}

long long par_sum(int arr[], int n) {
    long long s = 0;

    #pragma omp parallel for reduction(+:s)
    for (int i = 0; i < n; i++) {
        s += arr[i];
    }

    return s;
}

double par_avg(int arr[], int n) {
    return (double)par_sum(arr, n) / n;
}

// -------- MAIN --------
int main() {

    int n;

    cout << "Enter number of elements: ";
    cin >> n;

    int arr[n];

    srand(time(0));

    // Random numbers from -100000 to 100000
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % 200001 - 100000;
    }

    // -------- SEQUENTIAL --------
    auto start = high_resolution_clock::now();

    int mn1 = seq_min(arr, n);
    int mx1 = seq_max(arr, n);
    long long s1 = seq_sum(arr, n);
    double avg1 = seq_avg(arr, n);

    auto end = high_resolution_clock::now();

    double seq_time =
        duration_cast<microseconds>(end - start).count();

    // -------- PARALLEL --------
    start = high_resolution_clock::now();

    int mn2 = par_min(arr, n);
    int mx2 = par_max(arr, n);
    long long s2 = par_sum(arr, n);
    double avg2 = par_avg(arr, n);

    end = high_resolution_clock::now();

    double par_time =
        duration_cast<microseconds>(end - start).count();

    // -------- OUTPUT --------
    cout << "\nSequential Results:\n";
    cout << "Minimum : " << mn1 << endl;
    cout << "Maximum : " << mx1 << endl;
    cout << "Sum     : " << s1 << endl;
    cout << "Average : " << avg1 << endl;

    cout << "\nParallel Results:\n";
    cout << "Minimum : " << mn2 << endl;
    cout << "Maximum : " << mx2 << endl;
    cout << "Sum     : " << s2 << endl;
    cout << "Average : " << avg2 << endl;

    cout << "\nPerformance Comparison:\n";
    cout << "Sequential Time : "
         << seq_time << " microseconds" << endl;

    cout << "Parallel Time   : "
         << par_time << " microseconds" << endl;

    if (par_time > 0) {
        cout << "Speedup : "
             << seq_time / par_time << endl;
    }

    return 0;
}