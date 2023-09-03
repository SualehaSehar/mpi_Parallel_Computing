// don't know if it runs right

#include <iostream>
#include <mpi.h>
#include <vector>
using namespace std;


int n = 5;
int C[n][n];

void master(int num_processes) {

    int A[n][n] = { {2, 4, 6}, {8, 10 ,3}, {1 ,2 ,3} };
    int B[n][n] = { {2, 4, 6}, {8, 10 ,3}, {1 ,2 ,3} };

    int workload = n / (num_processes);
    int remaining_workload = n % (num_processes);
    MPI_Bcast(&workload, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Divide and send portions of vector A to slave processes
    for (int slave_rank = 1; slave_rank < num_processes; slave_rank++) {
        int start_index = slave_rank * workload;
        MPI_Ssend(&A[start_index][0], workload , MPI_INT, slave_rank, 0, MPI_COMM_WORLD); //send elements
        MPI_Ssend(&B[start_index][0], workload , MPI_INT, slave_rank, 0, MPI_COMM_WORLD); //send elements
    }

    MPI_Barrier(MPI_COMM_WORLD);

    // Perform the local vector addition 
    for (int i = 0; i < workload; i++) {
        for (int j = 0; j < n; j++) {
            C[i][j] = A[i][j] + B[i][j];
        }
    }

    // Receive results from slave processes
    int end_ind = 0;
    for (int slave_rank = 1; slave_rank < num_processes; slave_rank++) {
        int start_index = slave_rank * workload;
        MPI_Recv(&C[start_index][0], workload , MPI_INT, slave_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        end_ind = start_index + workload;
    }

    // doing remaining work
    for (int i = end_ind; i < remaining_workload; i++) {
        for (int j = 0; j < n; j++) {
            C[i][j] = A[i][j] + B[i][j];
        }

    }
}

void slave() {
    int workload;
    MPI_Bcast(&workload, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Receive portion of vector A from master
    int A[workload][n];
    int B[workload][n];

    MPI_Recv(&B[0][0], workload , MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&A[0][0], workload , MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);


    MPI_Barrier(MPI_COMM_WORLD);

    // Perform the local vector addition
    int C[workload][n];
    for (int i = 0; i < workload; i++) {
        for (int j = 0; j < n; j++) {
            C[i][j] = A[i][j] + B[i][j];
        }
    }

    // Send the result back to the master
    MPI_Ssend(&C[0][0], workload , MPI_INT, 0, 0, MPI_COMM_WORLD);

}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, num_processes;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

    if (rank == 0) {
        master(num_processes);
    }
    else {
        slave();
    }

    MPI_Finalize();
    return 0;
}
