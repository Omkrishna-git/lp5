#include <iostream>
#include <omp.h>
#include <chrono>
using namespace std;
using namespace chrono;

// -------- SEQUENTIAL --------
int seq_min(int arr[], int n) {
    int mn = arr[0];
    for (int i = 1; i < n; i++)
        if (arr[i] < mn) mn = arr[i];
    return mn;
}

int seq_max(int arr[], int n) {
    int mx = arr[0];
    for (int i = 1; i < n; i++)
        if (arr[i] > mx) mx = arr[i];
    return mx;
}

int seq_sum(int arr[], int n) {
    int s = 0;
    for (int i = 0; i < n; i++)
        s += arr[i];
    return s;
}

double seq_avg(int arr[], int n) {
    return (double)seq_sum(arr, n) / n;
}

// -------- PARALLEL --------
int par_min(int arr[], int n) {
    int mn = arr[0];
    #pragma omp parallel for reduction(min:mn)
    for (int i = 0; i < n; i++)
        if (arr[i] < mn) mn = arr[i];
    return mn;
}

int par_max(int arr[], int n) {
    int mx = arr[0];
    #pragma omp parallel for reduction(max:mx)
    for (int i = 0; i < n; i++)
        if (arr[i] > mx) mx = arr[i];
    return mx;
}

int par_sum(int arr[], int n) {
    int s = 0;
    #pragma omp parallel for reduction(+:s)
    for (int i = 0; i < n; i++)
        s += arr[i];
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
    cout << "Enter elements:\n";
    for (int i = 0; i < n; i++) cin >> arr[i];

    // -------- SEQUENTIAL --------
    auto start = high_resolution_clock::now();

    int mn1 = seq_min(arr, n);
    int mx1 = seq_max(arr, n);
    int s1 = seq_sum(arr, n);
    double avg1 = seq_avg(arr, n);

    auto end = high_resolution_clock::now();
    double seq_time = duration_cast<microseconds>(end - start).count();

    // -------- PARALLEL --------
    start = high_resolution_clock::now();

    int mn2 = par_min(arr, n);
    int mx2 = par_max(arr, n);
    int s2 = par_sum(arr, n);
    double avg2 = par_avg(arr, n);

    end = high_resolution_clock::now();
    double par_time = duration_cast<microseconds>(end - start).count();

    // -------- OUTPUT --------
    cout << "\nSequential Results:\n";
    cout << "Min: " << mn1 << "\nMax: " << mx1
         << "\nSum: " << s1 << "\nAvg: " << avg1 << endl;

    cout << "\nParallel Results:\n";
    cout << "Min: " << mn2 << "\nMax: " << mx2
         << "\nSum: " << s2 << "\nAvg: " << avg2 << endl;

    cout << "\nPerformance:\n";
    cout << "Sequential Time: " << seq_time << " microseconds\n";
    cout << "Parallel Time: " << par_time << " microseconds\n";

    if (par_time > 0)
        cout << "Speedup: " << seq_time / par_time << endl;

    return 0;
} 