#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "timer.h"

#define k 68

typedef struct unionizedArr_{
    double *merged_arr;
    int **ptr_arr;
    int length;
} unionizedArr;

typedef struct augmentedArr_{
    double *aug_arr;
    int *ptr_arr1;
    int *ptr_arr2;
    int length;
} augmentedArr;

typedef struct hashArr_{
    double min;
    double max;
    int *b1;
    int *b2;
} hashArr;

/* helper functions */
int binarySearch(double arr[], int n, double target);
int checkArr(double *array, int size);
int checkResults(int *results, double **array_set, int *array_sizes, double lookup_value);

/* structure 2 */
int mergeArrays( double arr1[],  double arr2[], int n1, int n2,  double arr3[]);
int mergeKArrays(double **arr, int *arr_sizes, int i, int j, double *output, int max_len);
void createPtrArray(double *merged_arr, int merged_length, double **array_set, int *array_sizes, int **p);
unionizedArr *createUArray(double **array_set, int *array_sizes);
void destroyUArray(unionizedArr *arr);
int *searchUArray(unionizedArr *arr, double lookup_value);

/* structure 3 */
augmentedArr *createAArray(double **array_set, int *array_sizes);
int *searchAArray(augmentedArr *structure3, double lookup_value);
void destroyAArray(augmentedArr *structure3);

/* structure 4 */
hashArr *createHArray(double **array_set, int *array_sizes, int m);
void destroyHArray(hashArr *structure4, int m);
int *searchHArray(hashArr *structure4, double lookup_value, double **array_set, int *array_sizes, int m);
int testStructure4(hashArr *structure4, double **array_set, int *array_sizes, int m);
void find_optimal_bucket(int max_buckets, int num_iters, double **array_set, int *array_sizes);

/* tests */
int testResults1(int num_iters, double **array_set, int *array_sizes);
int testResults2(int num_iters, unionizedArr *structure2, double **array_set, int *array_sizes);
int testResults3(int num_iters, augmentedArr *structure3, double **array_set, int *array_sizes);
int testResults4(int num_iters, hashArr *structure4, double **array_set, int *array_sizes, int m);

void lookup_kernel(int num_iters, double **array_set, int *array_sizes, unionizedArr *structure2, augmentedArr *structure3, hashArr *structure4, int m);

int main(){
    double *array_set[k];
    int array_sizes[k];
    int array_count = 0;
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    fp = fopen("arrays.txt", "r");

    if (fp == NULL)
        exit(EXIT_FAILURE); 

    /* CREATE STRUCTURE 1 */
    while ((read = getline(&line, &len, fp)) != -1 && array_count < k){
        char* str = strtok(line, " ");
        int length = atoi(str);
        double *array = malloc(sizeof(double) * length);

        int i = 0;
        array_sizes[array_count] = length;
        str = strtok(NULL, " ");
        while (str != NULL && str[0] != '\n'){ 
            array[i] = strtold(str, NULL);
            str = strtok(NULL, " ");
            i++;
        }
        array_set[array_count] = array;
        array_count++;
    }

    /* CREATE STRUCTURE 2 */
    unionizedArr *structure2 = createUArray(array_set,array_sizes);
    
    /* CREATE STRUCTURE 3 */
    augmentedArr *structure3 = createAArray(array_set,array_sizes);

    /* CREATE STUCTURE 4 */
    int m = 10350; // based on results of find_optimal_bucket
    hashArr *structure4 = createHArray(array_set,array_sizes, m);

    lookup_kernel(100000, array_set, array_sizes, structure2, structure3, structure4, m);

    /* free memory */
    fclose(fp);
    if (line)
        free(line);

    for (int i = 0; i < k; i++)
        free(array_set[i]);

    destroyUArray(structure2);
    destroyAArray(structure3);
    destroyHArray(structure4,m);

    return 0;

}

/* helper functions */
int binarySearch(double arr[], int n, double target) // referenced https://www.geeksforgeeks.org/find-closest-number-array/
{
    // Corner cases
    if (target == arr[0])
        return 0;
    else if (target < arr[0] || target > arr[n - 1]){
        return -1;
    }
    else if (target == arr[n - 1])
        return (n-1);
    // Doing binary search
    int i = 0, j = n, mid = 0;
    while (i < j) {
        mid = (i + j) / 2;
 
        if (arr[mid] == target)
            return mid;
 
        /* If target is less than array element, then search in left */
        if (target < arr[mid]) {
 
            // If target is greater than previous to mid, return mid
            if (mid > 0 && target > arr[mid - 1])
                return mid;
 
            /* Repeat for left half */
            j = mid;
        }
 
        // If target is greater than mid
        else {
            if (mid < (n - 1) && target < arr[mid + 1])
                return mid + 1;
            // update i
            i = mid + 1;
        }
    }
 
    // Only single element left after search
    return mid; 
}

int checkArr(double *array, int size){
    for (int i = 0; i < size-1; i++){
        if (array[i] >= array[i+1]){
            printf("Error - array is not correctly sorted.\n"); 
            return 0;
        }
    }
    return 1;
}

int checkResults(int *results, double **array_set, int *array_sizes, double lookup_value){
    for (int i = 0; i < k; i++){
        if( ((lookup_value > array_set[i][array_sizes[i]-1]) || (lookup_value < array_set[i][0]))){
            if(results[i] == -1)
                continue;
            else
                return 0;
        }
        int index = results[i];
        double indexed_value = array_set[i][index];
        if (index > 0)
            if(array_set[i][index-1] < lookup_value)
                ;
            else
                return 0;
        if (index < (array_sizes[i] - 1))
            if(array_set[i][index+1] >= lookup_value)
                ;
            else
                return 0;
    }
    return 1;
}

/* structure 2 */
int mergeArrays( double arr1[],  double arr2[], int n1, int n2,  double arr3[]){ //referenced https://www.geeksforgeeks.org/merge-k-sorted-arrays/ 
    int i = 0, j = 0, l = 0, n3 = 0;
    double last_insert = -1;
 
    // Traverse both array
    while (i<n1 && j <n2)
    {
        if (arr1[i] < arr2[j]){
            if (arr1[i] != last_insert){
                last_insert = arr1[i];
                arr3[l++] = arr1[i++];
                n3++;
            }
            else{
                i++;
            }
        }
        else if (arr1[i] > arr2[j]){
            if (arr2[j] != last_insert){
                last_insert = arr2[j];
                arr3[l++] = arr2[j++];
                n3++;
            }
            else{
                j++;
            }
        }
        else {
            if (arr2[j] != last_insert){
                last_insert = arr2[j];
                arr3[l++] = arr2[j++];
                i++;
                n3++;
            }
            else{
                i++;
                j++;
            }
        }
    }

    // Store remaining elements of first array
    while (i < n1){
        if (arr1[i] != last_insert){
            last_insert = arr1[i];
            arr3[l++] = arr1[i++];
            n3++;
        }
        else{
            i++;
        }
    }
 
    // Store remaining elements of second array
    while (j < n2){
        if (arr2[j] != last_insert){
            last_insert = arr2[j];
            arr3[l++] = arr2[j++];
            n3++;
        }
        else{
            j++;
        }
    }
    return n3;
}

int mergeKArrays(double **arr, int *arr_sizes, int i, int j, double *output, int max_len) //referenced https://www.geeksforgeeks.org/merge-k-sorted-arrays/ 
{
    //if one array is in range
    if(i==j)
    {
        for(int p = 0; p < arr_sizes[i]; p++)
            output[p]=arr[i][p];
        return arr_sizes[i];
    }
     
    //if only two arrays are left then merge them
    if(j-i == 1)
    {
        return mergeArrays(arr[i],arr[j],arr_sizes[i],arr_sizes[j],output);
    }
     
    //output arrays
    double *out1 = malloc(sizeof(double) * max_len);
    double *out2 = malloc(sizeof(double) * max_len);
     
    //divide the array into halves
    int out1_len = 0;
    int out2_len = 0;
    out1_len += mergeKArrays(arr, arr_sizes, i, (i+j)/2, out1, max_len);
    out2_len += mergeKArrays(arr, arr_sizes, (i+j)/2+1, j, out2, max_len);
     
    //merge the output array
    int merged_len_final = mergeArrays(out1, out2, out1_len, out2_len, output);
    free(out1);
    free(out2);
    return merged_len_final;
}

void createPtrArray(double *merged_arr, int merged_length, double **array_set, int *array_sizes, int **p){
    for (int i = 0; i < k; i++){
        p[i] = malloc(sizeof(int) * merged_length);
        for (int j = 0; j < merged_length; j++){
            p[i][j] = binarySearch(array_set[i],array_sizes[i],merged_arr[j]);
        }
    }
}

unionizedArr *createUArray(double **array_set, int *array_sizes){
    unionizedArr *arr = malloc(sizeof(unionizedArr));
    int max_len = 0;
    for (int i = 0; i < k; i++){
        max_len += array_sizes[i];
    }
    double *unionized_arr = malloc(sizeof(double) * max_len); 
    int merged_length = mergeKArrays(array_set,array_sizes, 0, k-1, unionized_arr, max_len);
    int **p = malloc(sizeof(int*) *k); 
    createPtrArray(unionized_arr,merged_length,array_set,array_sizes,p);
    arr->merged_arr = unionized_arr;
    arr->ptr_arr = p;
    arr->length = merged_length;
    return arr;
}

void destroyUArray(unionizedArr *arr){
    for (int i = 0; i < k; i++)
        free(arr->ptr_arr[i]);
    free(arr->ptr_arr);
    free(arr->merged_arr);
    free(arr);
}

int *searchUArray(unionizedArr *arr, double lookup_value){
    int index_merged_arr = binarySearch(arr->merged_arr,arr->length,lookup_value);
    int *results = malloc(sizeof(int) * k);
    for (int i = 0; i < k; i++){
        results[i] = arr->ptr_arr[i][index_merged_arr];
    }
    return results;
}

/* structure 3 */
augmentedArr *createAArray(double **array_set, int *array_sizes){
    augmentedArr *structure3 = malloc(sizeof(augmentedArr) * k); // structure 3 is an array of k aurgmentedArr
    structure3[k-1].length = array_sizes[k-1];
    structure3[k-1].aug_arr = malloc(sizeof(double) * structure3[k-1].length);
    structure3[k-1].ptr_arr1 = malloc(sizeof(int) * structure3[k-1].length);
    structure3[k-1].ptr_arr2 = malloc(sizeof(int) * structure3[k-1].length);
    for (int i = 0; i < structure3[k-1].length; i++){
        structure3[k-1].aug_arr[i] = array_set[k-1][i];
        structure3[k-1].ptr_arr1[i] = i;
        structure3[k-1].ptr_arr2[i] = 0;
    }
    for (int i = k-2; i >= 0; i--){
        int initial_length = array_sizes[i];
        double *temp_arr = malloc(sizeof(double) * (structure3[i+1].length/2 + 1));
        int temp_arr_length = 0;
        for (int j = 0; j < structure3[i+1].length; j+= 2){
            temp_arr[temp_arr_length] = structure3[i+1].aug_arr[j];
            temp_arr_length++;
        }
        structure3[i].aug_arr = malloc(sizeof(double) * (initial_length + temp_arr_length));
        structure3[i].length = mergeArrays(temp_arr, array_set[i], temp_arr_length, array_sizes[i], structure3[i].aug_arr);
        free(temp_arr);
        structure3[i].ptr_arr1 = malloc(sizeof(int) * structure3[i].length);
        structure3[i].ptr_arr2 = malloc(sizeof(int) * structure3[i].length);
        for (int j = 0; j < structure3[i].length; j++){
            structure3[i].ptr_arr1[j] = binarySearch(array_set[i],array_sizes[i],structure3[i].aug_arr[j]);
            structure3[i].ptr_arr2[j] = binarySearch(structure3[i+1].aug_arr,structure3[i+1].length,structure3[i].aug_arr[j]); 
        }
    }    
    return structure3; 
}
    
int *searchAArray(augmentedArr *structure3, double lookup_value){
    int *results_structure3 = malloc(sizeof(int) * k);
    int index = binarySearch(structure3[0].aug_arr,structure3[0].length,lookup_value); 
    
    for (int i = 0; i < (k-1); i++){
        int p1, p2;
        if(index == -1){
            p1 = -1;
            index = structure3[i+1].length - 1;
            p2 = index;
        }
        else{
            p1 = structure3[i].ptr_arr1[index];
            p2 = structure3[i].ptr_arr2[index];
        }
        results_structure3[i] = p1;

        if ( p2 <= 0 || (p2 < structure3[i+1].length && lookup_value <= structure3[i+1].aug_arr[p2] && lookup_value > structure3[i+1].aug_arr[p2-1]))
            ;
        else if (lookup_value <= structure3[i+1].aug_arr[p2-1])
            p2--;
        else if (lookup_value > structure3[i+1].aug_arr[structure3[i+1].length - 1])
            p2 = -1;
        else{
            assert(0);
        }
        index = p2; 
    }
    results_structure3[k-1] = structure3[k-1].ptr_arr1[index]; 
    return results_structure3;
}
    
void destroyAArray(augmentedArr *structure3){
    for (int i = 0; i < k; i++){
        free(structure3[i].ptr_arr1);
        free(structure3[i].ptr_arr2);
        free(structure3[i].aug_arr);
    }
    free(structure3); 
}

/* structure 4 */
hashArr *createHArray(double **array_set, int *array_sizes, int m){
    double global_min = array_set[0][0];
    double global_max = array_set[0][array_sizes[0]-1];
    for (int i = 1; i < k; i ++){
        if (array_set[i][0] < global_min)
            global_min = array_set[i][0];
        if (array_set[i][array_sizes[i]-1] > global_max)
            global_max = array_set[i][array_sizes[i]-1];
    }

    hashArr *structure4 = malloc(sizeof(hashArr) * m);
    double increment = (global_max - global_min) / (double)m;
    for (int i = 0; i < m; i++){
        if (i == 0)
            structure4[0].min = global_min;
        else
            structure4[i].min = structure4[i-1].max;
        structure4[i].max = structure4[i].min + increment;
        structure4[i].b1 = malloc(sizeof(int) * k);
        structure4[i].b2 = malloc(sizeof(int) * k);
    }

    for (int i = 0; i < m; i++){ // for each bucket
        for (int j = 0; j < k; j++){ // for each array
            if (structure4[i].min > array_set[j][array_sizes[j] -1] ){
                structure4[i].b1[j] = -1;
                structure4[i].b2[j] = -1;
            }
            else{
                if (i == 0)
                    structure4[i].b1[j] = 0;
                else
                    structure4[i].b1[j] = structure4[i-1].b2[j] + 1;
                int counter_max = structure4[i].b1[j];
                while ( (counter_max < array_sizes[j]) && (array_set[j][counter_max] < structure4[i].max) )
                    counter_max++;
                structure4[i].b2[j] = counter_max - 1;
            }
        }
    }
    return structure4;
}

void destroyHArray(hashArr *structure4, int m){
    for (int i = 0; i < m; i++){
        free(structure4[i].b1);
        free(structure4[i].b2);
    }
    free(structure4);
}

int testStructure4(hashArr *structure4, double **array_set, int *array_sizes, int m){
     for (int i = 0; i < m; i++){ // for each bucket
        for (int j = 0; j < k; j++){ // for each array
            printf("i is %d, j is %d\n",i,j);
            if (structure4[i].min > array_set[j][array_sizes[j] - 1]){
                assert(structure4[i].b1[j] == -1);
                assert(structure4[i].b2[j] == -1);
            }
            else{
                // printf("array b1 value: %f, x min: %f\n",array_set[j][structure4[i].b1[j]],structure4[i].min );
                // printf("array b2 value: %f, x max: %f\n",array_set[j][structure4[i].b2[j]],structure4[i].max );
                assert(array_set[j][structure4[i].b1[j]] >= structure4[i].min);
                assert(array_set[j][structure4[i].b2[j]] < structure4[i].max);
            }
        }
    }
    return 1;
}

int *searchHArray(hashArr *structure4, double lookup_value, double **array_set, int *array_sizes, int m){
    double increment = structure4[0].max - structure4[0].min;
    int bin = (int)lookup_value/increment; 
    if (bin == m) // to include global max in last bin
        bin--;
    int *results_structure4 = malloc(sizeof(int) * k);
    for (int i = 0; i < k; i++){
        if(structure4[bin].b1[i] == -1 || lookup_value > array_set[i][array_sizes[i] - 1]){
            results_structure4[i] = -1;
        }
        else{
            results_structure4[i] = structure4[bin].b1[i] + binarySearch(array_set[i] + (structure4[bin].b1[i]),(array_sizes[i] - structure4[bin].b1[i]), lookup_value); 
        }
    }
    return results_structure4;
}

void find_optimal_bucket(int max_buckets, int num_iters, double **array_set, int *array_sizes){
    FILE *datafile    = NULL; 
    datafile          = fopen("hash.txt","w");
    srand(time(NULL));
    double min_time = -1;
    int optimal_bucket = -1;
    for (int m = 1; m < max_buckets; m++){
        double totalTime4 = 0;
        hashArr *structure4 = createHArray(array_set,array_sizes,m);
        for (int iter = 1; iter <= num_iters; iter++){
            double lookup_value = ((double) rand() / RAND_MAX) * 150;
            StartTimer();
            int *results_structure4 = searchHArray(structure4,lookup_value, array_set,array_sizes,m);
            const double tElapsed4 = GetTimer() / 1000.0;
            if (iter > 1) {                          /* First iter is warm up */
                totalTime4 += tElapsed4; 
            }
            free(results_structure4);
        }
        if (m == 1){
            min_time = totalTime4;
            optimal_bucket = m;
        }
        else{
            if (totalTime4 < min_time){
                min_time = totalTime4;
                optimal_bucket = m;
            }
        }
        fprintf(datafile,"Num buckets: %d, Total time: %f\n", m, totalTime4);
        destroyHArray(structure4,m);
    }
    fprintf(datafile,"Optimal bucket: %d, Total time: %f\n", optimal_bucket, min_time);
    fclose(datafile);
    printf("Optimal number of buckets between 1 and %d is %d.\n",max_buckets,optimal_bucket);
}

/* tests */
int testResults1(int num_iters, double **array_set, int *array_sizes){
    srand(time(NULL));
    for (int i = 0; i < num_iters; i++){
        double lookup_value = ((double) rand() / RAND_MAX) * 150;
        //printf("%f ", lookup_value);
        int results_structure1[k];
        for (int i = 0; i < k; i++){
            results_structure1[i] = binarySearch(array_set[i],array_sizes[i],lookup_value);
        } 
        if(checkResults(results_structure1, array_set, array_sizes, lookup_value) == 0)
            return 0;
    }
    return 1;
}

int testResults2(int num_iters, unionizedArr *structure2, double **array_set, int *array_sizes){
    srand(time(NULL));
    for (int i = 0; i < num_iters; i++){
        double lookup_value = ((double) rand() / RAND_MAX) * 150;
        int *results_structure2 = searchUArray(structure2,lookup_value);
        for (int i = 0; i < k; i++){
            if (checkResults(results_structure2, array_set, array_sizes, lookup_value) == 0)
                return 0;
        }
        free(results_structure2);
    }
    return 1;
}

int testResults3(int num_iters, augmentedArr *structure3, double **array_set, int *array_sizes){
    srand(time(NULL));
    for (int i = 0; i < num_iters; i++){
        double lookup_value = ((double) rand() / RAND_MAX) * 150;
        int *results_structure3 = searchAArray(structure3,lookup_value);
        for (int i = 0; i < k; i++){
            if (checkResults(results_structure3, array_set, array_sizes, lookup_value) == 0)
                return 0;
        }
        free(results_structure3);
    }
    return 1;
}

int testResults4(int num_iters, hashArr *structure4, double **array_set, int *array_sizes, int m){
    srand(time(NULL));
    for (int i = 0; i < num_iters; i++){
        double lookup_value = ((double) rand() / RAND_MAX) * 150;
        int *results_structure4 = searchHArray(structure4,lookup_value, array_set,array_sizes,m);
        for (int i = 0; i < k; i++){
            if (checkResults(results_structure4, array_set, array_sizes, lookup_value) == 0)
                return 0;
        }
        free(results_structure4);
    }
    return 1;
}

void lookup_kernel(int num_iters, double **array_set, int *array_sizes, unionizedArr *structure2, augmentedArr *structure3, hashArr *structure4, int m){
    srand(time(NULL));

    double totalTime1  = 0.0;
    double totalTime2  = 0.0;
    double totalTime3  = 0.0;
    double totalTime4  = 0.0;

    FILE *datafile    = NULL; 
    datafile          = fopen("results.txt","w");

    for (int iter = 1; iter <= num_iters; iter++){
        double lookup_value = ((double) rand() / RAND_MAX) * 150;
        printf("Iteration: %d, Lookup value: %f\n", iter, lookup_value);

        
        int results_structure1[k];
        StartTimer();
        for (int i = 0; i < k; i++){
            results_structure1[i] = binarySearch(array_set[i],array_sizes[i],lookup_value);
        } 
        const double tElapsed1 = GetTimer() / 1000.0;
        if (iter > 1) {                          /* First iter is warm up */
            totalTime1 += tElapsed1; 
        }

        StartTimer();
        int *results_structure2 = searchUArray(structure2,lookup_value);
        const double tElapsed2 = GetTimer() / 1000.0;
        if (iter > 1) {                          /* First iter is warm up */
            totalTime2 += tElapsed2; 
        }

        StartTimer();
        int *results_structure3 = searchAArray(structure3,lookup_value);
        const double tElapsed3 = GetTimer() / 1000.0;
        if (iter > 1) {                          /* First iter is warm up */
            totalTime3 += tElapsed3; 
        }

        StartTimer();
        int *results_structure4 = searchHArray(structure4,lookup_value, array_set,array_sizes,m); 
        const double tElapsed4 = GetTimer() / 1000.0;
        if (iter > 1) {                          /* First iter is warm up */
            totalTime4 += tElapsed4; 
        }

        for (int i = 0; i < k; i++){
            assert(checkResults(results_structure1, array_set, array_sizes, lookup_value) == 1);
            assert(checkResults(results_structure2, array_set, array_sizes, lookup_value) == 1);
            assert(checkResults(results_structure3, array_set, array_sizes, lookup_value) == 1);
            assert(checkResults(results_structure4, array_set, array_sizes, lookup_value) == 1);
        }
        // printf("All results are accurate for iteration %d.\n",iter);
        free(results_structure2);
        free(results_structure3);
        free(results_structure4);
    }
    printf("All results are accurate for %d iterations.\n",num_iters);
    double avgTime1 = totalTime1 / (double)(num_iters-1);
    double avgTime2 = totalTime2 / (double)(num_iters-1);
    double avgTime3 = totalTime3 / (double)(num_iters-1);
    double avgTime4 = totalTime4 / (double)(num_iters-1); 
    fprintf(datafile, "Number of iterations: %d\nStructure 1 - avgTime: %.10f   totTime: %f \nStructure 2 - avgTime: %.10f   totTime: %f \nStructure 3 - avgTime: %.10f   totTime: %f \nStructure 4 (%d bins) - avgTime: %.10f   totTime: %f \n", num_iters, avgTime1, totalTime1, avgTime2, totalTime2, avgTime3, totalTime3, m, avgTime4, totalTime4);
    fclose(datafile);
}
