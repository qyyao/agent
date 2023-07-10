#ifndef READ_NIFTI_HPP
#define READ_NIFTI_HPP

#include <string>
#include <Eigen/Core>

long int numSlices(const std::string& nii_filename);
void loadNthSlice(const std::string& nii_filename, Eigen::MatrixXd& slice, int i);

#endif
