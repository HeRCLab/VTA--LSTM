/* MIT License

Copyright (C) IBM Corporation 2018

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Note: This code is written based on VTA (Versatile Tensor Accelerator) overlay to demonstrate the implementation of a three-cells LSTM network
Written by: Mehdi Yaghouti
Email     : MehdiYaghouti@gmail.com  */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <vta/driver.h>
#include <pynq_driver.h>

#include "lstm.h"

int main(void) {

  uint32_t iter=30;
  uint64_t timings[iter];
  uint64_t time;
  for(uint32_t k=0;k<=iter;k++){
      time = lstm_test(1,0);
      if(k>0){
      timings[k-1]=time;
      printf(" - Try number (%d), Running time: %.3f ns \n", k, static_cast<float>(timings[k-1]) );}
     
  printf("=====================================================================================================\n");
  }
  printf("+++++++++++++++++++++++++++++++++++++++ Running time Statistics +++++++++++++++++++++++++++++++++++++\n");
  float min =timings[0];
  float max =timings[0];
  float avg =0;
  for(uint32_t k=0;k<iter;k++)
  {
     avg+=timings[k];
     if (timings[k]<min) min=timings[k];
     if (timings[k]>max) max=timings[k];
  }
  avg/=iter;
  printf("min=%.3f  avg=%.3f  max=%.3f\n",min,avg,max);


 

 
  return 0;
}
