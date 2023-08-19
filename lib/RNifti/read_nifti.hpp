#ifndef READ_NIFTI_HPP
#define READ_NIFTI_HPP

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <stdexcept>

extern "C" {
    int is_nifti_filename(const char* nii_filename);
    long int getVoxelsPerSubject(const char* nii_filename);
    long int getNumChunks(const char* nii_filename, long int chunk_size);
    int getNumSubjects(const char* nii_filename);
    void loadChunk(const char* nii_filename, double* voxelArray, int num_subjects, long int startIndex, long int endIndex, long int arrayLength);
    void phenotype_to_nifti(const char* phenotypes_filename, const char* nii_filename, int dimX, int dimY, int dimZ);

}

#endif //READ_NIFTI_HPP
