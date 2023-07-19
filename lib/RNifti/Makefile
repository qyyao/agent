CC=gcc
CXX=g++
CFLAGS=-I./io_files -I./io_files/niftilib -I./io_files/znzlib -I./io_files/zlib -I./io_files/RNifti 

LIB_OBJS = io_files/niftilib/nifti2_io.o io_files/znzlib/znzlib.o read_nifti.o 

all: libRNifti.a

libRNifti.a: $(LIB_OBJS)
	ar rcs libRNifti.a $(LIB_OBJS)

io_files/niftilib/nifti2_io.o: io_files/niftilib/nifti2_io.c
	$(CC) $(CFLAGS) -c $< -o $@

read_nifti.o: read_nifti.cpp read_nifti.hpp
	$(CXX) $(CFLAGS) -c read_nifti.cpp -o read_nifti.o

clean:
	rm -f *.o libRNifti.a
