#include <iostream>
#include <vector>
#include <omp.h>
#include <chrono>

using namespace std;
using namespace chrono;

// ---------------- MERGE SORT ----------------
void merge(vector<int> &arr, int low, int mid, int high) {
    vector<int> temp;
    int i = low, j = mid + 1;

    while (i <= mid && j <= high) {
        if (arr[i] <= arr[j]) temp.push_back(arr[i++]);
        else temp.push_back(arr[j++]);
    }

    while (i <= mid) temp.push_back(arr[i++]);
    while (j <= high) temp.push_back(arr[j++]);

    for (int k = 0; k < temp.size(); k++)
        arr[low + k] = temp[k];
}

void mergeSort(vector<int> &arr, int low, int high) {
    if (low < high) {
        int mid = (low + high) / 2;
        mergeSort(arr, low, mid);
        mergeSort(arr, mid + 1, high);
        merge(arr, low, mid, high);
    }
}

void parallelMergeSort(vector<int> &arr, int low, int high) {
    if (low < high) {
        int mid = (low + high) / 2;

        #pragma omp parallel sections
        {
            #pragma omp section
            parallelMergeSort(arr, low, mid);

            #pragma omp section
            parallelMergeSort(arr, mid + 1, high);
        }

        merge(arr, low, mid, high);
    }
}

// ---------------- BUBBLE SORT ----------------
void bubble(vector<int> &arr, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1])
                swap(arr[j], arr[j + 1]);
        }
    }
}

void parallelBubble(vector<int> &arr, int n) {
    for (int i = 0; i < n; i++) {

        #pragma omp parallel for
        for (int j = 1; j < n; j += 2) {
            if (arr[j] < arr[j - 1])
                swap(arr[j], arr[j - 1]);
        }

        #pragma omp parallel for
        for (int j = 2; j < n; j += 2) {
            if (arr[j] < arr[j - 1])
                swap(arr[j], arr[j - 1]);
        }
    }
}

// ---------------- PRINT ----------------
void printArray(const vector<int> &arr) {
    for (int x : arr) cout << x << " ";
    cout << endl;
}

// ---------------- PERFORMANCE: MERGE ----------------
void compareMerge(vector<int> arr) {
    int runs = 5;
    double seq = 0, par = 0;

    for (int i = 0; i < runs; i++) {
        vector<int> temp = arr;

        auto start = high_resolution_clock::now();
        mergeSort(temp, 0, temp.size() - 1);
        auto end = high_resolution_clock::now();

        seq += duration_cast<microseconds>(end - start).count();
    }

    for (int i = 0; i < runs; i++) {
        vector<int> temp = arr;

        auto start = high_resolution_clock::now();
        parallelMergeSort(temp, 0, temp.size() - 1);
        auto end = high_resolution_clock::now();

        par += duration_cast<microseconds>(end - start).count();
    }

    seq /= runs;
    par /= runs;

    cout << "\nMerge Sort Performance:\n";
    cout << "Sequential Time: " << seq / 1e6 << " sec\n";
    cout << "Parallel Time: " << par / 1e6 << " sec\n";
    cout << "Speedup: " << seq / par << endl;
}

// ---------------- PERFORMANCE: BUBBLE ----------------
void compareBubble(vector<int> arr) {
    int runs = 5;
    double seq = 0, par = 0;

    for (int i = 0; i < runs; i++) {
        vector<int> temp = arr;

        auto start = high_resolution_clock::now();
        bubble(temp, temp.size());
        auto end = high_resolution_clock::now();

        seq += duration_cast<microseconds>(end - start).count();
    }

    for (int i = 0; i < runs; i++) {
        vector<int> temp = arr;

        auto start = high_resolution_clock::now();
        parallelBubble(temp, temp.size());
        auto end = high_resolution_clock::now();

        par += duration_cast<microseconds>(end - start).count();
    }

    seq /= runs;
    par /= runs;

    cout << "\nBubble Sort Performance:\n";
    cout << "Sequential Time: " << seq / 1e6 << " sec\n";
    cout << "Parallel Time: " << par / 1e6 << " sec\n";
    cout << "Speedup: " << seq / par << endl;
}

// ---------------- MAIN ----------------
int main() {
    int n;
    cout << "Enter size of array: ";
    cin >> n;

    vector<int> arr(n);
    cout << "Enter elements:\n";
    for (int i = 0; i < n; i++) cin >> arr[i];

    int choice;

    do {
        cout << "\n===== MENU =====\n";
        cout << "1. Sequential Merge Sort\n";
        cout << "2. Parallel Merge Sort\n";
        cout << "3. Sequential Bubble Sort\n";
        cout << "4. Parallel Bubble Sort\n";
        cout << "5. Compare Merge Sort\n";
        cout << "6. Compare Bubble Sort\n";
        cout << "7. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        vector<int> temp = arr;

        switch (choice) {
            case 1:
                mergeSort(temp, 0, n - 1);
                printArray(temp);
                break;

            case 2:
                parallelMergeSort(temp, 0, n - 1);
                printArray(temp);
                break;

            case 3:
                bubble(temp, n);
                printArray(temp);
                break;

            case 4:
                parallelBubble(temp, n);
                printArray(temp);
                break;

            case 5:
                compareMerge(arr);
                break;

            case 6:
                compareBubble(arr);
                break;
        }

    } while (choice != 7);

    return 0;
}