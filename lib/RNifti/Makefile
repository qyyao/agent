CC=g++
CFLAGS=-I./io_files -I./io_files/niftilib -I./io_files/znzlib -I./io_files/zlib -I./io_files/RNifti

OBJS = io_files/niftilib/nifti2_io.o io_files/znzlib/znzlib.o

all: main

main: main.cpp read_nifti.cpp
	$(CC) $(CFLAGS) -o main main.cpp read_nifti.cpp $(OBJS) -lz

clean:
	rm -f *.o main
