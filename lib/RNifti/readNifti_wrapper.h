// readNifti_wrapper.h
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  double *X;
  int N;
  int D;
} nifti_matrix;

long int numSlices_wrapper(const char* nii_filename);
void loadNthSlice_wrapper(const char* nii_filename, nifti_matrix* slice, int i);

#ifdef __cplusplus
}
#endif
