#include <iostream>
#include <chrono>
#include <vector>
#include <numeric>
#include <tuple>
#include <algorithm>



/*
 * In this code we use a zip operator to create a structure (tuple) out of two vectors
 * Then a customized comparator is used to compare the tuples
 * The customized comparator compareTuples bases the result of what it returns
 * exclusively on the value of the fist entry on the tuple
 */

using namespace std;

const int MAX_GRAPH_SIZE = 31;

struct isClique
{
  const int n;
  const int *adjM;

isClique(int _n,int *_adjM) : n(_n),adjM(_adjM) {}

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

    bool isClique = true;

    for(int i =0;i < n;i++) {
      if (candidateSet[i] == 1) {
	for(int j = 0;j < n;j++) {
	  if (candidateSet[j] == 1) {
	    if (adjM[i*n+j] == 0) {
	      isClique = false;
	    }
	  }
	}
      }
    }
    if (isClique) {
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
  std::vector<int> deviceAdjMat(n*n);
  deviceAdjMat = hostAdjMat;
  int powerOf2 = 1;
  for(int i = 0;i < n;i++) {
    powerOf2 = powerOf2 * 2;
  }
  std::cout << "Power of 2 is : " << powerOf2 << std::endl;


  std::vector<int>  nodesInClique(powerOf2);
  std::vector<int> encodingOfSubset(powerOf2);
  for(int i = 0; i < powerOf2;i++) {
    encodingOfSubset[i] = i;
  }

  isClique ic(n,(deviceAdjMat.data()));
  std::transform(encodingOfSubset.begin(),encodingOfSubset.end(),nodesInClique.begin(),ic);
  /*
  auto first = boost::compute::make_zip_iterator(std::make_tuple(nodesInClique.begin(), encodingOfSubset.begin()));
  auto last  = boost::compute::make_zip_iterator(std::make_tuple(nodesInClique.end(),   encodingOfSubset.end()));

 compareTuples binary_op;
 std::tuple<int,int> init = first[0];
 std::tuple < int, int > result  = std::reduce(first, last, init, binary_op);
  */
  int sizeOfMaxClique = 0;
  int encodingOfMaxClique = 0;
  for(int i = 0;i < powerOf2;i++) {
    if (nodesInClique[i] > sizeOfMaxClique) {
      sizeOfMaxClique = nodesInClique[i];
      encodingOfMaxClique = encodingOfSubset[i];
    }
  }
  std:: tuple < int, int > result(sizeOfMaxClique,encodingOfMaxClique);
 printResults(result,n);
 cout << get<0>(result) << " " << get<1>(result) <<  endl;
}
