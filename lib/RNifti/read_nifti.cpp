#include "read_nifti.hpp"

#define RNIFTI_NIFTILIB_VERSION 2
#include "RNifti.h"

extern "C" {

int is_nifti_file_wrapper(const char* nii_filename){
    return is_nifti_file(nii_filename);
}

long int getVoxelsPerSubject(const char* nii_filename){
    // Load the NIfTI image from a file
    RNifti::NiftiImage image(nii_filename);

    // Get the dimensions of the image
    std::vector<long int> dims = image.dim();

    // find total number of voxels in the first three dimensions
    long int numVoxels = 1;
    for (int i = 0; i < 3 && i < dims.size(); i++) {
        numVoxels *= dims[i];
    }

    return numVoxels;
}

long int getNumChunks(const char* nii_filename, long int chunk_size) {
    
    long int numVoxels = getVoxelsPerSubject(nii_filename);

    long int totalChunks = numVoxels/chunk_size;

    // Account for the last chunk if the total voxels are not exactly divisible by chunk size
    if (numVoxels % chunk_size != 0) {
        totalChunks++;
    }

    return totalChunks;
}

int getNumSubjects(const char* nii_filename){
    // Load the NIfTI image from a file
    RNifti::NiftiImage image(nii_filename);

    // Get the dimensions of the image
    std::vector<long int> dims = image.dim();

    //if only 3 dimensions, there is only one subject
    if (dims.size() < 4){
        return 1;
    }

    //return number of subjections
    return dims[3];
}

void loadChunk(const char* nii_filename, double* voxelArray, int num_subjects, long int startIndex, long int endIndex, long int arrayLength) {
    // Load the NIfTI image from a file using RNifti
    RNifti::NiftiImage image(nii_filename);

    // Access the pixel data of the NIfTI image
    RNifti::NiftiImageData imageData = image.data(); 
    
    //TODO: RAS

    long int voxelsPerSubject = getVoxelsPerSubject(nii_filename);

    // Fill the voxelArray with the relevant data
    // Iterate over each subject
    for (int subj = 0; subj < num_subjects; subj++) {
        // Iterate over each voxel in the chunk for the current subject
        for (long int i = startIndex; i < endIndex; i++) {
            long int idx = subj * voxelsPerSubject + i;
            long int out_idx = subj * (endIndex - startIndex) + (i - startIndex); //i-startIndex sets array to 0, endIndex - startIndex gives chunk size
            voxelArray[out_idx] = imageData[idx];
        }
    }

    // Pad the rest of the array with zeros
    for (long int i = num_subjects * (endIndex - startIndex); i < arrayLength; i++) {
        voxelArray[i] = 0.0;
    }
}

void agent_convert(const char* nii_filename, const char* phenotypes_filename) {
    // Get the number of subjects
    int numSubjects = getNumSubjects(nii_filename);

    // Get the total number of voxels per subject
    long int voxelsPerSubject = getVoxelsPerSubject(nii_filename);

    // Load the NIfTI image from a file using RNifti
    RNifti::NiftiImage image(nii_filename);

    // Access the pixel data of the NIfTI image
    RNifti::NiftiImageData imageData = image.data();

    // Open a file stream for the phenotype file, creating it if it does not exist
    std::ofstream phenotypeFile(phenotypes_filename);

    // Write the headers
    for (int voxel = 1; voxel <= voxelsPerSubject; voxel++) {
        phenotypeFile << "V" << voxel;
        if (voxel < voxelsPerSubject) {
            phenotypeFile << ' ';
        }
    }
    phenotypeFile << '\n';

    // Iterate over the subjects, and for each subject, write the corresponding value for each voxel
    for (int subj = 0; subj < numSubjects; subj++) {
        for (long int i = 0; i < voxelsPerSubject; i++) {
            long int idx = subj * voxelsPerSubject + i;
            phenotypeFile << static_cast<double>(imageData[idx]);
            if (i < voxelsPerSubject - 1) {
                phenotypeFile << ' ';
            }
        }
        phenotypeFile << '\n';
    }

    // Close the phenotype file
    phenotypeFile.close();

    std::cout << "Conversion complete. Phenotypes written to " << phenotypes_filename << std::endl;
}


} // extern "C"

