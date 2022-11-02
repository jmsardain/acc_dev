v++ -c -t sw_emu --platform xilinx_u250_gen3x16_xdma_4_1_202210_1 --config ../kernel/u250.cfg -k tk -I../src/ ../kernel/kernel.cxx -o tk.xo

if [ $? -eq 0 ];
then
    v++ -l -t sw_emu --platform xilinx_u250_gen3x16_xdma_4_1_202210_1 --config ../kernel/u250.cfg ./tk.xo -o tk.xclbin
fi
