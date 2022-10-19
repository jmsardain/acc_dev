#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <cstring>
#include <stdlib.h>
#include <math.h>
#include <limits>
using namespace std;

#ifndef PlotHelper_h
#define PlotHelper_h
float xtod0(int x, double m_step_x, double m_d0_range);
float ytoqoverpt(int y, double m_step_y, double m_qOverPt_range);
double GetR(double x, double y);
double GetEta(double x, double y, double z);
double GetPhi(double x, double y);
double GetDeta(double x1, double y1, double z1, double x2, double y2, double z2);
double GetDphi(double x1, double y1, double x2, double y2);
double GetDz(double z1, double z2);
double Phi_slope(double x1, double y1, double x2, double y2);
void getHists(string inFileName);
void makeDir(string outDir);


template <typename T>
class vector2D
{
    private:

    size_t d1,d2;
    std::vector<T> m_data;

    public:

    vector2D() :
        d1(0), d2(0)
    {}

    vector2D(size_t d1, size_t d2, T const & t=T()) :
        d1(d1), d2(d2), m_data(d1*d2, t)
    {}

    size_t size(int dim) const
    {
        if (dim == 0) return d1;
        if (dim == 1) return d2;
        else throw std::range_error("vector2D: Argument to size() must be 0 or 1");
    }

    void resize(size_t x1, size_t x2, T const & t=T())
    {
        d1 = x1;
        d2 = x2;
        m_data.resize(x1*x2, t);
    }

    T & operator()(size_t i, size_t j)
    {
#ifdef DEBUG_HTTVECTORS
        if (i >= d1 || j >= d2)
        {
            std::stringstream s;
            s << "vector2D out of bounds: request (" << i << "," << j << ") size (" << d1 << "," << d2 << ")";
            throw std::range_error(s.str());
        }
#endif
        return m_data[i*d2 + j];
    }

    T const & operator()(size_t i, size_t j) const
    {
#ifdef DEBUG_HTTVECTORS
        if (i >= d1 || j >= d2)
        {
            std::stringstream s;
            s << "vector2D out of bounds: request (" << i << "," << j << ") size (" << d1 << "," << d2 << ")";
            throw std::range_error(s.str());
        }
#endif
        return m_data[i*d2 + j];
    }

    T* operator[](size_t i)
    {
#ifdef DEBUG_HTTVECTORS
        if (i >= d1)
        {
            std::stringstream s;
            s << "vector2D out of bounds: request " << i << " size (" << d1 << "," << d2 << ")";
            throw std::range_error(s.str());
        }
#endif
        return m_data.data() + (i * d2);
    }

    const T* operator[](size_t i) const
    {
#ifdef DEBUG_HTTVECTORS
        if (i >= d1)
        {
            std::stringstream s;
            s << "vector2D out of bounds: request " << i << " size (" << d1 << "," << d2 << ")";
            throw std::range_error(s.str());
        }
#endif
        return m_data.data() + (i * d2);
    }

    T* data()
    {
        return m_data.data();
    }

    const T* data() const
    {
        return m_data.data();
    }
};

#endif
