maxClique: maxClique.cu
	nvcc maxClique.cu -o maxClique -O3

maxIndependentSet: maxIndependentSet.cu
	nvcc maxIndependentSet.cu -o maxIndependentSet -O3

minDominatingSet: minDominatingSet.cu
	nvcc minDominatingSet.cu -o minDominatingSet -O3

maxCliqueSequential: maxClique.cpp
	g++ maxClique.cpp -o maxCliqueSequential -std=c++17 -O3

minDominatingSetSequential: minDominatingSet.cpp
	g++ minDominatingSet.cpp -o minDominatingSetSequential -std=c++17 -O3

maxIndependentSetSequential: maxIndependentSet.cpp
	g++ maxIndependentSet.cpp -o maxIndependentSetSequential -std=c++17 -O3 



