// Author: zhangsongpeng

#include "base/trajectory.h"

#include <fstream>

#include "base/gps.h"
#include "base/pose.h"
#include "base/projection.h"
#include "base/triangulation.h"
#include "util/bitmap.h"
#include "util/misc.h"
#include "util/ply.h"
// #include <iostream>
// #include <iomanip>

namespace colmap {

Trajectory::Trajectory() {}

void Trajectory::AddKeyFrame(class KeyFrame key_frame) {
  const key_frame_t key_frame_id = key_frame.KeyFrameId();
  CHECK(key_frames_.emplace(key_frame_id, std::move(key_frame)).second);
  idx_umap_.emplace(key_frame_id, id_vec_.size());
  id_vec_.emplace_back(key_frame_id);
}

void Trajectory::Transform(const SimilarityTransform3& tform) {
  for (auto& key_frame : key_frames_) {
    tform.TransformPose(&key_frame.second.Qvec(), &key_frame.second.Tvec());
  }
}

const class KeyFrame* Trajectory::FindKeyFrameWithName(const std::string& name) const {
  for (const auto& key_frame : key_frames_) {
    if (key_frame.second.Name() == name) {
      return &key_frame.second;
    }
  }
  return nullptr;
}

void Trajectory::Read(const std::string& path) {
  if (ExistsFile(path)) {
    ReadKeyFramesText(path);
  } else {
    LOG(FATAL) << "key_frame files do not exist at " << path;
  }
}

void Trajectory::SetStaticSegments(const std::vector<std::pair<double, double>>& static_times) {
  static_frames_.clear();
  static_segments_.clear();

  std::size_t i = 0;

  for (std::size_t j = 0; j < static_times.size(); ++j) {
    while (i < id_vec_.size()) {
      const auto& key_frame = key_frames_.at(id_vec_[i]);
      if (key_frame.Timestamp() < static_times[j].first) {
        ++i;
      } else {
        break;
      }
    }

    if (i < id_vec_.size()) {
      const auto& key_frame = key_frames_.at(id_vec_[i]);

      if (key_frame.Timestamp() >= static_times[j].first && key_frame.Timestamp() <= static_times[j].second) {
        std::pair<std::size_t, std::size_t> segment(i, i);

        while (i < id_vec_.size()) {
          const auto& key_frame2 = key_frames_.at(id_vec_[i]);
          if (key_frame2.Timestamp() <= static_times[j].second) {
            segment.second = i;
            ++i;
          } else {
            break;
          }
        }

        if (segment.first < segment.second) {
          class KeyFrame static_frame;
          static_frame.SetQvec(key_frame.Qvec());
          static_frame.SetQvecPrior(key_frame.Qvec());
          static_frame.SetTvec(key_frame.Tvec());
          static_frame.SetTvecPrior(key_frame.Tvec());
          static_frame.SetName("static_" + std::to_string(static_frames_.size()));
          static_frames_.emplace_back(static_frame);
          static_segments_.emplace_back(segment);
        }
      }
    } else {
      break;
    }
    // std::cout << std::setprecision(12) << "static times: " << j << ", " << static_times[j].first << ", " << static_times[j].second << std::endl;
  }
//   for (std::size_t j = 0; j < static_segments_.size(); ++j) {
//     const auto& key_frame = key_frames_.at(id_vec_[static_segments_[j].first]);
//     const auto& key_frame2 = key_frames_.at(id_vec_[static_segments_[j].second]);
//     std::cout << std::setprecision(12) << "static seg: " << j << ", " << static_segments_[j].first << ", " << static_segments_[j].second << "; " << key_frame.Timestamp() << ", " << key_frame2.Timestamp() << std::endl;
//   }
}

void Trajectory::Write(const std::string& path) const { WriteKeyFramesText(path); }

void Trajectory::ReadKeyFramesText(const std::string& path) {
  key_frames_.clear();
  idx_umap_.clear();
  id_vec_.clear();

  std::ifstream file(path);
  CHECK(file.is_open()) << path;

  std::string line;
  std::string item;

  while (std::getline(file, line)) {
    StringTrim(&line);

    if (line.empty() || line[0] == '#') {
      continue;
    }

    std::stringstream line_stream1(line);

    // ID
    std::getline(line_stream1, item, ' ');
    const key_frame_t key_frame_id = std::stoul(item);

    class KeyFrame key_frame;
    key_frame.SetKeyFrameId(key_frame_id);

    // QVEC (qw, qx, qy, qz)
    std::getline(line_stream1, item, ' ');
    key_frame.Qvec(0) = std::stold(item);

    std::getline(line_stream1, item, ' ');
    key_frame.Qvec(1) = std::stold(item);

    std::getline(line_stream1, item, ' ');
    key_frame.Qvec(2) = std::stold(item);

    std::getline(line_stream1, item, ' ');
    key_frame.Qvec(3) = std::stold(item);

    key_frame.NormalizeQvec();

    key_frame.SetQvecPrior(key_frame.Qvec());

    // TVEC
    std::getline(line_stream1, item, ' ');
    key_frame.Tvec(0) = std::stold(item);

    std::getline(line_stream1, item, ' ');
    key_frame.Tvec(1) = std::stold(item);

    std::getline(line_stream1, item, ' ');
    key_frame.Tvec(2) = std::stold(item);

    key_frame.SetTvecPrior(key_frame.Tvec());

    // NAME
    std::getline(line_stream1, item, ' ');
    key_frame.SetName(item);
    key_frame.SetTimestamp(std::stod(item));

    key_frames_.emplace(key_frame.KeyFrameId(), key_frame);
    idx_umap_.emplace(key_frame.KeyFrameId(), id_vec_.size());
    id_vec_.emplace_back(key_frame.KeyFrameId());
  }
}

void Trajectory::WriteKeyFramesText(const std::string& path) const {
  std::ofstream file(path, std::ios::trunc);
  CHECK(file.is_open()) << path;

  // Ensure that we don't loose any precision by storing in text.
  file.precision(17);

  file << "# KeyFrame list:" << std::endl;
  file << "# KEYFRAME ID, QW, QX, QY, QZ, TX, TY, TZ, NAME"
       << std::endl;

  for (const auto& kid : id_vec_) {
    const auto& key_frame = key_frames_.at(kid);
    std::ostringstream line;
    line.precision(17);

    std::string line_string;

    line << kid << " ";

    // QVEC (qw, qx, qy, qz)
    const Eigen::Vector4d normalized_qvec =
        NormalizeQuaternion(key_frame.Qvec());
    line << normalized_qvec(0) << " ";
    line << normalized_qvec(1) << " ";
    line << normalized_qvec(2) << " ";
    line << normalized_qvec(3) << " ";

    // TVEC
    line << key_frame.Tvec(0) << " ";
    line << key_frame.Tvec(1) << " ";
    line << key_frame.Tvec(2) << " ";

    line << key_frame.Name();

    file << line.str() << std::endl;
  }
}

}  // namespace colmap
