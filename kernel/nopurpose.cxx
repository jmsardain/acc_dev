// A testing kernel, does nothing but return the input

extern "C"
{
    void tk(double *input, double *output, int size)
    {
#pragma HLS INTERFACE m_axi port = input bundle = gmem1
#pragma HLS INTERFACE m_axi port = output bundle = gmem1
    // return input
        for(int i = 0; i<size; i++)
        {
            output[i]=input[i];
        }
    }

}
