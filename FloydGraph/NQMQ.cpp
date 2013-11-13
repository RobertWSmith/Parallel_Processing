/*
 * NQMQ.cpp
 *
 *  Created on: Oct 13, 2013
 *      Author: rob
 */

#include "NQMQ.h"
//long n_cities;
//long n_threads;
//long total_distance;
//vector<string> place_name;
//vector<string> route;
//vector<int> inputs;

NQMQ::NQMQ( std::string file_name ) :
    n_cities(0), n_threads(0), s_total_distance(0), p_total_distance(0), vertexes(
        0), place_name(), route(), origin(0), destination(0), inputs(), outputs(), predicted() {
  std::string line;
  std::ifstream data_file(file_name.c_str());
  std::vector<int> temp_arr(3);
  std::stringstream ss;

  n_threads = (long) 16;

  int count = 0;
  if (data_file.is_open()) {
    while (getline(data_file, line)) {
      ss << line;
      if (count < 1) {
        ss >> n_cities;
        route.resize(n_cities);
        inputs.reshape(n_cities, n_cities);
        outputs.reshape(n_cities, n_cities);
        predicted.reshape(n_cities, n_cities);
        predicted.zeros();
        inputs.fill(9999);
        outputs.fill(9999);
        for (int i = 0; i < n_cities; i++) {
          inputs(i, i) = 0;
          outputs(i, i) = 0;
        }
      } else if (count <= n_cities) {
        place_name.push_back(line);
      } else {
        std::stringstream line_stream(line);
        line_stream >> temp_arr[0] >> temp_arr[1] >> temp_arr[2];
        if (temp_arr[0] > -1) {
          inputs(temp_arr[0] - 1, temp_arr[1] - 1) = temp_arr[2];
          outputs(temp_arr[0] - 1, temp_arr[1] - 1) = temp_arr[2];
          inputs(temp_arr[1] - 1, temp_arr[0] - 1) = temp_arr[2];
          outputs(temp_arr[1] - 1, temp_arr[0] - 1) = temp_arr[2];
          vertexes++;
        }
      }
      count++;
    }
    data_file.close();
  }

}

void NQMQ::serialFloyd( int orig, int dest ) {
  vector<int> rt;
  for (int k = 0; k < n_cities; k++) {
    for (int i = 0; i < n_cities; i++) {
      for (int j = 0; j < n_cities; j++) {
        if (outputs(i, j) > outputs(i, k) + outputs(k, j)) {
          outputs(i, j) = outputs(i, k) + outputs(k, j);
          predicted(i, j) = k;
        }
      }
    }
  }

  s_total_distance = outputs(orig, dest);
}

