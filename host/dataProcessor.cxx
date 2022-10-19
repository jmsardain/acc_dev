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

hit hits[10000];
particle particles[10000];

int main(int argc,char *argv[]){
  cout << " i am here " << endl;

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
  cout << " i am there " << endl;


  cout << " i am there " << endl;

  GetInfoFromFiles(hitsFile, partsFile, hits, particles);
  printout_struct(hits, particles);
  SelectEvents(hits, particles);
  return 0;

}
