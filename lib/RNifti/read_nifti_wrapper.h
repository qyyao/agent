// read_nifti_wrapper.h

#ifndef READNIFTI_WRAPPER_H
#define READNIFTI_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    double* data;
    long int size;
} Chunk_c;

long int numChunks_c(const char* nii_filename, long int chunk_size);
Chunk_c loadChunk_c(const char* nii_filename, long int chunk_index, long int chunk_size);

#ifdef __cplusplus
}
#endif

#endif //READNIFTI_WRAPPER_H
