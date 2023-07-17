CC=g++
CFLAGS=-I./io_files -I./io_files/niftilib -I./io_files/znzlib -I./io_files/zlib -I./io_files/RNifti 

LIB_OBJS = io_files/niftilib/nifti1_io.o io_files/niftilib/nifti2_io.o io_files/znzlib/znzlib.o read_nifti.o read_nifti_wrapper.o

all: libRNifti.a

libRNifti.a: $(LIB_OBJS)
	ar rcs libRNifti.a $(LIB_OBJS)

io_files/niftilib/nifti1_io.o: io_files/niftilib/nifti1_io.c
	$(CC) $(CFLAGS) -c $< -o $@

io_files/niftilib/nifti2_io.o: io_files/niftilib/nifti2_io.c
	$(CC) $(CFLAGS) -c $< -o $@

read_nifti.o: read_nifti.cpp read_nifti.hpp
	$(CC) $(CFLAGS) -c read_nifti.cpp -o read_nifti.o

read_nifti_wrapper.o: read_nifti_wrapper.cpp read_nifti_wrapper.h read_nifti.hpp
	$(CC) $(CFLAGS) -c read_nifti_wrapper.cpp -o read_nifti_wrapper.o

clean:
	rm -f *.o libRNifti.a
