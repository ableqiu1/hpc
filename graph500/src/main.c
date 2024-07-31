/* Copyright (C) 2010 The Trustees of Indiana University.                  */
/*                                                                         */
/* Use, modification and distribution is subject to the Boost Software     */
/* License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at */
/* http://www.boost.org/LICENSE_1_0.txt)                                   */
/*                                                                         */
/*  Authors: Jeremiah Willcock                                             */
/*           Andrew Lumsdaine                                              */
/*           Anton Korzh                                                   */


/* These need to be before any possible inclusions of stdint.h or inttypes.h.
 * */
#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include "../generator/make_graph.h"
#include "../generator/utils.h"
#include "aml.h"
#include "common.h"
#include <math.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <inttypes.h>

int isisolated(int64_t v);
// 比较双精度浮点数精度 a<b -> -1, a==b -> 0, a>b -> 1 用于qsort()函数
static int compare_doubles(const void* a, const void* b) {
	double aa = *(const double*)a;
	double bb = *(const double*)b;
	return (aa < bb) ? -1 : (aa == bb) ? 0 : 1;
}

// 枚举定义整数常量
// 表示结果数组中的下标，分别为最小值，第一四分位数，中位数，第三四分位数，最大值，均值，标准差
enum {s_minimum, s_firstquartile, s_median, s_thirdquartile, s_maximum, s_mean, s_std, s_LAST};


// 计算数组统计信息
// volatile是一个关键字，用于告诉编译器该变量可能会在程序的控制之外被改变。
// 这意味着编译器不应该对该变量进行优化，以确保每次访问该变量时都从内存中读取最新的值
void get_statistics(const double x[], int n, volatile double r[s_LAST]) {
	double temp;
	int i;
	/* Compute mean. */
	// 计算均值
	temp = 0.0;
	for (i = 0; i < n; ++i) temp += x[i];
	temp /= n;
	r[s_mean] = temp;
	double mean = temp;

	/* Compute std. dev. */
	// 计算标准差
	temp = 0;
	for (i = 0; i < n; ++i) temp += (x[i] - mean) * (x[i] - mean);
	temp /= n - 1;
	r[s_std] = sqrt(temp);

	/* Sort x. */
	// 对数组进行排序
	double* xx = (double*)xmalloc(n * sizeof(double));
	memcpy(xx, x, n * sizeof(double));
	/*
	数组排序：void qsort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *));
		其中compar：指向比较函数的指针，该函数用于确定两个元素的顺序。返回-1表示第一个元素排在第二个元素之前
	*/
	qsort(xx, n, sizeof(double), compare_doubles);

	/* Get order statistics. */
	r[s_minimum] = xx[0];   // 最小值
	r[s_firstquartile] = (xx[(n - 1) / 4] + xx[n / 4]) * .5;   // 第一四分位数
	r[s_median] = (xx[(n - 1) / 2] + xx[n / 2]) * .5;         // 中位数
	r[s_thirdquartile] = (xx[n - 1 - (n - 1) / 4] + xx[n - 1 - n / 4]) * .5;  // 第三四分位数
	r[s_maximum] = xx[n - 1];     // 最大值
	/* Clean up. */
	free(xx);
}




/*

1.main(int argc, char** argv)
  - argc和argv用于接收命令行参数
  - argc 表示命令行参数数量，至少包含程序本身的名称，所以min=1
  - argv则存储执行参数的字符串数组的指针
  - argv[0] 表示程序本身的名称
  - argv[1] 表示第一个命令行参数，即SCALE
  - argv[2] 表示第二个命令行参数，即edgefactor
  - 如果命令行参数数量小于2或大于3，则打印使用说明，并退出程序
  - 如果命令行参数数量等于2，则SCALE和edgefactor都取默认值	


 */
int main(int argc, char** argv) {
	aml_init(&argc,&argv); //includes MPI_Init inside
	setup_globals();   // 工具文件utils.c中，设置变量，包括rank,size等

	/* Parse arguments. */
	/*解析参数*/
	int SCALE = 16;
	int edgefactor = 16; /* nedges / nvertices, i.e., 2*avg. degree */
	if (argc >= 2) SCALE = atoi(argv[1]);    // 取第一个命令行参数作为SCALE atoi()函数用于将字符串转换为整数
	if (argc >= 3) edgefactor = atoi(argv[2]);	
	if (argc <= 1 || argc >= 4 || SCALE == 0 || edgefactor == 0) {
		 
		if (rank == 0) {
			// 如果是根进程（rank == 0），则打印使用说明到标准错误输出
			fprintf(stderr, "Usage: %s SCALE edgefactor\n  SCALE = log_2(# vertices) [integer, required]\n  edgefactor = (# edges) / (# vertices) = .5 * (average vertex degree) [integer, defaults to 16]\n(Random number seed and Kronecker initiator are in main.c)\n", argv[0]);
		}
		// 错误，终止所有进程
		// MPI_COMM_WORLD 
		// 1 错误代码
		MPI_Abort(MPI_COMM_WORLD, 1);

	}
	// uint64_t 是一种来自 C 标准库中 <stdint.h> 头文件的标准整数类型，表示一种无符号的64位整数类型
	uint64_t seed1 = 2, seed2 = 3;

	/*	getenv("TMPFILE") 是一个标准库函数，用于获取环境变量的值
		从环境变量 TMPFILE 中获取文件名，
		并将其存储在 filename 指针中。如果环境变量不存在，filename 将被设置为 NULL
	*/
	const char* filename = getenv("TMPFILE");

	
#ifdef SSSP

   /*根据已有的 filename 生成一个新的文件名 wfilename，并在其末尾添加 .weights 后缀*/
	int wmode;
	char *wfilename=NULL;  
	if(filename!=NULL) {
		wfilename=malloc(strlen(filename)+9);
		wfilename[0]='\0';
		// strcat() 函数用于将后续字符串连接到字符串
		strcat(wfilename,filename);
		strcat(wfilename,".weights");
	}

#endif
	const int reuse_file = getenv("REUSEFILE")? 1 : 0;
	/* If filename is NULL, store data in memory */

	/*通过左移操作计算了全局边和顶点的数量，
	并将这些值分别存储在 tuple_graph 结构体的成员变量 nglobaledges 和局部变量 nglobalverts 中*/
	tuple_graph tg;
	tg.nglobaledges = (int64_t)(edgefactor) << SCALE;
	int64_t nglobalverts = (int64_t)(1) << SCALE;

	tg.data_in_file = (filename != NULL);     // 指示数据是否存储在文件中 0表示存储在内存中
	tg.write_file = 1;

	if (tg.data_in_file) {  // 如果数据存储在文件中	
		/* Open the file for writing. */
		
		int is_opened = 0;
		// 表示文件将以读写模式打开，并且是独占的和唯一的
		int mode = MPI_MODE_RDWR | MPI_MODE_EXCL | MPI_MODE_UNIQUE_OPEN;

		if (!reuse_file) {
			// 如果环境变量 REUSEFILE 不存在，  则设置 文件在关闭时删除
			mode |= MPI_MODE_CREATE | MPI_MODE_DELETE_ON_CLOSE;
		} else {
			// 如果环境变量 REUSEFILE 存在
			// 则设置 错误处理程序'MPI_ERRORS_RETURN'
			MPI_File_set_errhandler(MPI_FILE_NULL, MPI_ERRORS_RETURN);
			// 尝试打开文件，如果成功，则设置 is_opened 为 1
			/* MPI_File_open() 函数用于打开一个 MPI 文件，并返回一个 MPI 文件句柄。
			第一个参数是 MPI 通信器，第二个参数是文件名，第三个参数是打开模式，第四个参数是 MPI 信息对象，*/
			if (MPI_File_open(MPI_COMM_WORLD, (char*)filename, mode,
						MPI_INFO_NULL, &tg.edgefile)) {
				if (0 == rank && getenv("VERBOSE"))
					fprintf (stderr, "%d: failed to open %s, creating\n",
							rank, filename);
				mode |= MPI_MODE_RDWR | MPI_MODE_CREATE;
#ifdef SSSP
				wmode=mode;
#endif
			} else {
				// 文件打开成功，获取文件大小
				MPI_Offset size;
				MPI_File_get_size(tg.edgefile, &size);
				if (size == tg.nglobaledges * sizeof(packed_edge)) {
#ifdef SSSP
					// 处理sssp文件
					wmode=mode;
					if(MPI_File_open(MPI_COMM_WORLD, (char*)wfilename, mode, MPI_INFO_NULL, &tg.weightfile))
					{
						wmode |= MPI_MODE_RDWR | MPI_MODE_CREATE;
						MPI_File_close (&tg.edgefile);
					}
					else { //both files were open succedfully
#endif
						is_opened = 1;
						tg.write_file = 0;
#ifdef SSSP
					}
#endif
				} else /* Size doesn't match, assume different parameters. */
					//文件大小不匹配，关闭文件
					MPI_File_close (&tg.edgefile);
			}
		}

		// 设置错误处理程序
		MPI_File_set_errhandler(MPI_FILE_NULL, MPI_ERRORS_ARE_FATAL);

		// 如果文件没打开，重新尝试打开文件
		if (!is_opened) {
			MPI_File_open(MPI_COMM_WORLD, (char*)filename, mode, MPI_INFO_NULL, &tg.edgefile);
			MPI_File_set_size(tg.edgefile, tg.nglobaledges * sizeof(packed_edge));
		}
#ifdef SSSP
		if (!is_opened) {
			MPI_File_open(MPI_COMM_WORLD, (char*)wfilename, wmode, MPI_INFO_NULL, &tg.weightfile);
			MPI_File_set_size(tg.weightfile, tg.nglobaledges * sizeof(float));
		}    
		MPI_File_set_view(tg.weightfile, 0, MPI_FLOAT, MPI_FLOAT, "native", MPI_INFO_NULL);
		MPI_File_set_atomicity(tg.weightfile, 0);
#endif
		// 设置MPI文件的视图,使得文件从开头开始,使用 packed_edge_mpi_type 数据类型，并且使用本地字节序
		// 对于 packed_edge_mpi_type 数据类型，其定义如下：
		// typedef struct {
		//     int64_t src;	
		MPI_File_set_view(tg.edgefile, 0, packed_edge_mpi_type, packed_edge_mpi_type, "native", MPI_INFO_NULL);
		
		// 设置MPI文件原子性,0表示禁用原子性操作
		MPI_File_set_atomicity(tg.edgefile, 0);
	}



	/* Make the raw graph edges. */
	/* Get roots for BFS runs, plus maximum vertex with non-zero degree (used by
	 * validator). */

	/*
	
	
	*/

	// 定义BFS根节点的数量为64，并分配相应大小的内存来存储这些根节点
	int num_bfs_roots = 64;
	int64_t* bfs_roots = (int64_t*)xmalloc(num_bfs_roots * sizeof(int64_t));

	// 记录图生成开始时间
	double make_graph_start = MPI_Wtime();
	// data_in_file 在文件中为1
	// 如果数据在内存中或者需要写入 进入if语句
	if( !tg.data_in_file || tg.write_file )
	{
		/* Spread the two 64-bit numbers into five nonzero values in the correct
		 * range. */

		// 将两个64位种子数扩展为五个非零值，用于图生成
		uint_fast32_t seed[5];
		make_mrg_seed(seed1, seed2, seed);

		/* As the graph is being generated, also keep a bitmap of vertices with
		 * incident edges.  We keep a grid of processes, each row of which has a
		 * separate copy of the bitmap (distributed among the processes in the
		 * row), and then do an allreduce at the end.  This scheme is used to avoid
		 * non-local communication and reading the file separately just to find BFS
		 * roots. */

		// 计算文件块数和位图大小，确保位图大小足够覆盖所有顶点
		MPI_Offset nchunks_in_file = (tg.nglobaledges + FILE_CHUNKSIZE - 1) / FILE_CHUNKSIZE;
		int64_t bitmap_size_in_bytes = int64_min(BITMAPSIZE, (nglobalverts + CHAR_BIT - 1) / CHAR_BIT);
		if (bitmap_size_in_bytes * size * CHAR_BIT < nglobalverts) {
			bitmap_size_in_bytes = (nglobalverts + size * CHAR_BIT - 1) / (size * CHAR_BIT);
		}

		// 创建一个二维的MPI通信网格，用于分布式图生成
		int ranks_per_row = tg.data_in_file ? ((nglobalverts + CHAR_BIT - 1) / CHAR_BIT + bitmap_size_in_bytes - 1) / bitmap_size_in_bytes : 1;
		int nrows = size / ranks_per_row;
		int my_row = -1, my_col = -1;
		MPI_Comm cart_comm;
		{
			int dims[2] = {size / ranks_per_row, ranks_per_row};
			int periods[2] = {0, 0};
			MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 1, &cart_comm);
		}

		int in_generating_rectangle = 0;
		if (cart_comm != MPI_COMM_NULL) {
			in_generating_rectangle = 1;
			{
				int dims[2], periods[2], coords[2];
				MPI_Cart_get(cart_comm, 2, dims, periods, coords);
				my_row = coords[0];
				my_col = coords[1];
			}
			MPI_Comm this_col;
			MPI_Comm_split(cart_comm, my_col, my_row, &this_col);
			MPI_Comm_free(&cart_comm);
			/* Every rank in a given row creates the same vertices (for updating the
			 * bitmap); only one writes them to the file (or final memory buffer). */

			
			packed_edge* buf = (packed_edge*)xmalloc(FILE_CHUNKSIZE * sizeof(packed_edge));
#ifdef SSSP
			float* wbuf = (float*)xmalloc(FILE_CHUNKSIZE*sizeof(float));
#endif
			MPI_Offset block_limit = (nchunks_in_file + nrows - 1) / nrows;
			/* fprintf(stderr, "%d: nchunks_in_file = %" PRId64 ", block_limit = %" PRId64 " in grid of %d rows, %d cols\n", rank, (int64_t)nchunks_in_file, (int64_t)block_limit, nrows, ranks_per_row); */
			if (tg.data_in_file) {
				tg.edgememory_size = 0;
				tg.edgememory = NULL;
			} else {
				int my_pos = my_row + my_col * nrows;
				int last_pos = (tg.nglobaledges % ((int64_t)FILE_CHUNKSIZE * nrows * ranks_per_row) != 0) ?
					(tg.nglobaledges / FILE_CHUNKSIZE) % (nrows * ranks_per_row) :
					-1;
				int64_t edges_left = tg.nglobaledges % FILE_CHUNKSIZE;
				int64_t nedges = FILE_CHUNKSIZE * (tg.nglobaledges / ((int64_t)FILE_CHUNKSIZE * nrows * ranks_per_row)) +
					FILE_CHUNKSIZE * (my_pos < (tg.nglobaledges / FILE_CHUNKSIZE) % (nrows * ranks_per_row)) +
					(my_pos == last_pos ? edges_left : 0);
				/* fprintf(stderr, "%d: nedges = %" PRId64 " of %" PRId64 "\n", rank, (int64_t)nedges, (int64_t)tg.nglobaledges); */
				tg.edgememory_size = nedges;
				tg.edgememory = (packed_edge*)xmalloc(nedges * sizeof(packed_edge));
#ifdef SSSP
				tg.weightmemory = (float*)xmalloc(nedges*sizeof(float));
#endif
			}
			MPI_Offset block_idx;
			for (block_idx = 0; block_idx < block_limit; ++block_idx) {
				/* fprintf(stderr, "%d: On block %d of %d\n", rank, (int)block_idx, (int)block_limit); */
				MPI_Offset start_edge_index = int64_min(FILE_CHUNKSIZE * (block_idx * nrows + my_row), tg.nglobaledges);
				MPI_Offset edge_count = int64_min(tg.nglobaledges - start_edge_index, FILE_CHUNKSIZE);
				packed_edge* actual_buf = (!tg.data_in_file && block_idx % ranks_per_row == my_col) ?
					tg.edgememory + FILE_CHUNKSIZE * (block_idx / ranks_per_row) :
					buf;
#ifdef SSSP
				float* actual_wbuf = (!tg.data_in_file && block_idx % ranks_per_row == my_col) ?
					tg.weightmemory + FILE_CHUNKSIZE * (block_idx / ranks_per_row) :
					wbuf;
#endif
				/* fprintf(stderr, "%d: My range is [%" PRId64 ", %" PRId64 ") %swriting into index %" PRId64 "\n", rank, (int64_t)start_edge_index, (int64_t)(start_edge_index + edge_count), (my_col == (block_idx % ranks_per_row)) ? "" : "not ", (int64_t)(FILE_CHUNKSIZE * (block_idx / ranks_per_row))); */
				if (!tg.data_in_file && block_idx % ranks_per_row == my_col) {
					assert (FILE_CHUNKSIZE * (block_idx / ranks_per_row) + edge_count <= tg.edgememory_size);
				}
				if (tg.write_file) {
					generate_kronecker_range(seed, SCALE, start_edge_index, start_edge_index + edge_count, actual_buf
#ifdef SSSP
							,actual_wbuf
#endif
							);
					if (tg.data_in_file && my_col == (block_idx % ranks_per_row)) { /* Try to spread writes among ranks */
						MPI_File_write_at(tg.edgefile, start_edge_index, actual_buf, edge_count, packed_edge_mpi_type, MPI_STATUS_IGNORE);
#ifdef SSSP
						MPI_File_write_at(tg.weightfile, start_edge_index, actual_wbuf, edge_count, MPI_FLOAT, MPI_STATUS_IGNORE);
#endif
					}
				} else {
					/* All read rather than syncing up for a row broadcast. */
					MPI_File_read_at(tg.edgefile, start_edge_index, actual_buf, edge_count, packed_edge_mpi_type, MPI_STATUS_IGNORE);
#ifdef SSSP
					MPI_File_read_at(tg.weightfile, start_edge_index, actual_wbuf, edge_count, MPI_FLOAT, MPI_STATUS_IGNORE);
#endif
				}
			}
			free(buf);
			MPI_Comm_free(&this_col);
		} else {
			tg.edgememory = NULL;
			tg.edgememory_size = 0;
#ifdef SSSP
			tg.weightmemory = NULL;
#endif
		}
		MPI_Allreduce(&tg.edgememory_size, &tg.max_edgememory_size, 1, MPI_INT64_T, MPI_MAX, MPI_COMM_WORLD);
		if (tg.data_in_file && tg.write_file) {
			MPI_File_sync(tg.edgefile);
#ifdef SSSP
			MPI_File_sync(tg.weightfile);
#endif
		}
	}

	double make_graph_stop = MPI_Wtime();
	double make_graph_time = make_graph_stop - make_graph_start;
	if (rank == 0) { /* Not an official part of the results */
		fprintf(stderr, "graph_generation:               %f s\n", make_graph_time);
	}

	/* Make user's graph data structure. */
	double data_struct_start = MPI_Wtime();
	make_graph_data_structure(&tg);
	double data_struct_stop = MPI_Wtime();
	double data_struct_time = data_struct_stop - data_struct_start;
	if (rank == 0) { /* Not an official part of the results */
		fprintf(stderr, "construction_time:              %f s\n", data_struct_time);
	}

	//generate non-isolated roots
	{
		uint64_t counter = 0;
		int bfs_root_idx;
		for (bfs_root_idx = 0; bfs_root_idx < num_bfs_roots; ++bfs_root_idx) {
			int64_t root;
			while (1) {
				double d[2];
				make_random_numbers(2, seed1, seed2, counter, d);
				root = (int64_t)((d[0] + d[1]) * nglobalverts) % nglobalverts;
				counter += 2;
				if (counter > 2 * nglobalverts) break;
				int is_duplicate = 0;
				int i;
				for (i = 0; i < bfs_root_idx; ++i) {
					if (root == bfs_roots[i]) {
						is_duplicate = 1;
						break;
					}
				}
				if (is_duplicate) continue; /* Everyone takes the same path here */
				int root_bad = isisolated(root);
				MPI_Allreduce(MPI_IN_PLACE, &root_bad, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
				if (!root_bad) break;
			}
			bfs_roots[bfs_root_idx] = root;
		}
		num_bfs_roots = bfs_root_idx;


	}
	/* Number of edges visited in each BFS; a double so get_statistics can be
	 * used directly. */
	double* edge_counts = (double*)xmalloc(num_bfs_roots * sizeof(double));

	/* Run BFS. */
	int validation_passed = 1;
	double* bfs_times = (double*)xmalloc(num_bfs_roots * sizeof(double));
	double* validate_times = (double*)xmalloc(num_bfs_roots * sizeof(double));
	uint64_t nlocalverts = get_nlocalverts_for_pred();
	int64_t* pred = (int64_t*)xMPI_Alloc_mem(nlocalverts * sizeof(int64_t));
	float* shortest = (float*)xMPI_Alloc_mem(nlocalverts * sizeof(float));


	int bfs_root_idx,i;
	if (!getenv("SKIP_BFS")) {
		clean_pred(&pred[0]); //user-provided function from bfs_implementation.c
		run_bfs(bfs_roots[0], &pred[0]); //warm-up
#ifdef ENERGYLOOP_BFS
                int eloop;
                if(!my_pe()) printf("starting energy loop BFS\n");
                for(eloop=0;eloop<1000000;eloop++)
                        for (bfs_root_idx = 0; bfs_root_idx < num_bfs_roots; ++bfs_root_idx) {
                clean_pred(&pred[0]);
                run_bfs(bfs_roots[bfs_root_idx], &pred[0]);
                }
                if(!my_pe()) printf("finished energy loop BFS\n");
#endif
		if (!getenv("SKIP_VALIDATION")) {
			int64_t nedges=0;
			validate_result(1,&tg, nlocalverts, bfs_roots[0], pred,shortest,NULL);
		}

		for (bfs_root_idx = 0; bfs_root_idx < num_bfs_roots; ++bfs_root_idx) {
			int64_t root = bfs_roots[bfs_root_idx];

			if (rank == 0) fprintf(stderr, "Running BFS %d\n", bfs_root_idx);

			clean_pred(&pred[0]); //user-provided function from bfs_implementation.c
			/* Do the actual BFS. */
			double bfs_start = MPI_Wtime();
			run_bfs(root, &pred[0]);
			double bfs_stop = MPI_Wtime();
			bfs_times[bfs_root_idx] = bfs_stop - bfs_start;
			if (rank == 0) fprintf(stderr, "Time for BFS %d is %f\n", bfs_root_idx, bfs_times[bfs_root_idx]);
			int64_t edge_visit_count=0;
			get_edge_count_for_teps(&edge_visit_count);
			edge_counts[bfs_root_idx] = (double)edge_visit_count;
			if (rank == 0) fprintf(stderr, "TEPS for BFS %d is %g\n", bfs_root_idx, edge_visit_count / bfs_times[bfs_root_idx]);

			/* Validate result. */
			if (!getenv("SKIP_VALIDATION")) {
				if (rank == 0) fprintf(stderr, "Validating BFS %d\n", bfs_root_idx);

				double validate_start = MPI_Wtime();
				int validation_passed_one = validate_result(1,&tg, nlocalverts, root, pred,shortest,&edge_visit_count);
				double validate_stop = MPI_Wtime();

				validate_times[bfs_root_idx] = validate_stop - validate_start;
				if (rank == 0) fprintf(stderr, "Validate time for BFS %d is %f\n", bfs_root_idx, validate_times[bfs_root_idx]);

				if (!validation_passed_one) {
					validation_passed = 0;
					if (rank == 0) fprintf(stderr, "Validation failed for this BFS root; skipping rest.\n");
					break;
				}
			} else
				validate_times[bfs_root_idx] = -1;
		}

	}
#ifdef SSSP
	double* sssp_times = (double*)xmalloc(num_bfs_roots * sizeof(double));
	double* validate_times2 = (double*)xmalloc(num_bfs_roots * sizeof(double));

	clean_shortest(shortest);
	clean_pred(pred);
	run_sssp(bfs_roots[0], &pred[0],shortest); //warm-up
#ifdef ENERGYLOOP_SSSP
		int eloop;
		if(!my_pe()) printf("starting energy loop SSSP\n");
		for(eloop=0;eloop<1000000;eloop++)
			for (bfs_root_idx = 0; bfs_root_idx < num_bfs_roots; ++bfs_root_idx) {
				clean_shortest(shortest);
				clean_pred(&pred[0]);
				run_sssp(bfs_roots[bfs_root_idx], &pred[0],shortest);
		}
		if(!my_pe()) printf("finished energy loop SSSP\n");
#endif
	for (bfs_root_idx = 0; bfs_root_idx < num_bfs_roots; ++bfs_root_idx) {
		int64_t root = bfs_roots[bfs_root_idx];

		if (rank == 0) fprintf(stderr, "Running SSSP %d\n", bfs_root_idx);

		clean_pred(&pred[0]);
		clean_shortest(shortest);

		/* Do the actual SSSP. */
		double sssp_start = MPI_Wtime();
		run_sssp(root, &pred[0],shortest);
		double sssp_stop = MPI_Wtime();
		sssp_times[bfs_root_idx] = sssp_stop - sssp_start;
		int64_t edge_visit_count=0;
		get_edge_count_for_teps(&edge_visit_count);
		edge_counts[bfs_root_idx] = (double)edge_visit_count;
		if (rank == 0) fprintf(stderr, "Time for SSSP %d is %f\n", bfs_root_idx, sssp_times[bfs_root_idx]);
		if (rank == 0) fprintf(stderr, "TEPS for SSSP %d is %g\n", bfs_root_idx, edge_counts[bfs_root_idx] / sssp_times[bfs_root_idx]);

		/* Validate result. */
		if (!getenv("SKIP_VALIDATION")) {
			if (rank == 0) fprintf(stderr, "Validating SSSP %d\n", bfs_root_idx);

			double validate_start = MPI_Wtime();
			int validation_passed_one = validate_result(0,&tg, nlocalverts, root, pred, shortest,&edge_visit_count);
			double validate_stop = MPI_Wtime();

			validate_times2[bfs_root_idx] = validate_stop - validate_start;
			if (rank == 0) fprintf(stderr, "Validate time for SSSP %d is %f\n", bfs_root_idx, validate_times2[bfs_root_idx]);

			if (!validation_passed_one) {
				validation_passed = 0;
				if (rank == 0) fprintf(stderr, "Validation failed for this SSSP root; skipping rest.\n");
				break;
			}
		} else {
			validate_times2[bfs_root_idx] = -1;
		}
	}

#endif
	MPI_Free_mem(pred);
#ifdef SSSP
	MPI_Free_mem(shortest);
#endif
	free(bfs_roots);
	free_graph_data_structure();

	if (tg.data_in_file) {
		MPI_File_close(&tg.edgefile);
#ifdef SSSP
		MPI_File_close(&tg.weightfile);
#endif    
	} else {
		free(tg.edgememory); tg.edgememory = NULL;
#ifdef SSSP
		free(tg.weightmemory); tg.weightmemory = NULL;
#endif
	}

	/* Print results. */
	if (rank == 0) {
		if (!validation_passed) {
			fprintf(stdout, "No results printed for invalid run.\n");
		} else {
			int i;
			//for (i = 0; i < num_bfs_roots; ++i) printf(" %g \n",edge_counts[i]);
			fprintf(stdout, "SCALE:                          %d\n", SCALE);
			fprintf(stdout, "edgefactor:                     %d\n", edgefactor);
			fprintf(stdout, "NBFS:                           %d\n", num_bfs_roots);
			fprintf(stdout, "graph_generation:               %g\n", make_graph_time);
			fprintf(stdout, "num_mpi_processes:              %d\n", size);
			fprintf(stdout, "construction_time:              %g\n", data_struct_time);
			volatile double stats[s_LAST];
			get_statistics(bfs_times, num_bfs_roots, stats);
			fprintf(stdout, "bfs  min_time:                  %g\n", stats[s_minimum]);
			fprintf(stdout, "bfs  firstquartile_time:        %g\n", stats[s_firstquartile]);
			fprintf(stdout, "bfs  median_time:               %g\n", stats[s_median]);
			fprintf(stdout, "bfs  thirdquartile_time:        %g\n", stats[s_thirdquartile]);
			fprintf(stdout, "bfs  max_time:                  %g\n", stats[s_maximum]);
			fprintf(stdout, "bfs  mean_time:                 %g\n", stats[s_mean]);
			fprintf(stdout, "bfs  stddev_time:               %g\n", stats[s_std]);
#ifdef SSSP
			get_statistics(sssp_times, num_bfs_roots, stats);
			fprintf(stdout, "sssp min_time:                  %g\n", stats[s_minimum]);
			fprintf(stdout, "sssp firstquartile_time:        %g\n", stats[s_firstquartile]);
			fprintf(stdout, "sssp median_time:               %g\n", stats[s_median]);
			fprintf(stdout, "sssp thirdquartile_time:        %g\n", stats[s_thirdquartile]);
			fprintf(stdout, "sssp max_time:                  %g\n", stats[s_maximum]);
			fprintf(stdout, "sssp mean_time:                 %g\n", stats[s_mean]);
			fprintf(stdout, "sssp stddev_time:               %g\n", stats[s_std]);
#endif
			get_statistics(edge_counts, num_bfs_roots, stats);
			fprintf(stdout, "min_nedge:                      %.11g\n", stats[s_minimum]);
			fprintf(stdout, "firstquartile_nedge:            %.11g\n", stats[s_firstquartile]);
			fprintf(stdout, "median_nedge:                   %.11g\n", stats[s_median]);
			fprintf(stdout, "thirdquartile_nedge:            %.11g\n", stats[s_thirdquartile]);
			fprintf(stdout, "max_nedge:                      %.11g\n", stats[s_maximum]);
			fprintf(stdout, "mean_nedge:                     %.11g\n", stats[s_mean]);
			fprintf(stdout, "stddev_nedge:                   %.11g\n", stats[s_std]);
			double* secs_per_edge = (double*)xmalloc(num_bfs_roots * sizeof(double));
			for (i = 0; i < num_bfs_roots; ++i) secs_per_edge[i] = bfs_times[i] / edge_counts[i];
			get_statistics(secs_per_edge, num_bfs_roots, stats);
			fprintf(stdout, "bfs  min_TEPS:                  %g\n", 1. / stats[s_maximum]);
			fprintf(stdout, "bfs  firstquartile_TEPS:        %g\n", 1. / stats[s_thirdquartile]);
			fprintf(stdout, "bfs  median_TEPS:               %g\n", 1. / stats[s_median]);
			fprintf(stdout, "bfs  thirdquartile_TEPS:        %g\n", 1. / stats[s_firstquartile]);
			fprintf(stdout, "bfs  max_TEPS:                  %g\n", 1. / stats[s_minimum]);
			fprintf(stdout, "bfs  harmonic_mean_TEPS:     !  %g\n", 1. / stats[s_mean]);
			/* Formula from:
			 * Title: The Standard Errors of the Geometric and Harmonic Means and
			 *        Their Application to Index Numbers
			 * Author(s): Nilan Norris
			 * Source: The Annals of Mathematical Statistics, Vol. 11, No. 4 (Dec., 1940), pp. 445-448
			 * Publisher(s): Institute of Mathematical Statistics
			 * Stable URL: http://www.jstor.org/stable/2235723
			 * (same source as in specification). */
			fprintf(stdout, "bfs  harmonic_stddev_TEPS:      %g\n", stats[s_std] / (stats[s_mean] * stats[s_mean] * sqrt(num_bfs_roots - 1)));
#ifdef SSSP
			for (i = 0; i < num_bfs_roots; ++i) secs_per_edge[i] = sssp_times[i] / edge_counts[i];
			get_statistics(secs_per_edge, num_bfs_roots, stats);
			fprintf(stdout, "sssp min_TEPS:                  %g\n", 1. / stats[s_maximum]);
			fprintf(stdout, "sssp firstquartile_TEPS:        %g\n", 1. / stats[s_thirdquartile]);
			fprintf(stdout, "sssp median_TEPS:               %g\n", 1. / stats[s_median]);
			fprintf(stdout, "sssp thirdquartile_TEPS:        %g\n", 1. / stats[s_firstquartile]);
			fprintf(stdout, "sssp max_TEPS:                  %g\n", 1. / stats[s_minimum]);
			fprintf(stdout, "sssp harmonic_mean_TEPS:     !  %g\n", 1. / stats[s_mean]);
			fprintf(stdout, "sssp harmonic_stddev_TEPS:      %g\n", stats[s_std] / (stats[s_mean] * stats[s_mean] * sqrt(num_bfs_roots - 1)));
#endif
			free(secs_per_edge); secs_per_edge = NULL;
			free(edge_counts); edge_counts = NULL;
			get_statistics(validate_times, num_bfs_roots, stats);
			fprintf(stdout, "bfs  min_validate:              %g\n", stats[s_minimum]);
			fprintf(stdout, "bfs  firstquartile_validate:    %g\n", stats[s_firstquartile]);
			fprintf(stdout, "bfs  median_validate:           %g\n", stats[s_median]);
			fprintf(stdout, "bfs  thirdquartile_validate:    %g\n", stats[s_thirdquartile]);
			fprintf(stdout, "bfs  max_validate:              %g\n", stats[s_maximum]);
			fprintf(stdout, "bfs  mean_validate:             %g\n", stats[s_mean]);
			fprintf(stdout, "bfs  stddev_validate:           %g\n", stats[s_std]);
#ifdef SSSP
			get_statistics(validate_times2, num_bfs_roots, stats);
			fprintf(stdout, "sssp min_validate:              %g\n", stats[s_minimum]);
			fprintf(stdout, "sssp firstquartile_validate:    %g\n", stats[s_firstquartile]);
			fprintf(stdout, "sssp median_validate:           %g\n", stats[s_median]);
			fprintf(stdout, "sssp thirdquartile_validate:    %g\n", stats[s_thirdquartile]);
			fprintf(stdout, "sssp max_validate:              %g\n", stats[s_maximum]);
			fprintf(stdout, "sssp mean_validate:             %g\n", stats[s_mean]);
			fprintf(stdout, "sssp stddev_validate:           %g\n", stats[s_std]);
#endif
#if 0
			for (i = 0; i < num_bfs_roots; ++i) {
				fprintf(stdout, "Run %3d:                        %g s, validation %g s\n", i + 1, bfs_times[i], validate_times[i]);
				fprintf(stdout, "Run %3d:                        %g s, validation %g s\n", i + 1, sssp_times[i], validate_times2[i]);
			}
#endif


		}
	}
	free(bfs_times);
	free(validate_times);
#ifdef SSSP
	free(sssp_times);
	free(validate_times2);
#endif
	cleanup_globals();
	aml_finalize(); //includes MPI_Finalize()
	return 0;
}
