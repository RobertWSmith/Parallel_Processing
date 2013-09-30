#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <time.h>
#include <sys/time.h>
#include <string>

using namespace std;

int rowCol(int r, int c) {
    return ((r * 2048) + c);
}

int main() {
    int dim = 2048;
    struct timeval start, end;

    ofstream outputFile;
    outputFile.open("P2_output.csv", ios::app | ios::out);

    short int *Xmtx, *Ymtx, *output;
    Xmtx = new short int[dim * dim];
    Ymtx = new short int[dim * dim];
    output = new short int[dim * dim];
    
    // assignment loop
    for (int i = 0; i < dim * dim; i++) { 
        output[i] = Ymtx[i] = Xmtx[i] = (short int) 1; 
    }

    unsigned long ops_counter = 0, elapsed_time = 0;
    int count = 0;
    int block_size = 64;

    while (count < 10) {
        count++;
        
        while (block_size < (dim)) {
            elapsed_time = (unsigned long) 0;
            ops_counter = 0;
            block_size *= 2;
            
            cout << "Block Size: " << block_size << endl;
            gettimeofday(&start, NULL);
            
            for (int k = 0; k < dim; k += block_size) {
                for (int j = 0; j < dim; j += block_size) {
                    for (int i = 0; i < dim; i++) {
                        for (int kk = k; kk < min(k + block_size, dim); kk++) {
                            for (int jj = j; jj < max(j + block_size, dim); jj++) {
                                output[ rowCol(i, jj) ] += Xmtx[ rowCol(i, kk) ] * Ymtx[ rowCol(kk, jj) ];
                                ops_counter++;
                            }
                        }
                    }
                }
                cout << ops_counter << " ";
                ops_counter = 0;
            }
            cout << endl;

            // Linux Stop Timer
            gettimeofday(&end, NULL);

            elapsed_time = (unsigned long) ((end.tv_sec * 1000000 + end.tv_usec) - 
                    (start.tv_sec * 1000000 + start.tv_usec));
            
            cout << "Elapsed Time: " << elapsed_time << " ns." << endl;
            cout << "Total Operations: " << ops_counter << endl;

            outputFile << block_size << "," << elapsed_time << "," << count << endl;
        }
    }
    
    cout << "End of Test" << endl;
    outputFile.close();

    return 0;
}

