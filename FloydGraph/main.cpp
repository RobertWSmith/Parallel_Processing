/*
 * main.cpp
 *
 *  Created on: Oct 13, 2013
 *      Author: rob
 */

#include "NQMQ.h"

#include <array>
#include <iostream>
#include <pthread.h>
#include <fstream>
#include <time.h>
#include <sys/time.h>

using namespace std;

NQMQ m( "nqmq.dat" );

void path( int i, int j ) {
  if ( m.predicted( i, j ) == 0 ) {
    cout << m.place_name[i] << endl;
    cout << m.place_name[j] << endl;
  } else {
    path( i, m.predicted( i, j ) );
    path( m.predicted( i, j ), j );
  }
}

void* floyd( void* rank ) {
  long k = ( long ) rank;

  for ( int i = 0; i < m.n_cities; i++ ) {
    for ( int j = 0; j < m.n_cities; j++ ) {
      if ( m.outputs( i, j ) > m.outputs( i, k ) + m.outputs( k, j ) ) {
        m.outputs( i, j ) = m.outputs( i, k ) + m.outputs( k, j );
        m.predicted( i, j ) = ( int ) k;
      }
    }
  }

  return NULL;
}

void pthread_floyd( NQMQ & nq ) {
  pthread_t threads[nq.n_threads];

  for ( long k = 0; k < nq.n_cities; k++ ) {
    for ( int i = 0; i < nq.n_threads; i++ ) {
      pthread_create( &threads[i], NULL, floyd, ( void* ) k );
    }
  }

  for ( int j = 0; j < nq.n_threads; j++ ) {
    pthread_join( threads[j], NULL );
  }

  nq.p_total_distance = nq.outputs( m.origin, m.destination );

}

int main( ) {

  unsigned long elapsed_time = 0;
//  cout << m.distances << endl;

  for ( int i = 0; i < ( int ) m.place_name.size( ); i++ ) {
    cout << i << " " << m.place_name[i] << endl;
  }

  cout << endl;

  char input = 'y';

  while ( input == 'y' ) {

    cout << "Origin City: ";
    cin >> m.origin;
    cout << "Destination City: ";
    cin >> m.destination;
    cout << endl;

//  m.serialFloyd(m.origin, m.destination);
//  cout << "Serial Total Distance: " << m.s_total_distance << endl;

    ofstream outputFile;
    outputFile.open( "P3.csv", ios::out );
    struct timeval start, end;

    std::array<long, 3> num_thrd = { { 2, 4, 7 } };

    for ( long thrd : num_thrd ) {
      for ( int i = 0; i < 25; i++ ) {
        gettimeofday( &start, NULL );
        pthread_floyd( m );
        gettimeofday( &end, NULL );
        elapsed_time =
            ( unsigned long ) ( ( end.tv_sec * 1000000 + end.tv_usec )
                - ( start.tv_sec * 1000000 + start.tv_usec ) );
        outputFile << thrd << ", " << elapsed_time << endl;
      }
    }

    cout << "~~~ Route ~~~" << endl;
    path( m.origin, m.destination );

    cout << "pthread Total Distance: " << m.p_total_distance << " miles"
        << endl;

    do {
      if ( input != 'y' || input != 'n' )
        cout << "I didn't understand that" << endl;
      cout << "continue? (y/n): ";
      cin >> input;

    } while ( input != 'y' || input != 'n' );
  }

  return 0;
}
