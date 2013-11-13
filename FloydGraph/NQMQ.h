/*
 * NQMQ.h
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

  vector<string> printMatrix();
  void serialFloyd( int, int );

};

#endif /* NQMQ_H_ */
