#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

#define N 7500
#define K 50

typedef struct
{
	double x;
	double y;
} point;

void init_observations(point *observations,FILE *input);
void init_centroids(point *centroids);
void init_clusters(point *observations, point *centroids, int *clusters);
int assign_observations(point *observations, point *centroids, int *clusters);
void calculate_means(point *observations, point *centroids, int *clusters);
double calculate_MSE(point *observations, point *centroids, int *clusters);
void print(point *observations, point *centroids, int *clusters,FILE *output);

int main()
{
	point observations[N] = {0};
	point centroids[K] = {0};
	int clusters[N] = {0}, change = 0;
	FILE *input, *output;
	char input_str[] = "C:/Users/olevi/Downloads/Or/in_a3.txt";
	char output_str[] = "C:/Users/olevi/Downloads/Or/out.txt";

	input = fopen(input_str, "r");
	output = fopen(output_str, "w");
	srand( (unsigned)time( NULL ) );

	init_observations(observations,input);
	init_centroids(centroids);
	init_clusters(observations,centroids,clusters);

	do
	{
		change = assign_observations(observations,centroids,clusters);
		calculate_means(observations,centroids,clusters);
		fprintf(output,"\n%lf",calculate_MSE(observations,centroids,clusters));
	} while(change == 1);

	//print(observations,centroids,clusters,output);

	fprintf(output,"\n%lf",calculate_MSE(observations,centroids,clusters));
	return 0;
}
/***************************************************************************************************************************/
void init_observations(point *observations,FILE *input)
{
	int i;
	for(i=0;i<N;++i)
	{
		fscanf(input,"%lf",&observations[i].x);
		fscanf(input,"%lf",&observations[i].y);
	}
}
/***************************************************************************************************************************/
void init_centroids(point *centroids)
{
	int i;
	for(i=0;i<K;++i)
	{
		centroids[i].x = -1;
		centroids[i].y = -1;
	}
}
/***************************************************************************************************************************/
void init_clusters(point *observations, point *centroids, int *clusters)
{
	int i , j;

	for(i=0;i<N;++i)
	{
		clusters[i] = -1;
	}

	i = 0;
	while(i < K)
	{
		j = rand() % N; // makes lower numbers slightly more likely
		if(clusters[j] == -1)
		{
			centroids[i] = observations[j];
			clusters[j] = i;
			++i;
		}
	}
}
/***************************************************************************************************************************/
int assign_observations(point *observations, point *centroids, int *clusters)
{
	int i,j, closest_pos, change = 0;
	double dist, closest_dist;
	for(i=0; i<N; ++i)
	{
		closest_pos = 0;
		closest_dist = sqrt(pow(observations[i].x-centroids[0].x,2) + pow(observations[i].y-centroids[0].y,2));   
		for(j=1; j<K; ++j)
		{
			dist = sqrt(pow(observations[i].x-centroids[j].x,2) + pow(observations[i].y-centroids[j].y,2)); 
			if(dist < closest_dist)
			{
				closest_dist = dist;
				closest_pos = j;
			}
		}
		if(clusters[i] != closest_pos)
		{
			change = 1;
		}
		clusters[i] = closest_pos;
	}
	return change;
}
/***************************************************************************************************************************/
void calculate_means(point *observations, point *centroids, int *clusters)
{
	int i, j, num;
	point total;
	for(i=0;i<K;++i)
	{

		total.x = 0; 
		total.y = 0;
		num = 0;
		for(j=0;j<N;++j)
		{
			if(clusters[j] == i)
			{
				total.x += observations[j].x;
				total.y += observations[j].y;
				++num;
			}
		}
		centroids[i].x = total.x / num;
		centroids[i].y = total.y / num;
	}
}
/***************************************************************************************************************************/
double calculate_MSE(point *observations, point *centroids, int *clusters)
{
	double MSE = 0;
	int i;
	for(i=0;i<N;++i)
	{
		MSE += pow(observations[i].x-centroids[clusters[i]].x,2)+pow(observations[i].y-centroids[clusters[i]].y,2);
	}
	return MSE;
}
/***************************************************************************************************************************/
void print(point *observations, point *centroids, int *clusters,FILE *output)
{
	int i;
	fprintf(output,"X = [");
	for(i=0;i<N;++i)
	{
		fprintf(output,"%lf ",observations[i].x);
	}
	for(i=0;i<K;++i)
	{
		fprintf(output,"%lf ",centroids[i].x);
	}
	fprintf(output,"]\n");

	fprintf(output,"Y = [");
	for(i=0;i<N;++i)
	{
		fprintf(output,"%lf ",observations[i].y);
	}
	for(i=0;i<K;++i)
	{
		fprintf(output,"%lf ",centroids[i].y);
	}
	fprintf(output,"]\n");

	fprintf(output,"C = [");
	for(i=0;i<N;++i)
	{
		fprintf(output,"%d ",clusters[i]);
	}
	for(i=0;i<K;++i)
	{
		fprintf(output,"%d ",2);
	}
	fprintf(output,"]\n");
}