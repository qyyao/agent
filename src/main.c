// Copyright 2019, Winfield Chen and Lloyd T. Elliott.

#include <stdio.h>
#include <unistd.h>
#include "agent.h"
#include "matrix.h"

// int main(int argc, char **argv) {
//   return agent_main(argc, argv);
// }

#include "read_nifti.hpp"

// Helper function to print a chunk with voxel coordinates
void print_voxel_data_with_coordinates(double* voxelData, int chunk_index, int chunk_size) {
    printf("Chunk %d:\n", chunk_index);

    // Calculate the starting voxel index for this chunk
    long int start_voxel_index = chunk_index * chunk_size;

    for (int i = 0; i < chunk_size; ++i) {
        // Calculate the global voxel index for this voxel
        long int voxel_index = start_voxel_index + i;

        // Calculate the x, y, and z coordinates
        int x = voxel_index % 64;
        int y = (voxel_index / 64) % 64;
        int z = voxel_index / (64 * 64);

        // Print out the voxel coordinates and the voxel value
        printf("Voxel (%d, %d, %d): %f\n", x, y, z, voxelData[i]);
    }
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <nifti_file>\n", argv[0]);
        return 1;
    }
    const char* nii_filename = argv[1];
    
    // Set chunk size
    long int chunk_size = 64;

    // Get and print number of chunks
    long int chunks = numChunks(nii_filename, chunk_size);
    printf("Number of chunks: %ld\n", chunks);
    
    // Allocate the voxel data array
    double* voxelData = new double[chunk_size];

    for (int i = 18; i < 25; i++){
        // Load voxel data into the array
        loadChunk(nii_filename, voxelData, i * chunk_size, (i + 1) * chunk_size);

        printf("%dth chunk:\n", i);
        print_voxel_data_with_coordinates(voxelData, i, chunk_size); // Use the new function here
    }

    // Deallocate the voxel data array
    delete[] voxelData;

    return 0;
}
