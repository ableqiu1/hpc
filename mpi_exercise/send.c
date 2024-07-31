#include <stdio.h>
#include <mpi.h>
#include "string.h"
int main(int argc, char **argv)
{
	int myid, numprocs, source;
	MPI_Status status;
	char message[100];

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    
    if(myid != 0) {
    	strcpy(message, "hello world!");
    	
    	//your code here
    	MPI_Send(message, strlen(message), MPI_CHAR, 0, 99, MPI_COMM_WORLD);
    	//end of your code
	}
	else { //myid == 0
		for(source=1; source<numprocs; source++) {
			//your code here
			MPI_Recv(message, 100, MPI_CHAR, MPI_ANY_SOURCE, 99, MPI_COMM_WORLD, &status);
			//end of your code
			
			printf("%s\n", message);
		}
	}

	MPI_Finalize();
	return 0;
}
