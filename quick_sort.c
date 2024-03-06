//
// File quick_sort.c
//
// @author Alex A Denny aad7700@g.rit.edu
//
/////////////////////////////////////////////


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
#include <ctype.h>

//declaring funcs
int *create_random_nums( int size );
int *quick_sort( int *arr, int size );
void *quick_sort_threaded( void *args );

//struct to be used in quick_sort_threaded
struct ThreadArr{
	int size;//size of original array
	int *arr;//integer array
};

// This function will create an array of random integers
//
// @param int size : size of array of random numbers to create
int *create_random_nums( int size ) {
	int * arr = (int*) malloc(sizeof(int) * size);
	
	if( arr == NULL ){
		fprintf(stderr, "Memory allocation failure");
		return NULL;
	}
	

	srand(time(NULL));//seeding random number generator

	//creating random number array
	for(int i = 0; i < size; i++){

		arr[i] = (rand() % (10 * size));
	
	}

	return arr;
}

// normal quick sort function
//
// int * arr: ptr to an int array containing the ints to quick sort
// size: int representing the number of ints in arr
int *quick_sort( int *arr, int size ) {

	if (size <= 1) {//base case: already sorted or single element
		
		//copy the array before returning it to avoid memory corruption
        	int *sorted_arr = (int *)malloc(sizeof(int) * size);
        	for (int i = 0; i < size; i++) {
            		sorted_arr[i] = arr[i];
        	}
        	return sorted_arr;
	}
	
	//defining partiction vars and arrays
	int pivot = arr[size / 2];//pivot as the middle element
	int *less = NULL;
	int *equal = NULL;
	int *more = NULL;
	int less_count = 0, equal_count = 0, more_count = 0;


	for (int i = 0; i < size; i++) {
        	if (arr[i] < pivot) {
			less_count++;
        	} 
		else if (arr[i] == pivot) {
            		equal_count++;
        	} 
		else {
            		more_count++;
        	}
    	}

	less = (int *)malloc(sizeof(int) * less_count);
    	equal = (int *)malloc(sizeof(int) * equal_count);
    	more = (int *)malloc(sizeof(int) * more_count);

    	less_count = 0;
    	equal_count = 0;
    	more_count = 0;


    	//prartitioning array into three parts
    	for (int i = 0; i < size; i++) {	
		if (arr[i] < pivot) {
			less[less_count++] = arr[i];
	    	} 
	    	else if (arr[i] > pivot) {
		    	more[more_count++] = arr[i];
        	} 
		else {
            		equal[equal_count++] = arr[i];
        	}
    	}
	

    	//recursively sort the less and more
    	int * sorted_less = quick_sort(less, less_count);
    	int * sorted_more = quick_sort(more, more_count);

    	//merging the three parts
    	int *sorted_arr = (int *)malloc(sizeof(int) * size);
	
	int idx = 0;
    	for (int i = 0; i < less_count; i++) {
       		sorted_arr[idx++] = sorted_less[i];
   	}
    	for (int i = 0; i < equal_count; i++) {
        	sorted_arr[idx++] = equal[i];
    	}
    	for (int i = 0; i < more_count; i++) {
        	sorted_arr[idx++] = sorted_more[i];
    	}


    	//Free memory of the temp arrays
    	if(less != NULL)
                free( less );
        if(equal != NULL)
                free( equal );
        if(more != NULL)
                free( more );
	if(sorted_less != NULL)
		free( sorted_less );
        if(sorted_more != NULL)
		free( sorted_more );
	
	return sorted_arr;
}


// Recursive threaded quick sort
//
// void *args: struct that holds required data
void *quick_sort_threaded( void *args ){
	
	//casting args to struct
	struct ThreadArr* params = (struct ThreadArr*) args;
	int* arr = params->arr;
    	int size = params->size;

	//base case: array sorted
    	if (size <= 1) {
        	int* sorted_arr = (int*)malloc(sizeof(int) * size);
        	for (int i = 0; i < size; i++) {
            		sorted_arr[i] = arr[i];
        	}
        	return (void*)sorted_arr;
    	}
	
	//defining partitioning vars and arrays
	int pivot = arr[size / 2];//pivot as the middle element
        int *less = NULL;
        int *equal = NULL;
        int *more = NULL;
        int less_count = 0, equal_count = 0, more_count = 0;


        for (int i = 0; i < size; i++) {
                if (arr[i] < pivot) {
                        less_count++;
                }
                else if (arr[i] == pivot) {
                        equal_count++;
                }
                else {
                        more_count++;
                }
        }

        less = (int *)malloc(sizeof(int) * less_count);
        equal = (int *)malloc(sizeof(int) * equal_count);
        more = (int *)malloc(sizeof(int) * more_count);

        less_count = 0;
        equal_count = 0;
        more_count = 0;


        //prartitioning array into three parts
        for (int i = 0; i < size; i++) {
                if (arr[i] < pivot) {
                        less[less_count++] = arr[i];
                }
                else if (arr[i] > pivot) {
                        more[more_count++] = arr[i];
                }
                else {
                        equal[equal_count++] = arr[i];
                }
        }

	//creating threads
    	pthread_t less_thread, more_thread;
    	struct ThreadArr less_params = {less_count, less};
    	struct ThreadArr more_params = {more_count, more};

    	//threds sort the partitions
    	pthread_create(&less_thread, NULL, quick_sort_threaded, (void*)&less_params);
    	pthread_create(&more_thread, NULL, quick_sort_threaded, (void*)&more_params);

    	// Wait for the threads to finish
    	void* sorted_less;
    	void* sorted_more;
    	pthread_join(less_thread, &sorted_less);
    	pthread_join(more_thread, &sorted_more);

	//merge the partitions
    	int* sorted_arr = (int*)malloc(sizeof(int) * size);
	
    	int idx = 0;
    	for (int i = 0; i < less_count; i++) {
        	sorted_arr[idx++] = ((int*)sorted_less)[i];
    	}
    	for (int i = 0; i < equal_count; i++) {
        	sorted_arr[idx++] = equal[i];
    	}
    	for (int i = 0; i < more_count; i++) {
        	sorted_arr[idx++] = ((int*)sorted_more)[i];
    	}

	//free temp memory
    	free(less);
    	free(equal);
    	free(more);
    	free(sorted_less);
    	free(sorted_more);

    	return (void*)sorted_arr;

}

int main( int argc, char *argv[] ){
	
	//error checking/input validation
	if( argc != 2 || !isdigit(*argv[1]) ){
		fprintf( stderr, "Error: Usage is ./quick_sort <int>, where <int> is at least 0\n" );
		return 1;
	}
	
	int size = atoi(argv[1]);
	int * arr_n = create_random_nums( size );//array for normal quicksort
	int * arr_copy = (int*) malloc(sizeof(int) * size);//copy of arr_n for threaded quicksort
	
	//copying arrau
	for(int i = 0; i < size; i++){
		arr_copy[i] = arr_n[i];
	}

	//output, mostly for debug
	printf("Size: %d\n", size);

        printf("Random gen: ");
        for(int i = 0; i < size; i++){
                printf("%d ", arr_n[i]);
        }
	printf("\n");

	//creating thread arg
	struct ThreadArr* arr_th = (struct ThreadArr*) malloc(sizeof(struct ThreadArr));
	arr_th->arr = arr_copy;
        arr_th->size = size;
	
	//thread quicksort
	clock_t begin = clock();
	int* thread_arr = (int*) quick_sort_threaded( arr_th );
	clock_t end = clock();
	double thread_end = (double)(end - begin) / CLOCKS_PER_SEC;

	//normal, nonthread quicksort
	begin = clock();
	int * sorted_arr = quick_sort( arr_n, size );
	end = clock();
        double normal_end = (double)(end - begin) / CLOCKS_PER_SEC;

	//outputing sorted arrays
	printf("Normal Sort: ");
	for(int i = 0; i < size; i++){
		printf("%d ", sorted_arr[i]);
	}
	
	printf("\nThreaded Sort: ");
	for(int i = 0; i < size; i++){
		printf("%d ", thread_arr[i]);
	}

	//displaying times
	printf("\nNormal sort time:%f\n", normal_end);
	printf("Threaded sort time:%f\n", thread_end);
	
	//freeing memory
	free( sorted_arr );
	free( thread_arr );
	free( arr_n );
	free( arr_th->arr );
	free( arr_th );
	return 0;

}
