#ifndef READNIFTI_WRAPPER_H
#define READNIFTI_WRAPPER_H

typedef struct {
    double* data;
    long int size;
} Chunk;

#ifdef __cplusplus
extern "C" {
#endif

long int numChunks_c(const char* nii_filename, long int chunk_size);
Chunk loadChunk_c(const char* nii_filename, long int chunk_index, long int chunk_size);

#ifdef __cplusplus
}
#endif

#endif //READNIFTI_WRAPPER_H
