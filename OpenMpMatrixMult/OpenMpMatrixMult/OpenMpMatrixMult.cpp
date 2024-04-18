#include <iostream>
#include <vector>
#include <chrono>
#include <omp.h>

using namespace std;
const int N = 2000;

vector<vector<int>> A(N, vector<int>(N));
vector<vector<int>> B(N, vector<int>(N));
vector<vector<int>> C(N, vector<int>(N));

void initialize_matrix(int n) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i][j] = rand() % 100;
            B[i][j] = rand() % 100;
        }
    }
}

void multiply_matrix_parallel(int threads_count) 
{
    omp_set_num_threads(threads_count);
    auto start_parallel = chrono::high_resolution_clock::now();
    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int sum = 0;
            for (int k = 0; k < N; k++) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
    auto end_parallel = chrono::high_resolution_clock::now();
    auto duration_parallel = chrono::duration_cast<chrono::milliseconds>(end_parallel - start_parallel);
    cout << "parallel with "<<threads_count<<" threads: " << duration_parallel.count() << " ms" << endl;
}

void multiply_matrix_serial()
{
    auto start_serial = chrono::high_resolution_clock::now();
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int sum = 0;
            for (int k = 0; k < N; k++) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
    auto end_serial = chrono::high_resolution_clock::now();
    auto duration_serial = chrono::duration_cast<chrono::milliseconds>(end_serial - start_serial);
    cout << "Serial: " << duration_serial.count() << " ms" << endl;
}

int main()
{
    initialize_matrix(N);
    multiply_matrix_serial();
    multiply_matrix_parallel(1);
    multiply_matrix_parallel(2);
    multiply_matrix_parallel(4);
    multiply_matrix_parallel(8);

    return 0;

}

