// readNifti_wrapper.h
#ifdef __cplusplus
extern "C" {
#endif

long int numSlices_wrapper(const char* nii_filename);
void loadNthSlice_wrapper(const char* nii_filename, t_matrix* slice, int i);

#ifdef __cplusplus
}
#endif
