#include <iostream>
#include <vector>
#include <omp.h>
#include <chrono>
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace chrono;

// ---------------- MERGE FUNCTION ----------------
void merge(vector<int> &arr, int low, int mid, int high) {

    vector<int> temp;

    int i = low;
    int j = mid + 1;

    while (i <= mid && j <= high) {

        if (arr[i] <= arr[j]) {
            temp.push_back(arr[i]);
            i++;
        }
        else {
            temp.push_back(arr[j]);
            j++;
        }
    }

    while (i <= mid) {
        temp.push_back(arr[i]);
        i++;
    }

    while (j <= high) {
        temp.push_back(arr[j]);
        j++;
    }

    for (int k = 0; k < temp.size(); k++) {
        arr[low + k] = temp[k];
    }
}

// ---------------- SEQUENTIAL MERGE SORT ----------------
void mergeSort(vector<int> &arr, int low, int high) {

    if (low < high) {

        int mid = (low + high) / 2;

        mergeSort(arr, low, mid);
        mergeSort(arr, mid + 1, high);

        merge(arr, low, mid, high);
    }
}

// ---------------- PARALLEL MERGE SORT ----------------
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

// ---------------- SEQUENTIAL BUBBLE SORT ----------------
void bubbleSort(vector<int> &arr, int n) {

    for (int i = 0; i < n - 1; i++) {

        for (int j = 0; j < n - i - 1; j++) {

            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
            }
        }
    }
}

// ---------------- PARALLEL BUBBLE SORT ----------------
void parallelBubbleSort(vector<int> &arr, int n) {

    for (int i = 0; i < n; i++) {

        // Odd Phase
        #pragma omp parallel for
        for (int j = 1; j < n - 1; j += 2) {

            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
            }
        }

        // Even Phase
        #pragma omp parallel for
        for (int j = 0; j < n - 1; j += 2) {

            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
            }
        }
    }
}

// ---------------- PRINT ARRAY ----------------
void printArray(vector<int> &arr) {

    for (int x : arr) {
        cout << x << " ";
    }

    cout << endl;
}

// ---------------- COMPARE MERGE SORT ----------------
void compareMerge(vector<int> arr) {

    vector<int> temp1 = arr;
    vector<int> temp2 = arr;

    // Sequential
    auto start = high_resolution_clock::now();

    mergeSort(temp1, 0, temp1.size() - 1);

    auto end = high_resolution_clock::now();

    double seq_time =
        duration_cast<microseconds>(end - start).count();

    // Parallel
    start = high_resolution_clock::now();

    parallelMergeSort(temp2, 0, temp2.size() - 1);

    end = high_resolution_clock::now();

    double par_time =
        duration_cast<microseconds>(end - start).count();

    cout << "\nMerge Sort Performance:\n";

    cout << "Sequential Time : "
         << seq_time << " microseconds\n";

    cout << "Parallel Time   : "
         << par_time << " microseconds\n";

    if (par_time > 0) {
        cout << "Speedup : "
             << seq_time / par_time << endl;
    }
}

// ---------------- COMPARE BUBBLE SORT ----------------
void compareBubble(vector<int> arr) {

    vector<int> temp1 = arr;
    vector<int> temp2 = arr;

    // Sequential
    auto start = high_resolution_clock::now();

    bubbleSort(temp1, temp1.size());

    auto end = high_resolution_clock::now();

    double seq_time =
        duration_cast<microseconds>(end - start).count();

    // Parallel
    start = high_resolution_clock::now();

    parallelBubbleSort(temp2, temp2.size());

    end = high_resolution_clock::now();

    double par_time =
        duration_cast<microseconds>(end - start).count();

    cout << "\nBubble Sort Performance:\n";

    cout << "Sequential Time : "
         << seq_time << " microseconds\n";

    cout << "Parallel Time   : "
         << par_time << " microseconds\n";

    if (par_time > 0) {
        cout << "Speedup : "
             << seq_time / par_time << endl;
    }
}

// ---------------- MAIN FUNCTION ----------------
int main() {

    int n;

    cout << "Enter size of array: ";
    cin >> n;

    vector<int> arr(n);

    srand(time(0));

    // Random array generation
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % 10000;
    }

    // cout << "Enter elements:\n";
    // for (int i = 0; i < n; i++) cin >> arr[i];

    int choice;

    do {

        cout << "\n===== MENU =====\n";

        cout << "1. Sequential Merge Sort\n";
        cout << "2. Parallel Merge Sort\n";
        cout << "3. Sequential Bubble Sort\n";
        cout << "4. Parallel Bubble Sort\n";
        cout << "5. Compare Merge Sort\n";
        cout << "6. Compare Bubble Sort\n";
        cout << "7. Print Original Array\n";
        cout << "8. Exit\n";

        cout << "Enter choice: ";
        cin >> choice;

        vector<int> temp = arr;

        switch (choice) {

            case 1: {

                auto start =
                    high_resolution_clock::now();

                mergeSort(temp, 0, n - 1);

                auto end =
                    high_resolution_clock::now();

                // cout << "\nSorted Array:\n";
                // printArray(temp);

                cout << "\nTime Taken : "
                     << duration_cast<microseconds>
                        (end - start).count()
                     << " microseconds\n";

                break;
            }

            case 2: {

                auto start =
                    high_resolution_clock::now();

                parallelMergeSort(temp, 0, n - 1);

                auto end =
                    high_resolution_clock::now();

                // cout << "\nSorted Array:\n";
                // printArray(temp);

                cout << "\nTime Taken : "
                     << duration_cast<microseconds>
                        (end - start).count()
                     << " microseconds\n";

                break;
            }

            case 3: {

                auto start =
                    high_resolution_clock::now();

                bubbleSort(temp, n);

                auto end =
                    high_resolution_clock::now();

                // cout << "\nSorted Array:\n";
                // printArray(temp);

                cout << "\nTime Taken : "
                     << duration_cast<microseconds>
                        (end - start).count()
                     << " microseconds\n";

                break;
            }

            case 4: {

                auto start =
                    high_resolution_clock::now();

                parallelBubbleSort(temp, n);

                auto end =
                    high_resolution_clock::now();

                // cout << "\nSorted Array:\n";
                // printArray(temp);

                cout << "\nTime Taken : "
                     << duration_cast<microseconds>
                        (end - start).count()
                     << " microseconds\n";

                break;
            }

            case 5:
                compareMerge(arr);
                break;

            case 6:
                compareBubble(arr);
                break;

            case 7:
                cout << "\nOriginal Array:\n";
                printArray(arr);
                break;

            case 8:
                cout << "\nExiting Program...\n";
                break;

            default:
                cout << "\nInvalid Choice\n";
        }

    } while (choice != 8);

    return 0;
}