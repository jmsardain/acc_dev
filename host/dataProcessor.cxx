#include "plotHelper.h"
#include "HoughHelper.h"
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
using namespace std;
#define NEVENTS 1000

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
  // std::vector<float> datavec;
  std::vector<std::vector<float>> datavec(NEVENTS);

  GetInfoFromFile(file, datavec);
  print_info_vec_data(datavec, NEVENTS); // you could do this just to check
  int nlines = 8884; // nlines is different than nevents, each event can have 8+ hits

  // unsigned int size_vec = datavec.size();
  // double data_arr[size_vec];
  ConvertVecToArr(datavec, data_arr);
  // print_info_array_data(data_arr, size_vec);


  //SelectEvent(data_arr, nlines);

  return 0;

}
