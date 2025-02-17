// Author: zhangsongpeng

#ifndef COLMAP_SRC_BASE_CAMERA_CALIB_H_
#define COLMAP_SRC_BASE_CAMERA_CALIB_H_

#include <unordered_map>
#include <vector>

#include "base/camera.h"
#include "base/pose.h"
#include "base/reconstruction.h"
#include "util/alignment.h"
#include "util/types.h"

namespace colmap {

struct CalibCamera {
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  Eigen::Vector4d rel_qvec = ComposeIdentityQuaternion();
  Eigen::Vector3d rel_tvec = Eigen::Vector3d(0, 0, 0);
  double rel_time = 0.0;
  Eigen::Vector4d qvec_prior = ComposeIdentityQuaternion();
  Eigen::Vector3d tvec_prior = Eigen::Vector3d(0, 0, 0);
  double time_prior = 0.0;
  std::string name;
  std::vector<std::pair<image_t, double>> images;
  std::vector<std::vector<key_frame_t>> key_frames;
  std::vector<std::pair<Eigen::Vector4d, Eigen::Vector3d>> key_poses;

  void sort() {
    
  }
};

// This class holds information about the camera calibration.
class CameraCalib {
 public:
  CameraCalib();

  // The number of cameras in the calibration.
  size_t NumCameras() const;

  // Get const objects.
  inline const class CalibCamera& Camera(const camera_t camera_id) const;

  // Get mutable objects.
  inline class CalibCamera& Camera(const camera_t camera_id);

  inline const EIGEN_STL_UMAP(camera_t, class CalibCamera) & Cameras() const;

  // Check whether the given camera is part of the calibration.
  bool HasCamera(const camera_t camera_id) const;

  // Get the identifiers of the cameras in the calibration.
  std::vector<camera_t> GetCameraIds() const;

  // Add a new camera to the calibration. The relative pose may contain dummy values and
  // can then be computed automatically from a given reconstruction using the
  // method `ComputeRelativePoses`.
  void AddCamera(const camera_t camera_id, const Eigen::Vector4d& rel_qvec,
                 const Eigen::Vector3d& rel_tvec, const double rel_time,
                 const std::string& camera_name);

  // Check whether the camera calibration setup is valid.
  void Check(const Reconstruction& reconstruction) const;

  // Get the relative poses of the cameras in the calibration.
  Eigen::Vector4d& RelativeQvec(const camera_t camera_id);
  const Eigen::Vector4d& RelativeQvec(const camera_t camera_id) const;
  Eigen::Vector3d& RelativeTvec(const camera_t camera_id);
  const Eigen::Vector3d& RelativeTvec(const camera_t camera_id) const;
  double& RelativeTime(const camera_t camera_id);
  const double& RelativeTime(const camera_t camera_id) const;

  // Read data from text file.
  void Read(const std::string& path);
  void Write(const std::string& path) const;

  void ReadCalibJson(const std::string& path);
  void WriteCalibJson(const std::string& path) const;

  void PrintInfo() const;

  EIGEN_STL_UMAP(camera_t, CalibCamera) calib_cameras_;
  double rel_time_ = 0.0;
  double time_prior_ = 0.0;
};

const class CalibCamera& CameraCalib::Camera(const camera_t camera_id) const {
  return calib_cameras_.at(camera_id);
}

class CalibCamera& CameraCalib::Camera(const camera_t camera_id) {
  return calib_cameras_.at(camera_id);
}

const EIGEN_STL_UMAP(camera_t, class CalibCamera) & CameraCalib::Cameras() const {
  return calib_cameras_;
}

}  // namespace colmap

#endif  // COLMAP_SRC_BASE_CAMERA_CALIB_H_
