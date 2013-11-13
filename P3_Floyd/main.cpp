/*
 * main.cpp
 *
 *  Created on: Oct 13, 2013
 *      Author: rob
 */

#ifndef NQMQ_H_
#define NQMQ_H_

#include <fstream>
#include <sstream>

#include <string>
#include <vector>
#include <armadillo>
#include <pthread.h>

using std::string;
using std::vector;
using arma::Mat;

class NQMQ {

public:
  long n_cities;
  long n_threads;
  long s_total_distance;
  long p_total_distance;
  int vertexes;
  vector<string> place_name;
  vector<vector<int> > route;
  int origin, destination;
  Mat<int> inputs, outputs, predicted;

  NQMQ( string );

  vector<string> printMatrix( );
  void serialFloyd( int, int );

} ;

#endif /* NQMQ_H_ */

//long n_cities;
//long n_threads;
//long total_distance;
//vector<string> place_name;
//vector<string> route;
//vector<int> inputs;

NQMQ::NQMQ( std::string file_name ) :
n_cities( 0 ), n_threads( 0 ), s_total_distance( 0 ), p_total_distance( 0 ), vertexes(
                                                                                       0 ), place_name( ), route( ), origin( 0 ), destination( 0 ), inputs( ), outputs( ), predicted( ) {
  std::string line;
  std::ifstream data_file( file_name.c_str( ) );
  std::vector<int> temp_arr( 3 );
  std::stringstream ss;

  n_threads = (long) 16;

  int count = 0;
  if ( data_file.is_open( ) ) {
    while ( getline( data_file, line ) ) {
      ss << line;
      if ( count < 1 ) {
        ss >> n_cities;
        route.resize( n_cities );
        inputs.reshape( n_cities, n_cities );
        outputs.reshape( n_cities, n_cities );
        predicted.reshape( n_cities, n_cities );
        predicted.zeros( );
        inputs.fill( 9999 );
        outputs.fill( 9999 );
        for ( int i = 0; i < n_cities; i++ ) {
          inputs( i, i ) = 0;
          outputs( i, i ) = 0;
        }
      } else if ( count <= n_cities ) {
        place_name.push_back( line );
      } else {
        std::stringstream line_stream( line );
        line_stream >> temp_arr[0] >> temp_arr[1] >> temp_arr[2];
        if ( temp_arr[0] > -1 ) {
          inputs( temp_arr[0] - 1, temp_arr[1] - 1 ) = temp_arr[2];
          outputs( temp_arr[0] - 1, temp_arr[1] - 1 ) = temp_arr[2];
          inputs( temp_arr[1] - 1, temp_arr[0] - 1 ) = temp_arr[2];
          outputs( temp_arr[1] - 1, temp_arr[0] - 1 ) = temp_arr[2];
          vertexes++;
        }
      }
      count++;
    }
    data_file.close( );
  }

}

void NQMQ::serialFloyd( int orig, int dest ) {
  vector<int> rt;
  for ( int k = 0; k < n_cities; k++ ) {
    for ( int i = 0; i < n_cities; i++ ) {
      for ( int j = 0; j < n_cities; j++ ) {
        if ( outputs( i, j ) > outputs( i, k ) + outputs( k, j ) ) {
          outputs( i, j ) = outputs( i, k ) + outputs( k, j );
          predicted( i, j ) = k;
        }
      }
    }
  }

  s_total_distance = outputs( orig, dest );
}


#include <pthread.h>
#include <array>
#include <iostream>
#include <pthread.h>
#include <fstream>
#include <time.h>
#include <sys/time.h>
#include <vector>
#include <string>

using namespace std;

vector<string> shortest_path;

NQMQ m( "nqmq.dat" );

void path( int i, int j ) {
  if ( m.predicted( i, j ) == 0 ) {
    shortest_path.emplace( shortest_path.begin( ), m.place_name[i] );
    shortest_path.push_back( m.place_name[j] );
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
  bool cont = true;

  do {

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

    std::array<long, 3> num_thrd = {
      { 2, 4, 7 }
    };

    //        for ( long thrd : num_thrd ) {
    //            for ( int i = 0; i < 25; i++ ) {
    //                gettimeofday( &start, NULL );

    pthread_floyd( m );

    //                gettimeofday( &end, NULL );
    //                elapsed_time =
    //                    ( unsigned long ) ( ( end.tv_sec * 1000000 + end.tv_usec )
    //                    - ( start.tv_sec * 1000000 + start.tv_usec ) );
    //                outputFile << thrd << ", " << elapsed_time << endl;
    //            }
    //        }

    cout << "~~~ Route ~~~" << endl;
    path( m.origin, m.destination );

    for ( size_t i = 0; i < shortest_path.size( ); i++ ) {
      if ( i < 2 ) {
        cout << i << ".\t" << shortest_path[i] << endl;
      } else {
        if ( shortest_path[i] == shortest_path[( i - 1 )] ) {
          // do nothing if the elements match
        } else {
          cout << i << ".\t" << shortest_path[i] << endl;
        }
      }
    }

    cout << "pthread Total Distance: " << m.p_total_distance << " miles"
        << endl;

    cout << "Would you like to continue? (y/n)";
    try {
      cin >> input;
    } catch ( exception& ex ) {
      throw ex;
    }

    while ( input != 'y' && input != 'n' ) {
      cout << endl;
      cout << "I didn't understand that, would you like to continue? (y/n)";
      try {
        cin >> input;
      } catch ( exception& ex ) {
        throw ex;
      }
    }

    if ( input == 'y' ) cont = true;
    if ( input == 'n' ) cont = false;

  } while ( cont );

  return 0;
}