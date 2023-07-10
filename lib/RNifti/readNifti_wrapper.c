#include <string>
#include <Eigen/Core>
#include "read_nifti.hpp"
#include "readNifti_wrapper.h"


// Helper function to convert t_matrix to Eigen::MatrixXd
Eigen::MatrixXd t_matrix_to_Eigen(const t_matrix *mat) {
    Eigen::MatrixXd eigen_mat(mat->N, mat->D);
    for (int i = 0; i < mat->N; ++i) {
        for (int j = 0; j < mat->D; ++j) {
            eigen_mat(i, j) = mat->X[i * mat->D + j];
        }
    }
    return eigen_mat;
}

// Helper function to convert Eigen::MatrixXd to t_matrix
void Eigen_to_t_matrix(const Eigen::MatrixXd &eigen_mat, t_matrix *mat) {
    mat->N = eigen_mat.rows();
    mat->D = eigen_mat.cols();
    mat->X = (double *)realloc(mat->X, mat->N * mat->D * sizeof(double));
    for (int i = 0; i < mat->N; ++i) {
        for (int j = 0; j < mat->D; ++j) {
            mat->X[i * mat->D + j] = eigen_mat(i, j);
        }
    }
}

extern "C" {
    long int numSlices_c(const char* nii_filename) {
        return numSlices(std::string(nii_filename));
    }

    void loadNthSlice_c(const char* nii_filename, t_matrix* slice, int i) {
        Eigen::MatrixXd eigen_slice;
        loadNthSlice(std::string(nii_filename), eigen_slice, i);
        Eigen_to_t_matrix(eigen_slice, slice);
    }
}