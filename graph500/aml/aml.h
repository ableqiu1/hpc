/* Copyright (c) 2011-2017 Graph500 Steering Committee
   All rights reserved.
   Developed by:                Anton Korzh anton@korzh.us
                                Graph500 Steering Committee
                                http://www.graph500.org
   New code under University of Illinois/NCSA Open Source License
   see license.txt or https://opensource.org/licenses/NCSA
*/

#ifdef __cplusplus
extern "C" {
#endif

    //extern是C/C++语言中的一个关键字，用于声明一个变量或函数是在其他地方定义的，通常是在另一个源文件中
	//MPI-like init,finalize calls
	// 初始化和结束函数
	extern int  aml_init(int *,char***);
	extern void aml_finalize(void);
	//barrier which ensures that all AM sent before the barrier are completed everywhere after the barrier
	// 确保所有AM（活动消息）在屏障之前发送完成，在屏障之后，所有地方都完成
	extern void aml_barrier( void );
	//register active message function(collective call)
	//注册活动消息处理函数（集体调用）
	extern void aml_register_handler(void(*f)(int,void*,int),int n);
	//send AM to another(myself is ok) node
	//execution of AM might be delayed till next aml_barrier() call
	//向另一个节点发送AM（自己可以）
	//AM的执行可能延迟到下一个aml_barrier()调用
	extern void aml_send(void *srcaddr, int type,int length, int node );

	// rank and size
	// 节点号和节点数
	extern int aml_my_pe( void );
	extern int aml_n_pes( void );

#ifdef __cplusplus
}
#endif

#define my_pe aml_my_pe
#define num_pes aml_n_pes

#define aml_time() MPI_Wtime()

/*
MPI_Allreduce 是MPI（Message Passing Interface）库中的一个函数，
用于在所有进程中执行归约操作，并将结果分发给所有进程。
归约操作可以是求和、求最小值、求最大值等。

int MPI_Allreduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);

参数说明：		
	sendbuf：发送缓冲区的起始地址。
	recvbuf：接收缓冲区的起始地址。
	count：要归约的元素数量。
	datatype：要归约的数据类型。
	op：归约操作类型，如 MPI_SUM、MPI_MIN、MPI_MAX 等。
	comm：通信子（ communicator ），表示一组进行通信的进程。

*/
#define aml_long_allsum(p) MPI_Allreduce(MPI_IN_PLACE,p,1,MPI_LONG_LONG,MPI_SUM,MPI_COMM_WORLD)
#define aml_long_allmin(p) MPI_Allreduce(MPI_IN_PLACE,p,1,MPI_LONG_LONG,MPI_MIN,MPI_COMM_WORLD)
#define aml_long_allmax(p) MPI_Allreduce(MPI_IN_PLACE,p,1,MPI_LONG_LONG,MPI_MAX,MPI_COMM_WORLD)
