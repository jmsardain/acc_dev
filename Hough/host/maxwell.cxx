#include "plotHelper.h"
#include "HoughHelper.h"
// #include "HoughHelper.cxx"
#include <getopt.h>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <array>
#include <cmath>
#include <unordered_set>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <vector>

using namespace std;
#define CL_HPP_CL_1_2_DEFAULT_BUILD
#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_ENABLE_PROGRAM_CONSTRUCTION_FROM_ARRAY_COMPATIBILITY 1
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define DATA_SIZE 5
#include <CL/cl2.hpp>

std::vector<cl::Device> get_xilinx_devices();
char *read_binary_file(const std::string &xclbin_file_name, unsigned &nb);

hit hits[10000];
particle particles[10000];

int main(int argc,char *argv[]){
  // Initialize the OpenCL environment
  cl_int err;
  std::string binaryFile = (argc != 2) ? "tk.xclbin" : argv[1];
  unsigned fileBufSize;
  std::vector<cl::Device> devices = get_xilinx_devices();
  devices.resize(1);
  cl::Device device = devices[0];
  cl::Context context(device, NULL, NULL, NULL, &err);
  char *fileBuf = read_binary_file(binaryFile, fileBufSize);
  cl::Program::Binaries bins{{fileBuf, fileBufSize}};
  cl::Program program(context, devices, bins, NULL, &err);
  cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE, &err);
  cl::Kernel krnl_tk(program, "tk", &err);


  // Create buffers and initialize
  // Create the buffers and allocate memory
  cl::Buffer in_buff(context, CL_MEM_READ_ONLY, sizeof(double) * DATA_SIZE, NULL, &err);
  cl::Buffer out_buff(context, CL_MEM_READ_WRITE, sizeof(double) * DATA_SIZE, NULL, &err);

  // Map host-side buffer memory to user-space pointers
  double *input = (double *)q.enqueueMapBuffer(in_buff, CL_TRUE, CL_MAP_WRITE, 0, sizeof(double) * DATA_SIZE);
  double *output = (double *)q.enqueueMapBuffer(out_buff, CL_TRUE, CL_MAP_WRITE | CL_MAP_READ, 0, sizeof(double) * DATA_SIZE);





  std::string inDir, outDir, hitsFile, partsFile;
  static struct option long_options[] =
  {
    {"inDir", 1, NULL, 'a'},
    {"outDir", 1, NULL, 'b'},
    {"hitFile", 1, NULL, 'c'},
    {"partFile", 1, NULL, 'd'},
    {NULL, 0, NULL, 0}
  };

  int opt;
  while ( (opt = getopt_long(argc, argv,"abcd", long_options, NULL)) != -1 ) {  // for each option...
    switch ( opt )
      {
      case 'a': inDir = optarg; break;
      case 'b': outDir = optarg; break;
      case 'c': hitsFile = optarg; break;
      case 'd': partsFile = optarg; break;
      case 0: break;
      }
  }


  GetInfoFromFiles(hitsFile, partsFile, hits, particles);
  // Initialize the input buffers
  for (int i = 0; i < DATA_SIZE; i++) {
    input[i] = particles[i].pt[0];
    output[i] = 0;
  }

  // Map buffers to kernel arguments
   krnl_tk.setArg(0, in_buff);
   krnl_tk.setArg(1, out_buff);
   krnl_tk.setArg(2, DATA_SIZE);

   // Schedule transfer of inputs to device memory,
   // execution of kernel, and transfer of outputs back to host memory
   q.enqueueMigrateMemObjects({in_buff}, 0); // 0 means from host
   q.enqueueTask(krnl_tk);
   q.enqueueMigrateMemObjects({out_buff}, CL_MIGRATE_MEM_OBJECT_HOST);

   q.finish();

   // Check output
    bool match = true;

    for (int i = 0; i < DATA_SIZE; i++){
      // Print input and output anyway
      std::cout << "input is: " << input[i] << " output is: " << output[i] << std::endl;
      if (output[i] != input[i]){
        std::cout << "Error: Results mismatch" << std::endl;
        std::cout << "i = " << i << " result = " << input[i] << std::endl;
        match = false;
        break;
      }
    }

    delete[] fileBuf;
    std::cout << "TEST " << (match ? "Passed" : "Failed") << std::endl;

    return (match ? EXIT_SUCCESS : EXIT_FAILURE);
    // return 0;
  // printout_struct();
  // SelectEvents();

}



std::vector<cl::Device> get_xilinx_devices()
{
    size_t i;
    cl_int err;
    std::vector<cl::Platform> platforms;
    err = cl::Platform::get(&platforms);
    cl::Platform platform;
    for (i = 0; i < platforms.size(); i++)
    {
        platform = platforms[i];
        std::string platformName = platform.getInfo<CL_PLATFORM_NAME>(&err);
        if (platformName == "Xilinx")
        {
            std::cout << "INFO: Found Xilinx Platform" << std::endl;
            break;
        }
    }
    if (i == platforms.size())
    {
        std::cout << "ERROR: Failed to find Xilinx platform" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Getting ACCELERATOR Devices and selecting 1st such device
    std::vector<cl::Device> devices;
    err = platform.getDevices(CL_DEVICE_TYPE_ACCELERATOR, &devices);
    return devices;
}

char *read_binary_file(const std::string &xclbin_file_name, unsigned &nb)
{
    if (access(xclbin_file_name.c_str(), R_OK) != 0)
    {
        printf("ERROR: %s xclbin not available please build\n", xclbin_file_name.c_str());
        exit(EXIT_FAILURE);
    }
    // Loading XCL Bin into char buffer
    std::cout << "INFO: Loading '" << xclbin_file_name << "'\n";
    std::ifstream bin_file(xclbin_file_name.c_str(), std::ifstream::binary);
    bin_file.seekg(0, bin_file.end);
    nb = bin_file.tellg();
    bin_file.seekg(0, bin_file.beg);
    char *buf = new char[nb];
    bin_file.read(buf, nb);
    return buf;
}
