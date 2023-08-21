#include "read_nifti.hpp"

#define RNIFTI_NIFTILIB_VERSION 2
#include "RNifti.h"

extern "C" {

int is_nifti_filename(const char* nii_filename){

    const char *extension = strrchr(nii_filename, '.');
    if (extension != NULL) {
        if (strcmp(extension, ".nii") == 0 || strcmp(extension, ".hdr") == 0 || strcmp(extension, ".img") == 0) {
            return 1;
        }
        // Check for .nii.gz
        if (strcmp(extension, ".gz") == 0 && strlen(nii_filename) > 4 && strcmp(extension - 4, ".nii") == 0) {
            return 1;
        }
    }

    return 0;
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

    // 2. Bounds checking for imageData access
    long int imageDataLength = imageData.length(); // Assuming .length() method exists
    if (imageDataLength <= 0) {
        std::cerr << "Image data is empty or invalid." << std::endl;
        return;
    }
    
    //TODO: RAS
    long int voxelsPerSubject = getVoxelsPerSubject(nii_filename);

    // Fill the voxelArray with the relevant data
    // Iterate over each subject
    for (int subj = 0; subj < num_subjects; subj++) {
        // Iterate over each voxel in the chunk for the current subject
        for (long int i = startIndex; i < endIndex; i++) {
            long int idx = subj * voxelsPerSubject + i;

            // Additional bound check
            if (idx >= imageDataLength) {
                std::cerr << "Trying to access out-of-bounds index in imageData." << std::endl;
                return;
            }

            long int out_idx = subj * (endIndex - startIndex) + (i - startIndex);
            
            // 4. Ensure voxelArray has been properly allocated
            if (out_idx >= arrayLength) {
                printf("out_idx is %d, greater than arrayLength of %d\n", out_idx, arrayLength);
                std::cerr << "voxelArray index out of bounds." << std::endl;
                return;
            }
            
            voxelArray[out_idx] = imageData[idx];
        }
    }

    // Pad the rest of the array with zeros
    for (long int i = num_subjects * (endIndex - startIndex); i < arrayLength; i++) {
        voxelArray[i] = 0.0;
    }
}


void nifti_to_phenotype(const char* nii_filename, const char* phenotypes_filename) {
    // Get the number of subjects
    int numSubjects = getNumSubjects(nii_filename);

    long int voxelsPerSubject = getVoxelsPerSubject(nii_filename);

    RNifti::NiftiImage image(nii_filename);

    RNifti::NiftiImageData imageData = image.data();

    // open phenotype file
    std::ofstream phenotypeFile(phenotypes_filename);

    // write headers
    for (int voxel = 1; voxel <= voxelsPerSubject; voxel++) {
        phenotypeFile << "V" << voxel;
        if (voxel < voxelsPerSubject) {
            phenotypeFile << ' ';
        }
    }
    phenotypeFile << '\n';

    // write values
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

void phenotype_to_nifti(const char* phenotypes_filename, const char* nii_filename, int dimX, int dimY, int dimZ) {
    
    printf("reading phenotype file\n");
    std::ifstream phenotypeFile(phenotypes_filename);

    printf("phenotype file is: %s", phenotypes_filename);

    // Check if file opened correctly
    if (!phenotypeFile.is_open()) {
        std::cerr << "Error opening phenotype file: " << phenotypes_filename << std::endl;
        return;
    }

    std::vector<uint8_t> phenotypeData;  // 1D vector to store data as uint8_t
    std::string line;
    int numRows = 0;  // Variable to count number of rows

    // Skip the header row
    std::getline(phenotypeFile, line);

    while (std::getline(phenotypeFile, line)) {
        numRows++;  // Increase the row count

        std::istringstream iss(line);
        double value;
        while (iss >> value) {
            if (value < 0 || value > 255) {
                std::cerr << "Value out of uint8_t range: " << value << std::endl;
                return;
            }
            phenotypeData.push_back(static_cast<uint8_t>(value));
        }
    }
    phenotypeFile.close();

    const std::vector<long int> dim = {dimX, dimY, dimZ};
    const size_t requiredSize = dimX * dimY * dimZ;

    // Check the size of the read data
    if (phenotypeData.size() != requiredSize) {
        std::cerr << "Mismatch in expected data size: read " << phenotypeData.size() << ", expected " << requiredSize << std::endl;
        return;
    }

    // Create image object then change the values to phenotypeData
    RNifti::NiftiImage image(dim, DT_UINT8); //check DT_UINT8 source, check constructors
    image.replaceData(phenotypeData);

    image.toFile(nii_filename);

    std::cout << "Phenotypes from " << phenotypes_filename << " written to " << nii_filename << std::endl;
}



} // extern "C"

