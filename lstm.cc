/* MIT License

Copyright (C) IBM Corporation 2018

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Note: This code is written based on VTA (Versatile Tensor Accelerator) overlay to demonstrate the implementation of a three-cells LSTM network
Written by: Mehdi Yaghouti
Email     : MehdiYaghouti@gmail.com  */

#include "lstm.h"
#include "arrays.h"

uint64_t vta( uint32_t insn_count, VTAGenericInsn *insns, VTAUop *uops, uint32_t *inputs, uint32_t *weights, uint32_t *biases, uint32_t *outputs) {
  
  uint64_t t_fpga;
  struct timespec start, stop;

  
  void* vta_fetch_handle   = VTAMapRegister(VTA_FETCH_ADDR);
  void* vta_load_handle    = VTAMapRegister(VTA_LOAD_ADDR);
  void* vta_compute_handle = VTAMapRegister(VTA_COMPUTE_ADDR);
  void* vta_store_handle   = VTAMapRegister(VTA_STORE_ADDR);

  
  uint32_t insn_phy0  = cma_get_phy_addr(insns);
  uint32_t insn_phy3  = cma_get_phy_addr(&(insns[3]));
  uint32_t uop_phy    = cma_get_phy_addr(uops);
  uint32_t input_phy  = cma_get_phy_addr(inputs) ;
  uint32_t weight_phy = cma_get_phy_addr(weights);
  uint32_t bias_phy   = cma_get_phy_addr(biases);
  uint32_t output_phy = cma_get_phy_addr(outputs);


  

  VTAWriteMappedReg(vta_fetch_handle,   VTA_FETCH_INSN_COUNT_OFFSET, 3);
  VTAWriteMappedReg(vta_fetch_handle,   VTA_FETCH_INSN_ADDR_OFFSET, insn_phy0);
  VTAWriteMappedReg(vta_load_handle ,   VTA_LOAD_INP_ADDR_OFFSET, input_phy);
  VTAWriteMappedReg(vta_load_handle,    VTA_LOAD_WGT_ADDR_OFFSET, weight_phy);
  VTAWriteMappedReg(vta_compute_handle, VTA_COMPUTE_UOP_ADDR_OFFSET, uop_phy);
  VTAWriteMappedReg(vta_compute_handle, VTA_COMPUTE_BIAS_ADDR_OFFSET, bias_phy);
  VTAWriteMappedReg(vta_store_handle,   VTA_STORE_OUT_ADDR_OFFSET, output_phy);
  
  
  
  // VTA start
  VTAWriteMappedReg(vta_fetch_handle, 0x0, 0x1);
  VTAWriteMappedReg(vta_load_handle, 0x0, 0x81);
  VTAWriteMappedReg(vta_compute_handle, 0x0, 0x81);
  VTAWriteMappedReg(vta_store_handle, 0x0, 0x81);
  sleep(2);
  //printf("%d\n",VTAReadMappedReg(vta_compute_handle,VTA_COMPUTE_DONE_RD_OFFSET)&VTA_DONE);
  
  
  VTAWriteMappedReg(vta_fetch_handle,VTA_FETCH_INSN_ADDR_OFFSET,insn_phy3);
  VTAWriteMappedReg(vta_fetch_handle, VTA_FETCH_INSN_COUNT_OFFSET, insn_count-3);
  
  //clock_gettime(CLOCK_REALTIME,&start);
  
  sleep(2);
  //clock_gettime(CLOCK_REALTIME,&start);
  VTAWriteMappedReg(vta_fetch_handle, 0x0, 0x1);
  
  clock_gettime(CLOCK_REALTIME,&start);
  
  
  
  while(!(VTAReadMappedReg(vta_compute_handle,VTA_COMPUTE_DONE_RD_OFFSET) & VTA_DONE)) {
  
  }
  


  clock_gettime(CLOCK_REALTIME, &stop);
  t_fpga = 1000000000ULL * (stop.tv_sec - start.tv_sec) + (stop.tv_nsec - start.tv_nsec);

  
  
  
  VTAUnmapRegister(vta_fetch_handle);
  VTAUnmapRegister(vta_load_handle);
  VTAUnmapRegister(vta_compute_handle);
  VTAUnmapRegister(vta_store_handle);

  return t_fpga;
}






template <typename DST_T, int DST_T_WIDTH, typename SRC_T, int SRC_T_WIDTH>
void packBuffer(DST_T *dst, SRC_T **src, int y_size, int x_size, int y_block, int x_block) {
  assert((SRC_T_WIDTH * x_block * y_block) % DST_T_WIDTH  == 0);
  assert(DST_T_WIDTH <= 64);
  int buffer_idx = 0;
  int ratio = DST_T_WIDTH / SRC_T_WIDTH;
  long long int mask = (1ULL << SRC_T_WIDTH) - 1;
  DST_T tmp = 0;
  for (int i = 0; i < y_size / y_block; i++) {
    for (int j = 0; j < x_size / x_block; j++) {
      for (int k = 0; k < y_block; k++) {
        for (int l = 0; l < x_block; l++) {
          int block_idx = l + k * x_block;
          tmp |= (src[i * y_block + k][j * x_block + l] & mask) << ((block_idx % ratio) * SRC_T_WIDTH);
          // When tmp is packed, write to destination array
          if (block_idx % ratio == ratio - 1) {
            dst[buffer_idx++] = tmp;
            tmp = 0;
          }
        }
      }
    }
  }
}

template <typename DST_T, int DST_T_WIDTH, typename SRC_T, int SRC_T_WIDTH>
void unpackBuffer(DST_T **dst, SRC_T *src, int y_size, int x_size, int y_block, int x_block) {
  assert((DST_T_WIDTH * x_block * y_block) % SRC_T_WIDTH == 0);
  int buffer_idx = 0;
  long long int mask = (1ULL << DST_T_WIDTH) - 1;
  int ratio = SRC_T_WIDTH / DST_T_WIDTH;
  for (int i = 0; i < y_size / y_block; i++) {
    for (int j = 0; j < x_size / x_block; j++) {
      for (int k = 0; k < y_block; k++) {
        for (int l = 0; l < x_block; l++) {
          int block_idx = l + k * x_block;
          dst[i * y_block + k][j * x_block + l] = (src[buffer_idx] >> ((block_idx % ratio) * DST_T_WIDTH)) & mask;
          if (block_idx % ratio == ratio - 1) {
            buffer_idx++;
          }
        }
      }
    }
  }
}





void * allocBuffer(size_t num_bytes) {
#ifdef NO_SIM
  return VTAMemAlloc(num_bytes, VTA_CACHED);
#else
  return malloc(num_bytes);
#endif
}





 


float lstm_test(int trip_num,bool debug) {
  
  
  
  VTAGenericInsn Ins;
  
  
  VTAGenericInsn *insn_buf = static_cast<VTAGenericInsn *>(allocBuffer(sizeof(VTAGenericInsn) * num_inst));

  int insn_idx = 0;
  for (uint32_t i = 0; i < num_inst; i++) {
    Ins.opcode        = inst_bitfields[7*i+0];
    Ins.pop_prev_dep  = inst_bitfields[7*i+1];
    Ins.pop_next_dep  = inst_bitfields[7*i+2];
    Ins.push_prev_dep = inst_bitfields[7*i+3];
    Ins.push_next_dep = inst_bitfields[7*i+4];
    Ins.pad_0         = inst_bitfields[7*i+5];
    Ins.pad_1         = inst_bitfields[7*i+6];
    insn_buf[insn_idx++]   = Ins;
    

  }

  
  
  // Prepare the uop buffer
  VTAUop *uop_buf = static_cast<VTAUop *>(VTAMemAlloc(sizeof(VTAUop) * num_uops, VTA_CACHED));
  int uops_idx = 0;
  for (uint32_t i = 0; i < num_uops; i++) {
    uop_buf[uops_idx].wgt_idx   = uops[3*i+0];
    uop_buf[uops_idx].src_idx   = uops[3*i+1];
    uop_buf[uops_idx++].dst_idx   = uops[3*i+2];
  }
  
  

  inp_T **inp_array = static_cast<inp_T **>(malloc(sizeof(inp_T *) * inp_rows));
  for (uint32_t i = 0; i < inp_rows; i++) {
    inp_array[i] = static_cast<inp_T *>(malloc(sizeof(inp_T) * inp_cols));
  }
  
  for (uint32_t i = 0; i < inp_rows; i++) {
    for (uint32_t j = 0; j < inp_cols; j++) {
      inp_array[i][j] = static_cast<inp_T>(inps[i*16+j]);
    }
  }
  
  uint32_t *input_buf = static_cast<uint32_t *>(allocBuffer(VTA_INP_ELEM_BYTES * inp_rows));
  packBuffer<uint32_t, 32, inp_T, VTA_INP_WIDTH>(input_buf, inp_array, inp_rows, inp_cols, VTA_BATCH, VTA_BLOCK_IN);

  
  
  
  
  
  acc_T **bias_array = static_cast<acc_T **>(malloc(sizeof(acc_T *) * bias_rows));
  for (uint32_t i = 0; i < bias_rows; i++) {
    bias_array[i] = static_cast<acc_T *>(malloc(sizeof(acc_T) * bias_cols));
  }
  
  for (uint32_t i = 0; i < bias_rows; i++) {
    for (uint32_t j = 0; j < bias_cols; j++) {
      bias_array[i][j] = static_cast<acc_T>(biases[i*16+j]);
    }
  }
  
  uint32_t *bias_buf = static_cast<uint32_t *>(allocBuffer(VTA_INP_ELEM_BYTES * bias_rows));
  packBuffer<uint32_t, 32, acc_T, VTA_ACC_WIDTH>(bias_buf, bias_array, bias_rows, bias_cols, VTA_BATCH, VTA_BLOCK_IN);


  
  if(debug!=0){
     printf("Input Vector: \n");
  
     for(uint32_t k=0;k<1;k++) {
         printf("[");
         for(uint32_t i=0;i<16;i++) {
  
             printf("%d ",inp_array[k][i]);
             if (i!=15)
                printf(", ");
         }
     
     printf(" ]\n");
     }
   }
  
  uint64_t t_fpga=0;
  for(uint32_t iter=0;iter<trip_num;iter++)
      t_fpga += vta(num_inst, insn_buf, uop_buf, input_buf, NULL, bias_buf, input_buf); 

  
  

 
 
  unpackBuffer<out_T, VTA_OUT_WIDTH, uint32_t, 32>(inp_array, input_buf, inp_rows, inp_cols, VTA_BATCH, VTA_BLOCK_OUT);

  if(debug!=0)
  {
     printf("Outputs h1, h2, h3:\n");
     for(uint32_t k=1;k<4;k++) {
         printf("[");
         for(uint32_t i=0;i<16;i++) {
             printf("%d ",inp_array[k][i]);
             if (i!=15)
                printf(", ");
         }
         printf(" ]\n");
     }
     
  }
  


  
  // Free allocated memory
  for (uint32_t i=0;i<inp_rows;i++) {
      free(inp_array[i]);
      free(bias_array[i]);
   }

  free(inp_array);
  free(bias_array);

  
  VTAMemFree(insn_buf);
  VTAMemFree(uop_buf);
  VTAMemFree(bias_buf);
  VTAMemFree(input_buf);
  return static_cast<float>(t_fpga);
}

