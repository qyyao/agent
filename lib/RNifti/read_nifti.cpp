#include "RNifti.h"  // For nifti_image struct and functions
#include "read_nifti.hpp"



long int numChunks(const std::string& nii_filename, long int chunk_size) {
    // Load the NIfTI image from a file
    RNifti::NiftiImage image(nii_filename);

    // Get the total number of voxels in the image
    std::vector<int> dims = image.dim();

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

Chunk loadChunk(const std::string& nii_filename, long int chunk_index, long int chunk_size) {
    // Load the NIfTI image from a file using RNifti
    RNifti::NiftiImage image(nii_filename);
    
    // Reorient the image to RAS
    image.reorient("RAS");

    // Get the total number of voxels
    std::vector<int> dims = image.dim();

    // find total number of voxels
    long int totalVoxels = 1;
    for (int dim : dims) {
        if (dim != 0) {
            totalVoxels *= dim;
        }
    }

    // Calculate the start and end indices for the chunk
    long int start_index = chunk_index * chunk_size;
    long int end_index = start_index + chunk_size;
    if (end_index > totalVoxels) {
        end_index = totalVoxels;
    }

    // Prepare a chunk struct
    Chunk chunk;
    chunk.size = end_index - start_index;
    chunk.data = new double[chunk.size];

    // Access the pixel data of the NIfTI image
    RNifti::NiftiImageData imageData = image.data();

    // Fill the chunk with the relevant data
    for (long int i = start_index; i < end_index; i++) {
        chunk.data[i - start_index] = imageData[i];
    }

    return chunk;
}
