// Copyright 2019, Winfield Chen and Lloyd T. Elliott.

#include "matrix.h"
#include "phenotype.h"
#include <stdlib.h>
#include "read_nifti.hpp"

int numPhenotypes(char *fname) {
  FILE *fp = fopen(fname, "r");
  int D = 0;
  if (fp == NULL) {
    error("Could not open phenotype file");
  }

  int c;
  while (1) {
    c = getc(fp);
    if (c == EOF) {
      error("Could not convert phenotype headers");
    }

    if (c == ' ') {
      D += 1;
    }

    if (c == '\n') {
      break;
    }
  }

  return D + 1;
}

void load_phenotypes2(char *fname, t_matrix *y, t_matrix *obs, t_matrix *denom, int N, int D0, int D) {
  FILE *fp = fopen(fname, "r");
  if (fp == NULL) {
    error("Could not open phenotype file");
  }

  int c;
  while (1) {
    c = getc(fp);
    if (c == EOF) {
      error("Could not convert phenotype headers");
    }

    if (c == '\n') {
      break;
    }
  }

  *y = load2(fp, N, D0, D);
  fclose(fp);
  *obs = create(N, D);
  *denom = create(1, D);
  zero(*denom);
  for (int j = 0; j < D0; j++) {
    for (int i = 0; i < N; i++) {
      if (isnan(get(*y, i, j))) {
        put(*obs, 0.0, i, j);
        put(*y, 0.0, i, j);
      } else {
        put(*obs, 1.0, i, j);
        put(*denom, get(*denom, 0, j) + 1, 0, j);
      }
    }
  }

  for (int j = 0; j < D0; j++) {
    if (get(*denom, 0, j) < 0.5) {
      error("Values all missing for j-th phenotype");
    }
  }

  for (int j = 0; j < D0; j++) {
    double mu = 0.0;
    for (int i = 0; i < N; i++) {
      mu += get(*y, i, j);
    }
    mu /= get(*denom, 0, j);
    for (int i = 0; i < N; i++) {
      put(*y, get(*obs, i, j) * (get(*y, i, j) - mu), i, j);
    }
  }

  for (int j = D0; j < D; j++) {
    for (int i = 0; i < N; i++) {
      put(*obs, 1.0, i, j);
    }
    put(*denom, N, 0, j);
  }
}

void load_phenotypes(char *fname, t_matrix *y, t_matrix *obs, t_matrix *denom) {
  *y = load(fname);
  int N = y->N;
  int D = y->D;
  *obs = create(N, D);
  *denom = create(1, D);
  zero(*denom);
  for (int j = 0; j < D; j++) {
    for (int i = 0; i < N; i++) {
      if (isnan(get(*y, i, j))) {
        put(*obs, 0.0, i, j);
        put(*y, 0.0, i, j);
      } else {
        put(*obs, 1.0, i, j);
        put(*denom, get(*denom, 0, j) + 1, 0, j);
      }
    }
  }

  for (int j = 0; j < D; j++) {
    if (get(*denom, 0, j) < 0.5) {
      error("Values all missing for j-th phenotype");
    }
  }

  for (int j = 0; j < D; j++) {
    double mu = 0.0;
    for (int i = 0; i < N; i++) {
      mu += get(*y, i, j);
    }
    mu /= get(*denom, 0, j);
    for (int i = 0; i < N; i++) {
      put(*y, get(*obs, i, j) * (get(*y, i, j) - mu), i, j);
    }
  }
}

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