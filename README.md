# Code to run doublet Hough transform on an accelerator card

## Data
The data to run on is in the txtfiles/ repository
```
hits file
event, layer, r, x, y, z

particles file
event, barcode, charge, pt, d0
```

## Setup xilinx
Use the setup.sh file in ./ to setup xilinx and to examine if the device is ready
```
source setup.sh
```

## Compile the different code
```
export XCL_EMULATION_MODE=sw_emu
cd sw_emu/
source compile_emul.sh  
source compile_host.sh
source compile_kernel.sh
./host_openCL --data ../txtfiles/merge.txt
```
