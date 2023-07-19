#include "read_nifti.hpp"

#define RNIFTI_NIFTILIB_VERSION 2
#include "RNifti.h"

long int numChunks(const std::string& nii_filename, long int chunk_size) {
    // Load the NIfTI image from a file
    RNifti::NiftiImage image(nii_filename);

    // Get the total number of voxels in the image
    std::vector<long int> dims = image.dim();

    // find total number of voxels
    long int totalVoxels = 1;
    for (int dim : dims) {
        if (dim != 0) {
            totalVoxels *= dim;
        }
    }

    // Calculate the total number of chunks
    long int totalChunks = totalVoxels / chunk_size;

    // Account for the last chunk if the total voxels are not exactly divisible by chunk size
    if (totalVoxels % chunk_size != 0) {
        totalChunks++;
    }

    return totalChunks;
}

void loadChunk(const std::string& nii_filename, double* voxelArray, long int startIndex, long int endIndex) {
    // Load the NIfTI image from a file using RNifti
    RNifti::NiftiImage image(nii_filename);

    // Reorient the image to RAS
    //NEEDED SOMETIMES - NEEDS MORE INVESTIGATING
    //image.reorient("RAS");

    // Get the total number of voxels
    std::vector<long int> dims = image.dim();

    // find total number of voxels
    long int totalVoxels = 1;
    for (int dim : dims) {
        if (dim != 0) {
            totalVoxels *= dim;
        }
    }

    // Verify the indices
    if (startIndex < 0 || endIndex > totalVoxels || startIndex > endIndex) {
        throw std::invalid_argument("Invalid voxel data indices provided.");
    }

    // Access the pixel data of the NIfTI image
    RNifti::NiftiImageData imageData = image.data();

    // Fill the voxelArray with the relevant data
    for (long int i = startIndex; i < endIndex; i++) {
        voxelArray[i - startIndex] = imageData[i];
    }
}

