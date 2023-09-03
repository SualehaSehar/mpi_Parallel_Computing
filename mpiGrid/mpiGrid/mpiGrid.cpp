#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    int rank, size;
    int periods[2] = { 0, 0 };
    int ndims = 2;
    int mapping = 0;
    int coordseven[2], coordsodd[2];
    int* evenProc, * oddProc;
    MPI_Group groupID, evengr, oddgr;
    MPI_Comm evencom, oddcom;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int q = size / 2;
    evenProc = (int*)malloc(q * sizeof(int));
    oddProc = (int*)malloc(q * sizeof(int));
    int dims[2] = { q / 2, 2 };

    int even = 0, odd = 0;
    for (int proc = 0; proc < size; proc++) {
        if (proc % 2 == 0) {
            evenProc[even] = proc;
            even++;
        }
        else {
            oddProc[odd] = proc;
            odd++;
        }
    }

    // Get default world group id
    MPI_Comm_group(MPI_COMM_WORLD, &groupID);

    // Create groups
    MPI_Group_incl(groupID, q, evenProc, &evengr);
    MPI_Group_incl(groupID, q, oddProc, &oddgr);

    // Create communicators for even and odd groups
    MPI_Comm_create(MPI_COMM_WORLD, evengr, &evencom);
    MPI_Comm_create(MPI_COMM_WORLD, oddgr, &oddcom);

    if (evencom!=MPI_COMM_NULL) {
        // Create a Cartesian communicator for the even group
        MPI_Comm cart_comm_even;
        MPI_Cart_create(evencom, ndims, dims, periods, mapping, &cart_comm_even);

        // Get the coordinates of the current process in the grid
        int rankeven;
        MPI_Comm_rank(cart_comm_even, &rankeven);
        MPI_Cart_coords(cart_comm_even, rankeven, ndims, coordseven);

        // Print the grid information
        printf("Rank %d (Even Group): Coordinates (%d, %d)\n", rank, coordseven[0], coordseven[1]);
    }

    if (oddcom != MPI_COMM_NULL) {

        // Create a Cartesian communicator for the odd group
        MPI_Comm cart_comm_odd;
        MPI_Cart_create(oddcom, ndims, dims, periods, mapping, &cart_comm_odd);

        // Get the coordinates of the current process in the grid
        int rankodd, rankeven;
        MPI_Comm_rank(cart_comm_odd, &rankodd);
        MPI_Cart_coords(cart_comm_odd, rankodd, ndims, coordsodd);

        // Print the grid information
        printf("Rank %d (Odd Group): Coordinates (%d, %d)\n", rank, coordsodd[0], coordsodd[1]);
    }

    MPI_Finalize();
    return 0;
}
