#include <iostream>
#include "read_nifti.hpp"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <nifti file>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];

    // For the purpose of demonstration, use a fixed voxel index
    std::array<int, 3> voxel_idx = {26, 24, 0};
    int subject_idx = 0;  // Assuming we want to read data for the first subject

    try {
        print_xform(filename);
        // Print number of subjects
        print_all_dimensions(filename);

        double voxel_value = read_voxel(filename, voxel_idx, subject_idx);
        std::cout << "The voxel value at " << voxel_idx[0] << ", " << voxel_idx[1] << ", " << voxel_idx[2] << " for subject " << subject_idx << " is: " << voxel_value << std::endl;
        

        voxel_value = -1.28526735305;
        // Find voxels with this value
        auto voxel_indices = find_voxels_with_value(filename, voxel_value);
        std::cout << "The voxel value " << voxel_value << " is found at the following locations:" << std::endl;
        for (const auto& idx : voxel_indices) {
            std::cout << "(" << std::get<0>(idx) << ", " << std::get<1>(idx) << ", " << std::get<2>(idx) << ")" << std::endl;
        }
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }

    return 0;
}
