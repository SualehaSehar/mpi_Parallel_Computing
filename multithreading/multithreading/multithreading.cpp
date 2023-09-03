#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include<mutex>
#include <vector>

// technique 1 global sum
int* vec1, * vec2 , *vec3;

int vector_length = 10;
int no_of_threads = 3;
int chunk_size = vector_length / no_of_threads;
int gSum;
using namespace std;

void* dot_product(void* arg) {
    int* localproduct = (int*)malloc(vector_length * sizeof(int));
    *localproduct = 0;
    int* id = ((int*)arg);
    int start = *id * chunk_size;  // id=2 cz=3
    int end;
    if (*id == no_of_threads - 1) {
        end = vector_length;    //  jb akhri thread ho tw last walay ka index mil jaega odd vectors honay ki surat mai
    }
    else {
        end = start + chunk_size;    // jaha se chora us k agay ka index mil jaega
    }

    for (int i = start; i < end; i++) {
        *localproduct += vec1[i] * vec2[i];

    }
    free(arg);
    pthread_exit(localproduct);
    return NULL;
}

int main() {
    pthread_t threads[3];

    vec1 = (int*)malloc(vector_length * sizeof(int));
    vec2 = (int*)malloc(vector_length * sizeof(int));

    for (int i = 0; i < vector_length; i++) {
        vec1[i] = 1 + (rand() % 3);
        vec2[i] = 1 + (rand() % 3);
    }

    for (int i = 0; i < vector_length; i++) {
        printf("%d ", vec1[i]);
    }
    printf("\n");
    for (int i = 0; i < vector_length; i++) {
        printf("%d ", vec2[i]);
    }


    for (int i = 0; i < no_of_threads; i++) {
        int* id = (int*)malloc(sizeof(int));
        *id = i;
        pthread_create(&threads[i], NULL, dot_product, (void*)id);
    }
   
    vec3 = (int*)malloc(no_of_threads * sizeof(int));

    for (int i = 0; i < no_of_threads; i++) {
        void* localproduct;
        pthread_join(threads[i], &localproduct);
        vec3[i] = *(int*)localproduct;
        free(localproduct);
    }


    for (int i = 0; i < no_of_threads; i++) {
        gSum = gSum + vec3[i];
    }

    printf("\nDot Product = %d ", gSum);

    free(vec1);
    free(vec2);
    free(vec3);

    return 0;
}

