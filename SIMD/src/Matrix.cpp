// -*- C++ -*-
/*
==================================================
Authors: A. Mithran; R.Lakos; I. Kulakov; M. Zyzak
==================================================
*/

/// use "g++ -O3 -fno-tree-vectorize -msse Matrix.cpp && ./a.out" to run
// Finish SIMDized version. Compare results and time.

#include <stdlib.h>
#include <iostream>

#include "fvec/P4_F32vec4.h"
#include "utils/TStopWatch.h"

const int N = 1000;     // matrix size. Has to be dividable by 4.

const int NIter = 100;  // repeat calculations many times in order to neglect
                        // memory reading time

float a[N][N];          // input array
float c[N][N];          // output array for scalar computations
float c_simd[N][N];     // output array for SIMD computations

template <typename T>   // required calculations
T f(T x) {
  return sqrt(x);
}

void CheckResults(const float a1[N][N], const float a2[N][N]) 
{
  bool ok = true;
  for (int i = 0; i < N; i++)
    for (int j = 0; j < N; j++)
      if (std::fabs(a1[i][j] - a2[i][j]) > 1.e-8) ok = false;

      
  if (ok)
    std::cout << "SIMD and scalar results are the same." << std::endl;
  else
    std::cout << "ERROR! SIMD and scalar results are not the same."
              << std::endl;
}

int main() 
{
  // initialize input array with random values
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      a[i][j] =
          float(rand()) / float(RAND_MAX);
    }
  }

  // Scalar version
  TStopwatch timerScalar;
  for (int ii = 0; ii < NIter; ii++)
    for (int i = 0; i < N; i++) {
      for (int j = 0; j < N; j++) {
        c[i][j] = f(a[i][j]);
      }
    }
  timerScalar.Stop();

  // SIMD version
  TStopwatch timerSIMD;
  for (int ii = 0; ii < NIter; ii++) {
        for (int i = 0; i < N; i++) {
            int j = 0;
            for (; j <= N - 4; j += 4) {
                F32vec4 va = *reinterpret_cast<F32vec4*>(&a[i][j]);
                F32vec4 vc = sqrt(va); 
                *reinterpret_cast<F32vec4*>(&c_simd[i][j]) = vc; 
            }
        }
    }
  timerSIMD.Stop();

  double tScal = timerScalar.RealTime() * 1000;
  double tSIMD1 = timerSIMD.RealTime() * 1000;

  std::cout << "Time scalar: " << tScal << " ms " << std::endl;
  std::cout << "Time SIMD:   " << tSIMD1 << " ms, speed up " << tScal / tSIMD1
            << std::endl;

  CheckResults(c, c_simd);

  return 0;
}
