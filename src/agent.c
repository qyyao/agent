// Copyright 2019, Winfield Chen and Lloyd T. Elliott.

#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include "threadpipe.h"
#include "writethread.h"
#include "computethread.h"
#include "readthread.h"
#include "bgenformat.h"
#include "file.h"
#include "options.h"
#include "errors.h"
#include "read.h"
#include "write.h"
#include "memman.h"
#include "matrix.h"
#include "phenotype.h"
#include "statistics.h"
#include "read_nifti.h"
// agent.c


void loop_each_variant(FILE* inputFile, FILE* outputFile, char *output_dir, t_phenotype *phenos, BgenHeader *bgenHeader, AgentHeader *agentHeader, uint16_t computeThreads, int action) {
    assert(!(bgenHeader == NULL && agentHeader == NULL));
    uint32_t m;
    int header_bgen;
    if (bgenHeader != NULL) {
        m = bgenHeader->m;
        header_bgen = 1;
    } else {
        m = agentHeader->m;
        header_bgen = 0;
    }
    int header_agent = !header_bgen;
	// I/O Threads
	pthread_t write_thread;
	pthread_t read_thread;
	// Compute Threads
	pthread_t* compute_threads = create_buf(
			sizeof(compute_threads[0]), computeThreads);
	// Thread pipes
	ThreadPipe write_pipe;
	ThreadPipe read_pipe;
	// Start thread pipes
	initialize_threadpipe(&write_pipe);
	initialize_threadpipe(&read_pipe);
	// I/O Thread arguments
	WriteThreadArgs write_args = {
	        .agentHeader = agentHeader,
	        .pipe_in = &write_pipe,
	        .outputFile = outputFile,
	        .m = m,
	        .is_agent = header_agent,
	        .action = action
	};
	ReadThreadArgs read_args = {
	        .pipe_out = &read_pipe,
	        .inputFile = inputFile,
	        .m = m,
	        .is_zstd = header_bgen,
	        .is_agent = header_agent
	};
	// Compute Threads arguments
	ComputeThreadArgs compute_args = {
	        .agentHeader = agentHeader,
		.pipe_in = &read_pipe,
		.pipe_out = &write_pipe,
		.is_zstd = header_bgen,
		.is_agent = header_agent,
		.action = action,
                .output_dir = output_dir,
                .phenos = phenos
	};
	// Start threads
	initializeargs_write_thread(&write_thread, &write_args);
	for (uint16_t i = 0; i < computeThreads; i += 1) {
		initializeargs_compute_thread(&compute_threads[i], &compute_args);
	}
	initializeargs_read_thread(&read_thread, &read_args);
	// Wait for and cleanup threads
	destroy_write_thread(&write_thread, &write_args);
	for (uint16_t i = 0; i < computeThreads; i += 1) {
		destroy_compute_thread(&compute_threads[i], &compute_args);
	}
	destroy_read_thread(&read_thread, &read_args);
	destroy_buf(compute_threads);
	// Cleanup thread pipes
	destroy_threadpipe(&write_pipe);
	destroy_threadpipe(&read_pipe);
}

void close_files(FILE* inputFile, FILE* outputFile) {
	close_file(inputFile);
	close_file(outputFile);
}

int agent_main(int argc, char** argv) {
        int action = -1;
	char* inputBgenFile = NULL;
        char* inputGenFile = NULL;
	char* inputA1File = NULL;
	char* outputA1File = NULL;
	char* inputPhenotypeFile = NULL;
	char* inputPhenotypeDirectory = NULL;
	char* outputGwasDirectory = NULL;
	char* outputDosageFile = NULL;
	uint16_t computeThreads = 1;

	read_options(
          argc,
          argv,
          &action,
          &inputBgenFile,
          &inputGenFile,
          &inputA1File,
          &outputA1File,
          &inputPhenotypeFile,
          &outputGwasDirectory,
          &outputDosageFile,
          &computeThreads);

	BgenHeader bgenHeader;
	AgentHeader* agentHeader = NULL;

        FILE *inputFile =  (FILE *)NULL;
        FILE *outputFile = (FILE *)NULL;

        switch (action) {
          case ACT_BGA1: {
            inputFile = open_file(inputBgenFile, "r");
            outputFile = open_file(outputA1File, "w");
            read_bgen_header(inputFile, &bgenHeader);
            validate_bgen_header(&bgenHeader);
            agentHeader = create_agent_header_bgen(&bgenHeader);
            write_agent_header(outputFile, agentHeader);
            loop_each_variant(inputFile, outputFile, outputGwasDirectory, NULL, &bgenHeader, agentHeader, computeThreads, action);
            write_agent_header(outputFile, agentHeader);
            destroy_agent_header(agentHeader);
            close_files(inputFile, outputFile);
            break;
          }
          
          case ACT_GWA1: {

            assert(inputA1File != NULL);
            assert(inputGenFile == NULL);
            assert(inputBgenFile == NULL);
            assert(outputGwasDirectory != NULL);
            assert(inputPhenotypeFile != NULL); 
            t_matrix y;
            t_matrix yt;
            t_matrix obs;
            t_matrix denom;

            int N = numSamples(inputA1File);
            int M = numVariants(inputA1File);

            int test = 1;

            if (is_nifti_filename(inputPhenotypeFile)){

              // phenotype_to_nifti('/home/annieyao/projects/def-lelliott/annieyao/projects/agent2/agent/association_test/phenotypes.txt', '/home/annieyao/projects/def-lelliott/annieyao/projects/agent2/agent/association_test/nifti_result.nii');

              // const char* nii_filename = "/home/annieyao/projects/def-lelliott/annieyao/projects/agent2/agent/association_test/nifti_result.nii";

              // // 1. Print the number of subjects
              // int numSubjects = getNumSubjects(nii_filename);
              // std::cout << "Number of subjects: " << numSubjects << std::endl;

              // // 2. Print the number of voxels per subject
              // long int voxelsPerSubject = getVoxelsPerSubject(nii_filename);
              // std::cout << "Number of voxels per subject: " << voxelsPerSubject << std::endl;

              // // 3. Print the first chunk with size 10
              // double voxelArray[10 * numSubjects]; // Assuming the chunk size is 10 and we'll extract for all subjects
              // loadChunk(nii_filename, voxelArray, numSubjects, 0, 10, 10 * numSubjects);
              
              // std::cout << "First chunk (size 10):" << std::endl;
              // for (int subj = 0; subj < numSubjects; subj++) {
              //     for (int i = 0; i < 10; i++) {
              //         long int idx = subj * 10 + i; // This is the index in our voxelArray
              //         std::cout << voxelArray[idx] << " ";
              //     }
              //     std::cout << std::endl; // New line per subject
              // }              

              int numSubjects = getNumSubjects(inputPhenotypeFile);

              int chunkSize = (sysconf(_SC_PAGESIZE) / 8) * 5; //Hhow big?

              long int numChunks = getNumChunks(inputPhenotypeFile, chunkSize);
              long int arraySize = chunkSize * numSubjects;
              double* voxelArray = (double*)malloc(arraySize * sizeof(double));

              int D0 = chunkSize;
              int D;

              if (D0 % (sysconf(_SC_PAGESIZE) / 8) == 0) { 
                D = D0;
              } else {
                D = (D0 / (sysconf(_SC_PAGESIZE) / 8) + 1) * (sysconf(_SC_PAGESIZE) / 8);
              }
              
              for (int i = 0; i < numChunks; i++){

                long int startIndex = i * chunkSize;
                long int endIndex = (i + 1) * chunkSize;

                loadChunk(inputPhenotypeFile, voxelArray, numSubjects, startIndex, endIndex, arraySize);
                load_phenotypes2_voxels(voxelArray, &y, &obs, &denom, N, numSubjects, D0, D); //why both N and numSubjects?

                t_phenotype phenos;
                yt = create(D, N);
                for (int i = 0; i < N; i++) {
                  for (int j = 0; j < D; j++) {
                    yt.X[D * i + j] = y.X[N * j + i];
                  }
                }
                phenos.y = &y;
                phenos.yt = &yt;
                phenos.obs = &obs;
                phenos.denom = &denom;
                if (D <= 0) {
                  error("No phenotypes provided");
                }
                //if ((D * 8) % sysconf(_SC_PAGESIZE) != 0) {
                //  error("Number of phenotypes must be a multiple of the page size divided by 8");
                //}
                if (N <= 2) { 
                  error("Phenotypes describe two or fewer subjects"); 
                }

                char subdirectory[PATH_MAX];
                snprintf(subdirectory, sizeof(subdirectory), "%s/chunk_%04d", outputGwasDirectory, i); 
                mkdir(subdirectory, 0755);

                char *fi_list[] = { subdirectory, "info.txt", NULL };
                char *fi_name = join(fi_list, "/");
                FILE *fi_file = fopen(fi_name, "w");
                if (fi_file == NULL) {
                  error("Error opening file");
                }
                free(fi_name);
                fprintf(fi_file, "D = %d\nD0 = %d\nM = %d\nN = %d\n", D, D0, M, N);
                fclose(fi_file);

                char *fu_list[] = { subdirectory, "unpack", NULL };
                char *fu_name = join(fu_list, "/");
                FILE *fu_file = fopen(fu_name, "w");
                if (fu_file == NULL) {
                  error("Error opening file");
                }
                #include "unpack.h" 
                if (fwrite(unpack, sizeof(unsigned char), unpack_len, fu_file) != unpack_len) {
                  error("Could not write unpacking script");
                }
                fclose(fu_file);
                chmod(fu_name, S_IRWXU); //make obs zero for zero padded values
                free(fu_name);

                inputFile = open_file(inputA1File, "r");
                char *fl_list[] = { subdirectory, "log.txt", NULL };
                char *fl_name = join(fl_list, "/");
                outputFile = open_file(fl_name, "w");
                if (outputFile == NULL) {
                  error("Error opening file");
                }
                free(fl_name);

                //initialize command and z
                char command[PATH_MAX + 50]; 
                int z;  

                // start with beta.bin
                char *fb_list[] = { subdirectory, "beta.bin", NULL };
                char *fb_name = join(fb_list, "/");

                off64_t size = (off64_t)D * (off64_t)M * (off64_t)sizeof(double);
                // printf("%d %d %d %lld %d\n", D, M, sizeof(double), (off64_t)D*(off64_t)M*(off64_t)sizeof(double), sizeof(off64_t));
                // printf("size = %lld\n", size);
                system("echo -n $(date)");
                printf(" Allocating beta.bin\n");

                //execute fallocate unix command
                snprintf(command, sizeof(command), "fallocate -x -l %lld %s", (long long)size, fb_name); //fallocate -x -l [size] [file_path]
                z = system(command);

                if (z != 0) {
                  fprintf(stderr, "fallocate failed: %d\n", z);
                  error("Could not resize output file");
                }
                free(fb_name);

                //se.bin
                char *fs_list[] = { subdirectory, "se.bin", NULL };
                char *fs_name = join(fs_list, "/");

                system("echo -n $(date)");
                printf(" Allocating se.bin\n");
                snprintf(command, sizeof(command), "fallocate -x -l %lld %s", (long long)size, fs_name); //fallocate -x -l [size] [file_path]
                z = system(command);
                if (z != 0) {
                  error("Could not resize output file");
                }
                free(fs_name);

                //tstat.bin
                char *ft_list[] = { subdirectory, "tstat.bin", NULL };
                char *ft_name = join(ft_list, "/");

                system("echo -n $(date)");
                printf(" Allocating tstat.bin\n");
                snprintf(command, sizeof(command), "fallocate -x -l %lld %s", (long long)size, ft_name); //fallocate -x -l [size] [file_path]
                z = system(command);
                if (z != 0) {
                  error("Could not resize output file");
                }
                free(ft_name);

                //pval.bin
                char *fp_list[] = { subdirectory, "pval.bin", NULL };
                char *fp_name = join(fp_list, "/");
                system("echo -n $(date)");
                printf(" Allocating pval.bin\n");
                snprintf(command, sizeof(command), "fallocate -x -l %lld %s", (long long)size, fp_name); //fallocate -x -l [size] [file_path]
                z = system(command);
                if (z != 0) {
                  error("Could not resize output file");
                }
                free(fp_name);
                
                uint64_t l0 = read_l0(inputFile);
                agentHeader = create_agent_header(l0);
                read_agent_header(inputFile, agentHeader);
                validate_agent_header(agentHeader);
                loop_each_variant(inputFile, outputFile, subdirectory, &phenos, NULL, agentHeader, computeThreads, action);
                destroy_agent_header(agentHeader);
                close_files(inputFile, outputFile);
                destroy(*phenos.y);
                destroy(*phenos.yt);
                destroy(*phenos.obs);
                destroy(*phenos.denom);
                system("echo -n $(date)");
                printf(" Allocation complete\n");
              }
            
            free(voxelArray);
            
            }

            else{

              int D0 = numPhenotypes(inputPhenotypeFile); 
              int D;
              if (D0 % (sysconf(_SC_PAGESIZE) / 8) == 0) { //make chunk size satisfy this
                D = D0;
              } else {
                D = (D0 / (sysconf(_SC_PAGESIZE) / 8) + 1) * (sysconf(_SC_PAGESIZE) / 8);
              }
              load_phenotypes2(inputPhenotypeFile, &y, &obs, &denom, N, D0, D); 

              t_phenotype phenos;
              yt = create(D, N);
              for (int i = 0; i < N; i++) {
                for (int j = 0; j < D; j++) {
                  yt.X[D * i + j] = y.X[N * j + i];
                }
              }
              phenos.y = &y;
              phenos.yt = &yt;
              phenos.obs = &obs;
              phenos.denom = &denom;
              if (D <= 0) {
                error("No phenotypes provided");
              }
              //if ((D * 8) % sysconf(_SC_PAGESIZE) != 0) {
              //  error("Number of phenotypes must be a multiple of the page size divided by 8");
              //}
              if (N <= 2) {
                error("Phenotypes describe two or fewer subjects");
              }
              
              char *fi_list[] = { outputGwasDirectory, "info.txt", NULL };
              char *fi_name = join(fi_list, "/");
              FILE *fi_file = fopen(fi_name, "w");
              if (fi_file == NULL) {
                error("Error opening file");
              }
              free(fi_name);
              fprintf(fi_file, "D = %d\nD0 = %d\nM = %d\nN = %d\n", D, D0, M, N);
              fclose(fi_file);

              char *fu_list[] = { outputGwasDirectory, "unpack", NULL };
              char *fu_name = join(fu_list, "/");
              FILE *fu_file = fopen(fu_name, "w");
              if (fu_file == NULL) {
                error("Error opening file");
              }
              #include "unpack.h" 
              if (fwrite(unpack, sizeof(unsigned char), unpack_len, fu_file) != unpack_len) {
                error("Could not write unpacking script");
              }
              fclose(fu_file);
              chmod(fu_name, S_IRWXU); //make obs zero for zero padded values
              free(fu_name);

              inputFile = open_file(inputA1File, "r");
              char *fl_list[] = { outputGwasDirectory, "log.txt", NULL };
              char *fl_name = join(fl_list, "/");
              outputFile = open_file(fl_name, "w");
              if (outputFile == NULL) {
                error("Error opening file");
              }
              free(fl_name);
                              //initialize command and z
              char command[PATH_MAX + 50]; 
              int z;
              // start with beta.bin
              char *fb_list[] = { outputGwasDirectory, "beta.bin", NULL };
              char *fb_name = join(fb_list, "/");

              off64_t size = (off64_t)D * (off64_t)M * (off64_t)sizeof(double);
              // printf("%d %d %d %lld %d\n", D, M, sizeof(double), (off64_t)D*(off64_t)M*(off64_t)sizeof(double), sizeof(off64_t));
              // printf("size = %lld\n", size);
              system("echo -n $(date)");
              printf(" Allocating beta.bin\n");

              //execute fallocate unix command
              snprintf(command, sizeof(command), "fallocate -x -l %lld %s", (long long)size, fb_name); //fallocate -x -l [size] [file_path]
              z = system(command);

              if (z != 0) {
                fprintf(stderr, "fallocate failed: %d\n", z);
                error("Could not resize output file");
              }
              free(fb_name);

              //se.bin
              char *fs_list[] = { outputGwasDirectory, "se.bin", NULL };
              char *fs_name = join(fs_list, "/");

              system("echo -n $(date)");
              printf(" Allocating se.bin\n");
              snprintf(command, sizeof(command), "fallocate -x -l %lld %s", (long long)size, fs_name); //fallocate -x -l [size] [file_path]
              z = system(command);
              if (z != 0) {
                error("Could not resize output file");
              }
              free(fs_name);

              //tstat.bin
              char *ft_list[] = { outputGwasDirectory, "tstat.bin", NULL };
              char *ft_name = join(ft_list, "/");

              system("echo -n $(date)");
              printf(" Allocating tstat.bin\n");
              snprintf(command, sizeof(command), "fallocate -x -l %lld %s", (long long)size, ft_name); //fallocate -x -l [size] [file_path]
              z = system(command);
              if (z != 0) {
                error("Could not resize output file");
              }
              free(ft_name);

              //pval.bin
              char *fp_list[] = { outputGwasDirectory, "pval.bin", NULL };
              char *fp_name = join(fp_list, "/");
              system("echo -n $(date)");
              printf(" Allocating pval.bin\n");
              snprintf(command, sizeof(command), "fallocate -x -l %lld %s", (long long)size, fp_name); //fallocate -x -l [size] [file_path]
              z = system(command);
              if (z != 0) {
                error("Could not resize output file");
              }
              free(fp_name);

              uint64_t l0 = read_l0(inputFile);
              agentHeader = create_agent_header(l0);
              read_agent_header(inputFile, agentHeader);
              validate_agent_header(agentHeader);
              loop_each_variant(inputFile, outputFile, outputGwasDirectory, &phenos, NULL, agentHeader, computeThreads, action);
              destroy_agent_header(agentHeader);
              close_files(inputFile, outputFile);
              destroy(*phenos.y);
              destroy(*phenos.yt);
              destroy(*phenos.obs);
              destroy(*phenos.denom);
              system("echo -n $(date)");
              printf(" Allocation complete\n");
            }
            break;
          }

          case ACT_DOSE: {
              if (outputDosageFile == NULL) {
                  error("No dosage file");
              }
              outputFile = open_file(outputDosageFile, "w");
              if (inputBgenFile) {
                  inputFile = open_file(inputBgenFile, "r");
                  read_bgen_header(inputFile, &bgenHeader);
                  validate_bgen_header(&bgenHeader);
                  loop_each_variant(inputFile, outputFile, outputGwasDirectory, NULL, &bgenHeader, agentHeader, computeThreads, action);
                  close_files(inputFile, outputFile);
              } else if (inputA1File) {
                  inputFile = open_file(inputA1File, "r");
                  uint64_t l0 = read_l0(inputFile);
                  agentHeader = create_agent_header(l0);
                  read_agent_header(inputFile, agentHeader);
                  validate_agent_header(agentHeader);
                  loop_each_variant(inputFile, outputFile, outputGwasDirectory, NULL, NULL, agentHeader, computeThreads, action);
                  destroy_agent_header(agentHeader);
                  close_files(inputFile, outputFile);
              } else if (inputGenFile) {
                  error("Unimplemented");
              } else {
                  error("No genotype file");
              }
              break;
          }

          case ACT_GWGE:
          case ACT_GWBG: {
            error("Unimplemented action specified");
            break;
          }

          default: {
            error("Could not determine action from command line");
            break;
          }
        }
        system("echo -n $(date)");
        printf(" Done\n");
	return 0;
}
