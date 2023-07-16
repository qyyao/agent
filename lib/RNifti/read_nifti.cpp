#include "nifti1_io.h"  // For nifti_image struct and functions
#include "read_nifti.hpp"

long int numChunks(const std::string& nii_filename, long int chunk_size) {
    // Load the NIfTI image from a file
    nifti_image* image = nifti_image_read(nii_filename.c_str(), 1);

    // Get the total number of voxels in the image
    long int totalVoxels = image->nvox;

    // Free the NIfTI image structure
    nifti_image_free(image);

    // Calculate the total number of chunks
    long int totalChunks = totalVoxels / chunk_size;

    // Account for the last chunk if the total voxels are not exactly divisible by chunk size
    if (totalVoxels % chunk_size != 0) {
        totalChunks++;
    }

    return totalChunks;
}

Chunk loadChunk(const std::string& nii_filename, long int chunk_index, long int chunk_size) {
    // Load the NIfTI image from a file
    nifti_image* image = nifti_image_read(nii_filename.c_str(), 1);

    // Get the total number of voxels
    long int totalVoxels = image->nvox;

    // Check if the chunk_index is valid
    if (chunk_index < 0 || chunk_index * chunk_size >= totalVoxels) {
        // Error handling code goes here - you probably want to throw an exception or at least return
    }

    // Calculate the start and end indices for the chunk
    long int start_index = chunk_index * chunk_size;
    long int end_index = start_index + chunk_size;
    if (end_index > totalVoxels) {
        end_index = totalVoxels;
    }

    // Prepare a chunk struct
    Chunk chunk;
    chunk.length = end_index - start_index;
    chunk.data = new double[chunk.length];

    // Fill the chunk with the relevant data
    double* data = (double*) image->data;
    for (long int i = start_index; i < end_index; i++) {
        chunk.data[i - start_index] = data[i];
    }

    // Free the NIfTI image structure
    nifti_image_free(image);

    return chunk;
}
