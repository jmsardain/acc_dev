#include "src/HTTConstGenAlgo.cxx"

#undef NDEBUG
#include <cassert>
#include <cmath>
#include <iomanip>



void test_getReducedMatrix()
{
    const size_t n_redu = 2;
    const size_t n_full = 4;

    vector<double> mtx =
    {
        1, 2, 3, 4,
        5, 6, 7, 8,
        9, 0, 1, 2,
        3, 4, 5, 6
    };

    vector<bool> usable = { false, false, true, true };

    vector<double> chk =
    {
        1, 2,
        5, 6
    };

    TMatrixD m = getReducedMatrix(n_full, mtx, usable, n_redu);

    cout << "Reduced matrix: \n";
    for (size_t i = 0; i < n_redu; i++)
    {
        cout << "\t";
        for (size_t j = 0; j < n_redu; j++)
        {
            cout << std::setw(8) << m[i][j] << " ";
            assert(m[i][j] == chk[i*n_redu + j]);
        }
        cout << "\n";
    }
}



void test_invert()
{
    const size_t n_redu = 3;
    const size_t n_full = 4;

    double mtx_redu[n_redu * n_redu] =
    {
        2, 1, 1,
        1, 1, 2,
        1, 2, 1
    };
    TMatrixD mtx(n_redu, n_redu, mtx_redu);

    std::vector<bool> usable = { true, true, false, true };

    double chk[n_full * n_full] =
    {
         3.0/4, -1.0/4, 0, -1.0/4,
        -1.0/4, -1.0/4, 0,  3.0/4,
             0,      0, 0,      0,
        -1.0/4,  3.0/4, 0, -1.0/4
    };

    std::vector<double> inv = invert(n_full, mtx, usable);

    assert(inv.size() == n_full * n_full);
    cout << "Inverse matrix: \n";
    for (size_t i = 0; i < n_full; i++)
    {
        cout << "\t";
        for (size_t j = 0; j < n_full; j++)
        {
            cout << std::setw(8) << inv[i*n_full + j] << " ";
            assert(fabs(inv[i*n_full + j] - chk[i*n_full + j]) < 0.0001);
        }
        cout << "\n";
    }
}



void test_eigen(size_t n_redu, size_t n_full, double* mtx_redu, vector<bool> usable, double* check_vals, double* check_vecs)
{
    // Ready inputs
    TMatrixD mtx(n_redu, n_redu, mtx_redu);
    std::vector<double> eigvals;
    vector2D<double> eigvecs;

    // Call eigen()
    eigen(n_redu, n_full, mtx, usable, eigvals, eigvecs);

    assert(eigvals.size() == n_full);
    assert(eigvecs.size(0) == n_full);
    assert(eigvecs.size(1) == n_full);

    // Print output
    cout << "Eigvals: ";
    for (size_t i = 0; i < n_full; i++)
        cout << eigvals[i] << " ";
    cout << "\n";

    cout << std::fixed;
    cout << "Eigvecs: \n";
    for (size_t i = 0; i < n_full; i++)
    {
        cout << "\t";
        for (size_t j = 0; j < n_full; j++)
            cout << std::setw(8) << eigvecs[i][j] << " ";
        cout << "\n";
    }

    cout << "Norms: ";
    for (size_t i = 0; i < n_full; i++)
    {
        double val = 0;
        for (size_t j = 0; j < n_full; j++)
            val += eigvecs[i][j] * eigvecs[i][j];
        cout << sqrt(val) << " ";
    }
    cout << endl;

    // Check output
    for (size_t i = 0; i < n_full; i++)
        assert(fabs(eigvals[i] - check_vals[i]) < 0.0001);

    // Eigenvectors have freedom of scale
    for (size_t i = 0; i < n_full; i++)
    {
        double scale = 0;
        for (size_t j = 0; j < n_full; j++)
        {
            if (check_vecs[i*n_full + j] == 0) continue;
            if (scale == 0) scale = eigvecs[i][j] / check_vecs[i*n_full + j];
            else assert(fabs(scale - eigvecs[i][j] / check_vecs[i*n_full + j]) < 0.0001);
        }
    }
}

void test_eigen_3x3()
{
    const size_t n_redu = 3;
    const size_t n_full = 4;

    double mtx_redu[n_redu * n_redu] =
    {
        -2, -7, 13,
        -2,  1,  4,
        -2, -2,  7
    };

    std::vector<bool> usable = { true, true, false, true };

    double check_vals[n_full] = { 1, 2, 0, 3 };
    double check_vecs[n_full * n_full] =
    {
        2, 1, 0, 1,
        3, 2, 0, 2,
        0, 0, 0, 0,
        1, 3, 0, 2
    };

    test_eigen(n_redu, n_full, mtx_redu, usable, check_vals, check_vecs);
}

void test_eigen_sym()
{
    const size_t n_redu = 3;
    const size_t n_full = 3;

    double mtx_redu[n_redu * n_redu] =
    {
        2, 1, 1,
        1, 1, 2,
        1, 2, 1
    };

    std::vector<bool> usable = { true, true, true };

    double check_vals[n_full] = { 1, -1, 4 };
    double check_vecs[n_full * n_full] =
    {
        -2, 1, 1,
        0, -1, 1,
        1, 1, 1
    };

    test_eigen(n_redu, n_full, mtx_redu, usable, check_vals, check_vecs);
}



int main()
{
    cout << std::setprecision(5);
    test_getReducedMatrix();
    cout << endl;
    test_invert();
    cout << endl;
    test_eigen_3x3();
    cout << endl;
    test_eigen_sym();
    return 0;
}
