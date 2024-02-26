/* MIT License

Copyright (C) IBM Corporation 2018

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Note: This code is written based on VTA (Versatile Tensor Accelerator) overlay to demonstrate the implementation of a three-cells LSTM network
Written by: Mehdi Yaghouti
Email     : MehdiYaghouti@gmail.com  */

#ifndef TESTS_HARDWARE_COMMON_TEST_LIB_H_
#define TESTS_HARDWARE_COMMON_TEST_LIB_H_
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <vta/hw_spec.h>
#include <vta/driver.h>
#include <pynq_driver.h>


typedef uint32_t uop_T;
typedef int8_t wgt_T;
typedef int8_t inp_T;
typedef int8_t out_T;
typedef int32_t acc_T;





uint64_t vta(
  uint32_t insn_count,
  VTAGenericInsn *insns,
  VTAUop *uops,
  inp_T *inputs,
  wgt_T *weights,
  acc_T *biases,
  out_T *outputs);





template <typename T, int T_WIDTH>
void packBuffer(T *dst, T **src, int y_size, int x_size, int y_block, int x_block);


template <typename T, int T_WIDTH>
void unpackBuffer(T **dst, T *src, int y_size, int x_size, int y_block, int x_block);






float lstm_test(int, bool);



#endif  //  TESTS_HARDWARE_COMMON_TEST_LIB_H_
