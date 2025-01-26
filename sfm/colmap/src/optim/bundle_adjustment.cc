// Copyright (c) 2023, ETH Zurich and UNC Chapel Hill.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//
//     * Neither the name of ETH Zurich and UNC Chapel Hill nor the names of
//       its contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// Author: Johannes L. Schoenberger (jsch-at-demuc-dot-de)

#include "optim/bundle_adjustment.h"

#include <iomanip>

#ifdef OPENMP_ENABLED
#include <omp.h>
#endif

#include "base/camera_models.h"
#include "base/cost_functions.h"
#include "base/projection.h"
#include "util/misc.h"
#include "util/threading.h"
#include "util/timer.h"

namespace colmap {

////////////////////////////////////////////////////////////////////////////////
// BundleAdjustmentOptions
////////////////////////////////////////////////////////////////////////////////

ceres::LossFunction* BundleAdjustmentOptions::CreateLossFunction() const {
  ceres::LossFunction* loss_function = nullptr;
  switch (loss_function_type) {
    case LossFunctionType::TRIVIAL:
      loss_function = new ceres::TrivialLoss();
      break;
    case LossFunctionType::SOFT_L1:
      loss_function = new ceres::SoftLOneLoss(loss_function_scale);
      break;
    case LossFunctionType::CAUCHY:
      loss_function = new ceres::CauchyLoss(loss_function_scale);
      break;
  }
  CHECK_NOTNULL(loss_function);
  return loss_function;
}

bool BundleAdjustmentOptions::Check() const {
  CHECK_OPTION_GE(loss_function_scale, 0);
  return true;
}

////////////////////////////////////////////////////////////////////////////////
// BundleAdjustmentConfig
////////////////////////////////////////////////////////////////////////////////

BundleAdjustmentConfig::BundleAdjustmentConfig() {}

size_t BundleAdjustmentConfig::NumImages() const { return image_ids_.size(); }

size_t BundleAdjustmentConfig::NumPoints() const {
  return variable_point3D_ids_.size() + constant_point3D_ids_.size();
}

size_t BundleAdjustmentConfig::NumConstantCameras() const {
  return constant_camera_ids_.size();
}

size_t BundleAdjustmentConfig::NumConstantPoses() const {
  return constant_poses_.size();
}

size_t BundleAdjustmentConfig::NumConstantTvecs() const {
  return constant_tvecs_.size();
}

size_t BundleAdjustmentConfig::NumVariablePoints() const {
  return variable_point3D_ids_.size();
}

size_t BundleAdjustmentConfig::NumConstantPoints() const {
  return constant_point3D_ids_.size();
}

size_t BundleAdjustmentConfig::NumResiduals(
    const Reconstruction& reconstruction) const {
  // Count the number of observations for all added images.
  size_t num_observations = 0;
  for (const image_t image_id : image_ids_) {
    num_observations += reconstruction.Image(image_id).NumPoints3D();
  }

  // Count the number of observations for all added 3D points that are not
  // already added as part of the images above.

  auto NumObservationsForPoint = [this,
                                  &reconstruction](const point3D_t point3D_id) {
    size_t num_observations_for_point = 0;
    const auto& point3D = reconstruction.Point3D(point3D_id);
    for (const auto& track_el : point3D.Track().Elements()) {
      if (image_ids_.count(track_el.image_id) == 0) {
        num_observations_for_point += 1;
      }
    }
    return num_observations_for_point;
  };

  for (const auto point3D_id : variable_point3D_ids_) {
    num_observations += NumObservationsForPoint(point3D_id);
  }
  for (const auto point3D_id : constant_point3D_ids_) {
    num_observations += NumObservationsForPoint(point3D_id);
  }

  return 2 * num_observations;
}

void BundleAdjustmentConfig::AddImage(const image_t image_id) {
  image_ids_.insert(image_id);
}

bool BundleAdjustmentConfig::HasImage(const image_t image_id) const {
  return image_ids_.find(image_id) != image_ids_.end();
}

void BundleAdjustmentConfig::RemoveImage(const image_t image_id) {
  image_ids_.erase(image_id);
}

void BundleAdjustmentConfig::SetConstantCamera(const camera_t camera_id) {
  constant_camera_ids_.insert(camera_id);
}

void BundleAdjustmentConfig::SetVariableCamera(const camera_t camera_id) {
  constant_camera_ids_.erase(camera_id);
}

bool BundleAdjustmentConfig::IsConstantCamera(const camera_t camera_id) const {
  return constant_camera_ids_.find(camera_id) != constant_camera_ids_.end();
}

void BundleAdjustmentConfig::SetConstantPose(const image_t image_id) {
  CHECK(HasImage(image_id));
  CHECK(!HasConstantTvec(image_id));
  constant_poses_.insert(image_id);
}

void BundleAdjustmentConfig::SetVariablePose(const image_t image_id) {
  constant_poses_.erase(image_id);
}

bool BundleAdjustmentConfig::HasConstantPose(const image_t image_id) const {
  return constant_poses_.find(image_id) != constant_poses_.end();
}

void BundleAdjustmentConfig::SetConstantTvec(const image_t image_id,
                                             const std::vector<int>& idxs) {
  CHECK_GT(idxs.size(), 0);
  CHECK_LE(idxs.size(), 3);
  CHECK(HasImage(image_id));
  CHECK(!HasConstantPose(image_id));
  CHECK(!VectorContainsDuplicateValues(idxs))
      << "Tvec indices must not contain duplicates";
  constant_tvecs_.emplace(image_id, idxs);
}

void BundleAdjustmentConfig::RemoveConstantTvec(const image_t image_id) {
  constant_tvecs_.erase(image_id);
}

bool BundleAdjustmentConfig::HasConstantTvec(const image_t image_id) const {
  return constant_tvecs_.find(image_id) != constant_tvecs_.end();
}

const std::unordered_set<image_t>& BundleAdjustmentConfig::Images() const {
  return image_ids_;
}

const std::unordered_set<point3D_t>& BundleAdjustmentConfig::VariablePoints()
    const {
  return variable_point3D_ids_;
}

const std::unordered_set<point3D_t>& BundleAdjustmentConfig::ConstantPoints()
    const {
  return constant_point3D_ids_;
}

const std::vector<int>& BundleAdjustmentConfig::ConstantTvec(
    const image_t image_id) const {
  return constant_tvecs_.at(image_id);
}

void BundleAdjustmentConfig::AddVariablePoint(const point3D_t point3D_id) {
  CHECK(!HasConstantPoint(point3D_id));
  variable_point3D_ids_.insert(point3D_id);
}

void BundleAdjustmentConfig::AddConstantPoint(const point3D_t point3D_id) {
  CHECK(!HasVariablePoint(point3D_id));
  constant_point3D_ids_.insert(point3D_id);
}

bool BundleAdjustmentConfig::HasPoint(const point3D_t point3D_id) const {
  return HasVariablePoint(point3D_id) || HasConstantPoint(point3D_id);
}

bool BundleAdjustmentConfig::HasVariablePoint(
    const point3D_t point3D_id) const {
  return variable_point3D_ids_.find(point3D_id) != variable_point3D_ids_.end();
}

bool BundleAdjustmentConfig::HasConstantPoint(
    const point3D_t point3D_id) const {
  return constant_point3D_ids_.find(point3D_id) != constant_point3D_ids_.end();
}

void BundleAdjustmentConfig::RemoveVariablePoint(const point3D_t point3D_id) {
  variable_point3D_ids_.erase(point3D_id);
}

void BundleAdjustmentConfig::RemoveConstantPoint(const point3D_t point3D_id) {
  constant_point3D_ids_.erase(point3D_id);
}

////////////////////////////////////////////////////////////////////////////////
// BundleAdjuster
////////////////////////////////////////////////////////////////////////////////

BundleAdjuster::BundleAdjuster(const BundleAdjustmentOptions& options,
                               const BundleAdjustmentConfig& config)
    : options_(options), config_(config) {
  CHECK(options_.Check());
}

bool BundleAdjuster::Solve(Reconstruction* reconstruction) {
  CHECK_NOTNULL(reconstruction);
  CHECK(!problem_) << "Cannot use the same BundleAdjuster multiple times";

  problem_ = std::make_unique<ceres::Problem>();

  ceres::LossFunction* loss_function = options_.CreateLossFunction();
  SetUp(reconstruction, loss_function);

  if (problem_->NumResiduals() == 0) {
    return false;
  }

  ceres::Solver::Options solver_options = options_.solver_options;
  const bool has_sparse =
      solver_options.sparse_linear_algebra_library_type != ceres::NO_SPARSE;

  // Empirical choice.
  const size_t kMaxNumImagesDirectDenseSolver = 50;
  const size_t kMaxNumImagesDirectSparseSolver = 1000;
  const size_t num_images = config_.NumImages();
  if (num_images <= kMaxNumImagesDirectDenseSolver) {
    solver_options.linear_solver_type = ceres::DENSE_SCHUR;
  } else if (num_images <= kMaxNumImagesDirectSparseSolver && has_sparse) {
    solver_options.linear_solver_type = ceres::SPARSE_SCHUR;
  } else {  // Indirect sparse (preconditioned CG) solver.
    solver_options.linear_solver_type = ceres::ITERATIVE_SCHUR;
    solver_options.preconditioner_type = ceres::SCHUR_JACOBI;
  }
//   solver_options.dense_linear_algebra_library_type = ceres::CUDA;

  if (problem_->NumResiduals() <
      options_.min_num_residuals_for_multi_threading) {
    solver_options.num_threads = 1;
#if CERES_VERSION_MAJOR < 2
    solver_options.num_linear_solver_threads = 1;
#endif  // CERES_VERSION_MAJOR
  } else {
    solver_options.num_threads =
        GetEffectiveNumThreads(solver_options.num_threads);
#if CERES_VERSION_MAJOR < 2
    solver_options.num_linear_solver_threads =
        GetEffectiveNumThreads(solver_options.num_linear_solver_threads);
#endif  // CERES_VERSION_MAJOR
  }

  std::string solver_error;
  CHECK(solver_options.IsValid(&solver_error)) << solver_error;

  ceres::Solve(solver_options, problem_.get(), &summary_);

  if (solver_options.minimizer_progress_to_stdout) {
    std::cout << std::endl;
  }

  if (options_.print_summary) {
    PrintHeading2("Bundle adjustment report");
    PrintSolverSummary(summary_);
  }

  TearDown(reconstruction);

  return true;
}

const ceres::Solver::Summary& BundleAdjuster::Summary() const {
  return summary_;
}

void BundleAdjuster::SetUp(Reconstruction* reconstruction,
                           ceres::LossFunction* loss_function) {
  // Warning: AddPointsToProblem assumes that AddImageToProblem is called first.
  // Do not change order of instructions!
  for (const image_t image_id : config_.Images()) {
    AddImageToProblem(image_id, reconstruction, loss_function);
  }
  for (const auto point3D_id : config_.VariablePoints()) {
    AddPointToProblem(point3D_id, reconstruction, loss_function);
  }
  for (const auto point3D_id : config_.ConstantPoints()) {
    AddPointToProblem(point3D_id, reconstruction, loss_function);
  }

  ParameterizeCameras(reconstruction);
  ParameterizePoints(reconstruction);
}

void BundleAdjuster::TearDown(Reconstruction*) {
  // Nothing to do
}

void BundleAdjuster::AddImageToProblem(const image_t image_id,
                                       Reconstruction* reconstruction,
                                       ceres::LossFunction* loss_function) {
  Image& image = reconstruction->Image(image_id);
  Camera& camera = reconstruction->Camera(image.CameraId());

  // CostFunction assumes unit quaternions.
  image.NormalizeQvec();

  double* qvec_data = image.Qvec().data();
  double* tvec_data = image.Tvec().data();
  double* camera_params_data = camera.ParamsData();

  const bool constant_pose =
      !options_.refine_extrinsics || config_.HasConstantPose(image_id);

  // Add residuals to bundle adjustment problem.
  size_t num_observations = 0;
  for (const Point2D& point2D : image.Points2D()) {
    if (!point2D.HasPoint3D()) {
      continue;
    }

    num_observations += 1;
    point3D_num_observations_[point2D.Point3DId()] += 1;

    Point3D& point3D = reconstruction->Point3D(point2D.Point3DId());
    assert(point3D.Track().Length() > 1);

    ceres::CostFunction* cost_function = nullptr;

    if (constant_pose) {
      switch (camera.ModelId()) {
#define CAMERA_MODEL_CASE(CameraModel)                                 \
  case CameraModel::kModelId:                                          \
    cost_function =                                                    \
        BundleAdjustmentConstantPoseCostFunction<CameraModel>::Create( \
            image.Qvec(), image.Tvec(), point2D.XY());                 \
    break;

        CAMERA_MODEL_SWITCH_CASES

#undef CAMERA_MODEL_CASE
      }

      problem_->AddResidualBlock(cost_function, loss_function,
                                 point3D.XYZ().data(), camera_params_data);
    } else {
      switch (camera.ModelId()) {
#define CAMERA_MODEL_CASE(CameraModel)                                   \
  case CameraModel::kModelId:                                            \
    cost_function =                                                      \
        BundleAdjustmentCostFunction<CameraModel>::Create(point2D.XY()); \
    break;

        CAMERA_MODEL_SWITCH_CASES

#undef CAMERA_MODEL_CASE
      }

      problem_->AddResidualBlock(cost_function, loss_function, qvec_data,
                                 tvec_data, point3D.XYZ().data(),
                                 camera_params_data);
    }
  }

  if (num_observations > 0) {
    camera_ids_.insert(image.CameraId());

    // Set pose parameterization.
    if (!constant_pose) {
      SetQuaternionManifold(problem_.get(), qvec_data);
      if (config_.HasConstantTvec(image_id)) {
        const std::vector<int>& constant_tvec_idxs =
            config_.ConstantTvec(image_id);
        SetSubsetManifold(3, constant_tvec_idxs, problem_.get(), tvec_data);
      }
    }
  }
}

void BundleAdjuster::AddPointToProblem(const point3D_t point3D_id,
                                       Reconstruction* reconstruction,
                                       ceres::LossFunction* loss_function) {
  Point3D& point3D = reconstruction->Point3D(point3D_id);

  // Is 3D point already fully contained in the problem? I.e. its entire track
  // is contained in `variable_image_ids`, `constant_image_ids`,
  // `constant_x_image_ids`.
  if (point3D_num_observations_[point3D_id] == point3D.Track().Length()) {
    return;
  }

  for (const auto& track_el : point3D.Track().Elements()) {
    // Skip observations that were already added in `FillImages`.
    if (config_.HasImage(track_el.image_id)) {
      continue;
    }

    point3D_num_observations_[point3D_id] += 1;

    Image& image = reconstruction->Image(track_el.image_id);
    Camera& camera = reconstruction->Camera(image.CameraId());
    const Point2D& point2D = image.Point2D(track_el.point2D_idx);

    // We do not want to refine the camera of images that are not
    // part of `constant_image_ids_`, `constant_image_ids_`,
    // `constant_x_image_ids_`.
    if (camera_ids_.count(image.CameraId()) == 0) {
      camera_ids_.insert(image.CameraId());
      config_.SetConstantCamera(image.CameraId());
    }

    ceres::CostFunction* cost_function = nullptr;

    switch (camera.ModelId()) {
#define CAMERA_MODEL_CASE(CameraModel)                                 \
  case CameraModel::kModelId:                                          \
    cost_function =                                                    \
        BundleAdjustmentConstantPoseCostFunction<CameraModel>::Create( \
            image.Qvec(), image.Tvec(), point2D.XY());                 \
    break;

      CAMERA_MODEL_SWITCH_CASES

#undef CAMERA_MODEL_CASE
    }
    problem_->AddResidualBlock(cost_function, loss_function,
                               point3D.XYZ().data(), camera.ParamsData());
  }
}

void BundleAdjuster::ParameterizeCameras(Reconstruction* reconstruction) {
  const bool constant_camera = !options_.refine_focal_length &&
                               !options_.refine_principal_point &&
                               !options_.refine_extra_params;
  for (const camera_t camera_id : camera_ids_) {
    Camera& camera = reconstruction->Camera(camera_id);

    if (constant_camera || config_.IsConstantCamera(camera_id)) {
      problem_->SetParameterBlockConstant(camera.ParamsData());
      continue;
    } else {
      std::vector<int> const_camera_params;

      if (!options_.refine_focal_length) {
        const std::vector<size_t>& params_idxs = camera.FocalLengthIdxs();
        const_camera_params.insert(const_camera_params.end(),
                                   params_idxs.begin(), params_idxs.end());
      }
      if (!options_.refine_principal_point) {
        const std::vector<size_t>& params_idxs = camera.PrincipalPointIdxs();
        const_camera_params.insert(const_camera_params.end(),
                                   params_idxs.begin(), params_idxs.end());
      }
      if (!options_.refine_extra_params) {
        const std::vector<size_t>& params_idxs = camera.ExtraParamsIdxs();
        const_camera_params.insert(const_camera_params.end(),
                                   params_idxs.begin(), params_idxs.end());
      }

      if (const_camera_params.size() > 0) {
        SetSubsetManifold(static_cast<int>(camera.NumParams()),
                          const_camera_params, problem_.get(),
                          camera.ParamsData());
      }
    }
  }
}

void BundleAdjuster::ParameterizePoints(Reconstruction* reconstruction) {
  for (const auto elem : point3D_num_observations_) {
    Point3D& point3D = reconstruction->Point3D(elem.first);
    if (point3D.Track().Length() > elem.second) {
      problem_->SetParameterBlockConstant(point3D.XYZ().data());
    }
  }

  for (const point3D_t point3D_id : config_.ConstantPoints()) {
    Point3D& point3D = reconstruction->Point3D(point3D_id);
    problem_->SetParameterBlockConstant(point3D.XYZ().data());
  }
}

////////////////////////////////////////////////////////////////////////////////
// RigBundleAdjuster
////////////////////////////////////////////////////////////////////////////////

RigBundleAdjuster::RigBundleAdjuster(const BundleAdjustmentOptions& options,
                                     const Options& rig_options,
                                     const BundleAdjustmentConfig& config)
    : BundleAdjuster(options, config), rig_options_(rig_options) {}

bool RigBundleAdjuster::Solve(Reconstruction* reconstruction,
                              std::vector<CameraRig>* camera_rigs) {
  CHECK_NOTNULL(reconstruction);
  CHECK_NOTNULL(camera_rigs);
  CHECK(!problem_) << "Cannot use the same BundleAdjuster multiple times";

  // Check the validity of the provided camera rigs.
  std::unordered_set<camera_t> rig_camera_ids;
  for (auto& camera_rig : *camera_rigs) {
    camera_rig.Check(*reconstruction);
    for (const auto& camera_id : camera_rig.GetCameraIds()) {
      CHECK_EQ(rig_camera_ids.count(camera_id), 0)
          << "Camera must not be part of multiple camera rigs";
      rig_camera_ids.insert(camera_id);
    }

    for (const auto& snapshot : camera_rig.Snapshots()) {
      for (const auto& image_id : snapshot) {
        CHECK_EQ(image_id_to_camera_rig_.count(image_id), 0)
            << "Image must not be part of multiple camera rigs";
        image_id_to_camera_rig_.emplace(image_id, &camera_rig);
      }
    }
  }

  problem_ = std::make_unique<ceres::Problem>();

  ceres::LossFunction* loss_function = options_.CreateLossFunction();
  SetUp(reconstruction, camera_rigs, loss_function);

  if (problem_->NumResiduals() == 0) {
    return false;
  }

  ceres::Solver::Options solver_options = options_.solver_options;
  const bool has_sparse =
      solver_options.sparse_linear_algebra_library_type != ceres::NO_SPARSE;

  // Empirical choice.
  const size_t kMaxNumImagesDirectDenseSolver = 50;
  const size_t kMaxNumImagesDirectSparseSolver = 1000;
  const size_t num_images = config_.NumImages();
  if (num_images <= kMaxNumImagesDirectDenseSolver) {
    solver_options.linear_solver_type = ceres::DENSE_SCHUR;
  } else if (num_images <= kMaxNumImagesDirectSparseSolver && has_sparse) {
    solver_options.linear_solver_type = ceres::SPARSE_SCHUR;
  } else {  // Indirect sparse (preconditioned CG) solver.
    solver_options.linear_solver_type = ceres::ITERATIVE_SCHUR;
    solver_options.preconditioner_type = ceres::SCHUR_JACOBI;
  }

  solver_options.num_threads =
      GetEffectiveNumThreads(solver_options.num_threads);
#if CERES_VERSION_MAJOR < 2
  solver_options.num_linear_solver_threads =
      GetEffectiveNumThreads(solver_options.num_linear_solver_threads);
#endif  // CERES_VERSION_MAJOR

  std::string solver_error;
  CHECK(solver_options.IsValid(&solver_error)) << solver_error;

  ceres::Solve(solver_options, problem_.get(), &summary_);

  if (solver_options.minimizer_progress_to_stdout) {
    std::cout << std::endl;
  }

  if (options_.print_summary) {
    PrintHeading2("Rig Bundle adjustment report");
    PrintSolverSummary(summary_);
  }

  TearDown(reconstruction, *camera_rigs);

  return true;
}

void RigBundleAdjuster::SetUp(Reconstruction* reconstruction,
                              std::vector<CameraRig>* camera_rigs,
                              ceres::LossFunction* loss_function) {
  ComputeCameraRigPoses(*reconstruction, *camera_rigs);

  for (const image_t image_id : config_.Images()) {
    AddImageToProblem(image_id, reconstruction, camera_rigs, loss_function);
  }
  for (const auto point3D_id : config_.VariablePoints()) {
    AddPointToProblem(point3D_id, reconstruction, loss_function);
  }
  for (const auto point3D_id : config_.ConstantPoints()) {
    AddPointToProblem(point3D_id, reconstruction, loss_function);
  }

  ParameterizeCameras(reconstruction);
  ParameterizePoints(reconstruction);
  ParameterizeCameraRigs(reconstruction);
}

void RigBundleAdjuster::TearDown(Reconstruction* reconstruction,
                                 const std::vector<CameraRig>& camera_rigs) {
  for (const auto& elem : image_id_to_camera_rig_) {
    const auto image_id = elem.first;
    const auto& camera_rig = *elem.second;
    auto& image = reconstruction->Image(image_id);
    ConcatenatePoses(*image_id_to_rig_qvec_.at(image_id),
                     *image_id_to_rig_tvec_.at(image_id),
                     camera_rig.RelativeQvec(image.CameraId()),
                     camera_rig.RelativeTvec(image.CameraId()), &image.Qvec(),
                     &image.Tvec());
  }
}

void RigBundleAdjuster::AddImageToProblem(const image_t image_id,
                                          Reconstruction* reconstruction,
                                          std::vector<CameraRig>* camera_rigs,
                                          ceres::LossFunction* loss_function) {
  const double max_squared_reproj_error =
      rig_options_.max_reproj_error * rig_options_.max_reproj_error;

  Image& image = reconstruction->Image(image_id);
  Camera& camera = reconstruction->Camera(image.CameraId());

  const bool constant_pose = config_.HasConstantPose(image_id);
  const bool constant_tvec = config_.HasConstantTvec(image_id);

  double* qvec_data = nullptr;
  double* tvec_data = nullptr;
  double* rig_qvec_data = nullptr;
  double* rig_tvec_data = nullptr;
  double* camera_params_data = camera.ParamsData();
  CameraRig* camera_rig = nullptr;
  Eigen::Matrix3x4d rig_proj_matrix = Eigen::Matrix3x4d::Zero();

  if (image_id_to_camera_rig_.count(image_id) > 0) {
    CHECK(!constant_pose)
        << "Images contained in a camera rig must not have constant pose";
    CHECK(!constant_tvec)
        << "Images contained in a camera rig must not have constant tvec";
    camera_rig = image_id_to_camera_rig_.at(image_id);
    rig_qvec_data = image_id_to_rig_qvec_.at(image_id)->data();
    rig_tvec_data = image_id_to_rig_tvec_.at(image_id)->data();
    qvec_data = camera_rig->RelativeQvec(image.CameraId()).data();
    tvec_data = camera_rig->RelativeTvec(image.CameraId()).data();

    // Concatenate the absolute pose of the rig and the relative pose the camera
    // within the rig to detect outlier observations.
    Eigen::Vector4d rig_concat_qvec;
    Eigen::Vector3d rig_concat_tvec;
    ConcatenatePoses(*image_id_to_rig_qvec_.at(image_id),
                     *image_id_to_rig_tvec_.at(image_id),
                     camera_rig->RelativeQvec(image.CameraId()),
                     camera_rig->RelativeTvec(image.CameraId()),
                     &rig_concat_qvec, &rig_concat_tvec);
    rig_proj_matrix = ComposeProjectionMatrix(rig_concat_qvec, rig_concat_tvec);
  } else {
    // CostFunction assumes unit quaternions.
    image.NormalizeQvec();
    qvec_data = image.Qvec().data();
    tvec_data = image.Tvec().data();
  }

  // Collect cameras for final parameterization.
  CHECK(image.HasCamera());
  camera_ids_.insert(image.CameraId());

  // The number of added observations for the current image.
  size_t num_observations = 0;

  // Add residuals to bundle adjustment problem.
  for (const Point2D& point2D : image.Points2D()) {
    if (!point2D.HasPoint3D()) {
      continue;
    }

    Point3D& point3D = reconstruction->Point3D(point2D.Point3DId());
    assert(point3D.Track().Length() > 1);

    if (camera_rig != nullptr &&
        CalculateSquaredReprojectionError(point2D.XY(), point3D.XYZ(),
                                          rig_proj_matrix,
                                          camera) > max_squared_reproj_error) {
      continue;
    }

    num_observations += 1;
    point3D_num_observations_[point2D.Point3DId()] += 1;

    ceres::CostFunction* cost_function = nullptr;

    if (camera_rig == nullptr) {
      if (constant_pose) {
        switch (camera.ModelId()) {
#define CAMERA_MODEL_CASE(CameraModel)                                 \
  case CameraModel::kModelId:                                          \
    cost_function =                                                    \
        BundleAdjustmentConstantPoseCostFunction<CameraModel>::Create( \
            image.Qvec(), image.Tvec(), point2D.XY());                 \
    break;

          CAMERA_MODEL_SWITCH_CASES

#undef CAMERA_MODEL_CASE
        }

        problem_->AddResidualBlock(cost_function, loss_function,
                                   point3D.XYZ().data(), camera_params_data);
      } else {
        switch (camera.ModelId()) {
#define CAMERA_MODEL_CASE(CameraModel)                                   \
  case CameraModel::kModelId:                                            \
    cost_function =                                                      \
        BundleAdjustmentCostFunction<CameraModel>::Create(point2D.XY()); \
    break;

          CAMERA_MODEL_SWITCH_CASES

#undef CAMERA_MODEL_CASE
        }

        problem_->AddResidualBlock(cost_function, loss_function, qvec_data,
                                   tvec_data, point3D.XYZ().data(),
                                   camera_params_data);
      }
    } else {
      switch (camera.ModelId()) {
#define CAMERA_MODEL_CASE(CameraModel)                                      \
  case CameraModel::kModelId:                                               \
    cost_function =                                                         \
        RigBundleAdjustmentCostFunction<CameraModel>::Create(point2D.XY()); \
                                                                            \
    break;

        CAMERA_MODEL_SWITCH_CASES

#undef CAMERA_MODEL_CASE
      }
      problem_->AddResidualBlock(cost_function, loss_function, rig_qvec_data,
                                 rig_tvec_data, qvec_data, tvec_data,
                                 point3D.XYZ().data(), camera_params_data);
    }
  }

  if (num_observations > 0) {
    parameterized_qvec_data_.insert(qvec_data);

    if (camera_rig != nullptr) {
      parameterized_qvec_data_.insert(rig_qvec_data);

      // Set the relative pose of the camera constant if relative pose
      // refinement is disabled or if it is the reference camera to avoid over-
      // parameterization of the camera pose.
      if (!rig_options_.refine_relative_poses ||
          image.CameraId() == camera_rig->RefCameraId()) {
        problem_->SetParameterBlockConstant(qvec_data);
        problem_->SetParameterBlockConstant(tvec_data);
      }
    }

    // Set pose parameterization.
    if (!constant_pose && constant_tvec) {
      const std::vector<int>& constant_tvec_idxs =
          config_.ConstantTvec(image_id);
      SetSubsetManifold(3, constant_tvec_idxs, problem_.get(), tvec_data);
    }
  }
}

void RigBundleAdjuster::AddPointToProblem(const point3D_t point3D_id,
                                          Reconstruction* reconstruction,
                                          ceres::LossFunction* loss_function) {
  Point3D& point3D = reconstruction->Point3D(point3D_id);

  // Is 3D point already fully contained in the problem? I.e. its entire track
  // is contained in `variable_image_ids`, `constant_image_ids`,
  // `constant_x_image_ids`.
  if (point3D_num_observations_[point3D_id] == point3D.Track().Length()) {
    return;
  }

  for (const auto& track_el : point3D.Track().Elements()) {
    // Skip observations that were already added in `AddImageToProblem`.
    if (config_.HasImage(track_el.image_id)) {
      continue;
    }

    point3D_num_observations_[point3D_id] += 1;

    Image& image = reconstruction->Image(track_el.image_id);
    Camera& camera = reconstruction->Camera(image.CameraId());
    const Point2D& point2D = image.Point2D(track_el.point2D_idx);

    // We do not want to refine the camera of images that are not
    // part of `constant_image_ids_`, `constant_image_ids_`,
    // `constant_x_image_ids_`.
    if (camera_ids_.count(image.CameraId()) == 0) {
      camera_ids_.insert(image.CameraId());
      config_.SetConstantCamera(image.CameraId());
    }

    ceres::CostFunction* cost_function = nullptr;

    switch (camera.ModelId()) {
#define CAMERA_MODEL_CASE(CameraModel)                                     \
  case CameraModel::kModelId:                                              \
    cost_function =                                                        \
        BundleAdjustmentConstantPoseCostFunction<CameraModel>::Create(     \
            image.Qvec(), image.Tvec(), point2D.XY());                     \
    problem_->AddResidualBlock(cost_function, loss_function,               \
                               point3D.XYZ().data(), camera.ParamsData()); \
    break;

      CAMERA_MODEL_SWITCH_CASES

#undef CAMERA_MODEL_CASE
    }
  }
}

void RigBundleAdjuster::ComputeCameraRigPoses(
    const Reconstruction& reconstruction,
    const std::vector<CameraRig>& camera_rigs) {
  camera_rig_qvecs_.reserve(camera_rigs.size());
  camera_rig_tvecs_.reserve(camera_rigs.size());
  for (const auto& camera_rig : camera_rigs) {
    camera_rig_qvecs_.emplace_back();
    camera_rig_tvecs_.emplace_back();
    auto& rig_qvecs = camera_rig_qvecs_.back();
    auto& rig_tvecs = camera_rig_tvecs_.back();
    rig_qvecs.resize(camera_rig.NumSnapshots());
    rig_tvecs.resize(camera_rig.NumSnapshots());
    for (size_t snapshot_idx = 0; snapshot_idx < camera_rig.NumSnapshots();
         ++snapshot_idx) {
      camera_rig.ComputeAbsolutePose(snapshot_idx, reconstruction,
                                     &rig_qvecs[snapshot_idx],
                                     &rig_tvecs[snapshot_idx]);
      for (const auto image_id : camera_rig.Snapshots()[snapshot_idx]) {
        image_id_to_rig_qvec_.emplace(image_id, &rig_qvecs[snapshot_idx]);
        image_id_to_rig_tvec_.emplace(image_id, &rig_tvecs[snapshot_idx]);
      }
    }
  }
}

void RigBundleAdjuster::ParameterizeCameraRigs(Reconstruction* reconstruction) {
  for (double* qvec_data : parameterized_qvec_data_) {
    SetQuaternionManifold(problem_.get(), qvec_data);
  }
}


////////////////////////////////////////////////////////////////////////////////
// AlignmentBundleAdjuster
////////////////////////////////////////////////////////////////////////////////

AlignmentBundleAdjuster::AlignmentBundleAdjuster(const BundleAdjustmentOptions& options,
                                                 const CalibOptions& calib_options,
                                                 const TrajOptions& traj_options,
                                                 const AlignmentOptions& align_options,
                                                 const BundleAdjustmentConfig& config)
    : BundleAdjuster(options, config), calib_options_(calib_options), traj_options_(traj_options), align_options_(align_options) {}

bool AlignmentBundleAdjuster::Solve(Reconstruction* reconstruction,
                                    CameraCalib* camera_calib,
                                    Trajectory* trajectory) {
  CHECK_NOTNULL(reconstruction);
  CHECK_NOTNULL(camera_calib);
  CHECK_NOTNULL(trajectory);
  CHECK(!problem_) << "Cannot use the same BundleAdjuster multiple times";

  // Check the validity of the provided camera rigs.
  camera_calib->Check(*reconstruction);

  problem_ = std::make_unique<ceres::Problem>();

  ceres::LossFunction* loss_function = options_.CreateLossFunction();
  SetUp(reconstruction, camera_calib, trajectory, loss_function);

  if (problem_->NumResiduals() == 0) {
    return false;
  }

  ceres::Solver::Options solver_options = options_.solver_options;
  const bool has_sparse =
      solver_options.sparse_linear_algebra_library_type != ceres::NO_SPARSE;

  // Empirical choice.
  // const size_t kMaxNumImagesDirectDenseSolver = 50;
  // const size_t kMaxNumImagesDirectSparseSolver = 1000;
  // const size_t num_images = config_.NumImages();
  // if (num_images <= kMaxNumImagesDirectDenseSolver) {
  //   solver_options.linear_solver_type = ceres::DENSE_SCHUR;
  // } else if (num_images <= kMaxNumImagesDirectSparseSolver && has_sparse) {
  //   solver_options.linear_solver_type = ceres::SPARSE_SCHUR;
  // } else {  // Indirect sparse (preconditioned CG) solver.
  //   solver_options.linear_solver_type = ceres::ITERATIVE_SCHUR;
  //   solver_options.preconditioner_type = ceres::SCHUR_JACOBI;
  // }

// #if CUDA_ENABLED
  solver_options.dense_linear_algebra_library_type = ceres::CUDA;
// #endif

  solver_options.num_threads =
      GetEffectiveNumThreads(solver_options.num_threads);
#if CERES_VERSION_MAJOR < 2
  solver_options.num_linear_solver_threads =
      GetEffectiveNumThreads(solver_options.num_linear_solver_threads);
#endif  // CERES_VERSION_MAJOR

  std::string solver_error;
  CHECK(solver_options.IsValid(&solver_error)) << solver_error;

  ceres::Solve(solver_options, problem_.get(), &summary_);

  if (solver_options.minimizer_progress_to_stdout) {
    std::cout << std::endl;
  }

  if (options_.print_summary) {
    PrintHeading2("Rig Bundle adjustment report");
    PrintSolverSummary(summary_);
  }

  TearDown(reconstruction, camera_calib, trajectory);

  return true;
}

void AlignmentBundleAdjuster::SetUp(Reconstruction* reconstruction,
                                    CameraCalib* camera_calib,
                                    Trajectory* trajectory,
                                    ceres::LossFunction* loss_function) {
  for (const image_t image_id : config_.Images()) {
    AddImageToProblem(image_id, reconstruction, loss_function);
  }
  for (const auto point3D_id : config_.VariablePoints()) {
    AddPointToProblem(point3D_id, reconstruction, loss_function);
  }
  for (const auto point3D_id : config_.ConstantPoints()) {
    AddPointToProblem(point3D_id, reconstruction, loss_function);
  }

  for (const auto &cam_id : camera_calib->GetCameraIds()) {
    AddCalibCamToProblem(cam_id, camera_calib, nullptr);
  }
  double* rel_time = &(camera_calib->rel_time_);
  ceres::CostFunction* cost_function2 = nullptr;
  cost_function2 = PriorScalarCostFunction::Create(camera_calib->time_prior_, calib_options_.weight_rel_time);
  problem_->AddResidualBlock(cost_function2, nullptr, rel_time);

  const auto &kfids = trajectory->KeyFrameIds();
  for (std::size_t idx = 0; idx < kfids.size(); ++idx) {
    AddKeyFrameToProblem(kfids[idx], trajectory, nullptr);
    // continue;
    if (idx > 0) {
      KeyFrame& key_frame0 = trajectory->KeyFrame(kfids[idx - 1]);
      double* qvec_data0 = key_frame0.Qvec().data();
      double* tvec_data0 = key_frame0.Tvec().data();

      KeyFrame& key_frame1 = trajectory->KeyFrame(kfids[idx]);
      double* qvec_data1 = key_frame1.Qvec().data();
      double* tvec_data1 = key_frame1.Tvec().data();

      const auto &qp0 = key_frame0.QvecPrior();
      Eigen::Quaterniond q0(qp0[0], qp0[1], qp0[2], qp0[3]);
      const auto &tp0 = key_frame0.TvecPrior();
      Eigen::Vector3d t0(tp0[0], tp0[1], tp0[2]);

      const auto &qp1 = key_frame1.QvecPrior();
      Eigen::Quaterniond q1(qp1[0], qp1[1], qp1[2], qp1[3]);
      const auto &tp1 = key_frame1.TvecPrior();
      Eigen::Vector3d t1(tp1[0], tp1[1], tp1[2]);

      Eigen::Quaterniond quat = q0.conjugate() * q1;
      Eigen::Vector4d deltaq(quat.w(), quat.x(), quat.y(), quat.z());
      Eigen::Vector3d deltat = q0.conjugate() * (t1 - t0);

      ceres::CostFunction* cost_function = nullptr;
      cost_function = RelPoseCostFunction::Create(deltaq, deltat, traj_options_.weight_q_rel, traj_options_.weight_t_rel);

      problem_->AddResidualBlock(cost_function, nullptr, qvec_data0, tvec_data0, qvec_data1, tvec_data1);
    }
  }

  AddAlignmentToProblem(reconstruction, camera_calib, trajectory);

  AddStaticConstraintToProblem(trajectory);

  ParameterizeCameras(reconstruction);
  ParameterizePoints(reconstruction);
}

void AlignmentBundleAdjuster::TearDown(Reconstruction* reconstruction,
                                       CameraCalib* camera_calib,
                                       Trajectory* trajectory) {
  // Nothing to do
}

void AlignmentBundleAdjuster::AddCalibCamToProblem(const camera_t camera_id,
                                                   CameraCalib* camera_calib,
                                                   ceres::LossFunction* loss_function) {
  CalibCamera& calib_cam = camera_calib->Camera(camera_id);

  // CostFunction assumes unit quaternions.
  calib_cam.rel_qvec = NormalizeQuaternion(calib_cam.rel_qvec);
  calib_cam.qvec_prior = NormalizeQuaternion(calib_cam.qvec_prior);

  double* rel_qvec = calib_cam.rel_qvec.data();
  double* rel_tvec = calib_cam.rel_tvec.data();

  ceres::CostFunction* cost_function = nullptr;
  cost_function = PriorPoseCostFunction::Create(calib_cam.qvec_prior, calib_cam.tvec_prior, calib_options_.weight_rel_qvec, calib_options_.weight_rel_tvec);
  problem_->AddResidualBlock(cost_function, loss_function, rel_qvec, rel_tvec);

  SetQuaternionManifold(problem_.get(), rel_qvec);

//   double* rel_time = &(calib_cam.rel_time);
//   ceres::CostFunction* cost_function2 = nullptr;
//   cost_function2 = PriorScalarCostFunction::Create(calib_cam.time_prior, calib_options_.weight_rel_time);
//   problem_->AddResidualBlock(cost_function2, loss_function, rel_time);
}

void AlignmentBundleAdjuster::AddKeyFrameToProblem(const key_frame_t key_frame_id,
                                                   Trajectory* trajectory,
                                                   ceres::LossFunction* loss_function) {
  KeyFrame& key_frame = trajectory->KeyFrame(key_frame_id);

  // CostFunction assumes unit quaternions.
  key_frame.NormalizeQvec();
  key_frame.SetQvecPrior(NormalizeQuaternion(key_frame.QvecPrior()));

  double* qvec_data = key_frame.Qvec().data();
  double* tvec_data = key_frame.Tvec().data();

  // Add pose_graph to problem
  ceres::CostFunction* cost_function = nullptr;
  cost_function = PriorPoseCostFunction::Create(key_frame.QvecPrior(), key_frame.TvecPrior(), traj_options_.weight_q_prior, traj_options_.weight_t_prior);

  problem_->AddResidualBlock(cost_function, loss_function, qvec_data, tvec_data);
  SetQuaternionManifold(problem_.get(), qvec_data);
}

void AlignmentBundleAdjuster::AddAlignmentToProblem(Reconstruction* reconstruction,
                                                    CameraCalib* camera_calib,
                                                    Trajectory* trajectory) {
  for (auto &cam_id : camera_calib->GetCameraIds()) {
    camera_calib->Camera(cam_id).images.clear();
    camera_calib->Camera(cam_id).key_frames.clear();
    camera_calib->Camera(cam_id).key_poses.clear();
  }
  for (auto &it : reconstruction->Images()) {
    const auto cam_id = reconstruction->Image(it.first).CameraId();
    camera_calib->Camera(cam_id).images.emplace_back(it.first, reconstruction->Image(it.first).GetTimestamp());
  }

  std::size_t neighbor_num = 4;
  for (auto &cam_id : camera_calib->GetCameraIds()) {
    // std::cout << std::setprecision(15) << std::endl;
    // for (auto &it: camera_calib->Camera(cam_id).images) {
    //   std::cout << it.first << " " << it.second << ", ";
    // }
    // std::cout << std::endl;
    std::sort(camera_calib->Camera(cam_id).images.begin(), camera_calib->Camera(cam_id).images.end(), [](const std::pair<image_t, double>& imagea, const std::pair<image_t, double>& imageb) {return imagea.second < imageb.second;});

    // for (auto &it: camera_calib->Camera(cam_id).images) {
    //   std::cout << it.first << " " << it.second << ", ";
    // }
    // std::cout << std::endl;

    const auto &imgids = camera_calib->Camera(cam_id).images;
    camera_calib->Camera(cam_id).key_frames.resize(imgids.size());
    camera_calib->Camera(cam_id).key_poses.resize(imgids.size());
    if (imgids.empty()) continue;
    const auto &kfids = trajectory->KeyFrameIds();
    if (kfids.empty()) continue;
    std::size_t ii = 0;
    std::size_t jj = 0;
    while (ii < imgids.size() && jj < kfids.size()) {
      auto iid = imgids[ii];
      auto jid = kfids[jj];
      auto img_time = imgids[ii].second;
      img_time += camera_calib->rel_time_;
      double key_time = trajectory->KeyFrame(jid).Timestamp();

      while (key_time < img_time && jj + 1 < kfids.size()) {
        jj += 1;
        jid = kfids[jj];
        key_time = trajectory->KeyFrame(jid).Timestamp();
      }

      std::vector<std::size_t> key_neighbors;
      key_neighbors.push_back(jj);
      if (jj > 0) {
        key_neighbors.push_back(jj - 1);
      }
      if (neighbor_num == 2) {
        if (key_neighbors.size() < 2 && jj + 1 < kfids.size()) {
          key_neighbors.push_back(jj + 1);
        }
      } else if (neighbor_num == 4) {
        if (jj > 1) {
          key_neighbors.push_back(jj - 2);
        }
        if (jj + 1 < kfids.size()) {
          key_neighbors.push_back(jj + 1);
        }
        if (key_neighbors.size() < 4 && jj > 2) {
          key_neighbors.push_back(jj - 3);
        }
        if (key_neighbors.size() < 4 && jj + 2 < kfids.size()) {
          key_neighbors.push_back(jj + 2);
        }
        if (key_neighbors.size() < 4 && jj + 3 < kfids.size()) {
          key_neighbors.push_back(jj + 3);
        }
      }
      std::sort(key_neighbors.begin(), key_neighbors.end());
      CHECK(key_neighbors.size() == neighbor_num);

      camera_calib->Camera(cam_id).key_frames[ii].clear();
      for (std::size_t m = 0; m < neighbor_num; ++m) {
        camera_calib->Camera(cam_id).key_frames[ii].push_back(kfids[key_neighbors[m]]);
      }
      const Eigen::Vector4d &qcw = reconstruction->Image(iid.first).Qvec();
      const Eigen::Vector3d &tcw = reconstruction->Image(iid.first).Tvec();
      const Eigen::Vector4d &qbc = camera_calib->Camera(cam_id).rel_qvec;
      const Eigen::Vector3d &tbc = camera_calib->Camera(cam_id).rel_tvec;
      Eigen::Quaterniond quat_cw(qcw[0], qcw[1], qcw[2], qcw[3]);
      Eigen::Quaterniond quat_bc(qbc[0], qbc[1], qbc[2], qbc[3]);
      Eigen::Quaterniond quat = quat_bc * quat_cw;
      quat.normalize();
      Eigen::Vector3d trans = quat_bc * tcw + tbc;
      trans = quat.conjugate() * (-trans);
      quat = quat.conjugate();
      Eigen::Vector4d qwb(quat.w(), quat.x(), quat.y(), quat.z());
      camera_calib->Camera(cam_id).key_poses[ii] = std::pair<Eigen::Vector4d, Eigen::Vector3d>(qwb, trans);
      ++ii;
    }
  }

  for (auto &cam_id : camera_calib->GetCameraIds()) {
    auto &calib_cam = camera_calib->Camera(cam_id);
    for (std::size_t i = 0; i < calib_cam.images.size(); ++i) {
      double* qbc = calib_cam.rel_qvec.data();
      double* tbc = calib_cam.rel_tvec.data();
      double* dt = &(camera_calib->rel_time_);
      const double tc = calib_cam.images[i].second;
      double* qcw = reconstruction->Image(calib_cam.images[i].first).Qvec().data();
      double* tcw = reconstruction->Image(calib_cam.images[i].first).Tvec().data();

      if (neighbor_num == 2) {
        double* qwb0 = trajectory->KeyFrame(calib_cam.key_frames[i][0]).Qvec().data();
        double* twb0 = trajectory->KeyFrame(calib_cam.key_frames[i][0]).Tvec().data();
        const double t0 = trajectory->KeyFrame(calib_cam.key_frames[i][0]).Timestamp();
        double* qwb1 = trajectory->KeyFrame(calib_cam.key_frames[i][1]).Qvec().data();
        double* twb1 = trajectory->KeyFrame(calib_cam.key_frames[i][1]).Tvec().data();
        const double t1 = trajectory->KeyFrame(calib_cam.key_frames[i][1]).Timestamp();

        ceres::CostFunction* cost_function = nullptr;
        cost_function = AlignmentCostFunction::Create(tc, t0, t1, align_options_.weight_q_interp, align_options_.weight_t_interp);

        problem_->AddResidualBlock(cost_function, nullptr, qbc, tbc, dt, qcw, tcw, qwb0, twb0, qwb1, twb1);
      } else if (neighbor_num == 4) {
        double* qwb = calib_cam.key_poses[i].first.data();
        double* twb = calib_cam.key_poses[i].second.data();
        {
          ceres::CostFunction* cost_function = nullptr;
          cost_function = RigidTransformationCostFunction::Create(align_options_.weight_q_rigid, align_options_.weight_t_rigid);
          problem_->AddResidualBlock(cost_function, nullptr, qbc, tbc, qcw, tcw, qwb, twb);
        }

        for (std::size_t j = 1; j < calib_cam.key_frames[i].size(); ++j) {
          double* qwb0 = trajectory->KeyFrame(calib_cam.key_frames[i][j - 1]).Qvec().data();
          double* twb0 = trajectory->KeyFrame(calib_cam.key_frames[i][j - 1]).Tvec().data();
          const double t0 = trajectory->KeyFrame(calib_cam.key_frames[i][j - 1]).Timestamp();
          double* qwb1 = trajectory->KeyFrame(calib_cam.key_frames[i][j]).Qvec().data();
          double* twb1 = trajectory->KeyFrame(calib_cam.key_frames[i][j]).Tvec().data();
          const double t1 = trajectory->KeyFrame(calib_cam.key_frames[i][j]).Timestamp();

          double deltat = tc + camera_calib->rel_time_;
          deltat = std::max(std::fabs(t0 - deltat), std::fabs(t1 - deltat));
          std::vector<double> weightq = align_options_.weight_q_interp;
          std::vector<double> weightt = align_options_.weight_t_interp;
          for (int kk = 0; kk < 3; ++kk) {
            weightq[kk] *= std::fabs(t1 - t0) / deltat;
            weightt[kk] *= std::fabs(t1 - t0) / deltat;
          }

          ceres::CostFunction* cost_function = nullptr;
          cost_function = InterpolationCostFunction::Create(tc, t0, t1, weightq, weightt);

          problem_->AddResidualBlock(cost_function, nullptr, qwb, twb, dt, qwb0, twb0, qwb1, twb1);
        }
      }
    }
  }
}

void AlignmentBundleAdjuster::AddStaticConstraintToProblem(Trajectory* trajectory) {
  const auto &static_sements = trajectory->StaticSegments();
  const auto &kfids = trajectory->KeyFrameIds();

  for (std::size_t idx = 0; idx < static_sements.size(); ++idx) {
    KeyFrame& static_frame = trajectory->StaticFrame(idx);

    double* qvec_data = static_frame.Qvec().data();
    double* tvec_data = static_frame.Tvec().data();

    for (std::size_t j = static_sements[idx].first; j <= static_sements[idx].second; ++j) {
      KeyFrame& key_frame = trajectory->KeyFrame(kfids[j]);
      double* qvec_dataj = key_frame.Qvec().data();
      double* tvec_dataj = key_frame.Tvec().data();

      Eigen::Vector4d deltaq(1.0, 0.0, 0.0, 0.0);
      Eigen::Vector3d deltat = Eigen::Vector3d::Zero();

      ceres::CostFunction* cost_function = nullptr;
      cost_function = RelPoseCostFunction::Create(deltaq, deltat, traj_options_.weight_q_static, traj_options_.weight_t_static);

      problem_->AddResidualBlock(cost_function, nullptr, qvec_data, tvec_data, qvec_dataj, tvec_dataj);
    }

    SetQuaternionManifold(problem_.get(), qvec_data);
  }
}

void PrintSolverSummary(const ceres::Solver::Summary& summary) {
  std::cout << std::right << std::setw(16) << "Residuals : ";
  std::cout << std::left << summary.num_residuals_reduced << std::endl;

  std::cout << std::right << std::setw(16) << "Parameters : ";
  std::cout << std::left << summary.num_effective_parameters_reduced
            << std::endl;

  std::cout << std::right << std::setw(16) << "Iterations : ";
  std::cout << std::left
            << summary.num_successful_steps + summary.num_unsuccessful_steps
            << std::endl;

  std::cout << std::right << std::setw(16) << "Time : ";
  std::cout << std::left << summary.total_time_in_seconds << " [s]"
            << std::endl;

  std::cout << std::right << std::setw(16) << "Initial cost : ";
  std::cout << std::right << std::setprecision(6)
            << std::sqrt(summary.initial_cost / summary.num_residuals_reduced)
            << " [px]" << std::endl;

  std::cout << std::right << std::setw(16) << "Final cost : ";
  std::cout << std::right << std::setprecision(6)
            << std::sqrt(summary.final_cost / summary.num_residuals_reduced)
            << " [px]" << std::endl;

  std::cout << std::right << std::setw(16) << "Termination : ";

  std::string termination = "";

  switch (summary.termination_type) {
    case ceres::CONVERGENCE:
      termination = "Convergence";
      break;
    case ceres::NO_CONVERGENCE:
      termination = "No convergence";
      break;
    case ceres::FAILURE:
      termination = "Failure";
      break;
    case ceres::USER_SUCCESS:
      termination = "User success";
      break;
    case ceres::USER_FAILURE:
      termination = "User failure";
      break;
    default:
      termination = "Unknown";
      break;
  }

  std::cout << std::right << termination << std::endl;
  std::cout << std::endl;
}

}  // namespace colmap
