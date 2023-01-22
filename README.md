# Code to run doublet Hough transform on an accelerator card

This code works and lives on eepp-ml2.physics.arizona.edu
If you are not using UArizona internet, you can use the UA VPN.

## Data
The data to run on is in the txtfiles/ repository. hits and truth (particles) file exist for single muons.

```
hits file
event, layer, r, x, y, z

particles file
event, barcode, charge, pt, d0
```

A merged file is created with the (very) simple code txtfiles/mergedf.py
```
python mergedf.py
```

## Setup xilinx
Use the setup.sh file in ./ to setup xilinx and to examine if the device is ready
```
source setup.sh
```

## Compile and run the code
The main function for the host is in host/host.cxx and it calls the helpers in include/, namely HoughHelper.cxx and plotHelper.cxx
The main function for the kernel is in  kernel/kernel.cxx
```
export XCL_EMULATION_MODE=sw_emu
cd sw_emu/
source compile_emul.sh    # compiles the emulation
source compile_host.sh    # compiles the host code
source compile_kernel.sh  # compiles the kernel code
```

To run the code:
```
./host_openCL --data ../txtfiles/merge.txt
```

A code that does not use the kernel (pure C++ code) exists and it lives in host/. To run it you need to:

```
cd Hough/
make
./dataProcessor --data txtfiles/merge.txt 
```

## More details about the code in the kernel
The merge file contains the information of the single muon in each event.
In fact, for one event, barcode, charge, pt and d0 shouldn't change.
However, a muon leaves multiple hits that are located in different places in the detector (layer, r, x, y, z).
The host code reads the data and constructs an array of blocks of 9 elements.

The 9i   th element represents the number of hits

The 9i+1 th element represents the layer where a hit exists

The 9i+2 th element represents the position in r of the hit

The 9i+3 th element represents the position in x of the hit

The 9i+4 th element represents the position in y of the hit

The 9i+5 th element represents the position in zs of the hit

The 9i+6 th element represents the charge of the muon

The 9i+7 th element represents the transverse momentum (pT) of the muon

The 9i+8 th element represents the d0 of the muon

The kernel code will then compare the different hits with one another (e.g. the first block of 9 elements with the second block of 9 elements).
The events that satisfy the condition below should be kept in the output array (output of the kernel) that will be analyzed in the function HoughTransform:

First condition: 2 hits belonging to the same particle cannot be on the same layer
Second condition: it uses the radius difference defined as radius of hit j - radius of hit i. radius difference should be between 200 and 600. 
