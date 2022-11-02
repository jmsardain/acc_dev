#include "math.h"

// A testing kernel, does nothing but return the input

extern "C"
{
  void tk(double *input, double *output, unsigned int size){
    // void tk(double *input, double *output){
#pragma HLS INTERFACE m_axi port = input bundle = gmem1
#pragma HLS INTERFACE m_axi port = output bundle = gmem1
    // return input

    // KERNEL
    const double m_acceptedDistanceBetweenLayersMin = 200; // min R disstance for hits pair filtering
    const double m_acceptedDistanceBetweenLayersMax = 600;

    int size_features = size/9;
    for (unsigned int i=0; i<size_features; i++) { // loop over features
      if(i==0){
        output[9*i]   = input[9*i];    // numhits
        output[9*i+1] = input[9*i+1];  // layer
        output[9*i+2] = input[9*i+2];  // r
        output[9*i+3] = input[9*i+3];  // x
        output[9*i+4] = input[9*i+4];  // y
        output[9*i+5] = input[9*i+5];  // z
        output[9*i+6] = input[9*i+6];  // charge
        output[9*i+7] = input[9*i+7];  // pt
        output[9*i+8] = input[9*i+8];  // d0
      }
      for (unsigned int j=i+1; j<size_features; j++) { // loop over features
        // if (i==j) continue;

        double radiusDifference = abs(input[9*j+2] - input[9*i+2]);
        if (input[9*i+1]==input[9*j+1]) continue; // cut on layer
        if (  not (m_acceptedDistanceBetweenLayersMin < radiusDifference && radiusDifference < m_acceptedDistanceBetweenLayersMax) ){ // cut on difference in r
          continue;
        }

        output[9*j]   = input[9*j];    // numhits
        output[9*j+1] = input[9*j+1];  // layer
        output[9*j+2] = input[9*j+2];  // r
        output[9*j+3] = input[9*j+3];  // x
        output[9*j+4] = input[9*j+4];  // y
        output[9*j+5] = input[9*j+5];  // z
        output[9*j+6] = input[9*j+6];  // charge
        output[9*j+7] = input[9*j+7];  // pt
        output[9*j+8] = input[9*j+8];  // d0
        // out[j] = temp[i];
      }
    }
  }
}
