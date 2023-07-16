#include <string>
#include "readNifti_wrapper.h"
#include "read_nifti.hpp"

extern "C" {
    long int numChunks_c(const char* nii_filename, long int chunk_size) {
        return numChunks(std::string(nii_filename), chunk_size);
    }

    Chunk loadChunk_c(const char* nii_filename, long int chunk_index, long int chunk_size) {
        Chunk chunk_cpp = loadChunk(std::string(nii_filename), chunk_index, chunk_size);
        
        // Copy the chunk from C++ to C
        Chunk chunk_c;
        chunk_c.length = chunk_cpp.length;
        chunk_c.data = chunk_cpp.data;

        return chunk_c;
    }
}
