# What's this?

This software "occucal" is a C implementation of NVIDIA's "CUDA Occupancy Calculator". Most of codes and data in occucal are referenced from the NVIDIA's original Excel spreadsheet, that is, CUDA_occupancy_calculator.xls (Version 5.1).

# Compile

> gcc -std=c99 -o occucal main.c -lm

# Run

> ./occucal (compute capability^1) (shared memory size config in bytes^2) (threads per block) (registers per thread) (shared memory per block in bytes)

 ^1 Set '12' if your compute cabpability is 1.2, for example.

 ^2 Set '49152', '16384', or '32768'. [NOTE: This parameter has been newly added.]

# Output 

- STDOUT: "number of active warps per SM", "occupancy of warps per SM [%]", "utilisation of registers per SM [%]" and "utilisation of shared memory per SM [%]". 
- STDERR: The information that the original spreadsheet provides except for the graphs. The message can be suppressed by giving QUIET option. 

# Example

> ./occucal 20 16384 64 32 2048  <-- Run

 CUDA occupancy calculator in C  <-- Output to STDERR
(1) Selected your Compute Capability: 2.0
(1b) Selected your Shared Memory Size Config (bytes): 16384
(2) Entered your resource usage
 Threads Per Block: 64
 Registers Per Thread: 32
 Shared Memory Per Block (bytes): 2048
(*) Physical Limits for GPU Compute Capability
 Threads per Warp: 32
 Warps per Multiprocessor: 48
 Threads per Multiprocessor: 1536
 Thread Blocks per Multiprocessor: 8
 Total # of 32-bit registers per Multiprocessor: 32768
 Register allocation unit size: 64
 Register allocation granularity: warp
 Registers per Thread: 63
 Shared Memory per Multiprocessor (bytes): 16384
 Shared Memory Allocation unit size: 128
 Warp allocation granularity: 2
 Maximum Thread Block Size: 1024
(*) Allocatable thread blocks per multiprocessor
 due to Warps (2 48): 8
 due to Registers (2 32): 16
 due to Shared Memory (2048 16384): 8
(*) Maximum Thread Blocks Per Multiprocessor
 Limited by Max Warps or Max Blocks per Multiprocessor (8 2): 16
 Limited by Registers per Multiprocessor (16 2): 0
 Limited by Shared Memory per Multiprocessor (8 2): 16
(3) GPU Occupancy Data is displayed here
 Active Threads per Multiprocessor: 512
 Active Warps per Multiprocessor: 16
 Active Thread Blocks per Multiprocessor: 8
 Occupancy of each Multiprocessor: 33.0%
(*) Utilization of registers and shared memory per multiprocessor
 Register: 50.0%
 Shared memory: 100.0%

 16 33.0 50.0 100.0  <--- Output to STDOUT

# Remark

This software comes with no warranty.
