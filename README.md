# Genetic K-Means
Presented at eBay Inc Data Conference: “Survival of the Fittest: Using Genetic Algorithm for Data Mining Optimization”

An efficient implementation of genetic k-means compared against standard k-means on the classic A3 data set.

# Motivation
K-means algorithm is a popular choice for cluster analysis due to its simplicity and effectiveness. Yet, it is very much dependent on the initial choice of cluster centers and this is why it tends to converge to local optimums. 
As demonstrated below, even running k-means multiple times can help to reduce the volatility of the results, but it is still cannot get past local optimums. 
In contrast, Genetic K-means utilizes operations inspired by natural evolution, specifically mutation and crossover, to get closer to global optimum.

# Demo

![alt text](https://github.com/orlevi18/GA_for_Data_Mining_Optimization/blob/master/demo_standard_k_means.gif?raw=true)

![alt text](https://github.com/orlevi18/GA_for_Data_Mining_Optimization/blob/master/demo_genetic_k_means.gif?raw=true)

![alt text](https://github.com/orlevi18/GA_for_Data_Mining_Optimization/blob/master/a3_results_comparison.jpg?raw=true)
