g++ --std=c++1y -I../include -I$XILINX_XRT/include -L$XILINX_XRT/lib -lOpenCL -lrt -pthread \
  ../host/maxwell.cxx ../include/HoughHelper.cxx  ../include/plotHelper.cxx -o host_openCL
