#ifndef CEILING_H
#define CEILING_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*
  Rounds a number up to the nearest multiple of Significance.

  Syntax
  CEILING(Number; Significance; Mode)

  Number is the number that is to be rounded up.

  Significance is the number to whose multiple the value is to be
  rounded up. 

  Mode is an optional value. If the Mode value is given and not equal
  to zero, and if Number and Significance are negative, then rounding
  is done based on the absolute value of Number. This parameter is
  ignored when exporting to MS Excel as Excel does not know any third
  parameter.

  Example
  =CEILING(-11;-2) returns -10
  =CEILING(-11;-2;0) returns -10
  =CEILING(-11;-2;1) returns -12

  Cited from OpenOffice's Help

*/


static int CEILING(const int num, const int sig, const int mode) // mode is ignored
{
  int n = num;
  while (n % sig != 0) {
    n ++;
  }
  return n;
}

#endif /* CEILING_H */
