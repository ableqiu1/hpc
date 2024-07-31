## GIT

> window直接下载安装包安装

#### **配置**

```
git config --global user.name "ableqiu1"
git config --global user.email "email"
```

```
--------ssh
// 生成密钥
ssh-keygen -t rsa -C "qacepisode@gmail.com"

// 公钥设置到github
// linux默认在~/.ssh
```



#### 创建一个仓库repository（ 版本库）

```
mkdir learn 
git init learn   //  把这个目录变成Git可以管理的仓库，之后文件会被追踪
```

##### <img src="C:\data\note\HPC\assets\image-20240715153437040.png" alt="image-20240715153437040" style="zoom:50%;" />

<img src="C:\data\note\HPC\assets\image-20240715153913882.png" alt="image-20240715153913882" style="zoom:50%; text-align:left; float:left;" />



#### 常用命令

```
git status 	//查看文件
git add  	// 提交文件到暂存区
	- git add filename,filename2...
	- git add *.txt
	- git add .     // 当前目录全部文件
git commit -m "relation information"   // 提交暂存区的文件到本地仓库，不理会工作区中文件

-------- 提交日志，查看版本编号
git log (--oneline)			// 查看提交日志

-------- 查看差异
git diff   					// 默认查看工作区、暂存区差异
git diff HEAD  				//查看工作区、版本库差异
git diff --cached			//查看暂存区、版本库差异
git diff 版本ID 版本ID2      //查看两个版本库差异
git diff branch_name branch_name2 


-------- 删除文件
git rm filename   // 从工作区和暂存区删除文件，提交后会将版本库中也删除
git rm --cached filename // 从暂存区中删除文件，保留工作区中文件，提交后会将版本库中也删除
git rm -r*   //递归删除某个目录下的所有子目录和文件，删除后提交
```



#### 回退命令

<img src="C:\data\note\HPC\assets\image-20240715160259900.png" alt="image-20240715160259900" style="zoom:50%;float:left;" />

soft模式：本地东西还在，只是git记录改变了，commit记录没了

hard模式：文件没了

mixed模式：文件在，只是git记录改变了，commit和add记录没了



#### 忽略文件.gitignore

- 仓库根目录添加文件.gitignore
- .gitignore文件中记录的文件不会提交
- .gitignore中记录的文件不对版本库中已经存在的文件生效
- 忽略文件夹格式如 `foldername/`
- 匹配规则:
  - #开始的行为注释行,注释行和空行会被忽略
  - `*` 表示匹配多个字符
  - `?` 表示匹配单个字符
  - `[]`表示匹配括号中的单个字符 , [0-9] 表示0到9中的任意一个数字, [a-z]类似
  - `**`两个星号表示匹配任意的中间目录
  - `!` 表示取反



#### 关联本地仓库和远程仓库

> 仓库名不需要相同

<img src="C:\data\note\HPC\assets\image-20240715194808826.png" alt="image-20240715194808826" style="zoom:50%; float:left" />

##### 克隆

`git clone address`

- 克隆后关联的远程仓库默认命名为`origin`

- 默认拉取主分支master/main
- `git checkout -b dev origin/feature` 可将分支之间建立联系，dev为分支名
- `git checkout -t origin/feature` -t参数默认在本地建立一个和远程仓库分支一样的分支
- 上面两种操作都会拉去远程仓库的分支，本地仓库和远程仓库分支之间具有关联关系，可以直接`git push`



##### 建立关联

```
// origin为自定义远程仓库显示名,直接克隆下来的默认为origin 
git remote add origin <address>
```

##### 推送

```
// 推送，建立分支关联
git push -u <remote_name> <local_branch>:<remote_branch>  
```

##### 拉取

```
// remote_name branch_name可省略，默认为origin和主分支
// 拉取后会合并

git pull <remote_name> <branch_name>
```



#### 分支相关

##### 基本操作

```
// 获取当前分支名 
git branch    

// 获取全部分支，包括远程仓库的分支
git branch -a

// 创建一个新的分支，不切换
git branch <new_branch_name>  

// 切换分支
git checkout <branch_name>
git switch <branch_name>

// 删除分支
git branch -d branch_name
git branch -D branch_name  // 强制删除未合并的分支

// 合并分支,在目标分支上
git merge dev   // 将dev分支合并到目标分支
```



##### 合并冲突

1. 使用merge合并分支时可能会产生冲突，这时需要手动修改产生冲突的文件然后再add和commit
2. 还可以使用rebase合并分支



# graph500

```

	这段代码的主要功能是生成一个图，并准备用于BFS（广度优先搜索）的根节点。具体步骤包括：
	1. 初始化BFS根节点数组：分配内存并初始化BFS根节点数组。
	2. 记录图生成开始时间：使用MPI_Wtime()记录图生成的开始时间。				
	3.初始化BFS根节点数组：分配内存并初始化BFS根节点数组。	
	4.记录图生成开始时间：使用MPI_Wtime()记录图生成的开始时间。
	5.生成原始图边：根据条件生成图边并写入文件或内存。
	6.生成种子：将两个64位种子数扩展为五个非零值，用于图生成。
	7.计算文件块数和位图大小：计算文件块数和位图大小，确保位图大小足够覆盖所有顶点。
	8.创建MPI通信网格：创建一个二维的MPI通信网格，用于分布式图生成。
	9.分配缓冲区：分配缓冲区用于存储生成的边数据。
	10.计算块限制和边数：计算每个进程需要处理的块数和边数，并分配相应的内存。
	11.生成图边：生成图边并写入文件或内存。
	12.释放缓冲区和通信网格：释放缓冲区和通信网格。
	13.记录图生成结束时间：记录图生成结束时间并打印生成时间。
	14.生成图数据结构并记录生成时间。
	15.生成非孤立的BFS根节点。
	16.分配内存用于存储每个BFS的边计数。
```





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
-  MPI1 中不支持数据类型转换但**支持数据表示的转换**，即改变一个值的二进制表示，比如高字节和低字节顺序的改变 将 32浮点数从32为表示改变为64为表示等



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
  -  OUT array_of_statuses  已完成对象的状态数组(状态数组)



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

#### 









### MPI部分接口

- `double MPI_Wtime(void)`
  - 返回一个用浮点数表示的秒数，表示从过去某一时刻到调用时刻所经历的时间
  - <img src="C:\data\note\HPC\assets\image-20240725213229025.png" alt="image-20240725213229025" style="zoom:67%;" />

- `int MPI_Get_processor_name ( char *name, int *resultlen)`
  - 获取机器的名字
  -  OUT name    当前进程所运行机器的名字
  -  OUT resultlen  返回名字的的长度 以可打印字符的形式

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




