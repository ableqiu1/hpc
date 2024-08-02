#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv)
{
    int myid;
	MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    int gsize,sendarray[100];
    int root, *rbuf, stride;
    int *displs, i, *rcounts;
    for(i=0;i<myid+1;i++){
        sendarray[i] = i;
    }

    MPI_Comm_size(MPI_COMM_WORLD, &gsize);
    // 接收缓冲区
    rbuf = (int *)malloc(gsize*stride*sizeof(int));
    // 偏移数组
    displs = (int *)malloc(gsize*sizeof(int));
    // 计数数组
    rcounts = (int *)malloc(gsize*sizeof(int));
    for(i=0;i<gsize;i++){
        displs[i] = i*stride;
        rcounts[i] = i+1;
    }

    MPI_Allgatherv(sendarray, 100, MPI_INT, rbuf, rcounts, displs, MPI_INT, MPI_COMM_WORLD);

    for(int j=0;j<gsize*100;j++){
        printf("%d ", rbuf[j]);
    }

	MPI_Finalize();
	return 0;
}
