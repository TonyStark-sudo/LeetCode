#pragma once

#include "pose.h"

#include <string>
#include <filesystem>
#include <nlohmann/json.hpp>


class Projector {
 public:
    Projector(const std::string &config_path);
    template <typename... Paths> 
    std::filesystem::path JoinPaths(const std::filesystem::path& base, const Paths&... parts) {
        std::filesystem::path result = base; 
        (result /= ... /= parts); 
        return result; 
    }                                                                                          
    void Project();
 private:
    float* mat3x3_to_pointer(const Eigen::Matrix3d& K);
    float* vec3x1_to_pointer(const Eigen::Vector3d& v);
    std::vector<PoseStamped> ReadPoses(const std::string &pose_path);
    nlohmann::json config_;
};
