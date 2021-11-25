#include <stdio.h>
#include <stdlib.h>

#include "occucal.h"

int main(int argc, char **argv)
{
#if(1)
  //150508  if (argc < 5) {
    //150508    fprintf(stderr, "Usage %s (compute capability e.g. 12 if 1.2)(threads per block)(registers per thread)(shared memory per block in bytes)\n", argv[0]);
  if (argc < 6) {
    fprintf(stderr, "Usage %s (compute capability *)(shared memory size config in bytes **)(threads per block)(registers per thread)(shared memory per block in bytes)\n", argv[0]);
    fprintf(stderr, " * Set '1.2' if your compute cabpability is 1.2, for example.\n");
    fprintf(stderr, "** Set 49152, 16384, or 32768.\n");
    exit(0);
  };
  const int MySMVersion = getSMversion(atoi(argv[1]));
  //150508  const int MyThreadsCount = atoi(argv[2]);
  //150508  const int MyRegCount = atoi(argv[3]);
  //150508  const int MySharedMemory = atoi(argv[4]);
  const int sharedSize = atoi(argv[2]);
  const int MyThreadsCount = atoi(argv[3]);
  const int MyRegCount = atoi(argv[4]);
  const int MySharedMemory = atoi(argv[5]);
#else
  const int MySMVersion = sm_35;
  const int sharedSize = 49152;
  const int MyThreadsCount = 256;
  const int MyRegCount = 32;
  const int MySharedMemory = 4096;
#endif

  int ActiveWarpsPerSM;
  double Occupancy, UtilRegsPerSM, UtilSmemPerSM;
  //150508  occucal(MySMVersion, MyThreadsCount, MyRegCount, MySharedMemory, &ActiveWarpsPerSM, &Occupancy, &UtilRegsPerSM, &UtilSmemPerSM);
  occucal(MySMVersion, sharedSize, MyThreadsCount, MyRegCount, MySharedMemory, &ActiveWarpsPerSM, &Occupancy, &UtilRegsPerSM, &UtilSmemPerSM);
  fprintf(stdout, "%d %4.1f %4.1f %4.1f\n", ActiveWarpsPerSM, Occupancy, UtilRegsPerSM, UtilSmemPerSM);
  return 0;
}
