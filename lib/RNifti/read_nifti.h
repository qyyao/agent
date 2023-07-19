#ifdef __cplusplus
extern "C" {
#endif

long int numChunks(const char* nii_filename, long int chunk_size);
void loadChunk(const char* nii_filename, double* voxelArray, long int startIndex, long int endIndex);

#ifdef __cplusplus
}
#endif
