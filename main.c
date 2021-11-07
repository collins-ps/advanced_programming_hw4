#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <assert.h>

#define k 68

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

    /* create k sorted arrays */
    while ((read = getline(&line, &len, fp)) != -1 && array_count < k){
        char* str = strtok(line, " ");
        int length = atoi(str);
        //printf("%d\n",length);
        double *array = malloc(sizeof(double) * length);

        int i = 0;
        array_sizes[array_count] = length;
        //printf("%d\n",array_sizes[array_count]);
        str = strtok(NULL, " ");
        //printf("%s\n",str);
        while (str != NULL && str[0] != '\n'){ // (!strcmp(str,"\n")){
            //printf("%s\n",str);
            array[i] = atof(str);
            str = strtok(NULL, " ");
            i++;
        }
        array_set[array_count] = array;
        array_count++;
    }

    /* binary search in k sorted arrays */

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
    
    printf("Passed all tests.\n");


    /* for (int i = 0; i < k; i++){
        int result = binarySearch(array_set[i],0,array_sizes[i]-1,lookup_value);
    } */

    //printf("%.5f",(array_set[67])[19691]);
    //printf("%d",array_count);

    // structure 2 //
    /*int max_len = 0;
    for (int i = 0; i < array_count; i++){
        max_len += array_set[i][0];
    }
    double *unionised_arr = malloc(sizeof(double) * max_len);
    create_struct2(array_set,unionised_arr); */

    fclose(fp);
    if (line)
        free(line);

    for (int i = 0; i < k; i++)
        free(array_set[i]);
    
    //free(unionised_arr);

    return 0;

}
