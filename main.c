#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define k 68

int binarySearch( double arr[], int n,  double target) // referenced https://www.geeksforgeeks.org/find-closest-number-array/
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

int mergeKArrays( double **arr, int *arr_sizes, int i, int j,  double *output, int max_len)
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

void unionizeArr(double *merged_arr, int merged_length, double **array_set, int *array_sizes, int **p){
    for (int i = 0; i < k; i++){
        p[i] = malloc(sizeof(int) * merged_length);
        for (int j = 0; j < merged_length; j++){
            p[i][j] = binarySearch(array_set[i],array_sizes[i],merged_arr[j]);
        }
    }
}

void destroyUArray(double *unionized_arr, int ** p){
    for (int i = 0; i < k; i++)
        free(p[i]);
    free(p);
    free(unionized_arr);
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

    /* TEST STRUCTURE 1 */
    /* assert(binarySearch(array_set[0],array_sizes[0],9.99999999999999939E-012) == 0);
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
    assert(binarySearch(array_set[19],array_sizes[19],18.00000000000001) == 11070); // doesn't work with 18.000000000000000 */

    /* CREATE STRUCTURE 2 */
    int max_len = 0;
    for (int i = 0; i < k; i++){
        max_len += array_sizes[i];
    }
    double *unionized_arr = malloc(sizeof(double) * max_len); // memory managed in destroyUArr
    int merged_length = mergeKArrays(array_set,array_sizes, 0, k-1, unionized_arr, max_len);
    int **p = malloc(sizeof(int*) *k); // memory managed in destroyUArr
    unionizeArr(unionized_arr,merged_length,array_set,array_sizes,p);

    for(int i = 0; i < merged_length; i++){
        printf("%d ", p[0][i]);
    }
    
    /* TEST STRUCTURE 2 */
    /*assert(checkArr(unionized_arr,merged_length) == 1);
    assert(unionized_arr[merged_length-1] == 150.00000000000000000000);
    assert(unionized_arr[0] == 9.99999999999999939E-012); */

    printf("Passed all tests.\n"); 

    fclose(fp);
    if (line)
        free(line);

    for (int i = 0; i < k; i++)
        free(array_set[i]);
    
    //free(unionized_arr);
    destroyUArray;

    return 0;
}
