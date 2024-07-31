# MPI并行程序设计

### 1 并行计算机

> 并行计算机即能在同一时间内执行多条指令 或处理多个数据 的计算机

- 按指令和数据分类：最初可分为SIMD、MIMD两种，后面引入SPMD、MPMD（P为program）
  - SIMD,SPMD适用于执行某些“重复”操作

- 按存储方式，并行计算机可分为
  - <img src="C:\data\note\HPC\assets\image-20240722152930743.png" alt="image-20240722152930743" style="zoom:67%;" />
  - 分布式内存的并行计算机：各个处理单元都**拥有自己独立的局部存储器**，由于不存在公共可用的存储单元 因此各个处理器之间**通过消息传递来交换信息**。（有很好的扩展性和很高的性能）



### 2 并行编程模型与并行语言

- 目前两种重要的并行编程模型是**数据并行**和**消息传递**
- 数据并行即**将相同的操作同时作用于不同的数据**
- 消息传递 通过在并行执行的部分之间 传递消息来交换信息  协调步伐 控制执行

![image-20240722190702997](C:\data\note\HPC\assets\image-20240722190702997.png)

- 并行语言三种实现
  - 设计全新的语言
  - 拓展串行语言
  - 提供可调用的并行库（mpi就是，难度较低的实现方法）



### 3 MPI入门

MPI是什么：

- 消息传递并行编程模型

- 一个库，有很多函数调用接口



- MPI命名接口规则
  - C语言版：`MPI_X`:  'MPI_' 开头，之后第一个字母大写，其余小写
  - 全部大写用于独立于语言的说明

- 在下面的介绍中，IN表示输入，OUT表示输出，INOUT表示输入输出
  - `IN` 调用部分传递给MPI的参数  MPI除了使用该参数外不允许对这一参 数做任何修改

  - `OUT` MPI返回给调用部分的结果参数 该参数的初始值对MPI没有任何 意义

  - `INOUT`调用部分首先将该参数传递给MPI   MPI对这一参数引用 修改后 将结果返回给外部调用 该参数的初始值和返回结果都有意义


#### 六个接口组成的子集（C语言版  MPI-1

- `int MPI_Init(int *argc, char **argv)`
  - MPI程序的第一个调用  它完成MPI程序所有的初始化工作 
  - 所有MPI程序的第一条可执行语句都是这条语句
- `int MPI_Finalize(void)`
  - MPI程序的最后一个调用 它结束MPI程序的运行 
  - 它是MPI程序的最 后一条可执行语句 否则程序的运行结果是不可预知的

- `int MPI_Comm_rank(MPI_Comm comm, int *rank)`
  - 当前进程标识，返回**调用进程**在给定的通信域中的**进程标识号**
  - `IN comm` 该进程所在的通信域（句柄）
  - `OUT rank`调用进程在comm中的标识号

- `int MPI_Comm_size(MPI_Comm comm, int *size)`
  - 返回给定的通信域中所包括的进程的个数
  - `IN comm`  该进程所在的通信域（句柄）
  - `OUT siz`  通信域comm内包括的进程数

- `int MPI_Send(void* buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)`
  - 消息发送，发送缓冲区是由count个类型为datatype的连续数据空间组成 起 始地址为buf
  - IN    buf     发送缓冲区的起始地址(可选类型)
  - IN    count   将发送的数据的个数(非负整数)
  - IN    datatype 发送数据的数据类型(句柄) 
  - IN    dest    目的进程标识号(整型)
  - IN    tag     消息标志(整型)
  - IN    comm  通信域(句柄)

- `int MPI_Recv(void* buf, int count, MPI_Datatype datatype, int source, int tag,      MPI_Comm comm, MPI_Status *status)`
  - 从指定的进程source接收消息，数据类型和消息标识和本接收 进程指定的datatype和tag相一致，接收到的消息所包含的数据元素的个数最多不能超过 count
  - 接收到消息的长度必须小于或等于接收缓冲区的长度
  - OUT   buf      接收缓冲区的起始地址(可选数据类型)
  - IN     count    最多可接收的数据的个数(整型)
  - IN     datatype  接收数据的数据类型，可为MPI预定义和自定义类型(句柄)
  - IN     source    接收数据的来源即发送数据的进程的进程标识号(整型) 
  - IN     tag       消息标识 与相应的发送操作的表示相匹配相同(整型) 
  - IN     comm    本进程和发送进程所在的通信域(句柄)
  - OUT   status     返回状态 (状态类型)

- 返回状态status
  - `status` 是MPI定义的一个数据类型，**使用之前要用户分配空间**
  - 该数据类型至少由三个域组成，分别是`MPI_SOURCE    MPI_TAG 和MPI_ERROR`
  - 可以通过形如`status.MPI_Source`的格式获取信息

- 一个简单的发送接收程序

```c
#include<stdio.h>
#include "mpi.h"
int main(int argc, cahr **argv){
    char message[20];
    int myrank;
    MPI_Init(argc,argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
    if(myrank==0){
        strcpy(message,"hello,process1");
        MPI_Send(message, strlen(messaege), MPI_Char, 1, 99, MPI_COMM_WORLD);
    }
    else if(myrank==1){
        MPI_Recv(message,20,MPI_CHAR,0,99,MPI_COMM_WORLD,&status);
        printf("received:%s",message);
    }
    MPI_Finalize();
}
```



#### MPI预定义数据类型

<img src="C:\data\note\HPC\assets\image-20240725202320567.png" alt="image-20240725202320567" style="zoom:80%;" />



#### MPI数据类型匹配和数据转换

- 宿主语言的类型和通信操作所指定的类型相匹配 即 signed char 和 MPI_CHAR
- 消息发送和接收需要指定相同的数据类型
- MPI1 中不支持数据类型转换但**支持数据表示的转换**，即改变一个值的二进制表示，比如高字节和低字节顺序的改变 将 32浮点数从32为表示改变为64为表示等



#### MPI消息

##### 消息结构

> 消息数据 + 消息信封

<img src="C:\data\note\HPC\assets\image-20240725205337469.png" alt="image-20240725205337469" style="zoom:80%;float:left" />

<img src="C:\data\note\HPC\assets\image-20240725205411212.png" alt="image-20240725205411212" style="zoom:80%;float:left" />

##### 任意源和任意标识

- 可以通过`MPI_ANY_SOURCE` 选择接收来自任意进程的消息
- 可以通过`MPI_ANY_TAG` 选择接收来自任意进程的消息
- 上面两个可以单独使用，也可以一起使用
- 发送时必须指定MPI_SOURCE 和  MPI_TAG
- 发送接收消息时必须指定通信域MPI_Comm

##### MPI通信域

- 通信域包含**进程组**和通信上下文两个部分
- 进程从0开始**顺序编号**
- 通信上下文提供一个相对独立的通信区域
- 一个预定义的通信域MPI_COMM_WORLD由MPI提供，用户可以在原有的通信域的基础上定义新的通信域



#### linux环境下的mpich









### 4 MPI 并行程序的两种基本模式

####  对等模式 MPI程序设计

####  主从模式 MPI程序设计





### 5 不同通信模式MPI 并行程序的设计

区分通信模式的核心问题：

- 是否缓存发送的数据
- 是否只有当接收调用执行后才可以执行发送操作
- 什么时候发送调用可以正确返 回 
- 发送调用正确返回是否意味着发送已完成 ，即发送缓冲区是否可以被覆盖，发送数据是否已到达接收缓冲区
- <img src="C:\data\note\HPC\assets\image-20240726141717269.png" alt="image-20240726141717269" style="zoom:80%;" />

> 四种通信模式的发送消息的函数名不同

#### 标准通信模式

<img src="C:\data\note\HPC\assets\image-20240726141756311.png" alt="image-20240726141756311" style="zoom:80%;float:left;" />

- 该模式下C语言发送消息的函数名即为` MPI_Send`

- 该模式缓冲区使用标准的系统提供的缓冲区
- 是否对发送的数据进行缓存是**由MPI自身决定的** 而不是由并行程序员来控制

#### 缓存通信模式

- 该模式下由用户直接对通信缓冲区进行申请使用和释放
- `int MPI_Bsend(void* buf, int count, MPI_Datatype datatype, int dest,
  int tag, MPI_Comm comm)`
  - 各参数含义与函数`MPI_Send`完全相同
- 该模式使用的缓冲区是用户自己提供的缓冲区
- 不管接收操作是否启动，发送操作都可以执行 但是在发送消息之前必须有缓冲区可用，即**发送操作是否成功依赖于缓冲区是否足够**
- **申请缓冲区函数**： `int MPI_Buffer_attach( void* buffer, int size)`
  - 将一个用户提供的缓冲区附加到MPI环境中。这个缓冲区将用于发送消息时的缓冲
  - IN buffer 初始缓存地址，指向用户提供的缓冲区的指针(可选数据类型)
  - IN size 按字节计数的缓存跨度(整型)

- **释放缓冲区函数**：`int MPI_Buffer_detach( void** buffer, int* size)`
  - 从MPI环境中分离之前附加的缓冲区，并返回该缓冲区的指针和大小
  - OUT buffer 指向缓冲区指针的指针（即 `void **buffer`）(可选数据类型)
  - OUT size 以字节为单位的缓冲区大小(整型)
- 示例代码
  - 

```c
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#define SIZE 6
/*总的数据大小*/
static int src  = 0;
static int dest = 1;
void Generate_Data ( double *, int );
/* 产生发送的数据*/
void Normal_Test_Recv ( double *, int );
/*标准接收*/
void Buffered_Test_Send ( double *, int );
/*缓存发送*/
void Generate_Data(double *buffer, int buff_size)
{
    int i;
    for (i = 0; i < buff_size; i++)
		buffer[i] = (double)i+1;
}

void Normal_Test_Recv(double *buffer, int buff_size)
{
    int i, j;
    MPI_Status Stat;
    double     *b;
    b = buffer;
	/* 先接收buff_size - 1个连续的双精度数 */
	MPI_Recv(b, (buff_size - 1), MPI_DOUBLE, src,2000, MPI_COMM_WORLD, &Stat);
    //fprintf是一个格式化输出函数，类似于 printf，但它允许指定输出流
    fprintf(stderr,"standard receive a message of %d data\n",buff_size-1);         
    for (j=0;j<buff_size-1;j++)
          fprintf(stderr," buf[%d]=%f\n",j,b[j]);
	b += buff_size - 1;
	/* 再接收一个双精度数 */
    MPI_Recv(b, 1, MPI_DOUBLE, src, 2000, MPI_COMM_WORLD, &Stat);    
    fprintf(stderr,"standard receive a message of one data\n");   
    fprintf(stderr,"buf[0]=%f\n",*b);
}

void Buffered_Test_Send(double *buffer, int buff_size)
{
    int i, j;
    void *bbuffer;
    int  size;
    fprintf(stderr,"buffered send message of %d data\n",buff_size-1);         
    for (j=0;j<buff_size-1;j++)
          fprintf(stderr,"buf[%d]=%f\n",j,buffer[j]);
	/*先用缓存方式方式发送buff_size - 1个双精度的数*/ 
    MPI_Bsend(buffer, (buff_size - 1), MPI_DOUBLE, dest, 2000,MPI_COMM_WORLD);
	buffer += buff_size - 1;
    fprintf(stderr,"buffered send message of one data\n");    
    fprintf(stderr,"buf[0]=%f\n",*buffer);
	/*再用缓存方式方式发送1个双精度数*/
    MPI_Bsend(buffer, 1, MPI_DOUBLE,dest, 2000, MPI_COMM_WORLD);
    /* 强制收回发送缓冲  这样也保证了该操作返回后消息已经送出*/         
    MPI_Buffer_detach( &bbuffer, &size );
	/*再重新递交发送缓存*/
    MPI_Buffer_attach( bbuffer, size );
}


int main(int argc, char **argv)
{
    int rank; /* My Rank (0 or 1) */
    double buffer[SIZE], *tmpbuffer, *tmpbuf;
    int tsize, bsize;
    char *Current_Test = NULL;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == src){
        /* 若当前进程为发送进程*/
        Generate_Data(buffer, SIZE);/*产生发送数据*/
        MPI_Pack_size( SIZE, MPI_DOUBLE, MPI_COMM_WORLD, &bsize ); 
        /*计算为发送SIZE个MPI_DOUBLE 类型的数据所需要的空间*/ 
        tmpbuffer = (double *) malloc( bsize + 2*MPI_BSEND_OVERHEAD );
        /*申请缓存发送所需要的空间*/
        if (!tmpbuffer) {
            fprintf( stderr, "Could not allocate bsend buffer of size %d\n",bsize );
            MPI_Abort( MPI_COMM_WORLD, 1 );
        }
        MPI_Buffer_attach( tmpbuffer, bsize + MPI_BSEND_OVERHEAD );
        /*将申请到的空间递交给MPI   从而MPI可以利用该空间进行消息缓存*/ 
        Buffered_Test_Send(buffer, SIZE);
        /*执行缓存消息发送*/
        MPI_Buffer_detach( &tmpbuf, &tsize );
        /*发送完成后收回递交的缓冲区*/     
	} else if (rank == dest) {
		/* 若当前进程为接收进程 */
		Normal_Test_Recv(buffer, SIZE);
    /*执行标准的接收操作*/
    } else {
		fprintf(stderr, "*** This program uses exactly 2 processes! ***\n"); 
   		/*本程序只能使用两个进程*/
        MPI_Abort( MPI_COMM_WORLD, 1 );
    }
    MPI_Finalize();
}
```

#### 同步通信模式

- <img src="C:\data\note\HPC\assets\image-20240726152636070.png" alt="image-20240726152636070" style="zoom:67%;float:left" />

- 开始发送操作不依赖接收进程，但需等到接收进程开始后才可以正确返回
- `int MPI_Ssend(void* buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)`





#### 就绪通信模式

- <img src="C:\data\note\HPC\assets\image-20240726153436248.png" alt="image-20240726153436248" style="zoom:67%;float:left" />

- 只有当接收进程的接收操作已经启动时 才可以启动发送操作
- `int MPI_Rsend`

- 就绪通信模式的特殊之处就在于它要求接收操作先于发送操作而被启动 因此 在一个 正确的程序中 一个就绪发送能被一个标准发送替代 它对程序的语义没有影响  而对程序 的性能有影响
  - 如果接收操作确实先于发送操作而启动，程序会正常运行
  - 但如果发送操作先启动则可能造成程序崩溃、数据丢失等后果
  - 通常在程序进行同步操作保证接收操作先启动
  - 就绪通信模式假设接收操作先启动从而不会进行额外的检查和同步操作，进而程序性能相较于标准模式更佳



### 6 非阻塞通信MPI程序设计

**阻塞通信特点：**

- 发送\接收 成功返回条件：
  - 操作完成，消息成功发送或成功接收
  - 缓冲区可用，若为发送操作即缓冲区可被覆盖，若为接收操作即缓冲区中数据已经完整
- 要求接收到的消息的消息信封和接 收操作自身的消息信封相一致
- 接收到的消息是最早发送给自己的消息，不能先接收后发送的消息
  - 那么如何判断该消息是不是最早的消息？？？？

**非阻塞通信：**

- 阻塞通信模式需要等待通信操作“完成”，浪费了处理机资源
- 跟并行IO类似，非阻塞通信采用专门的通信“部件”去异步执行通信操作
- 通信调用返回时  通信操作一般还未完成
- 所以**引入非阻塞通信完成对象**来让程序员知道什么时候发送操作完成
- <img src="C:\data\note\HPC\assets\image-20240729104214426.png" alt="image-20240729104214426" style="zoom:67%;float:left" />
- <img src="C:\data\note\HPC\assets\image-20240729104651162.png" alt="image-20240729104651162" style="zoom:67%;float:left" />



- <img src="C:\data\note\HPC\assets\image-20240729104713699.png" alt="image-20240729104713699" style="zoom:67%;float:left" />

- 发送和接收操作的类型虽然很多 **但只要消息信封相吻合 并且符合有序接收的语**
  **义约束 任何形式的发送和任何形式的接收都可以匹配**
- <img src="C:\data\note\HPC\assets\image-20240729104818882.png" alt="image-20240729104818882" style="zoom:67%;float:left" />



#### 非阻塞标准发送和接收

<img src="C:\data\note\HPC\assets\image-20240730144358050.png" alt="image-20240730144358050" style="zoom:67%;" />

- `int MPI_Isend(void* buf, int count, MPI_Datatype datatype, int dest, int tag,
  MPI_Comm comm, MPI_Request *request)`
  - OUT request      返回的非阻塞通信对象(句柄)

- `int MPI_Isend(void* buf, int count, MPI_Datatype datatype, int dest, int tag,
  MPI_Comm comm, MPI_Request *request)`
  - OUT request      返回的非阻塞通信对象(句柄)



#### 其它非阻塞通信模式

- `I`表示非阻塞

  - `MPI_ISSEND(buf, count, datatype, dest, tag, comm, request)`

  - `MPI_IBSEND(buf, count, datatype, dest, tag, comm, request)`

  - `MPI_IRSEND(buf, count, datatype, dest, tag, comm, request)`

    

#### 非阻塞通信的完成

- 非阻塞通信调用返回不代表通信的完成
- 使用专门的通信语句来检查通信是否完成
- 多种通信模式检查语句相同

##### 单个非阻塞通信的完成

- `int MPI_Wait(MPI_Request *request, MPI_Status *status)`
  - 该函数一直等到非阻塞通信 完成后才返回 
  - 有关的信息放在返回的状态参数status中
  - OUT status           返回的状态 (状态类型)
- `int MPI_Test(MPI_Request*request, int *flag, MPI_Status *status)`
  - 通信未完成即可返回，flag 标识通信是否完成，完成时 `flag=true`
  - OUT flag               操作是否完成标志(逻辑型)
  - OUT status           返回的状态 (状态类型)

##### 多个非阻塞通信的完成

- `int MPI_Waitany(int count, MPI_Request *array_of_requests, int *index,
  MPI_Status *status)`
  - 用于等待非阻塞通信对象表中任何一个非阻塞通信对象的完成，释放已完成的非阻塞通信对象，然后返回
  - IN count 非阻塞通信对象的个数(整型)
  - INOUT array_of_requests 非阻塞通信完成对象数组(句柄数组)
  - OUT index     完成对象对应的句柄索引(整型)
  - OUT status     返回的状态(状态类型)

- `int MPI_Waitall(int count, MPI_Request *array_of_requests,
  MPI_Status *array_of_statuses)`
  - 用于等待非阻塞通信对象表中所有的非阻塞通信对象的完成



- `int MPI_Waitsome(int incount,MPI_Request *array_of_request, int *outcount, int *array_of_indices, MPI_Status *array_of_statuses)`
  - 只要有一个或多个非阻塞 通信完成  则该调用就返回
  - OUT outcount          已完成对象的数目(整型)
  - OUT array_of_indices  已完成对象的下标数组(整型数组)
  - OUT array_of_statuses  已完成对象的状态数组(状态数组)



- `int MPI_Testany(int count, MPI_Request *array_of_requests, int *index,
  int *flag, MPI_Status *status)`



- `int MPI_Testall(int count, MPI_Request *array_of_requests, int *flag,
  MPI_Status *array_of_statuses)`



- `int MPI_Testsome(int incount,MPI_Request *array_of_request, int *outcount, int *array_of_indices, MPI_Status *array_of_statuses)`



#### 非阻塞通信对象

- 存储通信相关信息，所有的非阻塞发送或接收通信都会返回 一个 非阻塞通信对象
- 非 阻塞通信对象是MPI内部的对象 通过一个句柄存取
- 可以获取发送模式，通信缓冲区，通信上下文，标识，目的参数等

##### 非阻塞通信的取消

- `int MPI_Cancel(MPI_Request *request)`
  - 当非阻塞通信还未开始时可正常取消，释放通信占用资源
  - 当通信开始后该取消操作无效

##### 非阻塞通信对象的释放

- `int MPI_Request_free(MPI_Request * request)`
  - 释放后非阻塞通信对象request变为MPI_REQUEST_NULL
  - 释放操作会等待通信完成后再进行，所以该操作不会影响非阻塞通信的完成



#### 消息到达的检查

- 可以不执行接收操作直接检查消息是否到达

- ```c
  int MPI_Probe(int source,int tag,MPI_Comm comm,MPI_Status *status);
  	- 阻塞调用，只有找到一个匹配的消息到达之后才返回
  
  int MPI_Iprobe(int source,int tag,MPI_Comm comm,int *flag, MPI_Status *status);
  	- 非阻塞调用
  	- IN source    源进程标识或任意进程标识MPI_ANY_SOURCE(整型)      
      - IN tag       特定tag值或任意tag值MPI_ANY_TAG   整型
  	- OUT flag    是否有消息到达标志
      - OUT status   返回的状态 状态类型
  
  
  ```



#### 非阻塞通信有序接收的语义约束

- 和阻塞通信一样  也有有序接收的语义约束  两者的含义是类似的
- 进程A向进程B发送的消息只能被进程B第一个匹配的接收语句接收 下面的接收语句即
  使匹配也不能超前接收消息



#### 重复非阻塞通信

- 如果一个通信会被重复执行，MPI提供了特殊的实现方式 对这样的通信进行优化，以降低不必要的通信开销
- 它将通信参数和MPI的内部对象建立固 定的联系 然后通过该对象完成重复通信的任务
- 这样的通信方式在MPI中都是非阻塞通信
- 重复非阻塞通信的步骤：
  - 通信的初始化 比如MPI_SEND_INIT
  - 启动通信  MPI_START
  - 完成通信  MPI_WAIT  或者 MPI_TEST等
  - 释放查询对象 MPI_REQUEST_FREE  （只有非活动状态才行）
  - 消息的完成操作并不释放相应的非阻塞通信对象  只是将其状态置为 非活动状态  若下面进行重复通信  则再由MPI_START 将该对象置为活动状态 并启动通信
  - 当不需要再进行通信时  必须通过显式的语句MPI_REQUEST_FREE将非阻塞通信对象 释放掉 这是重复通信和一般的非阻塞通信不同的地方

- 重复非阻塞通信有四种模式（标准，缓存，同步，就绪）
  - 函数形如：`int MPI_Bsend_init(void* buf, int count, MPI_Data type,int dest, int tag,
    MPI_Comm comm, MPI_Request *request)`
  - 单个通信对象激活函数：`int MPI_Start(MPI_Request *request)`  
    - INOUT   request
  - 多个通信对象激活函数：`int MPI_Startall(int count, MPI_Request *array_of_requests)`  



#### 非阻塞通信实现Jacobi迭代





### 7 组通信MPI程序设计

#### 概述

> 组通信一般实现三个功能：通信、同步和计算

- 通信可分为一对多、多对一和多对多（ 一 被称为ROOT	
- 组通信还提供专门的调用以完成各个进程之间的同步

<img src="C:\data\note\HPC\assets\image-20240731162842777.png" alt="image-20240731162842777" style="zoom:67%;" />

- <img src="C:\data\note\HPC\assets\image-20240731163101293.png" alt="image-20240731163101293" style="zoom: 70%;" />



#### 广播

```c
IN/OUT   buffer     通信消息缓冲区的起始地址(可选数据类型)
IN        count     将广播出去/或接收的数据个数(整型)
IN        datatype   广播/接收数据的数据类型(句柄)
IN        root       广播数据的根进程的标识号(整型)
IN        comm     通信域(句柄)
    
int MPI_Bcast(void* buffer,int count,MPI_Datatype datatype,int root, MPI_Comm comm)
```

- 对于广播消息的根进程或接收消息的进程，在**调用形式上完全一致**，即指明相同的根  相同的元素个数以及相同的数据类型，但各自指明自己的消息缓冲区


```C
#include<stdio.h>
#include "mpi.h"
int main(int argc, char **argv){
    int rank, value;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    do{
        if(rank==0){
            scanf("%d",&value);
        }
		MPI_Bcast(&value, 1, MPI_INT, 0, MPI_COMM_WORLD);
		printf("Processor %d got %d\n",rank,value);
    }while(value>=0);
    MPI_Finalize();
    return 0;
}
```



#### 收集

```c
IN  sendbuf     发送消息缓冲区的起始地址(可选数据类型)
IN  sendcount   发送消息缓冲区中的数据个数(整型)
IN  sendtype    发送消息缓冲区中的数据类型(句柄)
OUT recvbuf    接收消息缓冲区的起始地址(可选数据类型)
IN  recvcount   待接收的元素个数(整型,仅对于根进程有意义)
IN  recvtype    接收元素的数据类型(句柄,仅对于根进程有意义)
IN  root        接收进程的序列号(整型)
IN  comm      通信域(句柄)
int MPI_Gather(void* sendbuf, int sendcount, MPI_Datatype sendtype,
               void* recvbuf, int recvcount, MPI_Datatype recvtype, 
               int root, MPI_Comm comm)
```

- 每个进程 包括根进 程本身 将其发送缓冲区中的消息发送到根进程
- 根进程根据发送进程的进程标识的序号即 **进程的rank值** 将它们各自的消息**依次存放到自已的消息缓冲区中**

- 对于所有非根进程 接收消息缓冲区被忽略 但是各个进程必须提供这一参数
- 根进程中指定的接收数 据个数是指从每一个进程接收到的数据的个数 而不是总的接收个数
  - 每个进程的发送数据个数sendcount和发送数据类型sendtype都是相同的 都和 根进程中接收数据个数recvcount和接收数据类型recvtype相同



```C
IN     sendbuf      发送消息缓冲区的起始地址(可选数据类型)
IN     sendcount    发送消息缓冲区中的数据个数(整型)
IN     sendtype     发送消息缓冲区中的数据类型(句柄)
OUT    recvbuf     接收消息缓冲区的起始地址(可选数据类型,仅对于根进程有意义) 
IN     recvcounts    整型数组(长度为组的大小), 其值为从每个进程接收的数据个数 
IN     displs        整数数组,每个入口表示相对于recvbuf的位移
IN     recvtype      接收消息缓冲区中数据类型 (句柄)
IN     root         接收进程的标识号(句柄)
IN     comm       通信域(句柄)
int MPI_Gatherv(void* sendbuf, int sendcount, MPI_Datatype sendtype, 
                void* recvbuf, int *recvcounts, int *displs, MPI_Datatype recvtype, 
                int root, MPI_Comm comm)

```

- MPI_GATHERV 和MPI_GATHER的功能类似，但是它可以从不同的进程接收不同数量的数据
- 位置偏移displs数组，用户可以将接收的数据存放到根进程消息缓冲区的任意位置



```C
#include<stdio.h>
#include "mpi.h"
int main(int argc, char **argv){
    int gsize, sendarray[100];
    int root, *rbuf, stride;
    int *displs, i, *rcounts;
   MPI_Comm_size(comm, &gsize);
    rbuf = (int *)malloc(gsize*stride*sizeof(int));
    displs = (int *)malloc(gsize*sizeof(int));
    rcounts = (int *)malloc(gsize*sizeof(int));
    for (i=0; i<gsize; ++i) {
        displs[i] = i*stride;
        rcounts[i] = 100;
    }
    MPI_Gatherv(sendarray, 100, MPI_INT, rbuf, rcounts, displs, MPI_INT, root, comm);
    
    MPI_Finalize();
    return 0;
}
```



#### 散发

- 散发是一对多通信

- 和广播不同  ROOT 向各个进程发送的数据可以是不同的

- 基础模式：（只能发送）

  - ```c
    IN   sendbuf      发送消息缓冲区的起始地址(可选数据类型)
     IN   sendcount    发送到各个进程的数据个数(整型)
     IN   sendtype     发送消息缓冲区中的数据类型(句柄)
     OUT  recvbuf     接收消息缓冲区的起始地址(可选数据类型)
     IN   recvcount    待接收的元素个数(整型)
     IN   recvtype     接收元素的数据类型(句柄)
     IN   root         发送进程的序列号(整型)
     IN   comm       通信域(句柄)
    int MPI_Scatter(void* sendbuf, int sendcount, MPI_Datatype sendtype,                 
                    void* recvbuf, int recvcount,  MPI_Datatype recvtype,                
                    int root, MPI_Comm comm)
    ```

  - 

- 





### MPI部分接口

- `double MPI_Wtime(void)`
  - 返回一个用浮点数表示的秒数，表示从过去某一时刻到调用时刻所经历的时间
  - <img src="C:\data\note\HPC\assets\image-20240725213229025.png" alt="image-20240725213229025" style="zoom:67%;" />

- `int MPI_Get_processor_name ( char *name, int *resultlen)`
  - 获取机器的名字
  - OUT name    当前进程所运行机器的名字
  - OUT resultlen  返回名字的的长度 以可打印字符的形式

- `int MPI_Get_version(int * version, int * subversion)`
  - 获取mpi版本号
  - out version  主版本号
  - out subversion   次版本号

- `int MPI_Initialized(int *flag)`
  - 是否初始化
  - out flag    初始化了则为true,否则为false

- `int MPI_Abort(MPI_Comm comm, int errorcode)`
  - 使通信域comm中的所有进程退出，本调用并不要求外部环境对错误码采取 任何动作
  - in comm 退出进程所在的通信域
  - in errorcode  返回到所嵌环境的错误码



