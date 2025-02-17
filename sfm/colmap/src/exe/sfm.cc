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

#include "exe/sfm.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "base/reconstruction.h"
#include "controllers/automatic_reconstruction.h"
#include "controllers/bundle_adjustment.h"
#include "controllers/hierarchical_mapper.h"
#include "exe/gui.h"
#include "util/misc.h"
#include "util/opengl_utils.h"
#include "util/option_manager.h"
#include "base/camera_calib.h"
#include "base/trajectory.h"

namespace colmap {

int RunAutomaticReconstructor(int argc, char** argv) {
  AutomaticReconstructionController::Options reconstruction_options;
  std::string data_type = "individual";
  std::string quality = "high";
  std::string mesher = "poisson";

  OptionManager options;
  options.AddRequiredOption("workspace_path",
                            &reconstruction_options.workspace_path);
  options.AddRequiredOption("image_path", &reconstruction_options.image_path);
  options.AddDefaultOption("mask_path", &reconstruction_options.mask_path);
  options.AddDefaultOption("vocab_tree_path",
                           &reconstruction_options.vocab_tree_path);
  options.AddDefaultOption("data_type", &data_type,
                           "{individual, video, internet}");
  options.AddDefaultOption("quality", &quality, "{low, medium, high, extreme}");
  options.AddDefaultOption("camera_model",
                           &reconstruction_options.camera_model);
  options.AddDefaultOption("single_camera",
                           &reconstruction_options.single_camera);
  options.AddDefaultOption("sparse", &reconstruction_options.sparse);
  options.AddDefaultOption("dense", &reconstruction_options.dense);
  options.AddDefaultOption("mesher", &mesher, "{poisson, delaunay}");
  options.AddDefaultOption("num_threads", &reconstruction_options.num_threads);
  options.AddDefaultOption("use_gpu", &reconstruction_options.use_gpu);
  options.AddDefaultOption("gpu_index", &reconstruction_options.gpu_index);
  options.Parse(argc, argv);

  StringToLower(&data_type);
  if (data_type == "individual") {
    reconstruction_options.data_type =
        AutomaticReconstructionController::DataType::INDIVIDUAL;
  } else if (data_type == "video") {
    reconstruction_options.data_type =
        AutomaticReconstructionController::DataType::VIDEO;
  } else if (data_type == "internet") {
    reconstruction_options.data_type =
        AutomaticReconstructionController::DataType::INTERNET;
  } else {
    LOG(FATAL) << "Invalid data type provided";
  }

  StringToLower(&quality);
  if (quality == "low") {
    reconstruction_options.quality =
        AutomaticReconstructionController::Quality::LOW;
  } else if (quality == "medium") {
    reconstruction_options.quality =
        AutomaticReconstructionController::Quality::MEDIUM;
  } else if (quality == "high") {
    reconstruction_options.quality =
        AutomaticReconstructionController::Quality::HIGH;
  } else if (quality == "extreme") {
    reconstruction_options.quality =
        AutomaticReconstructionController::Quality::EXTREME;
  } else {
    LOG(FATAL) << "Invalid quality provided";
  }

  StringToLower(&mesher);
  if (mesher == "poisson") {
    reconstruction_options.mesher =
        AutomaticReconstructionController::Mesher::POISSON;
  } else if (mesher == "delaunay") {
    reconstruction_options.mesher =
        AutomaticReconstructionController::Mesher::DELAUNAY;
  } else {
    LOG(FATAL) << "Invalid mesher provided";
  }

  ReconstructionManager reconstruction_manager;

  if (reconstruction_options.use_gpu && kUseOpenGL) {
    QApplication app(argc, argv);
    AutomaticReconstructionController controller(reconstruction_options,
                                                 &reconstruction_manager);
    RunThreadWithOpenGLContext(&controller);
  } else {
    AutomaticReconstructionController controller(reconstruction_options,
                                                 &reconstruction_manager);
    controller.Start();
    controller.Wait();
  }

  return EXIT_SUCCESS;
}

int RunBundleAdjuster(int argc, char** argv) {
  std::string input_path;
  std::string output_path;

  OptionManager options;
  options.AddRequiredOption("input_path", &input_path);
  options.AddRequiredOption("output_path", &output_path);
  options.AddBundleAdjustmentOptions();
  options.Parse(argc, argv);

  if (!ExistsDir(input_path)) {
    std::cerr << "ERROR: `input_path` is not a directory" << std::endl;
    return EXIT_FAILURE;
  }

  if (!ExistsDir(output_path)) {
    std::cerr << "ERROR: `output_path` is not a directory" << std::endl;
    return EXIT_FAILURE;
  }

  Reconstruction reconstruction;
  reconstruction.Read(input_path);

  BundleAdjustmentController ba_controller(options, &reconstruction);
  ba_controller.Start();
  ba_controller.Wait();

  reconstruction.Write(output_path);

  return EXIT_SUCCESS;
}

int RunColorExtractor(int argc, char** argv) {
  std::string input_path;
  std::string output_path;

  OptionManager options;
  options.AddImageOptions();
  options.AddDefaultOption("input_path", &input_path);
  options.AddRequiredOption("output_path", &output_path);
  options.Parse(argc, argv);

  Reconstruction reconstruction;
  reconstruction.Read(input_path);
  reconstruction.ExtractColorsForAllImages(*options.image_path);
  reconstruction.Write(output_path);

  return EXIT_SUCCESS;
}

int RunMapper(int argc, char** argv) {
  std::string input_path;
  std::string output_path;
  std::string image_list_path;

  OptionManager options;
  options.AddDatabaseOptions();
  options.AddImageOptions();
  options.AddDefaultOption("input_path", &input_path);
  options.AddRequiredOption("output_path", &output_path);
  options.AddDefaultOption("image_list_path", &image_list_path);
  options.AddMapperOptions();
  options.Parse(argc, argv);

  if (!ExistsDir(output_path)) {
    std::cerr << "ERROR: `output_path` is not a directory." << std::endl;
    return EXIT_FAILURE;
  }

  if (!image_list_path.empty()) {
    const auto image_names = ReadTextFileLines(image_list_path);
    options.mapper->image_names =
        std::unordered_set<std::string>(image_names.begin(), image_names.end());
  }

  ReconstructionManager reconstruction_manager;
  if (input_path != "") {
    if (!ExistsDir(input_path)) {
      std::cerr << "ERROR: `input_path` is not a directory." << std::endl;
      return EXIT_FAILURE;
    }
    reconstruction_manager.Read(input_path);
  }

  IncrementalMapperController mapper(options.mapper.get(), *options.image_path,
                                     *options.database_path,
                                     &reconstruction_manager);

  // In case a new reconstruction is started, write results of individual sub-
  // models to as their reconstruction finishes instead of writing all results
  // after all reconstructions finished.
  size_t prev_num_reconstructions = 0;
  if (input_path == "") {
    mapper.AddCallback(
        IncrementalMapperController::LAST_IMAGE_REG_CALLBACK, [&]() {
          // If the number of reconstructions has not changed, the last model
          // was discarded for some reason.
          if (reconstruction_manager.Size() > prev_num_reconstructions) {
            const std::string reconstruction_path = JoinPaths(
                output_path, std::to_string(prev_num_reconstructions));
            const auto& reconstruction =
                reconstruction_manager.Get(prev_num_reconstructions);
            CreateDirIfNotExists(reconstruction_path);
            reconstruction.Write(reconstruction_path);
            options.Write(JoinPaths(reconstruction_path, "project.ini"));
            prev_num_reconstructions = reconstruction_manager.Size();
          }
        });
  }

  mapper.Start();
  mapper.Wait();

  if (reconstruction_manager.Size() == 0) {
    std::cerr << "ERROR: failed to create sparse model" << std::endl;
    return EXIT_FAILURE;
  }

  // In case the reconstruction is continued from an existing reconstruction, do
  // not create sub-folders but directly write the results.
  if (input_path != "" && reconstruction_manager.Size() > 0) {
    reconstruction_manager.Get(0).Write(output_path);
  }

  return EXIT_SUCCESS;
}

int RunHierarchicalMapper(int argc, char** argv) {
  HierarchicalMapperController::Options hierarchical_options;
  SceneClustering::Options clustering_options;
  std::string output_path;

  OptionManager options;
  options.AddRequiredOption("database_path",
                            &hierarchical_options.database_path);
  options.AddRequiredOption("image_path", &hierarchical_options.image_path);
  options.AddRequiredOption("output_path", &output_path);
  options.AddDefaultOption("num_workers", &hierarchical_options.num_workers);
  options.AddDefaultOption("image_overlap", &clustering_options.image_overlap);
  options.AddDefaultOption("leaf_max_num_images",
                           &clustering_options.leaf_max_num_images);
  options.AddMapperOptions();
  options.Parse(argc, argv);

  if (!ExistsDir(output_path)) {
    std::cerr << "ERROR: `output_path` is not a directory." << std::endl;
    return EXIT_FAILURE;
  }

  ReconstructionManager reconstruction_manager;

  HierarchicalMapperController hierarchical_mapper(
      hierarchical_options, clustering_options, *options.mapper,
      &reconstruction_manager);
  hierarchical_mapper.Start();
  hierarchical_mapper.Wait();

  if (reconstruction_manager.Size() == 0) {
    std::cerr << "ERROR: failed to create sparse model" << std::endl;
    return EXIT_FAILURE;
  }

  reconstruction_manager.Write(output_path, &options);

  return EXIT_SUCCESS;
}

int RunPointFiltering(int argc, char** argv) {
  std::string input_path;
  std::string output_path;

  size_t min_track_len = 2;
  double max_reproj_error = 4.0;
  double min_tri_angle = 1.5;

  OptionManager options;
  options.AddRequiredOption("input_path", &input_path);
  options.AddRequiredOption("output_path", &output_path);
  options.AddDefaultOption("min_track_len", &min_track_len);
  options.AddDefaultOption("max_reproj_error", &max_reproj_error);
  options.AddDefaultOption("min_tri_angle", &min_tri_angle);
  options.Parse(argc, argv);

  Reconstruction reconstruction;
  reconstruction.Read(input_path);

  size_t num_filtered =
      reconstruction.FilterAllPoints3D(max_reproj_error, min_tri_angle);

  for (const auto point3D_id : reconstruction.Point3DIds()) {
    const auto& point3D = reconstruction.Point3D(point3D_id);
    if (point3D.Track().Length() < min_track_len) {
      num_filtered += point3D.Track().Length();
      reconstruction.DeletePoint3D(point3D_id);
    }
  }

  std::cout << "Filtered observations: " << num_filtered << std::endl;

  reconstruction.Write(output_path);

  return EXIT_SUCCESS;
}

int RunPointTriangulator(int argc, char** argv) {
  std::string input_path;
  std::string output_path;
  bool clear_points = false;

  OptionManager options;
  options.AddDatabaseOptions();
  options.AddImageOptions();
  options.AddRequiredOption("input_path", &input_path);
  options.AddRequiredOption("output_path", &output_path);
  options.AddDefaultOption(
      "clear_points", &clear_points,
      "Whether to clear all existing points and observations");
  options.AddMapperOptions();
  options.Parse(argc, argv);

  if (!ExistsDir(input_path)) {
    std::cerr << "ERROR: `input_path` is not a directory" << std::endl;
    return EXIT_FAILURE;
  }

  if (!ExistsDir(output_path)) {
    std::cerr << "ERROR: `output_path` is not a directory" << std::endl;
    return EXIT_FAILURE;
  }

  PrintHeading1("Loading model");

  Reconstruction reconstruction;
  reconstruction.Read(input_path);

  return RunPointTriangulatorImpl(reconstruction, *options.database_path,
                                  *options.image_path, output_path,
                                  *options.mapper, clear_points);
}

int RunPointTriangulatorImpl(Reconstruction& reconstruction,
                             const std::string database_path,
                             const std::string image_path,
                             const std::string output_path,
                             const IncrementalMapperOptions& mapper_options,
                             const bool clear_points) {
  PrintHeading1("Loading database");

  DatabaseCache database_cache;

  {
    Timer timer;
    timer.Start();

    Database database(database_path);

    const size_t min_num_matches =
        static_cast<size_t>(mapper_options.min_num_matches);
    database_cache.Load(database, min_num_matches,
                        mapper_options.ignore_watermarks,
                        mapper_options.image_names);

    if (clear_points) {
      reconstruction.DeleteAllPoints2DAndPoints3D();
      reconstruction.TranscribeImageIdsToDatabase(database);
    }

    std::cout << std::endl;
    timer.PrintMinutes();
  }

  std::cout << std::endl;

  CHECK_GE(reconstruction.NumRegImages(), 2)
      << "Need at least two images for triangulation";

  IncrementalMapper mapper(&database_cache);
  mapper.BeginReconstruction(&reconstruction);

  //////////////////////////////////////////////////////////////////////////////
  // Triangulation
  //////////////////////////////////////////////////////////////////////////////

  const auto tri_options = mapper_options.Triangulation();

  const auto& reg_image_ids = reconstruction.RegImageIds();

  for (size_t i = 0; i < reg_image_ids.size(); ++i) {
    const image_t image_id = reg_image_ids[i];

    const auto& image = reconstruction.Image(image_id);

    PrintHeading1(StringPrintf("Triangulating image #%d (%d)", image_id, i));

    const size_t num_existing_points3D = image.NumPoints3D();

    std::cout << "  => Image sees " << num_existing_points3D << " / "
              << image.NumObservations() << " points" << std::endl;

    mapper.TriangulateImage(tri_options, image_id);

    std::cout << "  => Triangulated "
              << (image.NumPoints3D() - num_existing_points3D) << " points"
              << std::endl;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Retriangulation
  //////////////////////////////////////////////////////////////////////////////

  PrintHeading1("Retriangulation");

  CompleteAndMergeTracks(mapper_options, &mapper);

  //////////////////////////////////////////////////////////////////////////////
  // Bundle adjustment
  //////////////////////////////////////////////////////////////////////////////

  auto ba_options = mapper_options.GlobalBundleAdjustment();
  ba_options.refine_focal_length = false;
  ba_options.refine_principal_point = false;
  ba_options.refine_extra_params = false;
  ba_options.refine_extrinsics = false;

  // Configure bundle adjustment.
  BundleAdjustmentConfig ba_config;
  for (const image_t image_id : reconstruction.RegImageIds()) {
    ba_config.AddImage(image_id);
  }

  for (int i = 0; i < mapper_options.ba_global_max_refinements; ++i) {
    // Avoid degeneracies in bundle adjustment.
    reconstruction.FilterObservationsWithNegativeDepth();

    const size_t num_observations = reconstruction.ComputeNumObservations();

    PrintHeading1("Bundle adjustment");
    BundleAdjuster bundle_adjuster(ba_options, ba_config);
    CHECK(bundle_adjuster.Solve(&reconstruction));

    size_t num_changed_observations = 0;
    num_changed_observations += CompleteAndMergeTracks(mapper_options, &mapper);
    num_changed_observations += FilterPoints(mapper_options, &mapper);
    const double changed =
        static_cast<double>(num_changed_observations) / num_observations;
    std::cout << StringPrintf("  => Changed observations: %.6f", changed)
              << std::endl;
    if (changed < mapper_options.ba_global_max_refinement_change) {
      break;
    }
  }

  PrintHeading1("Extracting colors");
  reconstruction.ExtractColorsForAllImages(image_path);

  const bool kDiscardReconstruction = false;
  mapper.EndReconstruction(kDiscardReconstruction);

  reconstruction.Write(output_path);

  return EXIT_SUCCESS;
}

namespace {

// Read the configuration of the camera rigs from a JSON file. The input images
// of a camera rig must be named consistently to assign them to the appropriate
// camera rig and the respective snapshots.
//
// An example configuration of a single camera rig:
// [
//   {
//     "ref_camera_id": 1,
//     "cameras":
//     [
//       {
//           "camera_id": 1,
//           "image_prefix": "left1_image"
//           "rel_tvec": [0, 0, 0],
//           "rel_qvec": [1, 0, 0, 0]
//       },
//       {
//           "camera_id": 2,
//           "image_prefix": "left2_image"
//           "rel_tvec": [0, 0, 0],
//           "rel_qvec": [0, 1, 0, 0]
//       },
//       {
//           "camera_id": 3,
//           "image_prefix": "right1_image"
//           "rel_tvec": [0, 0, 0],
//           "rel_qvec": [0, 0, 1, 0]
//       },
//       {
//           "camera_id": 4,
//           "image_prefix": "right2_image"
//           "rel_tvec": [0, 0, 0],
//           "rel_qvec": [0, 0, 0, 1]
//       }
//     ]
//   }
// ]
//
// The "camera_id" and "image_prefix" fields are required, whereas the
// "rel_tvec" and "rel_qvec" fields optionally specify the relative
// extrinsics of the camera rig in the form of a translation vector and a
// rotation quaternion. The relative extrinsics rel_qvec and rel_tvec transform
// coordinates from rig to camera coordinate space. If the relative extrinsics
// are not provided then they are automatically inferred from the
// reconstruction.
//
// This file specifies the configuration for a single camera rig and that you
// could potentially define multiple camera rigs. The rig is composed of 4
// cameras: all images of the first camera must have "left1_image" as a name
// prefix, e.g., "left1_image_frame000.png" or "left1_image/frame000.png".
// Images with the same suffix ("_frame000.png" and "/frame000.png") are
// assigned to the same snapshot, i.e., they are assumed to be captured at the
// same time. Only snapshots with the reference image registered will be added
// to the bundle adjustment problem. The remaining images will be added with
// independent poses to the bundle adjustment problem. The above configuration
// could have the following input image file structure:
//
//    /path/to/images/...
//        left1_image/...
//            frame000.png
//            frame001.png
//            frame002.png
//            ...
//        left2_image/...
//            frame000.png
//            frame001.png
//            frame002.png
//            ...
//        right1_image/...
//            frame000.png
//            frame001.png
//            frame002.png
//            ...
//        right2_image/...
//            frame000.png
//            frame001.png
//            frame002.png
//            ...
//
std::vector<CameraRig> ReadCameraRigConfig(const std::string& rig_config_path,
                                           const Reconstruction& reconstruction,
                                           bool estimate_rig_relative_poses) {
  boost::property_tree::ptree pt;
  boost::property_tree::read_json(rig_config_path.c_str(), pt);

  std::vector<CameraRig> camera_rigs;
  for (const auto& rig_config : pt) {
    CameraRig camera_rig;

    std::vector<std::string> image_prefixes;
    for (const auto& camera : rig_config.second.get_child("cameras")) {
      const int camera_id = camera.second.get<int>("camera_id");
      image_prefixes.push_back(camera.second.get<std::string>("image_prefix"));
      Eigen::Vector3d rel_tvec;
      Eigen::Vector4d rel_qvec;
      int index = 0;
      auto rel_tvec_node = camera.second.get_child_optional("rel_tvec");
      if (rel_tvec_node) {
        for (const auto& node : rel_tvec_node.get()) {
          rel_tvec[index++] = node.second.get_value<double>();
        }
      } else {
        estimate_rig_relative_poses = true;
      }
      index = 0;
      auto rel_qvec_node = camera.second.get_child_optional("rel_qvec");
      if (rel_qvec_node) {
        for (const auto& node : rel_qvec_node.get()) {
          rel_qvec[index++] = node.second.get_value<double>();
        }
      } else {
        estimate_rig_relative_poses = true;
      }

      camera_rig.AddCamera(camera_id, rel_qvec, rel_tvec);
    }

    camera_rig.SetRefCameraId(rig_config.second.get<int>("ref_camera_id"));

    std::unordered_map<std::string, std::vector<image_t>> snapshots;
    for (const auto image_id : reconstruction.RegImageIds()) {
      const auto& image = reconstruction.Image(image_id);
      for (const auto& image_prefix : image_prefixes) {
        if (StringContains(image.Name(), image_prefix)) {
          const std::string image_suffix =
              StringGetAfter(image.Name(), image_prefix);
          snapshots[image_suffix].push_back(image_id);
        }
      }
    }

    for (const auto& snapshot : snapshots) {
      bool has_ref_camera = false;
      for (const auto image_id : snapshot.second) {
        const auto& image = reconstruction.Image(image_id);
        if (image.CameraId() == camera_rig.RefCameraId()) {
          has_ref_camera = true;
        }
      }

      if (has_ref_camera) {
        camera_rig.AddSnapshot(snapshot.second);
      }
    }

    camera_rig.Check(reconstruction);
    if (estimate_rig_relative_poses) {
      PrintHeading2("Estimating relative rig poses");
      if (!camera_rig.ComputeRelativePoses(reconstruction)) {
        std::cout << "WARN: Failed to estimate rig poses from reconstruction; "
                     "cannot use rig BA"
                  << std::endl;
        return std::vector<CameraRig>();
      }
    }

    camera_rigs.push_back(camera_rig);
  }

  return camera_rigs;
}

}  // namespace

int RunRigBundleAdjuster(int argc, char** argv) {
  std::string input_path;
  std::string output_path;
  std::string rig_config_path;
  bool estimate_rig_relative_poses = true;

  RigBundleAdjuster::Options rig_ba_options;

  OptionManager options;
  options.AddRequiredOption("input_path", &input_path);
  options.AddRequiredOption("output_path", &output_path);
  options.AddRequiredOption("rig_config_path", &rig_config_path);
  options.AddDefaultOption("estimate_rig_relative_poses",
                           &estimate_rig_relative_poses);
  options.AddDefaultOption("RigBundleAdjustment.refine_relative_poses",
                           &rig_ba_options.refine_relative_poses);
  options.AddBundleAdjustmentOptions();
  options.Parse(argc, argv);

  Reconstruction reconstruction;
  reconstruction.Read(input_path);

  PrintHeading1("Camera rig configuration");

  auto camera_rigs = ReadCameraRigConfig(rig_config_path, reconstruction,
                                         estimate_rig_relative_poses);

  BundleAdjustmentConfig config;
  for (size_t i = 0; i < camera_rigs.size(); ++i) {
    const auto& camera_rig = camera_rigs[i];
    PrintHeading2(StringPrintf("Camera Rig %d", i + 1));
    std::cout << StringPrintf("Cameras: %d", camera_rig.NumCameras())
              << std::endl;
    std::cout << StringPrintf("Snapshots: %d", camera_rig.NumSnapshots())
              << std::endl;

    // Add all registered images to the bundle adjustment configuration.
    for (const auto image_id : reconstruction.RegImageIds()) {
      config.AddImage(image_id);
    }
  }

  PrintHeading1("Rig bundle adjustment");

  BundleAdjustmentOptions ba_options = *options.bundle_adjustment;
  ba_options.solver_options.minimizer_progress_to_stdout = true;
  RigBundleAdjuster bundle_adjuster(ba_options, rig_ba_options, config);
  CHECK(bundle_adjuster.Solve(&reconstruction, &camera_rigs));

  reconstruction.Write(output_path);

  return EXIT_SUCCESS;
}

void WriteAlignment(const std::string &cam_name,
                    const boost::property_tree::ptree &pt,
                    Reconstruction* reconstruction,
                    CameraCalib* camera_calib,
                    Trajectory* trajectory) {
  std::string local_path = pt.get<std::string>("local_path");
  {
    std::string path = local_path + "/" + pt.get<std::string>(cam_name + "_sfm_opti_image_poses");
    std::ofstream file(path, std::ios::trunc);
    CHECK(file.is_open()) << path;
    // Ensure that we don't loose any precision by storing in text.
    file.precision(17);
    file << "# ImagePoses list:" << std::endl;
    file << "# ImagePoses ID, QW, QX, QY, QZ, TX, TY, TZ, NAME" << std::endl;

    for (const auto& cam_id : camera_calib->GetCameraIds()) {
      for (const auto& kid : camera_calib->Camera(cam_id).images) {
        const auto& image = reconstruction->Image(kid.first);
        std::ostringstream line;
        line.precision(17);

        std::string line_string;

        line << kid.first << " ";

        // QVEC (qw, qx, qy, qz)
        Eigen::Vector4d qvec = image.Qvec();
        Eigen::Quaterniond quat(qvec[0], qvec[1], qvec[2], qvec[3]);
        quat.normalize();
        Eigen::Vector3d tvec = image.Tvec();
        tvec = quat.conjugate() * (-tvec);
        quat = quat.conjugate();
        qvec[0] = quat.w();
        qvec[1] = quat.x();
        qvec[2] = quat.y();
        qvec[3] = quat.z();
        line << qvec(0) << " ";
        line << qvec(1) << " ";
        line << qvec(2) << " ";
        line << qvec(3) << " ";

        // TVEC
        line << tvec(0) << " ";
        line << tvec(1) << " ";
        line << tvec(2) << " ";

        line << image.Name();

        file << line.str() << std::endl;
      }
    }
  }

  {
    std::string path = local_path + "/" + pt.get<std::string>(cam_name + "_sfm_opti_key_poses_1");
    std::ofstream file(path, std::ios::trunc);
    CHECK(file.is_open()) << path;
    // Ensure that we don't loose any precision by storing in text.
    file.precision(17);
    file << "# KeyPoses_1 list:" << std::endl;
    file << "# KeyPoses_1 ID, QW, QX, QY, QZ, TX, TY, TZ, NAME" << std::endl;

    for (const auto& cam_id : camera_calib->GetCameraIds()) {
      const auto &camera = camera_calib->Camera(cam_id);
      for (std::size_t i = 0; i < camera.images.size(); ++i) {
        const auto &kid = camera.images[i];
        const auto& image = reconstruction->Image(kid.first);
        std::ostringstream line;
        line.precision(17);

        std::string line_string;

        line << kid.first << " ";

        // QVEC (qw, qx, qy, qz)
        const Eigen::Vector4d normalized_qvec = NormalizeQuaternion(camera.key_poses[i].first);
        line << normalized_qvec(0) << " ";
        line << normalized_qvec(1) << " ";
        line << normalized_qvec(2) << " ";
        line << normalized_qvec(3) << " ";

        // TVEC
        line << camera.key_poses[i].second(0) << " ";
        line << camera.key_poses[i].second(1) << " ";
        line << camera.key_poses[i].second(2) << " ";

        line << image.Name();

        file << line.str() << std::endl;
      }
    }
  }

  {
    std::string path = local_path + "/" + pt.get<std::string>(cam_name + "_sfm_opti_key_poses_2");
    std::ofstream file(path, std::ios::trunc);
    CHECK(file.is_open()) << path;
    // Ensure that we don't loose any precision by storing in text.
    file.precision(17);
    file << "# KeyPoses_2 list:" << std::endl;
    file << "# KeyPoses_2 ID, QW, QX, QY, QZ, TX, TY, TZ, NAME" << std::endl;

    for (const auto& cam_id : camera_calib->GetCameraIds()) {
      const auto &camera = camera_calib->Camera(cam_id);

      Eigen::Vector4d qvec_bc = camera.rel_qvec;
      Eigen::Quaterniond quat_bc(qvec_bc[0], qvec_bc[1], qvec_bc[2], qvec_bc[3]);
      quat_bc.normalize();
      Eigen::Vector3d tvec_bc = camera.rel_tvec;

      for (std::size_t i = 0; i < camera.images.size(); ++i) {
        const auto &kid = camera.images[i];
        const auto& image = reconstruction->Image(kid.first);
        std::ostringstream line;
        line.precision(17);

        std::string line_string;

        line << kid.first << " ";

        // QVEC (qw, qx, qy, qz)
        Eigen::Vector4d qvec = image.Qvec();
        Eigen::Quaterniond quat(qvec[0], qvec[1], qvec[2], qvec[3]);
        quat.normalize();
        Eigen::Vector3d tvec = image.Tvec();
        tvec = quat_bc * tvec + tvec_bc;
        quat = quat_bc * quat;
        tvec = quat.conjugate() * (-tvec);
        quat = quat.conjugate();
        qvec[0] = quat.w();
        qvec[1] = quat.x();
        qvec[2] = quat.y();
        qvec[3] = quat.z();
        line << qvec(0) << " ";
        line << qvec(1) << " ";
        line << qvec(2) << " ";
        line << qvec(3) << " ";

        // TVEC
        line << tvec(0) << " ";
        line << tvec(1) << " ";
        line << tvec(2) << " ";

        line << image.Name();

        file << line.str() << std::endl;
      }
    }
  }
}

std::vector<double> GetDoubleList(const boost::property_tree::ptree& pt) {
  std::vector<double> read_list;
  for (const auto& value : pt) {
    read_list.push_back(value.second.get_value<double>());
  }
  return read_list;
}

int RunTrajectoryAlignment(int argc, char** argv) {
  std::string config_path;
  std::string cam_name;
  bool clear_points = false;

  OptionManager options;
  options.AddRequiredOption("config_path", &config_path);
  options.AddRequiredOption("cam_name", &cam_name);
  options.AddDefaultOption(
      "clear_points", &clear_points,
      "Whether to clear all existing points and observations");
  options.AddMapperOptions();
  options.Parse(argc, argv);

  if (!ExistsPath(config_path)) {
    std::cerr << "ERROR: `config_path` is not a path" << std::endl;
    return EXIT_FAILURE;
  }

  IncrementalMapperOptions& mapper_options = *(options.mapper);
  mapper_options.mapper.filter_max_reproj_error = 40.0;
  mapper_options.mapper.filter_min_tri_angle = 0.2;

  PrintHeading1("Loading config");
  boost::property_tree::ptree pt;
  boost::property_tree::read_json(config_path.c_str(), pt);

  std::string local_path = pt.get<std::string>("local_path");
  std::string cam_subpath = pt.get<std::string>(cam_name + "_sfm_subdir");
  std::string cam_path = local_path + "/" + cam_subpath;

  if (!ExistsDir(cam_path)) {
    std::cerr << "ERROR: `cam_path` is not a directory: " << cam_path << std::endl;
    return EXIT_FAILURE;
  }

  std::string input_path = local_path + "/" + pt.get<std::string>(cam_name + "_sfm_init_subdir");
  PrintHeading1("Loading model");
  Reconstruction reconstruction;
  reconstruction.Read(input_path);

  std::string calib_path = local_path + "/" + pt.get<std::string>(cam_name + "_sfm_init_calib");
  PrintHeading1("Loading calib");
  CameraCalib camera_calib;
  camera_calib.Read(calib_path);

  std::string traj_path = local_path + "/" + pt.get<std::string>(cam_name + "_sfm_init_trajectory");
  PrintHeading1("Loading trajectory");
  Trajectory traj;
  traj.Read(traj_path);

  const auto &static_times = pt.get_child("static_time_segments");
  std::vector<std::pair<double, double>> time_segs;
  for (const auto &seg : static_times) {
    std::vector<double> time_seg = GetDoubleList(seg.second);
    if (time_seg.size() != 2) {
      std::cerr << "ERROR: static_time_segments size != 2 !!!" << std::endl;
      return EXIT_FAILURE;
    }
    time_segs.emplace_back(time_seg[0], time_seg[1]);
  }
  traj.SetStaticSegments(time_segs);

  PrintHeading1("Loading database");

  DatabaseCache database_cache;

  {
    Timer timer;
    timer.Start();

    std::string database_path = local_path + "/" + pt.get<std::string>(cam_name + "_sfm_opti_subdir") + "/database.db";
    Database database(database_path);

    const size_t min_num_matches =
        static_cast<size_t>(mapper_options.min_num_matches);
    database_cache.Load(database, min_num_matches,
                        mapper_options.ignore_watermarks,
                        mapper_options.image_names);

    if (clear_points) {
      reconstruction.DeleteAllPoints2DAndPoints3D();
      reconstruction.TranscribeImageIdsToDatabase(database);
    }

    std::cout << std::endl;
    timer.PrintMinutes();
  }

  std::cout << std::endl;

  CHECK_GE(reconstruction.NumRegImages(), 2)
      << "Need at least two images for triangulation";

  IncrementalMapper mapper(&database_cache);
  mapper.BeginReconstruction(&reconstruction);

  //////////////////////////////////////////////////////////////////////////////
  // Triangulation
  //////////////////////////////////////////////////////////////////////////////

  const auto tri_options = mapper_options.Triangulation();

  const auto& reg_image_ids = reconstruction.RegImageIds();

  for (size_t i = 0; i < reg_image_ids.size(); ++i) {
    const image_t image_id = reg_image_ids[i];

    const auto& image = reconstruction.Image(image_id);

    PrintHeading1(StringPrintf("Triangulating image #%d (%d)", image_id, i));

    const size_t num_existing_points3D = image.NumPoints3D();

    std::cout << "  => Image sees " << num_existing_points3D << " / "
              << image.NumObservations() << " points" << std::endl;

    mapper.TriangulateImage(tri_options, image_id);

    std::cout << "  => Triangulated "
              << (image.NumPoints3D() - num_existing_points3D) << " points"
              << std::endl;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Retriangulation
  //////////////////////////////////////////////////////////////////////////////

//   PrintHeading1("Retriangulation");

//   CompleteAndMergeTracks(mapper_options, &mapper);

  //////////////////////////////////////////////////////////////////////////////
  // Bundle adjustment
  //////////////////////////////////////////////////////////////////////////////

  AlignmentBundleAdjuster::CalibOptions calib_options;
  AlignmentBundleAdjuster::TrajOptions traj_options;
  AlignmentBundleAdjuster::AlignmentOptions align_options;

  auto ba_options = mapper_options.GlobalBundleAdjustment();
  ba_options.refine_focal_length = false;
  ba_options.refine_principal_point = false;
  ba_options.refine_extra_params = false;
  ba_options.refine_extrinsics = true;
  mapper_options.ba_global_max_refinements = 6;

  // Configure bundle adjustment.
  BundleAdjustmentConfig ba_config;
  for (const image_t image_id : reconstruction.RegImageIds()) {
    ba_config.AddImage(image_id);
  }

  for (int i = 0; i < mapper_options.ba_global_max_refinements; ++i) {
    // Avoid degeneracies in bundle adjustment.
    reconstruction.FilterObservationsWithNegativeDepth();

    const size_t num_observations = reconstruction.ComputeNumObservations();

    PrintHeading1("Bundle adjustment for " + std::to_string(i+1) + " of " + std::to_string(mapper_options.ba_global_max_refinements));
    AlignmentBundleAdjuster bundle_adjuster(ba_options, calib_options, traj_options, align_options, ba_config);
    CHECK(bundle_adjuster.Solve(&reconstruction, &camera_calib, &traj));

    if (i < 4) {
      mapper_options.mapper.filter_max_reproj_error = 40.0 / std::pow(2.0, i);
      mapper_options.mapper.filter_min_tri_angle = 0.2 * (1 + i);
    }

    size_t num_changed_observations = 0;
    num_changed_observations += CompleteAndMergeTracks(mapper_options, &mapper);
    num_changed_observations += FilterPoints(mapper_options, &mapper);
    const double changed =
        static_cast<double>(num_changed_observations) / num_observations;
    std::cout << StringPrintf("  => Changed observations: %.6f", changed)
              << std::endl;
    camera_calib.PrintInfo();
    if (changed < mapper_options.ba_global_max_refinement_change) {
      break;
    }
  }

  PrintHeading1("Extracting colors");
  reconstruction.ExtractColorsForAllImages(local_path);

  const bool kDiscardReconstruction = false;
  mapper.EndReconstruction(kDiscardReconstruction);

  std::string output_path = local_path + "/" + pt.get<std::string>(cam_name + "_sfm_opti_subdir");
  reconstruction.Write(output_path);
  calib_path = local_path + "/" + pt.get<std::string>(cam_name + "_sfm_opti_calib");
  camera_calib.Write(calib_path);
  traj_path = local_path + "/" + pt.get<std::string>(cam_name + "_sfm_opti_trajectory");
  traj.Write(traj_path);

  WriteAlignment(cam_name, pt, &reconstruction, &camera_calib, &traj);

  return EXIT_SUCCESS;
}

}  // namespace colmap
