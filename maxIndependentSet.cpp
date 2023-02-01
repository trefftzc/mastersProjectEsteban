#include <iostream>
#include <chrono>
#include <vector>
#include <numeric>
#include <tuple>
#include <algorithm>


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
  // __device__
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
	for(int j = 0;j < n;j++) {
	  // If this is true, this is not an independent set
	  // nodes i and j are connected
	  if (i!=j && candidateSet[j] ==1) {
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
struct compareTuples : public std::binary_function<
  std::tuple<int,int>,
    std::tuple<int,int>,
    std::tuple<int,int> >
  {
    //    __host__ __device__
      std::tuple<int,int> operator()(std::tuple<int,int> x,
		       std::tuple<int,int> y)
    { 
      if (get<0>(x)  > get<0>(y)) {
	return x;
      }
      else {
	return y;
      }
    }
  };

void printResults(std::tuple<int,int> r,int n) {
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
  std::vector<int> hostAdjMat(n*n);
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

  int powerOf2 = 1;
  for(int i = 0;i < n;i++) {
    powerOf2 = powerOf2 * 2;
  }
  std::cout << "Power of 2 is : " << powerOf2 << std::endl;

  std::vector<int>  nodesInMaxIndSet(powerOf2);
  std::vector<int> encodingOfSubset(powerOf2);
  for(int i = 0; i < powerOf2;i++) {
    encodingOfSubset[i] = i;
  }


  isIndependentSet imis(n,(hostAdjMat.data()));
  std::transform(encodingOfSubset.begin(),encodingOfSubset.end(),nodesInMaxIndSet.begin(),imis);

  int sizeOfMaxIndSet = 0;
  int encodingOfMaxIndSet = 0;
  for(int i = 0;i < powerOf2;i++) {
    if (nodesInMaxIndSet[i] > sizeOfMaxIndSet) {
      sizeOfMaxIndSet = nodesInMaxIndSet[i];
      encodingOfMaxIndSet = encodingOfSubset[i];
    }
  }
  std:: tuple < int, int > result(sizeOfMaxIndSet,encodingOfMaxIndSet);


 printResults(result,n);
 
}
