// Author: zhangsongpeng

#ifndef COLMAP_SRC_BASE_TRAJECTORY_H_
#define COLMAP_SRC_BASE_TRAJECTORY_H_

#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <Eigen/Core>

#include "base/key_frame.h"
#include "base/similarity_transform.h"
#include "estimators/similarity_transform.h"
#include "util/types.h"

namespace colmap {

// Trajectory class holds all information about continuous frames.
class Trajectory {
 public:

  Trajectory();

  // Get number of objects.
  inline size_t NumKeyFrames() const;

  // Get const objects.
  inline const class KeyFrame& KeyFrame(const key_frame_t key_frame_id) const;

  // Get mutable objects.
  inline class KeyFrame& KeyFrame(const key_frame_t key_frame_id);

  // Get reference to all objects.
  inline const EIGEN_STL_UMAP(key_frame_t, class KeyFrame) & KeyFrames() const;

  inline const std::vector<key_frame_t>& KeyFrameIds() const;

  // Check whether specific object exists.
  inline bool ExistsKeyFrame(const key_frame_t key_frame_id) const;

  // Add new key_frame.
  void AddKeyFrame(class KeyFrame key_frame);

  // Apply the 3D similarity transformation to all key frames.
  void Transform(const SimilarityTransform3& tform);

  // Find specific key_frame by name. Note that this uses linear search.
  const class KeyFrame* FindKeyFrameWithName(const std::string& name) const;

  // Get const objects.
  inline const class KeyFrame& StaticFrame(const std::size_t segment_idx) const;

  // Get mutable objects.
  inline class KeyFrame& StaticFrame(const std::size_t segment_idx);

  // Get reference to all objects.
  inline const std::vector<class KeyFrame> & StaticFrames() const;

  inline const std::vector<std::pair<std::size_t, std::size_t>>& StaticSegments() const;

  void SetStaticSegments(const std::vector<std::pair<double, double>>& static_times);

  // Read data from text file.
  void Read(const std::string& path);
  void Write(const std::string& path) const;

  void ReadKeyFramesText(const std::string& path);
  void WriteKeyFramesText(const std::string& path) const;

 private:

  EIGEN_STL_UMAP(key_frame_t, class KeyFrame) key_frames_;
  EIGEN_STL_UMAP(key_frame_t, std::size_t) idx_umap_;
  std::vector<key_frame_t> id_vec_;
  std::vector<class KeyFrame> static_frames_;
  std::vector<std::pair<std::size_t, std::size_t>> static_segments_;
};

////////////////////////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////////////////////////

size_t Trajectory::NumKeyFrames() const { return key_frames_.size(); }

const class KeyFrame& Trajectory::KeyFrame(const key_frame_t key_frame_id) const {
  return key_frames_.at(key_frame_id);
}

class KeyFrame& Trajectory::KeyFrame(const key_frame_t key_frame_id) {
  return key_frames_.at(key_frame_id);
}

const EIGEN_STL_UMAP(key_frame_t, class KeyFrame) & Trajectory::KeyFrames() const {
  return key_frames_;
}

const std::vector<key_frame_t>& Trajectory::KeyFrameIds() const {
  return id_vec_;
}

const class KeyFrame& Trajectory::StaticFrame(const std::size_t segment_idx) const {
  return static_frames_[segment_idx];
}

class KeyFrame& Trajectory::StaticFrame(const std::size_t segment_idx) {
  return static_frames_[segment_idx];
}

const std::vector<class KeyFrame> & Trajectory::StaticFrames() const {
  return static_frames_;
}

const std::vector<std::pair<std::size_t, std::size_t>>& Trajectory::StaticSegments() const {
  return static_segments_;
}

bool Trajectory::ExistsKeyFrame(const key_frame_t key_frame_id) const {
  return key_frames_.find(key_frame_id) != key_frames_.end();
}

}  // namespace colmap

#endif  // COLMAP_SRC_BASE_TRAJECTORY_H_
