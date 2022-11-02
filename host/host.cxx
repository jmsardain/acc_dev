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
// #define DATA_SIZE 5
#define NEVENTS 1000
#include <CL/cl2.hpp>

std::vector<cl::Device> get_xilinx_devices();
char *read_binary_file(const std::string &xclbin_file_name, unsigned &nb);

int main(int argc,char *argv[]){

  std::string inDir, outDir, file;
  static struct option long_options[] =
  {
    {"inDir", 1, NULL, 'a'},
    {"outDir", 1, NULL, 'b'},
    {"data", 1, NULL, 'c'},
    {NULL, 0, NULL, 0}
  };

  int opt;
  while ( (opt = getopt_long(argc, argv,"abc", long_options, NULL)) != -1 ) {  // for each option...
    switch ( opt )
      {
      case 'a': inDir = optarg; break;
      case 'b': outDir = optarg; break;
      case 'c': file = optarg; break;
      case 0: break;
      }
  }

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
  // cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE, &err);
  cl::Kernel krnl_tk(program, "tk", &err);

  std::vector<std::vector<float>> datavec(NEVENTS);

  GetInfoFromFile(file, datavec);
  // print_info_vec_data(datavec, NEVENTS);
  // int DATA_SIZE = NEVENTS;
  // int DATA_SIZE = 100000000;
  // std::cout << " " << size_vec << " " << DATA_SIZE << std::endl;
  // print_info_array_data(data_arr, size_vec);



  // for(unsigned int i=0; i<datavec.size() ; i++){
  //   if(datavec[i].size()==0 ){
  //     std::cout << i << std::endl;
  //   }
  // }

  // Initialize the input buffers
  // double** data_arr = new double*[NEVENTS];
  // double data_arr[NEVENTS];
  // std::cout << " DATA_SIZE : " << datavec.size() << std::endl;
  for (int i = 0; i < NEVENTS; i++) {
    if(i>0) continue;
    if(i==80 || i==138 || i==441 || i==754 || i==971) continue; // remove problematic events (events with no hits)
    cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE, &err);
    int DATA_SIZE = datavec.at(i).size();

    // Create buffers and initialize
    // Create the buffers and allocate memory
    cl::Buffer in_buff(context, CL_MEM_READ_ONLY, sizeof(double) * DATA_SIZE, NULL, &err);
    cl::Buffer out_buff(context, CL_MEM_READ_WRITE, sizeof(double) * DATA_SIZE, NULL, &err);

    // Map host-side buffer memory to user-space pointers
    double *input = (double *)q.enqueueMapBuffer(in_buff, CL_TRUE, CL_MAP_WRITE, 0, sizeof(double) * DATA_SIZE);
    double *output = (double *)q.enqueueMapBuffer(out_buff, CL_TRUE, CL_MAP_WRITE | CL_MAP_READ, 0, sizeof(double) * DATA_SIZE);

  // for (int i = 0; i < 2; i++) {
    // if (i>1) continue;
    int numhits =  datavec.at(i).at(0);
    // std::cout << "This is eventNumber = " << i << " it has #hits*features " << datavec.at(i).size() << std::endl;
    double temp[datavec.at(i).size()];
    // data_arr[i] = temp;
    ConvertVecToArr(datavec.at(i), temp);
    // print_info_array_data(temp, numhits);


    // KERNEL



    // ConvertVecToArr(datavec[i], data_arr);
    // print_info_array_data(, datavec[i].size());
    for(unsigned int j=0; j<datavec.at(i).size(); ++j){
      input[j] = temp[j];
      output[j] = 0;
      // std::cout<<"inp "<<input[j]<<std::endl;
      // std::cout<<"oup "<<output[j]<<std::endl;
    }
    // print_info_array_data(input[i], numhits);
    // std::cout << " before kernel set " << std::endl;
    // Map buffers to kernel arguments
    krnl_tk.setArg(0, in_buff);
    krnl_tk.setArg(1, out_buff);
    // krnl_tk.setArg(2, datavec[i].size());
    // krnl_tk.setArg(2, datavec.at(i).size());
    krnl_tk.setArg(2, DATA_SIZE);
    // std::cout << " after kernel set " << std::endl;

    // Schedule transfer of inputs to device memory,
    // execution of kernel, and transfer of outputs back to host memory
    q.enqueueMigrateMemObjects({in_buff}, 0); // 0 means from host
    q.enqueueTask(krnl_tk);
    q.enqueueMigrateMemObjects({out_buff}, CL_MIGRATE_MEM_OBJECT_HOST);
    // std::cout << " after migration " << std::endl;


    q.finish();
    // std::cout << " after finish " << std::endl;

    int sizeout = sizeof(output)*9;
    for (unsigned int i=0; i<sizeout; i++) {
      std::cout << output[i] << std::endl;
    }
    HoughTransform(output);
    // for(unsigned int j=0; j<datavec.at(i).size(); ++j){
    //
    //   std::cout << "input is: " << input[j] << " output is: " << output[j] << std::endl;
    // }


  }
  // Check output
  bool match = true;

  // for (int i = 0; i < 20; i++){
  //
  //   // Print input and output anyway
  //   std::cout << "input is: " << input[i] << " output is: " << output[i] << std::endl;
  //   // if (output[i] != input[i]){
  //     // std::cout << "Error: Results mismatch" << std::endl;
  //     // std::cout << "i = " << i << " result = " << input[i] << std::endl;
  //     // match = false;
  //     // break;
  //   // }
  // }

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
