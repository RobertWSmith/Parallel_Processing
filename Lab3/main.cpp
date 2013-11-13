
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

class Map {
public:
  long n_cities;
  long n_threads;
  long total_distance;
  vector< string > place_name;
  vector< string > route;
  vector<int> distances;

  Map(string);
  Map( const Map& );
  virtual ~Map( );

  void serialFloyd( int, int );
  void pthreadFloyd( int, int );
  int rowCol( int, int );
};

Map::Map( string file_name ) :
    n_cities(), n_threads(), total_distance(), place_name(), route(), distances() {

  string line;
  ifstream data_file(file_name.c_str());
  vector<int> temp_arr(3);
  stringstream ss;

  n_threads = (long) 4;

  int count = 0;
  if (data_file.is_open()) {
    while (getline(data_file, line)) {
      count++;
      ss << line;
      if (count == 1) {
        ss >> n_cities;
        distances.resize(n_cities * n_cities);
        fill(distances.begin(), distances.end(), 9999);
      } else if (count <= n_cities) {
        place_name.push_back(line);
      } else {
        ss >> temp_arr[0] >> temp_arr[1] >> temp_arr[2];
        if (temp_arr[0] > -1) {
          distances[(rowCol(temp_arr[0], temp_arr[1]))] = temp_arr[2];
        }
      }
    }
    data_file.close();
  }

}

int Map::rowCol( int row, int col ) {
  return ((row * n_cities) + col);
}

void Map::serialFloyd( int orig, int dest ) {
  int k, i, j;
  vector<int> output(distances);

  for (k = 0; k < n_cities; k++) {
    for (i = 0; i < n_cities; i++) {
      for (j = 0; j < n_cities; j++) {
        if (output[rowCol(i, j)]
            > (output[rowCol(i, j)] + output[rowCol(k, j)])) {
          output[rowCol(i, j)] = output[rowCol(i, k)] + output[rowCol(k, j)];
        }
      }
    }
  }

  total_distance = output[rowCol(orig, dest)];
}

void Map::pthreadFloyd( int orig, int dest ) {

}


int main() {
  Map m("nqmq.dat");

  int orig, dest;
  cout << "Origin City: ";
  cin >> orig;
  cout << "Destination City: ";
  cin >> dest;

  m.serialFloyd(orig, dest);
  cout << "Total Distance: " << m.total_distance << endl;

  return 0;
}
