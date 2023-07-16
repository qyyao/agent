#include "read_nifti.hpp"

#define RNIFTI_NIFTILIB_VERSION 2
#include "RNifti.h"


long int numSlices(const std::string& nii_filename) {

    long int numSlices = 1;

    // Load the NIfTI image from a file
    RNifti::NiftiImage image(nii_filename);

    // Get the dimensions
    std::vector<long int> dims = image.dim();

    // Calculate total number of slices
    // This is the product of the dimensions from the third (z-axis) onwards
    for (size_t i = 2; i < dims.size(); i++){
        numSlices *= dims[i];
    }

    return numSlices;
}


void loadNthSlice(const std::string& nii_filename, Eigen::MatrixXd& slice, int i) {
    // Load the NIfTI image from a file
    nifti_image* image = nifti_image_read(nii_filename.c_str(), 1);

    // Check dimensions
    if (image->ndim < 3) {
        // Error handling code goes here - you probably want to throw an exception or at least return
    }

    // Get the dimensions
    int nx = image->nx;
    int ny = image->ny;
    int nz = image->nz;

    // Total number of z-slices
    int total_slices = nz;

    // Adjust for higher dimensions, if they exist
    for (int dim = 3; dim < image->ndim; ++dim) {
        total_slices *= image->dim[dim];
    }

    // Check if the slice index is valid
    if (i < 0 || i >= total_slices) {
        // Error handling code goes here - you probably want to throw an exception or at least return
    }

    // Calculate which z-slice and which set of z-slices this index corresponds to
    int set_index = i / nz;
    int z_index = i % nz;

    // Resize the target slice to hold the data
    slice.resize(nx, ny);

    // Copy the voxel values into the slice
    double* data = static_cast<double*>(image->data);
    for (int x = 0; x < nx; ++x) {
        for (int y = 0; y < ny; ++y) {
            int idx = x + nx * (y + ny * (z_index + nz * set_index));
            slice(x, y) = data[idx];
        }
    }

    // Free the image after use
    nifti_image_free(image);
}


