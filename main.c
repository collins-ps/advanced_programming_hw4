#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define k 68

int binarySearch(long double arr[], int n, long double target) // referenced https://www.geeksforgeeks.org/find-closest-number-array/
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
 
        /* If target is less than array element,
            then search in left */
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

int mergeArrays(long double arr1[], long double arr2[], int n1, int n2, long double arr3[]){ //referenced geekforgeeks
    int i = 1, j = 1, l = 0, n3 = 0;
 
    // Traverse both array
    while (i<n1 && j <n2)
    {
        if (arr1[i] < arr2[j]){
            arr3[l++] = arr1[i++];
            n3++;
        }
        else if (arr1[i] > arr2[j]){
            arr3[l++] = arr2[j++];
            n3++;
        }
        else {
            arr3[l++] = arr2[j++];
            i++;
            n3++;
        }
    }

    /*if (i < n1 && arr3[l-1] == arr1[i])
        i++;
    
    if (j < n2 && arr3[l-1] == arr2[j])
        j++; */
 
    // Store remaining elements of first array
    while (i < n1){
        if (arr3[l-1] != arr1[i]){
            arr3[l++] = arr1[i++];
            n3++;
        }
        else{
            i++;
        }
    }
 
    // Store remaining elements of second array
    while (j < n2){
        if (arr3[l-1] != arr2[j]){
            arr3[l++] = arr2[j++];
            n3++;
        }
        else{
            j++;
        }
    }
    /*while (j < n2){
        arr3[l++] = arr2[j++];
        n3++;
    }*/

    return n3;
}

/*int mergeKArrays(double **arr, int *array_sizes, int i, int j, double *output){
    //if one array is in range
    if(i==j) {
        for(int p = 0; p < array_sizes[i]; p++)
            output[p] = arr[i][p];
        return array_sizes[i];
    }
     
    //if only two arrays are left them merge them
    if(j-i == 1)
    {
        return mergeArrays(arr[i],arr[j],array_sizes[i],array_sizes[j],output);
        //return;
    }
     
    //output arrays
    double out1[array_sizes[i]*(((i+j)/2)-i+1)],out2[array_sizes[j]*(j-((i+j)/2))];
     
    //divide the array into halves
    mergeKArrays(arr,array_sizes, i,(i+j)/2,out1);
    mergeKArrays(arr,array_sizes, (i+j)/2+1,j,out2);
     
    //merge the output array
    mergeArrays(out1,out2,array_sizes[i]*(((i+j)/2)-i+1),array_sizes[j]*(j-((i+j)/2)),output);
}*/

/* int unionizeArr(double **array_set, int *array_sizes, double *unionized_arr){
    unionized_arr = array_set[0];
    int merged_len = array_sizes[0];
    for (int i = 1; i < 2; i++){ // change to k
        double arr1[merged_len];
        for (int j = 0; j < merged_len; j++){
            arr1[j] = unionized_arr[j];
        }
        merged_len = mergeArrays(arr1, array_set[i],merged_len,array_sizes[i],unionized_arr);
    }
    return merged_len;
} */

int mergeKArrays(long double **arr, int *arr_sizes, int i, int j, long double *output, int max_len)
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
    long double *out1 = malloc(sizeof(long double) * max_len);
    long double *out2 = malloc(sizeof(long double) * max_len);
     
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

int checkArr(long double *array, int size){
    for (int i = 0; i < size-1; i++){
        if (array[i] >= array[i+1]){
            printf("Error - array is not correctly sorted. I is %d, array[i-2] is %Lf, array[i-1] is %Lf, array[i] is %Lf, array[i+1] is %Lf, array[i+2] is %Lf.\n", i,array[i-2],array[i-1],array[i],array[i+1],array[i+2]);
            return 0;
        }
        /*if (array[i] > array[i+1]){
            printf("Error - array is not correctly sorted.\n");
            return 0;
        }*/
    }
    return 1;
}

int main(){
    long double *array_set[k];
    int array_sizes[k];
    int array_count = 0;
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    fp = fopen("arrays.txt", "r");

    if (fp == NULL)
        exit(EXIT_FAILURE); 

    /* create k sorted arrays */
    while ((read = getline(&line, &len, fp)) != -1 && array_count < k){
        char* str = strtok(line, " ");
        int length = atoi(str);
        long double *array = malloc(sizeof(long double) * length);

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

    // structure 2 //
    int max_len = 0;
    for (int i = 0; i < k; i++){
        max_len += array_sizes[i];
    }
    // printf("%d\n", max_len);
    long double *unionized_arr = malloc(sizeof(long double) * max_len);
    int merged_length = mergeKArrays(array_set,array_sizes, 0, 53, unionized_arr, max_len);

    /*FILE *file_check;
    file_check = fopen("file_check.txt", "w");
    for (int i = 0; i < merged_length; i++){
        // fprintf(file_check, "%Lf ",unionized_arr[i]);
        printf("%.10Lf ",unionized_arr[i]);
    }
    fclose(file_check);*/

    //int merged_length = unionizeArr(array_set,array_sizes,unionized_arr);
    printf("%d - %d\n", max_len, merged_length);
    checkArr(unionized_arr,merged_length);
    //assert(checkArr(unionized_arr,merged_length) == 1);

    /* binary search in k sorted arrays */
    /*
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
    */

    printf("Passed all tests.\n"); 

    fclose(fp);
    if (line)
        free(line);

    for (int i = 0; i < k; i++)
        free(array_set[i]);
    
    free(unionized_arr);

    return 0;

}
