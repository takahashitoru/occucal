#ifndef OCCUCAL_H
#define OCCUCAL_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "gpu.h"

#define MAX(x, y) ( (x) > (y) ? (x) : (y) )
#define MIN(x, y) ( (x) < (y) ? (x) : (y) )

#include "ceiling.h"
#include "floor.h"


static int getSMversion(const int n)
{
  if (n == 10) {
    return sm_10;
  } else if (n == 11) {
    return sm_11;
  } else if (n == 12) {
    return sm_12;
  } else if (n == 13) {
    return sm_13;
  } else if (n == 20) {
    return sm_20;
  } else if (n == 21) {
    return sm_21;
  } else if (n == 30) {
    return sm_30;
  } else if (n == 35) {
    return sm_35;
  } else {
    fprintf(stderr, "## Invalid version number. Exit.\n");
    exit(EXIT_FAILURE);
  }
}


static void occucal(const int cc, // Compute Capability
		    const int sharedSize,
		    const int MyThreadsCount, // Threads Per Block
		    const int MyRegCount, // Registers Per Thread
		    const int MySharedMemory, // Shared Memory Per Block (bytes)
		    int *ActiveWarpsPerSM, // Number of active warps per multiprocessor
		    double *Occupancy, // Occupancy of warps per multiprocessor (%)
		    double *UtilRegsPerSM, // Utilization of registers per multiprocessor
		    double *UtilSmemPerSM) // Utilization of shared memory per multiprocessor
{
#ifndef QUIET
  fprintf(stderr, "# CUDA occupancy calculator in C\n");
  //150508  fprintf(stderr, "#(1) Selected your parameters\n");
  //150508  fprintf(stderr, "# %s: %2.1f\n", "Compute Capability", Compute_Capability[cc]);
  //150508  fprintf(stderr, "# %s: %d\n", "Shared Memory Size Config (bytes)", Max_Shared_Memory_per_Multiprocessor[cc]);
  fprintf(stderr, "#(1) Selected your Compute Capability: %2.1f\n", Compute_Capability[cc]);
  fprintf(stderr, "#(1b) Selected your Shared Memory Size Config (bytes): %d\n", MIN(sharedSize, Max_Shared_Memory_per_Multiprocessor[cc]));
#endif

#ifndef QUIET
  fprintf(stderr, "#(2) Entered your resource usage\n");
  fprintf(stderr, "# %s: %d\n", "Threads Per Block", MyThreadsCount);
  fprintf(stderr, "# %s: %d\n", "Registers Per Thread", MyRegCount);
  fprintf(stderr, "# %s: %d\n", "Shared Memory Per Block (bytes)", MySharedMemory);
#endif

  // Physical Limits for GPU Compute Capability

  const int limitThreadsPerWarp = Threads_per_Warp[cc];
  const int limitWarpsPerMultiprocessor	= Warps_per_Multiprocessor[cc];
  const int limitThreadsPerMultiprocessor = Threads_per_Multiprocessor[cc];
  const int limitBlocksPerMultiprocessor = Thread_Blocks_per_Multiprocessor[cc];
  const int limitTotalRegisters = Register_File_Size[cc];
  const int myAllocationSize = Register_Allocation_Unit_Size[cc];
  const int myAllocationGranularity = Allocation_Granularity[cc];
  const int limitRegsPerThread = Max_Registers_per_Thread[cc];
  //150508  const int limitTotalSharedMemory = Max_Shared_Memory_per_Multiprocessor[cc];
  const int limitTotalSharedMemory = MIN(sharedSize, Max_Shared_Memory_per_Multiprocessor[cc]);
  const int mySharedMemAllocationSize = Shared_Memory_Allocation_Unit_Size[cc];
  const int myWarpAllocationGranularity = Warp_allocation_granularity[cc];
  const int maxThreadsPerBlock = Max_Thread_Block_Size[cc];

#ifndef QUIET
  //150508  fprintf(stderr, "#(*) Physical Limits for GPU Compute Capability\n");
  fprintf(stderr, "#(*) Physical Limits for GPU Compute Capability: %2.1f\n", Compute_Capability[cc]);
  fprintf(stderr, "# %s: %d\n", "Threads per Warp", limitThreadsPerWarp);
  fprintf(stderr, "# %s: %d\n", "Warps per Multiprocessor", limitWarpsPerMultiprocessor);
  fprintf(stderr, "# %s: %d\n", "Threads per Multiprocessor", limitThreadsPerMultiprocessor);
  fprintf(stderr, "# %s: %d\n", "Thread Blocks per Multiprocessor", limitBlocksPerMultiprocessor);
  fprintf(stderr, "# %s: %d\n", "Total # of 32-bit registers per Multiprocessor", limitTotalRegisters);
  fprintf(stderr, "# %s: %d\n", "Register allocation unit size", myAllocationSize);
  fprintf(stderr, "# %s: %s\n", "Register allocation granularity", myAllocationGranularity == BLOCK ? "block" : "warp");
  fprintf(stderr, "# %s: %d\n", "Registers per Thread", limitRegsPerThread);
  fprintf(stderr, "# %s: %d\n", "Shared Memory per Multiprocessor (bytes)", limitTotalSharedMemory);
  fprintf(stderr, "# %s: %d\n", "Shared Memory Allocation unit size", mySharedMemAllocationSize);
  fprintf(stderr, "# %s: %d\n", "Warp allocation granularity", myWarpAllocationGranularity);
  fprintf(stderr, "# %s: %d\n", "Maximum Thread Block Size", maxThreadsPerBlock);
#endif

  /*
    Allocated Resources
  */

  // Warps; Per Block
  const int MyWarpsPerBlock = CEILING((double)MyThreadsCount / limitThreadsPerWarp, 1, 1);
  // Warps; Limit Per SM
  // nothing
  // Warps; Allocatable Blocks Per SM
  const int limitBlocksDueToWarps = MIN(limitBlocksPerMultiprocessor, FLOOR(limitWarpsPerMultiprocessor / MyWarpsPerBlock, 1, 1));
  // Registers; Per Block
  int MyRegsPerBlock;
  if (myAllocationGranularity == BLOCK) {
    MyRegsPerBlock = CEILING(CEILING(MyWarpsPerBlock, myWarpAllocationGranularity, 1) * MyRegCount * limitThreadsPerWarp, myAllocationSize, 1);
  } else {
    MyRegsPerBlock = MyWarpsPerBlock;
  }
  // Registers; Limit Per SM
  int C39; // Warps Limit Per SM
  if (myAllocationGranularity == BLOCK) {
    C39 = limitTotalRegisters;
  } else {
    C39 = FLOOR((double)limitTotalRegisters / CEILING(MyRegCount * limitThreadsPerWarp, myAllocationSize, 1), myWarpAllocationGranularity, 1);
  }
  // Warps; Allocatable Blocks Per SM
  int limitBlocksDueToRegs;
  if (MyRegCount > limitRegsPerThread) {
    limitBlocksDueToRegs = 0;
  } else if (MyRegCount > 0) {
    limitBlocksDueToRegs = FLOOR((double)C39 / MyRegsPerBlock, 1, 1);
  } else {
    limitBlocksDueToRegs = limitBlocksPerMultiprocessor;
  }
  // Shared Memory; Per Block
  const int MySharedMemPerBlock = CEILING(MySharedMemory, mySharedMemAllocationSize, 1);
  // Shared Memory; Liimit Per SM
  // nothing
  // Shared Memory; Allocatable Blocks Per SM
  int limitBlocksDueToSMem;
  if (MySharedMemPerBlock > 0) {
    limitBlocksDueToSMem = FLOOR((double) limitTotalSharedMemory / MySharedMemPerBlock, 1, 1);
  } else {
    limitBlocksDueToSMem = limitBlocksPerMultiprocessor;
  }

#ifndef QUIET
  //  fprintf(stderr, "#(*) Allocated Resources\n");
  //  fprintf(stderr, "# %s: %d %d %d\n", "Warps", MyWarpsPerBlock, limitWarpsPerMultiprocessor, limitBlocksDueToWarps);
  //  fprintf(stderr, "# %s: %d %d %d\n", "Registers", MyRegsPerBlock, C39, limitBlocksDueToRegs);
  //  fprintf(stderr, "# %s: %d %d %d\n", "Shared Memory", MySharedMemPerBlock, limitTotalSharedMemory, limitBlocksDueToSMem);
  fprintf(stderr, "#(*) Allocatable thread blocks per multiprocessor\n");
  fprintf(stderr, "# %s (%d %d): %d\n", "due to Warps", MyWarpsPerBlock, limitWarpsPerMultiprocessor, limitBlocksDueToWarps);
  fprintf(stderr, "# %s (%d %d): %d\n", "due to Registers", MyRegsPerBlock, C39, limitBlocksDueToRegs);
  fprintf(stderr, "# %s (%d %d): %d\n", "due to Shared Memory", MySharedMemPerBlock, limitTotalSharedMemory, limitBlocksDueToSMem);
#endif


  /*
    Maximum Thread Blocks Per Multiprocessor
  */

  // Limited by Max Blocks per Multiprocessor; Blocks/SM
  const int B44 = limitBlocksDueToWarps;
  // Limited by Resisters per Multiprocessor; Blocks/SM
  const int B45 = limitBlocksDueToRegs;
  // Limited by Shared Memory per Multiprocessor; Blocks/SM
  const int B46 = limitBlocksDueToSMem;
  ////////////////////////////////////////////////////////////
  //  fprintf(stderr, "B44=%d B45=%d B46=%d\n", B44, B45, B46);
  ////////////////////////////////////////////////////////////

  // Active Thread Blocks per Multiprocessor
  const int ActiveThreadBlocksPerMultiprocessor = MIN(B44, MIN(B45, B46));

  // Limited by Max Warps per Multiprocessor; Warps/Block
  const int C44 = MyWarpsPerBlock;
  // Limited by Resisters per Multiprocessor; Warps/Block
  const int C45 = MyWarpsPerBlock;
  // Limited by Shared Memory per Multiprocessor; Warps/Block
  const int C46 = MyWarpsPerBlock;
  ////////////////////////////////////////////////////////////
  //  fprintf(stderr, "C44=%d C45=%d C46=%d\n", C44, C45, C46);
  ////////////////////////////////////////////////////////////

  // Limited by Max Warps or Max Blocks per Multiprocessor; Warps/SM
  const int D44 = (B44 == ActiveThreadBlocksPerMultiprocessor ? B44 * C44 : 0);
  // Limited by Resisters per Multiprocessor; Warps/SM
  const int D45 = (B45 == ActiveThreadBlocksPerMultiprocessor ? B45 * C45 : 0);
  // Limited by Shared Memory per Multiprocessor; Warps/SM
  const int D46 = (B46 == ActiveThreadBlocksPerMultiprocessor ? B46 * C46 : 0);
  ////////////////////////////////////////////////////////////
  //  fprintf(stderr, "D44=%d D45=%d D46=%d\n", D44, D45, D46);
  ////////////////////////////////////////////////////////////

#ifndef QUIET
  fprintf(stderr, "#(*) Maximum Thread Blocks Per Multiprocessor\n");
  fprintf(stderr, "# %s (%d %d): %d\n", "Limited by Max Warps or Max Blocks per Multiprocessor", B44, C44, D44);
  fprintf(stderr, "# %s (%d %d): %d\n", "Limited by Registers per Multiprocessor", B45, C45, D45);
  fprintf(stderr, "# %s (%d %d): %d\n", "Limited by Shared Memory per Multiprocessor", B46, C46, D46);
#endif

  /*
    GPU Occupancy Data
  */

  // Active Thread Blocks per Multiprocessor
  const int B19 = ActiveThreadBlocksPerMultiprocessor;
  // Active Warps per Multiprocessor
  const int B18 = B19 * MyWarpsPerBlock;
  // Active Threads per Multiprocessor
  const int ActiveThreadsPerMultiprocessor = B18 * limitThreadsPerWarp;
  // Occupancy of each Multprocessor
  const double B20 = (double)B18 / limitWarpsPerMultiprocessor;

  // Acitve Warps per Multiprocessor
  //  const int ActiveWarpsPerMultiprocessor = MAX(D44, MAX(D45, D46)); // =ActiveWarpsPerSM
  *ActiveWarpsPerSM = MAX(D44, MAX(D45, D46));
  if (*ActiveWarpsPerSM != B18) {
    fprintf(stderr, "Something wrong. Exit.\n");
    exit(EXIT_FAILURE);
  }

  //Occupancy of each Multiprocessor (uint: %)
  *Occupancy = round(B20 * 100); // this must be the same as ActiveWaprsPerSM/limitWarpsPerMultiprocessor

#ifndef QUIET
  fprintf(stderr, "#(3) GPU Occupancy Data is displayed here\n");
  fprintf(stderr, "# %s: %d\n", "Active Threads per Multiprocessor", ActiveThreadsPerMultiprocessor);
  fprintf(stderr, "# %s: %d\n", "Active Warps per Multiprocessor", B18);
  fprintf(stderr, "# %s: %d\n", "Active Thread Blocks per Multiprocessor", ActiveThreadBlocksPerMultiprocessor);
  fprintf(stderr, "# %s: %4.1f\%\n", "Occupancy of each Multiprocessor", *Occupancy);
#endif

  /*
    Utilization of registers and shared memory per multiprocessor
  */

  *UtilRegsPerSM = (double)ActiveThreadsPerMultiprocessor * MyRegCount / limitTotalRegisters * 100;
  *UtilSmemPerSM = (double)ActiveThreadBlocksPerMultiprocessor * MySharedMemory / limitTotalSharedMemory * 100;
  
#ifndef QUIET
  fprintf(stderr, "#(*) Utilization of registers and shared memory per multiprocessor\n");
  fprintf(stderr, "# %s: %4.1f\%\n", "Register", *UtilRegsPerSM);
  fprintf(stderr, "# %s: %4.1f\%\n", "Shared memory", *UtilSmemPerSM);
#endif
}

#endif /* OCCUCAL_H */
