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

            case 1: {
                auto start_time = chrono::high_resolution_clock::now();

                mergeSort(temp, 0, n - 1);

                auto end_time = chrono::high_resolution_clock::now();

                cout << "\nSorted Array:\n";
                printArray(temp);

                cout << "\nTime taken: "
                    << chrono::duration_cast<chrono::microseconds>
                        (end_time - start_time).count()
                    << " microseconds\n";

                break;
            }

            case 2: {
                auto start_time = chrono::high_resolution_clock::now();

                parallelMergeSort(temp, 0, n - 1);

                auto end_time = chrono::high_resolution_clock::now();

                cout << "\nSorted Array:\n";
                printArray(temp);

                cout << "\nTime taken: "
                    << chrono::duration_cast<chrono::microseconds>
                        (end_time - start_time).count()
                    << " microseconds\n";

                break;
            }

            case 3: {
                auto start_time = chrono::high_resolution_clock::now();

                bubble(temp, n);

                auto end_time = chrono::high_resolution_clock::now();

                cout << "\nSorted Array:\n";
                printArray(temp);

                cout << "\nTime taken: "
                    << chrono::duration_cast<chrono::microseconds>
                        (end_time - start_time).count()
                    << " microseconds\n";

                break;
            }

            case 4: {
                auto start_time = chrono::high_resolution_clock::now();

                parallelBubble(temp, n);

                auto end_time = chrono::high_resolution_clock::now();

                cout << "\nSorted Array:\n";
                printArray(temp);

                cout << "\nTime taken: "
                    << chrono::duration_cast<chrono::microseconds>
                        (end_time - start_time).count()
                    << " microseconds\n";

                break;
            }

            case 5: {
                compareMerge(arr);
                break;
            }

            case 6: {
                compareBubble(arr);
                break;
            }

            case 7: {
                cout << "Exiting Program...\n";
                break;
            }

            default:
                cout << "Invalid Choice\n";
        }

    } while (choice != 7);

    return 0;
}

// 13 2 19 7 4 16 1 11 20 5 9 18 3 14 6 17 8 12 10 15

/*

27 4 49 12 35 1 44 18 7 50
23 9 31 15 42 6 28 11 39 2
47 20 14 33 8 25 41 5 30 17
46 10 22 37 3 48 16 29 13 40
19 34 24 45 21 32 26 38 43 36

*/

/*

57 12 89 3 76 45 21 98 34 67
5 90 43 16 72 28 99 54 11 63
37 80 2 95 26 48 70 14 85 31
60 9 52 100 18 74 41 7 93 24
66 35 1 87 50 13 79 29 58 4
97 22 64 39 10 82 27 55 91 6
73 44 17 96 32 68 8 53 84 25
61 15 77 40 94 19 56 30 88 47
23 71 36 92 49 20 65 42 78 33
59 86 69 51 81 38 62 83 75 46

*/