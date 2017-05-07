#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

#define POP_SIZE 10
#define GENERATIONS_NUM	20
#define CROSS_PROB 0.8 // 0.8
#define	MUTE_PROB 0.05
#define ELITE_RATE 0.2 // POP_SIZE * ELITE_RATE needs to be an even number for 2 offsprings crossover
#define N 7500
#define K 50
#define INIT_VAL -1

typedef struct
{
	double x;
	double y;
} point;

typedef struct
{
	point centroids[K];
	int obvClusters[N];
	double fitness;
	double accFitness;
} partition;

double calculate_distance(point *a,point *b);

void init_observations(point *observations,FILE *input);

void reset_partition(partition *part);
int comparePartition(const void *a, const void *b);

void init_centroids(point *observations,partition *part);
void init_partition(point *observations,partition *part);
void init_population(point *observations,partition *pop);

void assign_observations(point *observations,partition *part);
void calculate_means(point *observations,partition *part);
double calculate_MSE(point *observations,partition *part);
void calculate_fitness(point *observations,partition *pop);

void elitism(partition *pop,partition *newPop);

//partition mutation(partition *part);
//partition arithmetic_crossover(partition *pop);
//void single_point_crossover(partition *pop,partition *offsprings);
//void arithmetic_reproduction(partition *pop,partition *newPop);
partition mutation(point *observations,partition *part);
int find_closest_centroid(partition *part,point *p);
void crossover(point *observations,partition *pop,partition *offsprings);
void reproduction(point *observations,partition *pop,partition *newPop);

void print_observations(point *observations,FILE *output);
void print_partition(partition *part,FILE *output);
void print_average_fitness(partition *pop,FILE *output);
void print_population(partition *pop,FILE *output);
void print_partition_MATLAB(point *observations,partition **part,FILE *output);


int main()
{
	point observations[N] = {0};
	
	partition population[POP_SIZE] = {0};
	partition newPopulation[POP_SIZE] = {0};

	partition bestSolution;
	int currentGeneration = 0;
	
	FILE *input,*output,*outputObv;
	char input_str[] = "C:/Users/olevi/Downloads/Or/in_a3.txt";
	char output_str[] = "C:/Users/olevi/Downloads/Or/out.txt";
	char outputObv_str[] = "C:/Users/olevi/Downloads/Or/outObv.txt";
	//char output_str[] = "C:/Documents and Settings/Or/My Documents/Downloads/out.txt";
	input = fopen(input_str, "r");
	output = fopen(output_str, "w");
	outputObv = fopen(outputObv_str, "w");
	srand( (unsigned)time( NULL ) );

	init_observations(observations,input);
	print_observations(observations,outputObv);
	
	init_population(observations,population);
	//print_population(population,output);

	calculate_fitness(observations,population);

	//print_population(population,output);

	while(currentGeneration < GENERATIONS_NUM)
	{
		qsort(population,POP_SIZE,sizeof(partition),comparePartition);
		bestSolution = population[0];
		fprintf(output,"%lf\n",bestSolution.fitness);
		//print_population(population,output);
		//print_average_fitness(population,stdout);
		elitism(population,newPopulation);
		//printf("%lf\n",bestSolution.fitness);
		//print_population(population,output);
		reproduction(observations,population,newPopulation);
		++currentGeneration;
		calculate_fitness(observations,population);
	}

	//printf("%lf\n",bestSolution.fitness);
	//print_population(population,output);
	return 0;
}
/***************************************************************************************************************************/
double calculate_distance(point *a,point *b)
{
	return sqrt(pow(a->x - b->x,2) + pow(a->y - b->y,2));
}
/***************************************************************************************************************************/
void init_observations(point *observations,FILE *input)
{
	int i;
	/*for(i=0;i<N;++i)
	{
		observations[i].x = rand() % 4;
		observations[i].y = rand() % 4;
	}*/
	/*for(i=0;i<10;++i)
	{
		observations[i].x = 2 + rand() % 6;
		observations[i].y = 2 + rand() % 6;
	}
	for(i=0;i<20;++i)
	{
		observations[i].x = 12 + rand() % 6;
		observations[i].y = 12 + rand() % 6;
	}
	for(i=0;i<30;++i)
	{
		observations[i].x = 2 + rand() % 6;
		observations[i].y = 12 + rand() % 6;
	}
	for(i=0;i<40;++i)
	{
		observations[i].x = 12 + rand() % 6;
		observations[i].y = 2 + rand() % 6;
	}*/
	for(i=0;i<N;++i)
	{
		fscanf(input,"%lf",&observations[i].x);
		fscanf(input,"%lf",&observations[i].y);
	}
}
/***************************************************************************************************************************/
int comparePartition(const void *a, const void *b)
{
  if ( ((partition*)a)->fitness <  ((partition*)b)->fitness ) return -1;
  if ( ((partition*)a)->fitness == ((partition*)b)->fitness ) return 0;
  if ( ((partition*)a)->fitness >  ((partition*)b)->fitness ) return 1;
}
/***************************************************************************************************************************/
void init_centroids(point *observations,partition *part)
{
	int i , j;

	for(i=0;i<N;++i)
	{
		part->obvClusters[i] = INIT_VAL;
	}

	i = 0;
	while(i < K)
	{
		j = rand() % N; // makes lower numbers slightly more likely
		if(part->obvClusters[j] == INIT_VAL)
		{
			part->centroids[i] = observations[j];
			part->obvClusters[j] = i;
			++i;
		}
	}
}
/***************************************************************************************************************************/
void init_partition(point *observations,partition *part)
{
	int i;
	for(i=0;i<K;++i)
	{
		part->centroids[i].x = INIT_VAL;
		part->centroids[i].y = INIT_VAL;
	}
	init_centroids(observations,part);
	part->fitness = INIT_VAL;
	part->accFitness = INIT_VAL;
}

/***************************************************************************************************************************/
void init_population(point *observations,partition *pop)
{
	int i;
	for(i=0;i<POP_SIZE;++i)
	{
		init_partition(observations,&pop[i]);
	}
	// check that all partitions are unique? 
	// float point high precision -> 0 chances for identical K centroids
	// only 1st generation (centroids = integers from data set)?
	// still small chances of identical K centroids for large N
}
/***************************************************************************************************************************/
void assign_observations(point *observations,partition *part)
{
	int i,j, closest_pos;
	double dist, closest_dist;
	for(i=0; i<N; ++i)
	{
		// if data set contains duplicates, following will prevent empty sets as result of 2 identical centroids in 1st gen
		/*
		if(part->obvClusters[i] != INIT_VAL)
		{
			closest_pos = part->obvClusters[i];
			closest_dist = calculate_distance(&observations[i],&((part)->centroids[closest_pos])); 
			if(closest_dist == 0) break;
		}*/
		closest_pos = 0;
		closest_dist = calculate_distance(&observations[i],&(part->centroids[0])); 
		for(j=1; j<K; ++j)
		{
			dist = calculate_distance(&observations[i],&(part->centroids[j])); 
			if(dist < closest_dist)
			{
				closest_dist = dist;
				closest_pos = j;
			}
		}
		part->obvClusters[i] = closest_pos;
	}
}
/***************************************************************************************************************************/
void calculate_means(point *observations,partition *part)
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
			if((part)->obvClusters[j] == i)
			{
				total.x += observations[j].x;
				total.y += observations[j].y;
				++num;
			}
		}
		part->centroids[i].x = total.x / num;
		part->centroids[i].y = total.y / num;
	}
}
/***************************************************************************************************************************/
double calculate_MSE(point *observations,partition *part)
{
	double MSE = 0;
	int i;
	for(i=0;i<N;++i)
	{
		MSE += pow(calculate_distance(&observations[i],&((part)->centroids[(part)->obvClusters[i]])),2);
	}
	return MSE;
}
/***************************************************************************************************************************/
void calculate_fitness(point *observations,partition *pop)
{
	int i;
	double totalFitness = 0;

	for(i=0;i<POP_SIZE;++i)
	{
		assign_observations(observations,&pop[i]);
		calculate_means(observations,&pop[i]);
		pop[i].fitness = calculate_MSE(observations,&pop[i]);
		totalFitness += pop[i].fitness;
	}
	pop[0].accFitness = pop[0].fitness/totalFitness;
	for(i=1;i<POP_SIZE;++i)
	{
		pop[i].accFitness = pop[i-1].accFitness + pop[i].fitness/totalFitness;	
	}
}

/***************************************************************************************************************************/
void elitism(partition *pop,partition *newPop)
{
	int i;
	for(i=0;i<ELITE_RATE*POP_SIZE;++i)
	{
		newPop[i] = pop[i];
	}
}
/***************************************************************************************************************************/
int selection(partition *pop)
{
	int i;
	double num = (double)rand()/(double)RAND_MAX;
	for(i=0;i<POP_SIZE;++i)
	{
		if(pop[i].accFitness >= num)
		{
			return i;
		}
	}
}
/***************************************************************************************************************************/
/*partition mutation_move_point(partition *part)
{
	int i;
	double num1;
	double num2;
	double sign;

	for(i=0;i<K;++i)
	{
		num1 = (double)rand()/(double)RAND_MAX;
		if(num1 < MUTE_PROB)
		{
			num2 = (double)rand()/(double)RAND_MAX;
			sign = (double)rand()/(double)RAND_MAX;
			if(sign < 0.5)
			{
				part->centroids[i].x += 2*num2*part->centroids[i].x;
				part->centroids[i].y += 2*num2*part->centroids[i].y;
			}
			else
			{
				part->centroids[i].x -= 2*num2*part->centroids[i].x;
				part->centroids[i].y -= 2*num2*part->centroids[i].y;
			}
		}
	}
	return *part;
}*/
/***************************************************************************************************************************/
/*partition arithmetic_crossover(partition *pop)
{
	int i,j,m;
	partition part;
	double num = (double)rand()/(double)RAND_MAX;
	double alpha = (double)rand()/(double)RAND_MAX;
	i = selection(pop);
	if(num <= CROSS_PROB)
	{
		do
		{
			j = selection(pop);
		} while(j == i);
		for(m=0;m<K;++m)
		{
			part.centroids[m].x = alpha*(pop[i].centroids[m].x) + (1-alpha)*(pop[j].centroids[m].x);
			part.centroids[m].y = alpha*(pop[i].centroids[m].y) + (1-alpha)*(pop[j].centroids[m].y);
		}
		return mutation(&part);
	}
	return mutation(&pop[i]);
}*/
/***************************************************************************************************************************/
/*void arithmetic_reproduction(partition *pop,partition *newPop)
{
	int i;
	for(i=ELITE_RATE*POP_SIZE + 1;i<POP_SIZE;++i)
	{
		newPop[i] = arithmetic_crossover(pop);
	}
}*/
/***************************************************************************************************************************/
/*void single_point_crossover(partition *pop,partition *offsprings)
{
	int i,j,m;
	double num = (double)rand()/(double)RAND_MAX;
	int alpha = rand() % K-1;
	i = selection(pop);
	if(num <= CROSS_PROB)
	{
		do
		{
			j = selection(pop);
		} while(j == i);
		for(m=0;m<=alpha;++m)
		{
			offsprings[0].centroids[m].x = pop[i].centroids[m].x;
			offsprings[0].centroids[m].y = pop[i].centroids[m].y;
			offsprings[1].centroids[m].x = pop[j].centroids[m].x;
			offsprings[1].centroids[m].y = pop[j].centroids[m].y;
		}
		for(m=alpha+1;m<K;++m)
		{
			offsprings[0].centroids[m].x = pop[j].centroids[m].x;
			offsprings[0].centroids[m].y = pop[j].centroids[m].y;
			offsprings[1].centroids[m].x = pop[i].centroids[m].x;
			offsprings[1].centroids[m].y = pop[i].centroids[m].y;
		}
		offsprings[0] = mutation(&offsprings[0]);
		offsprings[1] = mutation(&offsprings[1]);
	}
}*/
/***************************************************************************************************************************/
partition mutation(point *observations,partition *part)
{
	int i,obv;
	double num;

	for(i=0;i<K;++i)
	{
		num = (double)rand()/(double)RAND_MAX;
		if(num < MUTE_PROB)
		{
			obv = rand() % N;
			part->centroids[i].x = observations[obv].x;
			part->centroids[i].y = observations[obv].y;
		}
	}
	return *part;
}
/***************************************************************************************************************************/
int find_closest_centroid(partition *part,point *p)
{
	int i, closest = 0;
	double dist, min = calculate_distance(&(part->centroids[0]),p);
	for(i=1;i<K;++i)
	{
		dist = calculate_distance(&(part->centroids[i]),p);
		if(dist < min)
		{
			min = dist;
			closest = i;
		}
	}
	return closest;
}
/***************************************************************************************************************************/
void crossover(point *observations,partition *pop,partition *offsprings)
{
	int i,j,m,closest;
	double num = (double)rand()/(double)RAND_MAX;
	double alpha1 = (double)rand()/(double)RAND_MAX;
	double alpha2 = (double)rand()/(double)RAND_MAX;
	i = selection(pop);
	do
		{
			j = selection(pop);
		} while(j == i);
	if(num <= CROSS_PROB)
	{
		for(m=0;m<K;++m)
		{
			closest = find_closest_centroid(&pop[j],&(pop[i].centroids[m]));
			offsprings[0].centroids[m].x = alpha1*pop[i].centroids[m].x + (1-alpha1)*pop[j].centroids[closest].x;
			offsprings[0].centroids[m].y = alpha1*pop[i].centroids[m].y + (1-alpha1)*pop[j].centroids[closest].y;
			closest = find_closest_centroid(&pop[i],&(pop[j].centroids[m]));
			offsprings[1].centroids[m].x = alpha2*pop[i].centroids[closest].x + (1-alpha2)*pop[j].centroids[m].x;
			offsprings[1].centroids[m].y = alpha2*pop[i].centroids[closest].y + (1-alpha2)*pop[j].centroids[m].y;
		}
	}
	else
	{
		offsprings[0] = pop[i];
		offsprings[1] = pop[j];
	}
	offsprings[0] = mutation(observations,&offsprings[0]);
	offsprings[1] = mutation(observations,&offsprings[1]);
}
/***************************************************************************************************************************/
void reproduction(point *observations,partition *pop,partition *newPop)
{
	int i;
	partition offsprings[2];
	for(i=ELITE_RATE*POP_SIZE;i<POP_SIZE;i=i+2)
	{
		crossover(observations,pop,offsprings);
		newPop[i] = offsprings[0];
		newPop[i+1] = offsprings[1];
	}
}
/***************************************************************************************************************************/
void print_observations(point *observations,FILE *output)
{
int i;
	/*fprintf(output,"X = [");
	for(i=0;i<N;++i)
	{
		fprintf(output,"%lf ",observations[i].x);
	}
	fprintf(output,"]\n");

	fprintf(output,"Y = [");
	for(i=0;i<N;++i)
	{
		fprintf(output,"%lf ",observations[i].y);
	}
	fprintf(output,"]\n\n\n");*/
	for(i=0;i<N;++i)
	fprintf(output,"%lf %lf\n",observations[i].x,observations[i].y);
}
/***************************************************************************************************************************/
void print_partition(partition *part,FILE *output)
{
	int i;
	fprintf(output,"X = [");
	for(i=0;i<K;++i)
	{
		fprintf(output,"%lf ",(part)->centroids[i].x);
	}
	fprintf(output,"]\n");

	fprintf(output,"Y = [");
	for(i=0;i<K;++i)
	{
		fprintf(output,"%lf ",(part)->centroids[i].y);
	}
	fprintf(output,"]\n");

	fprintf(output,"C = [");
	for(i=0;i<N;++i)
	{
		fprintf(output,"%d ",(part)->obvClusters[i]);
	}
	fprintf(output,"]\nfitness = %lf\n",part->fitness);
}
/***************************************************************************************************************************/
void print_average_fitness(partition *pop,FILE *output)
{
	int i;
	double total = 0;
	for(i=0;i<POP_SIZE;++i)
	{
		total += pop[i].fitness;
	}
	fprintf(output,"%lf\n",total/POP_SIZE);
}
/***************************************************************************************************************************/
void print_population(partition *pop,FILE *output)
{
	int i;
	double total = 0;
	for(i=0;i<POP_SIZE;++i)
	{
		print_partition(&pop[i],output);
	}
	print_average_fitness(pop,output);
	fprintf(output,"\n\n");
}
/***************************************************************************************************************************/
void print_partition_MATLAB(point *observations,partition **part,FILE *output)
{
	int i;
	fprintf(output,"X = [");
	for(i=0;i<N;++i)
	{
		fprintf(output,"%lf ",observations[i].x);
	}
	for(i=0;i<K;++i)
	{
		fprintf(output,"%lf ",(*part)->centroids[i].x);
	}
	fprintf(output,"]\n");

	fprintf(output,"Y = [");
	for(i=0;i<N;++i)
	{
		fprintf(output,"%lf ",observations[i].y);
	}
	for(i=0;i<K;++i)
	{
		fprintf(output,"%lf ",(*part)->centroids[i].y);
	}
	fprintf(output,"]\n");

	fprintf(output,"C = [");
	for(i=0;i<N;++i)
	{
		fprintf(output,"%d ",(*part)->obvClusters[i]);
	}
	for(i=0;i<K;++i)
	{
		fprintf(output,"%d ",2);
	}
	fprintf(output,"]\n");
}