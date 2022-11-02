#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <sys/types.h>
#include <sys/stat.h>
#include <getopt.h>
#include <unistd.h>
#include <array>
#include <cmath>
#include <unordered_set>
#include "plotHelper.h"
#include <dirent.h>
#include <cstring>
#include <stdlib.h>
#include <math.h>
#include <limits>
using namespace std;


#ifndef HoughHelper_h
#define HoughHelper_h

struct hit {
  double layer[100]; double r[100]; double x[100]; double y[100]; double z[100];
};

struct particle {
  double barcode[100]; double charge[100]; double pt[100]; double d0[100];
};

// ================================================
// ================================================
// Definitions for LRT Hough transform
typedef double fp_t; //  floating point type alias, can change it to float from double

typedef std::array<fp_t, 2> pvec;
pvec operator+(const pvec& a, const pvec& b);
pvec operator-(const pvec& a, const pvec& b);
pvec operator*(const pvec& a, double scale);

double length(const pvec& v) ;
pvec rotate90( const pvec& v);
double crossProduct( const pvec& a, const pvec& b ) ;

// 2d vector
void GetInfoFromFile(string mergeFile, std::vector<std::vector<float>>& vec);
void print_info_vec_data(std::vector<std::vector<float>>& vec, int size);
void HoughTransform(double *arr);
bool passThreshold(vector2D<std::pair<int, hit>> &image, int x, int y,  double m_step_x, double m_d0_range);
bool isLocalMaxima(vector2D<std::pair<int, hit>> &image, int x, int y, int m_imageSize_x, int m_imageSize_y);
// 1d vector
void GetInfoFromFile(string mergeFile, std::vector<float>& vec);
void print_info_vec_data(std::vector<float>& vec, int size);
void ConvertVecToArr(std::vector<float>& vec, double *arr);
void print_info_array_data(double *arr, unsigned int size);
void SelectEvent(double *arr, unsigned int size);
// old way
void GetInfoFromFiles(string hitsFile, string partsFile, hit *hits, particle *particles, int nevents);
void printout_struct(hit *hits, particle *particles);
bool passThreshold(vector2D<std::pair<int, hit>> &image, int x, int y,  double m_step_x, double m_d0_range, hit *hits, particle *particles);
bool isLocalMaxima(vector2D<std::pair<int, hit>> &image, int x, int y, int m_imageSize_x, int m_imageSize_y,hit *hits, particle *particles) ;
void SelectEvents(hit *hits, particle *particles, int nevents) ;

#endif
