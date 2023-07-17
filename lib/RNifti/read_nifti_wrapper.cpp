// read_nifti_wrapper.cpp

#include "read_nifti.hpp"
#include "read_nifti_wrapper.h"

extern "C" {

    long int numChunks_c(const char* nii_filename, long int chunk_size) {
        return numChunks(nii_filename, chunk_size);
    }

    Chunk_c loadChunk_c(const char* nii_filename, long int chunk_index, long int chunk_size) {
        Chunk cpp_chunk = loadChunk(nii_filename, chunk_index, chunk_size);
        
        Chunk_c c_chunk;
        c_chunk.data = cpp_chunk.data;
        c_chunk.size = cpp_chunk.size;
        
        return c_chunk;
    }
}
