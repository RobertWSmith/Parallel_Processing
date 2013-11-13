/*
 * main.cpp
 *
 *  Created on: Oct 13, 2013
 *      Author: rob
 */

#include <array>
#include <string>
#include <vector>

#include <fstream>
#include <iostream>
#include <sstream>

#include <pthread.h>

using std::cout;
using std::endl;
using std::array;
using std::vector;
using std::string;
using std::stringstream;
using std::ifstream;

template<class T>
class Matrix {

private:
  vector<T> data;
  size_t R, C;

  size_t RC( size_t row, size_t col ) {
    return row * C + col;
  }

public:

  Matrix( ) : data( ) { }

  Matrix( T fill_val ) : data( ) {
    fill( fill_val );
  }

  Matrix ( T fill_val, T diag_val ) : data( ) {
    fill( fill_val, diag_val );
  }

  Matrix( const Matrix& m ) : data( m.getData( ) ) { }

  T& operator( )( size_t row, size_t col ) {
    return data[RC( row, col )];
  }

  void resize( size_t rows, size_t cols ) {
    setRows( rows );
    setCols( cols );
    data.resize( rows * cols );
  }

  void setRows( size_t rows ) {
    R = rows;
  }

  void setCols( size_t cols ) {
    C = cols;
  }

  size_t getRows( ) {
    return R;
  }

  size_t getCols( ) {
    return C;
  }

  const vector<T>& getData( ) const {
    return data;
  }

  void fill( T val ) {
    for ( T& d : data ) {
      d = val;
    }
  }

  void fill( T val, T diag_val ) {
    fill( val );
    setDiag( diag_val );
  }

  void setDiag( T val ) {
    if ( R == C ) {
      for ( size_t i = 0; i < R; i++ ) {
        data[RC( i, i )] = val;
      }
    }
  }
} ;

template <class T>
class NQMQ {

public:
  long n_cities, n_threads, s_total_distance, p_total_distance;
  int vertexes, origin, destination;
  vector<string> place_name;
  vector<vector<int> > route;
  vector<string> shortest_path;
  Matrix<T> inputs, outputs, predicted;
  Matrix<size_t> path;

  NQMQ( string file_name ) :
  n_cities( 0 ), n_threads( 2 ), s_total_distance( 0 ), p_total_distance( 0 ),
  vertexes( 0 ), origin( 0 ), destination( 0 ), place_name( ), route( ),
  inputs( ), outputs( ), predicted( ), path( ) {

    string line;
    ifstream data_file( file_name.c_str( ) );
    vector<int> temp_arr( 3 );
    stringstream ss;

    int count = 0;
    if ( data_file.is_open( ) ) {
      while ( getline( data_file, line ) ) {
        ss << line;
        if ( count < 1 ) {
          ss >> n_cities;
          route.resize( n_cities );

          inputs.resize( n_cities, n_cities );
          outputs.resize( n_cities, n_cities );
          predicted.resize( n_cities, n_cities );
          path.resize( n_cities, n_cities );

          inputs.fill( 9999, 0 );
          outputs.fill( 9999, 0 );
          path.setDiag( 9999 );

        } else if ( count <= n_cities ) {

          place_name.push_back( line );

        } else {
          stringstream line_stream( line );
          line_stream >> temp_arr[0] >> temp_arr[1] >> temp_arr[2];
          if ( temp_arr[0] > -1 ) {

            inputs( temp_arr[0] - 1, temp_arr[1] - 1 ) = temp_arr[2];
            outputs( temp_arr[0] - 1, temp_arr[1] - 1 ) = temp_arr[2];
            path( temp_arr[0] - 1 , temp_arr[1] - 1 ) = temp_arr[0];
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

  // expects cleared vector of shortest path

  void findShortestPath( int orig, int dest )  {
    if ( predicted( orig, dest ) == 0 ) {

      //      shortest_path.emplace( shortest_path.begin( ) + ( shortest_path.size( )  / 2 ), place_name[orig] );
      shortest_path.push_back( place_name[orig] );

      shortest_path.push_back( place_name[dest] );



    } else {

      findShortestPath( orig, predicted( orig, dest ) );
      findShortestPath( predicted( orig, dest ), dest );

    }
  }

  void printShortestPath( int orig, int dest ) {
    shortest_path.clear( );

    findShortestPath( orig, dest );

    //    for ( size_t i = 0; i < shortest_path.size( ); i++ ) {
    //      cout << i + 1 << ".\t" << shortest_path[i] << endl;
    //    }

    for ( size_t i = 0; i < shortest_path.size( ); i++ ) {
      if ( i < 2 ) {
        cout << i << ".\t" << shortest_path[i] << endl;
      } else {
        if ( shortest_path[i] == shortest_path[( i - 1 )] ) {
          // do nothing if the elements are the same
        } else {
          cout << i << ".\t" << shortest_path[i] << endl;
        }
      }
    }


  }

  void serialFloyd( int orig, int dest ) {
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


} ;


using namespace std;

/******************************************************************************
 *  BEGIN global declarations
 */
NQMQ<int> m( "nqmq.dat" );

/******************************************************************************
 *  END global declarations
 */

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

template <class T>
void pthread_floyd( NQMQ<T> &nq ) {
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

  //  unsigned long elapsed_time = 0;
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
    //    struct timeval start, end;

    //    std::array<long, 3> num_thrd = {
    //      { 2, 4, 7 }
    //    };

    //        for ( long thrd : num_thrd ) {
    //            for ( int i = 0; i < 25; i++ ) {
    //                gettimeofday( &start, NULL );
    m.n_threads = 2;
    pthread_floyd( m );

    //                gettimeofday( &end, NULL );
    //                elapsed_time =
    //                    ( unsigned long ) ( ( end.tv_sec * 1000000 + end.tv_usec )
    //                    - ( start.tv_sec * 1000000 + start.tv_usec ) );
    //                outputFile << thrd << ", " << elapsed_time << endl;
    //            }
    //        }

    cout << "~~~ Route ~~~" << endl;
    m.printShortestPath( m.origin, m.destination );

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