// Author: zhangsongpeng

#ifndef COLMAP_SRC_BASE_KEY_FRAME_H_
#define COLMAP_SRC_BASE_KEY_FRAME_H_

#include <string>
#include <vector>

#include <Eigen/Core>

#include "util/logging.h"
#include "util/math.h"
#include "util/types.h"

namespace colmap {

// Class that holds information about a key frame.
class KeyFrame {
 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  KeyFrame();

  // Access the unique identifier of the key frame.
  inline key_frame_t KeyFrameId() const;
  inline void SetKeyFrameId(const key_frame_t key_frame_id);

  // Access the name of the key frame.
  inline const std::string& Name() const;
  inline std::string& Name();
  inline void SetName(const std::string& name);

  inline const double& Timestamp() const;
  inline double& Timestamp();
  inline void SetTimestamp(const double timestamp);

  // Access quaternion vector as (qw, qx, qy, qz) specifying the rotation of the
  // pose which is defined as the transformation from key frame to world space.
  inline const Eigen::Vector4d& Qvec() const;
  inline Eigen::Vector4d& Qvec();
  inline double Qvec(const size_t idx) const;
  inline double& Qvec(const size_t idx);
  inline void SetQvec(const Eigen::Vector4d& qvec);

  // Quaternion prior, e.g. given by ins or slam.
  inline const Eigen::Vector4d& QvecPrior() const;
  inline Eigen::Vector4d& QvecPrior();
  inline double QvecPrior(const size_t idx) const;
  inline double& QvecPrior(const size_t idx);
  inline bool HasQvecPrior() const;
  inline void SetQvecPrior(const Eigen::Vector4d& qvec);

  // Access translation vector as (tx, ty, tz) specifying the translation of the
  // pose which is defined as the transformation from key frame to world space.
  inline const Eigen::Vector3d& Tvec() const;
  inline Eigen::Vector3d& Tvec();
  inline double Tvec(const size_t idx) const;
  inline double& Tvec(const size_t idx);
  inline void SetTvec(const Eigen::Vector3d& tvec);

  // Translation prior, e.g. given by ins or slam.
  inline const Eigen::Vector3d& TvecPrior() const;
  inline Eigen::Vector3d& TvecPrior();
  inline double TvecPrior(const size_t idx) const;
  inline double& TvecPrior(const size_t idx);
  inline bool HasTvecPrior() const;
  inline void SetTvecPrior(const Eigen::Vector3d& tvec);

  // Normalize the quaternion vector.
  void NormalizeQvec();

  // Compose the projection matrix from key frame to world space.
  Eigen::Matrix3x4d ProjectionMatrix() const;

  // Compose the inverse projection matrix from key frame to world space
  Eigen::Matrix3x4d InverseProjectionMatrix() const;

  // Compose rotation matrix from quaternion vector.
  Eigen::Matrix3d RotationMatrix() const;

  // Extract the projection center in world space.
  Eigen::Vector3d ProjectionCenter() const;

 private:
  // Identifier of the key frame, if not specified `kInvalidKeyFrameId`.
  key_frame_t key_frame_id_;

  // The name of the key frame, i.e. the timestamp string.
  std::string name_;
  double timestamp_;

  // The pose of the key frame, defined as the transformation from key frame to world.
  Eigen::Vector4d qvec_;
  Eigen::Vector3d tvec_;

  // The pose prior of the key frame, e.g. extracted from ins or slam.
  Eigen::Vector4d qvec_prior_;
  Eigen::Vector3d tvec_prior_;
};

////////////////////////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////////////////////////

key_frame_t KeyFrame::KeyFrameId() const { return key_frame_id_; }

void KeyFrame::SetKeyFrameId(const key_frame_t key_frame_id) { key_frame_id_ = key_frame_id; }

const std::string& KeyFrame::Name() const { return name_; }

std::string& KeyFrame::Name() { return name_; }

void KeyFrame::SetName(const std::string& name) { name_ = name; }

const double& KeyFrame::Timestamp() const { return timestamp_; }

double& KeyFrame::Timestamp() { return timestamp_; }

void KeyFrame::SetTimestamp(const double timestamp) { timestamp_ = timestamp; }

const Eigen::Vector4d& KeyFrame::Qvec() const { return qvec_; }

Eigen::Vector4d& KeyFrame::Qvec() { return qvec_; }

inline double KeyFrame::Qvec(const size_t idx) const { return qvec_(idx); }

inline double& KeyFrame::Qvec(const size_t idx) { return qvec_(idx); }

void KeyFrame::SetQvec(const Eigen::Vector4d& qvec) { qvec_ = qvec; }

const Eigen::Vector4d& KeyFrame::QvecPrior() const { return qvec_prior_; }

Eigen::Vector4d& KeyFrame::QvecPrior() { return qvec_prior_; }

inline double KeyFrame::QvecPrior(const size_t idx) const {
  return qvec_prior_(idx);
}

inline double& KeyFrame::QvecPrior(const size_t idx) { return qvec_prior_(idx); }

inline bool KeyFrame::HasQvecPrior() const { return !IsNaN(qvec_prior_.sum()); }

void KeyFrame::SetQvecPrior(const Eigen::Vector4d& qvec) { qvec_prior_ = qvec; }

const Eigen::Vector3d& KeyFrame::Tvec() const { return tvec_; }

Eigen::Vector3d& KeyFrame::Tvec() { return tvec_; }

inline double KeyFrame::Tvec(const size_t idx) const { return tvec_(idx); }

inline double& KeyFrame::Tvec(const size_t idx) { return tvec_(idx); }

void KeyFrame::SetTvec(const Eigen::Vector3d& tvec) { tvec_ = tvec; }

const Eigen::Vector3d& KeyFrame::TvecPrior() const { return tvec_prior_; }

Eigen::Vector3d& KeyFrame::TvecPrior() { return tvec_prior_; }

inline double KeyFrame::TvecPrior(const size_t idx) const {
  return tvec_prior_(idx);
}

inline double& KeyFrame::TvecPrior(const size_t idx) { return tvec_prior_(idx); }

inline bool KeyFrame::HasTvecPrior() const { return !IsNaN(tvec_prior_.sum()); }

void KeyFrame::SetTvecPrior(const Eigen::Vector3d& tvec) { tvec_prior_ = tvec; }

}  // namespace colmap

EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION_CUSTOM(colmap::KeyFrame)

#endif  // COLMAP_SRC_BASE_KEY_FRAME_H_
