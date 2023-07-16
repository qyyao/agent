#ifndef READ_NIFTI_HPP
#define READ_NIFTI_HPP

#include <string>

struct Chunk {
    double* data;
    long int size;
};

long int numChunks(const std::string& nii_filename, long int chunk_size);

Chunk loadChunk(const std::string& nii_filename, long int chunk_index, long int chunk_size);

#endif //READ_NIFTI_HPP
