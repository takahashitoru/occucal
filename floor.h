#ifndef FLOOR_H
#define FLOOR_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*
  Rounds a number down to the nearest multiple of Significance.

  Syntax
  FLOOR(Number; Significance; Mode)

  Number is the number that is to be rounded down.

  Significance is the value to whose multiple the number is to be
  rounded down.

  Mode is an optional value. If the Mode value is given and not equal
  to zero, and if Number and Significance are negative, then rounding
  is done based on the absolute value of the number. This parameter is
  ignored when exporting to MS Excel as Excel does not know any third
  parameter.

  Example
  =FLOOR( -11;-2) returns -12
  =FLOOR( -11;-2;0) returns -12
  =FLOOR( -11;-2;1) returns -10

  Cited from OpenOffice's Help
*/


static int FLOOR(const int num, const int sig, const int mode) // mode is ignored
{
  int n = num;
  while (n % sig != 0) {
    n --;
  }
  return n;
}

#endif /* FLOOR_H */
