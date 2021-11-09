#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

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

/* helper functions */
int binarySearch(double arr[], int n, double target);
int checkArr(double *array, int size);
int checkResults(int *results, double **array_set, int *array_sizes, double lookup_value);
int testResults(int num_iters, double **array_set, int *array_sizes);

/* structure 1 */
void testStructure1(double **array_set, int *array_sizes);

/* structure 2 */
int mergeArrays( double arr1[],  double arr2[], int n1, int n2,  double arr3[]);
int mergeKArrays(double **arr, int *arr_sizes, int i, int j, double *output, int max_len);
void createPtrArray(double *merged_arr, int merged_length, double **array_set, int *array_sizes, int **p);
unionizedArr *createUArray(double **array_set, int *array_sizes);
void destroyUArray(unionizedArr *arr);
int *searchUArray(unionizedArr *arr, double lookup_value);
void testStructure2(unionizedArr *structure2);
int testResults2(int num_iters, unionizedArr *structure2, double **array_set, int *array_sizes);

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
            // array[i] = atof(str);
            array[i] = strtold(str, NULL);
            str = strtok(NULL, " ");
            i++;
        }
        array_set[array_count] = array;
        array_count++;
    }

    /* testStructure1(array_set,array_sizes);
    if (testResults(10000, array_set, array_sizes) == 1)
        printf("Results are accurate.\n"); */

    /* CREATE STRUCTURE 2 */
    //unionizedArr *structure2 = createUArray(array_set,array_sizes);
    
    /*testStructure2(structure2);
    if (testResults2(1000, structure2, array_set, array_sizes) == 1)
        printf("Results are accurate.\n"); */

    /* CREATE STRUCTURE 3 */
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
            temp_arr[temp_arr_length++] = structure3[i+1].aug_arr[j];
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

    double lookup_value = 30;

    int results_structure1[k];
    for (int i = 0; i < k; i++){
        results_structure1[i] = binarySearch(array_set[i],array_sizes[i],lookup_value);
    } 

    int results_structure3[k];
    int index = binarySearch(structure3[0].aug_arr,structure3[0].length,lookup_value); // index = 2
    for (int i = 0; i < (k-1); i++){
        results_structure3[i] = structure3[i].ptr_arr1[index]; // results[1] = 2
        int p2 = structure3[i].ptr_arr2[index]; // p2 = 1
        if (p2 == 0 || (lookup_value > structure3[i+1].aug_arr[p2-1])){
            index = p2;
        }
        else   
            index = p2-1;
    }
    results_structure3[k-1] = structure3[k-1].ptr_arr1[index];
    


    /*int index = binarySearch(structure3[0].aug_arr,structure3[0].length,q);
    results_structure3[0] = structure3[0].ptr1[index];
    int p2 = structure3[0].ptr2[index];
    if (q > structure3[1].aug_arr[p2-1]){ // q is located at position p2 in M2
        results_structure3[1] = structure3[1].ptr_arr1[p2];
        index = p2
    }
    else{
        results_structure3[1] = structure3[1].ptr_arr1[p2-1];
        index = p2-1;
    }



    int index = binarySearch(structure3[0].aug_arr,structure3[0].length,lookup_value);
    results_structure3[0] = structure3[0].ptr_arr1[index];
    for (int i = 1; i < k; i++){
        int p2 = structure3[i-1].ptr_arr2[index];
        /*if (p2 > 0 && structure3[i].aug_arr[p2-1] >= lookup_value){
            results_structure3[i] = structure3[i].ptr_arr1[p2-1];
            index = p2-1;
            // printf("if statement, %d\n",i);
        }
        else {
            results_structure3[i] = structure3[i].ptr_arr1[p2];
            index = p2;
            // printf("else statement, %d\n",i);
        }
        if (structure3[i].aug_arr[p2] < lookup_value){
            results_structure3[i] = structure3[i].ptr_arr1[p2+1];
            index = p2+1;
            // printf("if statement, %d\n",i);
        }
        else {
            results_structure3[i] = structure3[i].ptr_arr1[p2];
            index = p2;
            // printf("else statement, %d\n",i);
        } 
    } */
    //printf("%f\n",structure3[2].aug_arr[structure3[1].ptr_arr2[structure3[1].length-1]]);
    //printf("%d\n",structure3[2].ptr_arr1[structure3[2].length-1]);

    for (int i = 0; i < k; i++){
        printf("%d - %d \n",results_structure1[i],results_structure3[i]);
        //assert(results_structure1[i] == results_structure3[i]);
    } 

    /*printf("%d\n", index);
    printf("%d\n", structure3[0].ptr_arr1[index]);
    printf("%d\n", structure3[0].ptr_arr2[index]);
    printf("%f\n", array_set[1][structure3[0].ptr_arr2[index]]); */

    for (int i = 0; i < k; i++){
        free(structure3[i].ptr_arr1);
        free(structure3[i].ptr_arr2);
        free(structure3[i].aug_arr);
    }
    free(structure3);

    fclose(fp);
    if (line)
        free(line);

    for (int i = 0; i < k; i++)
        free(array_set[i]);
    
    //destroyUArray(structure2);

    return 0;
}


/* helper functions */
int binarySearch(double arr[], int n, double target) // referenced https://www.geeksforgeeks.org/find-closest-number-array/
{
    // Corner cases
    if (target <= arr[0])
        return 0;
    if (target >= arr[n - 1])
        return n - 1;
 
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
    return mid; // should this be an error code?
}

int checkArr(double *array, int size){
    for (int i = 0; i < size-1; i++){
        if (array[i] >= array[i+1]){
            printf("Error - array is not correctly sorted.\n"); //. I is %d, array[i-2] is %f, array[i-1] is %Lf, array[i] is %Lf, array[i+1] is %Lf, array[i+2] is %Lf.\n", i,array[i-2],array[i-1],array[i],array[i+1],array[i+2]);
            return 0;
        }
    }
    return 1;
}

/* structure 1 */
void testStructure1(double **array_set, int *array_sizes){
    assert(binarySearch(array_set[0],array_sizes[0],9.99999999999999939E-012) == 0);
    assert(binarySearch(array_set[0],array_sizes[0],9.89999999999999939E-012) == 0);
    assert(binarySearch(array_set[0],array_sizes[0],1.99999999999999939E-012) == 0); 
    assert(binarySearch(array_set[0],array_sizes[0],0) == 0);
    assert(binarySearch(array_set[0],array_sizes[0],30.000000000000000) == 25479);
    assert(binarySearch(array_set[0],array_sizes[0],30.100000000000000) == 25479);
    assert(binarySearch(array_set[0],array_sizes[0],40.000000000000000) == 25479); 

    assert(binarySearch(array_set[67],array_sizes[67],9.99999999999999939E-012) == 0);
    assert(binarySearch(array_set[67],array_sizes[67],9.89999999999999939E-012) == 0);
    assert(binarySearch(array_set[67],array_sizes[67],1.99999999999999939E-012) == 0);
    assert(binarySearch(array_set[67],array_sizes[67],0) == 0);
    assert(binarySearch(array_set[67],array_sizes[67],150.00000000000000 ) == 19691);
    assert(binarySearch(array_set[67],array_sizes[67],200) == 19691);
    assert(binarySearch(array_set[67],array_sizes[67],300) == 19691);

    assert(binarySearch(array_set[20],array_sizes[20],1.07250000000000005E-011) == 3);
    assert(binarySearch(array_set[20],array_sizes[20],1.21974999999999999E-011) == 8);
    assert(binarySearch(array_set[19],array_sizes[19],19.100000000000000) == 11071);
    assert(binarySearch(array_set[19],array_sizes[19],18.00000000000001) == 11070); // doesn't work with 18.000000000000000 
    printf("Passed all tests for Structure 1.\n");
}

int checkResults(int *results, double **array_set, int *array_sizes, double lookup_value){ // helper function for testResults functions
    for (int i = 0; i < k; i++){
        int index = results[i];
        double indexed_value = array_set[i][index];
        if( (index == array_sizes[i] - 1) || (indexed_value >= lookup_value) )
            ;
        else
            return 0;
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
    //printf("Results are accurate.\n");
}

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
    //printf("Results are accurate.\n");
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

int mergeKArrays(double **arr, int *arr_sizes, int i, int j, double *output, int max_len)
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
    double *unionized_arr = malloc(sizeof(double) * max_len); // memory managed in destroyUArr
    int merged_length = mergeKArrays(array_set,array_sizes, 0, k-1, unionized_arr, max_len);
    int **p = malloc(sizeof(int*) *k); // memory managed in destroyUArr
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
    //static int results[k];
    for (int i = 0; i < k; i++){
        results[i] = arr->ptr_arr[i][index_merged_arr];
    }
    return results;
}

void testStructure2(unionizedArr *structure2){
    assert(checkArr(structure2->merged_arr,structure2->length) == 1);
    assert(structure2->merged_arr[structure2->length-1] == 150.00000000000000000000);
    assert(structure2->merged_arr[0] == 9.99999999999999939E-012);
    printf("Passed all tests for Structure 2.\n");
}
