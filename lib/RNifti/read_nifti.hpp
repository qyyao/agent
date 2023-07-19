#ifndef READ_NIFTI_HPP
#define READ_NIFTI_HPP

#include <string>

extern "C" {
    long int numChunks(const char* nii_filename, long int chunk_size);
    void loadChunk(const char* nii_filename, double* voxelArray, long int startIndex, long int endIndex);
}

#endif //READ_NIFTI_HPP
