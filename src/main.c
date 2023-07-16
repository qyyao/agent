// Copyright 2019, Winfield Chen and Lloyd T. Elliott.

#include <stdio.h>
#include <unistd.h>
#include "agent.h"
#include "matrix.h"

// int main(int argc, char **argv) {
//   return agent_main(argc, argv);
// }


#include "readNifti_wrapper.h"

// Helper function to print a chunk
void print_chunk(const Chunk* chunk) {
    for (int i = 0; i < chunk->size; ++i) {
        printf("%f ", chunk->data[i]);
    }
    printf("\n");
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <nifti_file>\n", argv[0]);
        return 1;
    }
    const char* nii_filename = argv[1];
    
    // Set chunk size
    long int chunk_size = 20;

    // Get and print number of chunks
    long int chunks = numChunks_c(nii_filename, chunk_size);
    printf("Number of chunks: %ld\n", chunks);
    
    // Load and print the first chunk
    if (chunks > 0) {
        Chunk chunk1;
        chunk1.data = (double*) malloc(chunk_size * sizeof(double));  // Allocate memory for chunk data
        chunk1 = loadChunk_c(nii_filename, 0, chunk_size);
        printf("First chunk:\n");
        print_chunk(&chunk1);
        free(chunk1.data); // Remember to free the dynamically allocated memory
    }

    // Load and print the second chunk
    if (chunks > 1) {
        Chunk chunk2;
        chunk2.data = (double*) malloc(chunk_size * sizeof(double));  // Allocate memory for chunk data
        chunk2 = loadChunk_c(nii_filename, 1, chunk_size);
        printf("Second chunk:\n");
        print_chunk(&chunk2);
        free(chunk2.data); // Remember to free the dynamically allocated memory
    }

    return 0;
}
