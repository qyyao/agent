CC=g++
CFLAGS=-I./io_files -I./io_files/niftilib -I./io_files/znzlib -I./io_files/zlib -I./io_files/RNifti

LIB_OBJS = io_files/niftilib/nifti2_io.o io_files/znzlib/znzlib.o readNifti_wrapper.o

all: libRNifti.a

libRNifti.a: $(LIB_OBJS)
	ar rcs libRNifti.a $(LIB_OBJS)

readNifti_wrapper.o: readNifti_wrapper.c
	$(CC) $(CFLAGS) -c readNifti_wrapper.c -o readNifti_wrapper.o

clean:
	rm -f *.o libRNifti.a
