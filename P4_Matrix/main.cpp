/*
 * File:   main.cpp
 * Author: rob
 *
 * Created on October 19, 2013, 9:35 PM
 */

#include <cstdlib>
#include <cmath>

#include <string>
#include <sstream>

#include <iostream>
#include <fstream>

#include <chrono>
#include <omp.h>

#include <array>

#include <armadillo>

using namespace std;
using namespace std::chrono;
using namespace arma;

Mat<double> Xmtx, Ymtx, output, output2;
unsigned int dim, n_threads, tid, block_size, block_size_[3] = { 4, 8, 16 };

int main( ) {

    high_resolution_clock::time_point start, end;
    nanoseconds elapsed_time;
    string lineX, lineY;
    ofstream outputFile;
    ifstream inputX, inputY;
    dim = 16 * 16;

    outputFile.open( "P4_mtx.csv", ios::app | ios::out );
    cout << "Size: " << dim << endl;

    inputX.open( "matrix1.txt", ios::in );
    getline( inputX, lineX );
    istringstream issX( lineX );

    inputY.open( "matrix2.txt", ios::in );
    getline( inputY, lineY );
    istringstream issY( lineY );

    Xmtx.resize( 16, 16 );
    Ymtx.resize( 16, 16 );
    output.resize( 16, 16 );
    output2.resize( 16, 16 );
    output.zeros( );
    output2.zeros( );

    for ( int row = 0; row < 16; row++ ) {
        for ( int col = 0; col < 16; col++ ) {
            issX >> Xmtx( row, col );
            issY >> Ymtx( row, col );
        }
    }

    inputX.close( );
    inputY.close( );
    int i, j, k;
    for ( i = 0; i < 16; i++ ) {
        for ( j = 0; j < 16; j++ ) {
            cout << "X " << Xmtx( i, j ) << " Y " << Ymtx( i, j ) << endl;
        }
    }
    output2 = Xmtx * Ymtx;
    int count = 0;

    for ( int b = 0; b < 3; b++ ) {
        block_size = block_size_[b];
        n_threads = block_size * block_size;

        count = 0;
        while ( count < 50000 ) {



            start = high_resolution_clock::now();
#pragma omp parallel shared(Xmtx, Ymtx, output ,n_threads, block_size) private(tid,i,j,k)
            {
#pragma omp parallel num_threads(n_threads)
                {
                    tid = omp_get_thread_num( );
#pragma omp parallel for
                    for ( i = 0; i < 16; i++ ) {
                        for ( j = 0; j < 16; j++ )
                            for ( k = 0; k < 16; k++ )
#pragma omp critical
                                output( i, j ) += Xmtx( i, k ) * Ymtx( k, j );
                    }
                }
            }
            end = high_resolution_clock::now();
            elapsed_time = duration_cast<nanoseconds>(end - start);
            cout << block_size << ", " << elapsed_time.count() << endl;
            outputFile << block_size << ", " << elapsed_time.count() << endl;
            count++;
        }
    }
    return 0;
}
