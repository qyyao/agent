#ifndef READ_NIFTI_HPP
#define READ_NIFTI_HPP

#include <string>
#include "read_nifti.hpp"

long int numChunks(const std::string& nii_filename, long int chunk_size);
void loadChunk(const std::string& nii_filename, double* voxelArray, long int startIndex, long int endIndex);

#endif //READ_NIFTI_HPP
