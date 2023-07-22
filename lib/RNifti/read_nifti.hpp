#ifndef READ_NIFTI_HPP
#define READ_NIFTI_HPP

#include <string>

extern "C" {
    int is_nifti_file_wrapper(const char* nii_filename);
    long int getVoxelsPerSubject(const char* nii_filename);
    long int numChunks(const char* nii_filename, long int chunk_size);
    int getNumSubjects(const char* nii_filename);
    void loadChunk(const char* nii_filename, double* voxelArray, int num_subjects, long int startIndex, long int endIndex, long int arrayLength);
}

#endif //READ_NIFTI_HPP
