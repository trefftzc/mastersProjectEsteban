#include <iostream>
#include <chrono>
#include <vector>
#include <numeric>
#include <thrust/reduce.h>
#include <thrust/device_vector.h>
#include <thrust/functional.h>
#include <thrust/host_vector.h>
#include <thrust/tuple.h>
#include <thrust/iterator/zip_iterator.h>
#include <thrust/sequence.h>

// Calculate the maximum independent set

/*
 * In this code we use a zip operator to create a structure (tuple) out of two vectors
 * Then a customized comparator is used to compare the tuples
 * The customized comparator compareTuples bases the result of what it returns
 * exclusively on the value of the fist entry on the tuple
 */

using namespace std;

const int MAX_GRAPH_SIZE = 31;

struct isIndependentSet
{
  const int n;
  const int *adjM;

  isIndependentSet(int _n,int *_adjM) : n(_n),adjM(_adjM) {}

  //  __host__
  __device__
  int operator() (const int x) {
    int mask = 1;
    int bitCounter = 0;
    int candidateSet[MAX_GRAPH_SIZE]; // ????
    for(int i = 0;i < n;i++) {
	if (mask & x) {
	  candidateSet[i] = 1;
	  bitCounter++;
	}
	else {
	  candidateSet[i] = 0;
	}
	mask = mask * 2;
    }

    bool isIndependentSet = true;

    for(int i =0;i < n;i++) {
      // Check if this candidate node is connected
      // to any other node in the subset
      if (candidateSet[i] == 1) {
	for(int j = i+1;j < n;j++) {
	  // If this is true, this is not an independent set
	  // nodes i and j are connected
	  if (candidateSet[j] ==1) {
	    if (adjM[i*n+j]==1) {
	      isIndependentSet = false;
	    }
	  }
	}
      }
    }
    if (isIndependentSet) {
      return bitCounter;
    }
    else {
      return -1;
    }
  }
};

// The custom comparator for tuples
struct compareTuples : public thrust::binary_function<
  thrust::tuple<int,int>,
    thrust::tuple<int,int>,
    thrust::tuple<int,int> >
  {
    __host__ __device__
      thrust::tuple<int,int> operator()(thrust::tuple<int,int> x,
		       thrust::tuple<int,int> y)
    { 
      if (get<0>(x)  > get<0>(y)) {
	return x;
      }
      else {
	return y;
      }
    }
  };

void printResults(thrust::tuple<int,int> r,int n) {
  std::cout << "The encoded set is: " << get<1>(r) << std::endl;
  std::cout << "Which corresponds to nodes: " << std::endl;
  int mask = 1;
  for(int i = 0;i < n;i++) {
    if ((get<1>(r) & mask) != 0) {
      std::cout << i << " " ;
    }
    mask = mask*2;
  }
  std::cout << std::endl;
  std::cout << "That is " << get<0>(r) << " elements. " << std::endl;
}


// initialize vectors
int main(int argc, char *argv[]) {
  int n;
  std::cin >> n;
  cout << "The value of n is " << n << std::endl;
  thrust::host_vector<int> hostAdjMat(n*n);
  for(int i = 0;i < n;i++) {
	  for(int j = 0;j < n;j++) {
		  std::cin >> hostAdjMat[i*n+j];
	  }
  }
  // Print the adjacency matrix
  /*
  std::cout << "This graph has " << n << " nodes." << std::endl;
  std::cout << "This is its adjacency matrix: " << std::endl;

  for(int i = 0;i < n;i++) {
    for(int j = 0;j < n;j++) {
      std::cout << hostAdjMat[i*n+j] << " " ;
    }
    std::cout << endl;
  }
 */ 
  thrust::device_vector<int> deviceAdjMat(n*n);
  deviceAdjMat = hostAdjMat;
  int powerOf2 = 1;
  for(int i = 0;i < n;i++) {
    powerOf2 = powerOf2 * 2;
  }
  std::cout << "Power of 2 is : " << powerOf2 << std::endl;
  thrust::host_vector<int> sequentialValues(powerOf2);
  thrust::sequence(sequentialValues.begin(),sequentialValues.end());
  thrust::device_vector<int>  nodesInClique(powerOf2);
  thrust::device_vector<int> encodingOfSubset(powerOf2);

  encodingOfSubset = sequentialValues;

  isIndependentSet imis(n,thrust::raw_pointer_cast(deviceAdjMat.data()));
  thrust::transform(encodingOfSubset.begin(),encodingOfSubset.end(),nodesInClique.begin(),imis);

 auto first = thrust::make_zip_iterator(thrust::make_tuple(nodesInClique.begin(), encodingOfSubset.begin()));
 auto last  = thrust::make_zip_iterator(thrust::make_tuple(nodesInClique.end(),   encodingOfSubset.end()));

 compareTuples binary_op;
 thrust::tuple<int,int> init = first[0];
 thrust::tuple < int, int > result  = thrust::reduce(first, last, init, binary_op);

 printResults(result,n);
 
}
