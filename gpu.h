#ifndef GPU_H
#define GPU_H

#include <stdio.h>
#include <stdlib.h>

/* The data included in this file is from the CUDA_Occupancy_calculator.xls */

#define BLOCK 0
#define WARP 1

double        Compute_Capability[]={		1.0,	1.1,	1.2,	1.3,	2.0,	2.1,	3.0,	3.5};
enum                SM_Version {		sm_10,	sm_11,	sm_12,	sm_13,	sm_20,	sm_21,	sm_30,	sm_35};
int         Threads_per_Warp[]={		32,	32,	32,	32,	32,	32,	32,	32};
int         Warps_per_Multiprocessor[]={	24,	24,	32,	32,	48,	48,	64,	64};
int       Threads_per_Multiprocessor[]={	768,	768,	1024,	1024,	1536,	1536,	2048,	2048};
int Thread_Blocks_per_Multiprocessor[]={	8,	8,	8,	8,	8,	8,	16,	16};
int Max_Shared_Memory_per_Multiprocessor[]={	16384,	16384,	16384,	16384,	49152,	49152,	49152,	49152};
int       Register_File_Size[]={		8192,	8192,	16384,	16384,	32768,	32768,	65536,	65536};
int Register_Allocation_Unit_Size[]={		256,	256,	512,	512,	64,	64,	256,	256};
int        Allocation_Granularity[]={           BLOCK,	BLOCK,	BLOCK,	BLOCK,	WARP,	WARP,	WARP,	WARP};
int Max_Registers_per_Thread[]={		124,	124,	124,	124,	63,	63,	63,	255};
int Shared_Memory_Allocation_Unit_Size[]={	512,	512,	512,	512,	128,	128,	256,	256};
int Warp_allocation_granularity[]={		2,	2,	2,	2,	2,	2,	4,	4};
int    Max_Thread_Block_Size[]={		512,	512,	512,	512,	1024,	1024,	1024,	1024};
int Shared_Memory_Size_Configurations[]={	16384,	16384,	16384,	16384,	49152,	49152,	49152,	49152};
//[note: default at top of list]						16384	16384	16384	16384
//                              								32768	32768
//									
//Warp register allocation granularities					64	64	256	256
//[note: default at top of list]                         			128	128		

#endif /* GPU_H */
