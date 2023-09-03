#include <iostream>
#include <mpi.h>
#include <vector>

using namespace std;
int vector_size = 5;

vector<int> C(vector_size);

void master(int num_processes) {
    
    vector<int> A = { 2, 4, 6, 8, 10 };
    vector<int> B = { 1, 3, 5, 7, 9 };
    
    // Send vector B to all slave processes
    MPI_Bcast(&B[0], vector_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Divide and send portions of vector A to slave processes
    int portion_size = vector_size / (num_processes);
    for (int slave_rank = 1; slave_rank < num_processes; slave_rank++) {
        int start_index = (slave_rank ) * portion_size;
        //printf("startmaster: %d\t", start_index);
        MPI_Ssend(&start_index, 1, MPI_INT, slave_rank, 0, MPI_COMM_WORLD); //send index
        MPI_Ssend(&A[start_index], portion_size, MPI_INT, slave_rank, 0, MPI_COMM_WORLD); //send elements
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
    // Perform the local vector addition
    for (int i = 0; i < portion_size; i++) {
        C[i] = A[i] + B[i];
    }
    
    int end_ind = 0;
    // Receive results from slave processes
    for (int slave_rank = 1; slave_rank < num_processes; slave_rank++) {
        int start_index = (slave_rank ) * portion_size;
        MPI_Recv(&C[start_index], portion_size, MPI_INT, slave_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        end_ind = start_index+portion_size;
    }

    // doing remaining work
    for (int i = end_ind; i < vector_size; i++) {
        C[i] = A[i] + B[i];
    }

}

void slave(int rank) {

    // Receive vector B from master
    vector<int> B(vector_size);
    MPI_Bcast(&B[0], vector_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Determine portion size
    int num_processes;
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    int portion_size = vector_size / (num_processes);

    // Receive portion of vector A from master
    int start;
    vector<int> A(portion_size);
    MPI_Recv(&start, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // receive start index to match A's elements with B's
    MPI_Recv(&A[0], portion_size, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    printf("start: %d\t", start);
    //printf("A12: %d\t", (start + portion_size));

    // printing matrice A
    for (int i = 0; i < A.size(); i++) {
        printf("A: %d\t", A[i]);
    }

    // printing matrice B
    for (int i = 0; i < B.size(); i++) {
        printf("B: %d\t", B[i]);
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
    // Perform the local vector addition
    vector<int> C(portion_size);
    for (int i = 0; i < portion_size; i++) {
        printf("\nABC: %d\t%d\t%d\t",A[i],B[start],C[i]=A[i] + B[start]);
        start++;
        //C[i] = A[i] + B[i];
    }
    
    // Send the result back to the master
    MPI_Ssend(&C[0], portion_size, MPI_INT, 0, 0, MPI_COMM_WORLD);

}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, num_processes;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

    if (rank == 0) {
        master(num_processes);
        // Print the result vector C
        for (int i = 0; i < vector_size; i++) {
            cout << C[i] << " ";
        }
        cout << endl;
    }
    else {
        cout << "SLAVE CALLING" << endl;
        slave(rank);
    }

    MPI_Finalize();
    return 0;
}