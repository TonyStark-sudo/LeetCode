import argparse
import sys
import os
import re
import numpy as np
import cv2
from tqdm import tqdm
from pathlib import Path
from pyquaternion import Quaternion
import multiprocessing
import h5py
sys.path.append(str(Path(__file__).parent.parent))
from utils import utility
sys.path.append(str(Path(__file__).parent))
from hierarchical_localization.hloc import extract_features, pairs_from_poses, match_features, triangulation
import hierarchical_localization.hloc.utils as hutils
import hierarchical_localization.hloc.utils.viz as hviz
import hierarchical_localization.hloc.utils.io as hio
import hierarchical_localization.hloc.utils.read_write_model as read_write_model
import hierarchical_localization.hloc.feature_pairs_filter_by_poses as hlh_feature_pairs_filte_by_poses
import torch
import pycolmap
import matplotlib
torch.multiprocessing.set_sharing_strategy('file_system')

local_feature_conf = {
    # "output": "feats-superpoint-n4096-rmax1600",
    "model": {
        "name": "superpoint",
        "nms_radius": 3,
        "max_keypoints": 4096,
    },
    "preprocessing": {
        "grayscale": True,
        "resize_max": 1600,
        "resize_force": True,
    },
}

local_match_conf = {
    'model': {
        'name': 'superglue',
        'weights': 'outdoor',
        'sinkhorn_iterations': 50,
    },
}

# local_match_conf = {
#     'SG':{
#         'model': {
#             'name': 'superglue',
#             'weights': 'outdoor',
#             'sinkhorn_iterations': 50,
#         },
#     },
#     'LG':{
#         'model': {
#             'name': 'lightglue',
#             'features': 'superpoint',
#         },
#     },
#     'SG-fast': {
#         'model': {
#             'name': 'superglue',
#             'weights': 'outdoor',
#             'sinkhorn_iterations': 20, # original 5
#         },
#     },
# }

def init_camera(param):
    cfg, idx, cam = param
    HEADER = (
        "# Camera list with one line of data per camera:\n"
        + "# CAMERA_ID, MODEL, WIDTH, HEIGHT, PARAMS[]\n"
        + "# Number of cameras: {}\n".format(1)
    )

    str_lines = HEADER
    camera_info = utility.read_json(os.path.join(cfg["local_path"], cfg[f"{cam}_intrinsic_subpath"]))

    K = camera_info["K"]
    D = camera_info["D"]

    K = np.array(K).reshape(3, 3)
    D = np.array(D).reshape(-1, 1)

    cam_model = "PINHOLE"
    cam_width = camera_info["resize"][0]
    cam_height = camera_info["resize"][1]
    params = []
    distortion_model = camera_info["distortion_model"]
    if distortion_model == "pinhole" or camera_info["undistortion"]:
        cam_model = "PINHOLE"
        params.append(K[0, 0])
        params.append(K[1, 1])
        params.append(K[0, 2])
        params.append(K[1, 2])
    elif distortion_model == "fisheye":
        cam_model = "OPENCV_FISHEYE"
        params.append(K[0, 0])
        params.append(K[1, 1])
        params.append(K[0, 2])
        params.append(K[1, 2])
        params.append(D[0, 0])
        params.append(D[1, 0])
        params.append(D[2, 0])
        params.append(D[3, 0])
    else:
        print(f"{cam} model fail !!!")
        return False

    if camera_info["resize"][0] != camera_info["resolution"][0]:
        scalex = float(camera_info["resize"][0]) / float(camera_info["resolution"][0])
        params[0] *= scalex
        params[2] *= scalex
    if camera_info["resize"][1] != camera_info["resolution"][1]:
        scaley = float(camera_info["resize"][1]) / float(camera_info["resolution"][1])
        params[1] *= scaley
        params[3] *= scaley

    to_write = [idx, cam_model, cam_width, cam_height, *params]
    line = " ".join([str(elem) for elem in to_write])
    line += "\n"
    str_lines += line

    path = os.path.join(cfg["local_path"], cfg[f"{cam}_sfm_init_cameras_txt"])
    with open(path, "w") as f:
        f.write(str_lines)

    return True

def init_images(param):
    cfg, idx, cam = param
    HEADER = (
        "# Image list with two lines of data per image: world2cam\n"
        + "# IMAGE_ID, QW, QX, QY, QZ, TX, TY, TZ, CAMERA_ID, NAME\n"
        + "# POINTS2D[] as (X, Y, POINT3D_ID)\n"
    )

    str_lines = HEADER
    path = os.path.join(cfg["local_path"], cfg[f"{cam}_pose_cam_enu_subpath"])
    if not os.path.exists(path):
        print(f'{cfg[f"{cam}_pose_cam_enu_subpath"]} not exist !!!')
        return False
    img_list = utility.get_txt_list(path)
    for imgid, img_line in enumerate(img_list):
        timestamp = img_line[0]
        imgid_offset = cfg[f"{cam}_id_offset"] + imgid
        name = os.path.join(cfg[f"{cam}_data_subdir"], timestamp + ".jpg")
        p = np.array([np.float64(x) for x in img_line[1:4]])
        q = [np.float64(x) for x in img_line[4:8]]
        q = utility.list2quat_xyzw(q)
        p, q = utility.inverse(p, q)

        str_lines += f"{imgid_offset} {q.w} {q.x} {q.y} {q.z} {p[0]} {p[1]} {p[2]} {idx} {name}\n\n"

    path = os.path.join(cfg["local_path"], cfg[f"{cam}_sfm_init_images_txt"])
    with open(path, "w") as f:
        f.write(str_lines)

    return True

def init_points(param):
    cfg, idx, cam = param
    path = os.path.join(cfg["local_path"], cfg[f"{cam}_sfm_init_points3D_txt"])
    with open(path, "w") as f:
        pass
    return True

def extract_local_features(param):
    cfg, idx, cam = param
    image_dir = Path(cfg["local_path"])
    txt_name = os.path.join(cfg["local_path"], cfg[f"{cam}_nori"])
    nori_list = utility.get_txt_list(txt_name)
    image_list = [os.path.join(cfg[f"{cam}_data_subdir"], line[0] + ".jpg") for line in nori_list]
    feature_path = Path(os.path.join(cfg["local_path"], cfg[f"{cam}_local_feature_subpath"]))
    device = cfg[f"{cam}_sfm_device"]

    # extract_features.main(cfg[f"{cam}_local_feature_conf"], image_dir, image_list=image_list, feature_path=feature_path, device=device)
    # return True

    img_seg_infos = {}
    if 'img_seg_subdir' in cfg:
        img_seg_infos['img_seg_scale'] = cfg['img_seg_scale']
        img_seg_infos['img_seg_npy_subdir'] = f"{image_dir}/{cfg[f'{cam}_img_seg_npy_subdir']}"
    else:
        img_seg_infos = None
    extract_features.main(cfg[f"{cam}_local_feature_conf"], image_dir, image_list=image_list, feature_path=feature_path, device=device, img_seg_infos=img_seg_infos)
    return True

def match_local_features(param):
    cfg, idx, cam = param

    sfm_init_dir = Path(os.path.join(cfg["local_path"], cfg[f"{cam}_sfm_init_subdir"]))
    pairs_txt = Path(os.path.join(cfg["local_path"], cfg[f"{cam}_pairs_sfm_txt"]))
    pairs_from_poses.main(sfm_init_dir, pairs_txt, cfg[f"{cam}_num_pairs_sfm"], cfg[f"{cam}_pair_rotation_threshold"], ext=".txt")

    feature_path = Path(os.path.join(cfg["local_path"], cfg[f"{cam}_local_feature_subpath"]))
    match_path = Path(os.path.join(cfg["local_path"], cfg[f"{cam}_local_match_subpath"]))
    device = cfg[f"{cam}_sfm_device"]
    match_features.main(cfg[f"{cam}_local_match_conf"], pairs_txt, feature_path, matches=match_path, device=device)
    return True

def init_reconstruction(param):
    if not init_camera(param):
        return False
    if not init_images(param):
        return False
    if not init_points(param):
        return False
    return True

def init_calib(param):
    cfg, idx, cam = param

    txt_path = os.path.join(cfg["local_path"], cfg[f"{cam}_to_ins_txt"])
    txt_list = utility.get_txt_list(txt_path)
    calib_dict = dict()
    calib_dict["camera_id"] = idx
    calib_dict["rel_tvec"] = [float(x) for x in txt_list[0][0:3]]
    calib_dict["rel_qvec"] = [float(txt_list[0][6])]
    calib_dict["rel_qvec"] += [float(x) for x in txt_list[0][3:6]]
    calib_dict["rel_time"] = 0.0

    calib_json = dict()
    calib_json[cam] = calib_dict

    json_path = os.path.join(cfg["local_path"], cfg[f"{cam}_sfm_init_calib"])
    utility.write_json(calib_json, json_path)
    return True

def init_trajectory(param):
    cfg, idx, cam = param
    txt_list = utility.get_txt_list(os.path.join(cfg["local_path"], cfg["pose_ins_enu_subpath"]))
    time_pose_ins_enu = []
    for i, time_pos in enumerate(txt_list):
        time_pose_ins_enu.append([i, time_pos[7], time_pos[4], time_pos[5], time_pos[6], time_pos[1], time_pos[2], time_pos[3], time_pos[0]])

    path = os.path.join(cfg["local_path"], cfg[f"{cam}_sfm_init_trajectory"])
    utility.write_txt(path, time_pose_ins_enu)
    return True

def structure_from_motion(param):
    cfg, idx, cam = param

    local_cfg = os.path.join(cfg["local_path"], "config.json")
    triangulation.trajectory_alignment(config_path=local_cfg, cfg=cfg, cam=cam, verbose=True, skip_geometric_verification=True)

    pose_path = os.path.join(cfg["local_path"], cfg[f"{cam}_sfm_opti_trajectory"])
    txt_list = open(pose_path,'r').readlines()
    txt_list = [line.strip() for line in txt_list]
    txt_list = [line for line in txt_list if len(line) > 0 and line[0] != "#"]
    txt_list = [line.split(" ") for line in txt_list]
    key_poses = []
    for _, time_pos in enumerate(txt_list):
        key_poses.append([time_pos[8], time_pos[5], time_pos[6], time_pos[7], time_pos[2], time_pos[3], time_pos[4], time_pos[1]])

    pose_path = os.path.join(cfg["local_path"], cfg[f"{cam}_sfm_output_trajectory"])
    utility.write_txt(pose_path, key_poses)

    image_path = os.path.join(cfg["local_path"], cfg[f"{cam}_sfm_opti_image_poses"])
    txt_list = open(image_path, 'r').readlines()
    txt_list = [line.strip() for line in txt_list]
    txt_list = [line for line in txt_list if len(line) > 0 and line[0] != "#"]
    txt_list = [line.split(" ") for line in txt_list]
    img_poses = []
    for _, time_pos in enumerate(txt_list):
        img_poses.append([time_pos[8].strip().split("/")[-1].strip(".jpg"), time_pos[5], time_pos[6], time_pos[7], time_pos[2], time_pos[3], time_pos[4], time_pos[1]])

    image_path = os.path.join(cfg["local_path"], cfg[f"{cam}_sfm_output_image_poses"])
    utility.write_txt(image_path, img_poses)

    for i in range(1,3):
        key_path = os.path.join(cfg["local_path"], cfg[f"{cam}_sfm_opti_key_poses_{i}"])
        txt_list = open(key_path, 'r').readlines()
        txt_list = [line.strip() for line in txt_list]
        txt_list = [line for line in txt_list if len(line) > 0 and line[0] != "#"]
        txt_list = [line.split(" ") for line in txt_list]
        key_poses = []
        for _, time_pos in enumerate(txt_list):
            key_poses.append([time_pos[8].strip().split("/")[-1].strip(".jpg"), time_pos[5], time_pos[6], time_pos[7], time_pos[2], time_pos[3], time_pos[4], time_pos[1]])

        key_path = os.path.join(cfg["local_path"], cfg[f"{cam}_sfm_output_key_poses_{i}"])
        utility.write_txt(key_path, key_poses)

    return True

def plot_local_features(param):
    cfg, idx, cam = param
    feature_path = Path(os.path.join(cfg["local_path"], cfg[f"{cam}_local_feature_subpath"]))
    img_txt = os.path.join(os.path.join(cfg["local_path"], cfg[f"{cam}_sfm_init_subdir"]), "images.txt")
    images_list = hutils.read_write_model.read_images_text(img_txt)

    for _, img_line in tqdm(images_list.items()):
        image_name = img_line.name
        keypoints = hio.get_keypoints(feature_path, image_name)
        # keypoints += 0.5  # COLMAP origin

        hviz.plot_images([hio.read_image(Path(cfg["local_path"]) / image_name)])
        hviz.plot_keypoints([keypoints], ps=4)
        path = os.path.join(cfg["local_path"], cfg[f"{cam}_viz_feature_subdir"], image_name.split("/")[-1])
        hviz.save_plot(path)
        matplotlib.pyplot.close()
        continue
        img = cv2.imread(os.path.join(cfg["local_path"], image_name))
        for kp in keypoints:
            cv2.circle(img, (int(kp[0]), int(kp[1])), 3, (255, 255, 0), 2)
        path = os.path.join(cfg["local_path"], cfg[f"{cam}_viz_feature_subdir"], image_name.split("/")[-1])
        cv2.imwrite(path, img)

    return True

def plot_local_matches(param):
    cfg, idx, cam = param
    feature_path = Path(os.path.join(cfg["local_path"], cfg[f"{cam}_local_feature_subpath"]))
    pairs_txt = Path(os.path.join(cfg["local_path"], cfg[f"{cam}_pairs_sfm_txt"]))
    pairs = utility.get_txt_list(pairs_txt)
    matches_path = Path(os.path.join(cfg["local_path"], cfg[f"{cam}_local_match_subpath"]))

    for pair in tqdm(pairs):
        name0, name1 = pair
        db_image = hio.read_image(Path(cfg["local_path"]) /name0)
        db_kps = hio.get_keypoints(feature_path, name0)
        q_image = hio.read_image(Path(cfg["local_path"]) / name1)
        q_kps = hio.get_keypoints(feature_path, name1)
        matches, scores = hio.get_matches(matches_path, name0, name1)
        # random_select = np.random.choice(np.arange(len(matches)), size=min(len(matches), 100), replace=False)
        # matches = matches[random_select]
        kp_db = db_kps[matches[:, 0]]
        kp_q = q_kps[matches[:, 1]]
        hviz.plot_images([db_image, q_image])
        hviz.plot_keypoints([db_kps, q_kps], ps=4)
        hviz.plot_matches(kp_db, kp_q)
        name = name0.split("/")[-1].strip(".jpg") + "_" + name1.split("/")[-1]
        path = os.path.join(cfg["local_path"], cfg[f"{cam}_viz_match_subdir"], name)
        hviz.save_plot(path)
        matplotlib.pyplot.close()

    return True

def plot_sfm_tracks(param):
    cfg, idx, cam = param
    img_txt = os.path.join(os.path.join(cfg["local_path"], cfg[f"{cam}_sfm_output_subdir"]), "images.txt")
    images_list = read_write_model.read_images_text(img_txt)

    for _, img_line in tqdm(images_list.items()):
        image_name = img_line.name
        img = cv2.imread(os.path.join(cfg["local_path"], image_name))
        mask = img_line.point3D_ids > -1
        keypoints = img_line.xys[mask]
        for kp in keypoints:
            cv2.circle(img, (int(kp[0]), int(kp[1])), 3, (255, 255, 0), 2)
        path = os.path.join(cfg["local_path"], cfg[f"{cam}_viz_sfm_track_subdir"], image_name.split("/")[-1])
        cv2.imwrite(path, img)

    return True

def sfm_per_cam(param):
    if not init_calib(param):
        return False

    if not init_trajectory(param):
        return False

    if not init_reconstruction(param):
        return False

    if not extract_local_features(param):
        return False

    if not match_local_features(param):
        return False

    # if not structure_from_motion(param):
    #     return False

    # if not plot_local_features(param):
    #     return False

    # if not plot_local_matches(param):
    #     return False

    # if not plot_sfm_tracks(param):
    #     return False

    return True

def sfm_all_cam(cfg):
    calib_json = dict()
    for _, cam in enumerate(cfg['sfm_camera_names']):
        json_path = os.path.join(cfg["local_path"], cfg[f"{cam}_sfm_init_calib"])
        calib_dict = utility.read_json(json_path)
        calib_json[cam] = calib_dict[cam]

    json_path = os.path.join(cfg["local_path"], cfg["all_sfm_init_calib"])
    utility.write_json(calib_json, json_path)

    path = os.path.join(cfg["local_path"], cfg[f"{cam}_sfm_init_trajectory"])
    time_pose_ins_enu = utility.get_txt_list(path)
    path = os.path.join(cfg["local_path"], cfg["all_sfm_init_trajectory"])
    utility.write_txt(path, time_pose_ins_enu)

    cam_txt = []
    for idx, cam in enumerate(cfg['sfm_camera_names']):
        path = os.path.join(cfg["local_path"], cfg[f"{cam}_sfm_init_cameras_txt"])
        txt_list = utility.get_txt_list(path)
        txt_list[-1][0] = str(idx)
        cam_txt.append(txt_list[-1])

    path = os.path.join(cfg["local_path"], cfg["all_sfm_init_cameras_txt"])
    utility.write_txt(path, cam_txt)

    str_lines = ""
    for idx, cam in enumerate(cfg['sfm_camera_names']):
        path = os.path.join(cfg["local_path"], cfg[f"{cam}_sfm_init_images_txt"])
        txt_list = open(path,'r').readlines()
        txt_list = [line.strip() for line in txt_list]
        txt_list = [line for line in txt_list if len(line) > 0 and line[0] != "#"]
        txt_list = [line.split(" ") for line in txt_list]
        for line in txt_list:
            str_lines += " ".join(line) + "\n\n"
    path = os.path.join(cfg["local_path"], cfg["all_sfm_init_images_txt"])
    with open(path, "w") as f:
        f.write(str_lines)

    path = os.path.join(cfg["local_path"], cfg["all_sfm_init_points3D_txt"])
    with open(path, "w") as f:
        pass

    pairs = []
    for idx, cam in enumerate(cfg['sfm_camera_names']):
        path = os.path.join(cfg["local_path"], cfg[f"{cam}_pairs_sfm_txt"])
        pair_cam = utility.get_txt_list(path)
        pairs.extend(pair_cam)
    path = os.path.join(cfg["local_path"], cfg["all_pairs_sfm_txt"])
    utility.write_txt(path, pairs)

    files = []
    files2 = []
    for idx, cam in enumerate(cfg['sfm_camera_names']):
        files.append(os.path.join(cfg["local_path"], cfg[f"{cam}_local_feature_subpath"]))
        files2.append(os.path.join(cfg["local_path"], cfg[f"{cam}_local_match_subpath"]))
    utility.merge_h5py(files, os.path.join(cfg["local_path"], cfg["all_local_feature_subpath"]))
    utility.merge_h5py(files2, os.path.join(cfg["local_path"], cfg["all_local_match_subpath"]))
    hlh_feature_pairs_filte_by_poses.main(cfg=cfg, ext='txt')

    local_cfg = os.path.join(cfg["local_path"], "config.json")
    triangulation.trajectory_alignment(config_path=local_cfg, cfg=cfg, cam="all", verbose=True, skip_geometric_verification=True)

    pose_path = os.path.join(cfg["local_path"], cfg["all_sfm_opti_trajectory"])
    txt_list = open(pose_path,'r').readlines()
    txt_list = [line.strip() for line in txt_list]
    txt_list = [line for line in txt_list if len(line) > 0 and line[0] != "#"]
    txt_list = [line.split(" ") for line in txt_list]
    key_poses = []
    for _, time_pos in enumerate(txt_list):
        key_poses.append([time_pos[8], time_pos[5], time_pos[6], time_pos[7], time_pos[2], time_pos[3], time_pos[4], time_pos[1]])

    pose_path = os.path.join(cfg["local_path"], cfg["all_sfm_output_trajectory"])
    utility.write_txt(pose_path, key_poses)

    image_path = os.path.join(cfg["local_path"], cfg["all_sfm_opti_image_poses"])
    txt_list = open(image_path, 'r').readlines()
    txt_list = [line.strip() for line in txt_list]
    txt_list = [line for line in txt_list if len(line) > 0 and line[0] != "#"]
    txt_list = [line.split(" ") for line in txt_list]

    img_poses = dict()
    for idx, cam in enumerate(cfg['sfm_camera_names']):
        img_poses[cam] = []
    for _, time_pos in enumerate(txt_list):
        for idx, cam in enumerate(cfg['sfm_camera_names']):
            if re.search(cam, time_pos[8]):
                img_poses[cam].append([time_pos[8].strip().split("/")[-1].strip(".jpg"), time_pos[5], time_pos[6], time_pos[7], time_pos[2], time_pos[3], time_pos[4], time_pos[1]])
                break

    for idx, cam in enumerate(cfg['sfm_camera_names']):
        pose_path = os.path.join(cfg["local_path"], cfg[f"all_sfm_output_image_poses_{cam}"])
        utility.write_txt(pose_path, img_poses[cam])

    for i in range(1,3):
        key_path = os.path.join(cfg["local_path"], cfg[f"all_sfm_opti_key_poses_{i}"])
        txt_list = open(key_path, 'r').readlines()
        txt_list = [line.strip() for line in txt_list]
        txt_list = [line for line in txt_list if len(line) > 0 and line[0] != "#"]
        txt_list = [line.split(" ") for line in txt_list]

        key_poses = dict()
        for idx, cam in enumerate(cfg['sfm_camera_names']):
            key_poses[cam] = []
        for _, time_pos in enumerate(txt_list):
            for idx, cam in enumerate(cfg['sfm_camera_names']):
                if re.search(cam, time_pos[8]):
                    key_poses[cam].append([time_pos[8].strip().split("/")[-1].strip(".jpg"), time_pos[5], time_pos[6], time_pos[7], time_pos[2], time_pos[3], time_pos[4], time_pos[1]])
                    break

        for idx, cam in enumerate(cfg['sfm_camera_names']):
            pose_path = os.path.join(cfg["local_path"], cfg[f"all_sfm_output_key_poses_{i}_{cam}"])
            utility.write_txt(pose_path, key_poses[cam])

    img_txt = os.path.join(os.path.join(cfg["local_path"], cfg[f"all_sfm_output_subdir"]), "images.txt")
    images_list = read_write_model.read_images_text(img_txt)
    for _, img_line in tqdm(images_list.items()):
        image_name = img_line.name
        img = cv2.imread(os.path.join(cfg["local_path"], image_name))
        mask = img_line.point3D_ids > -1
        keypoints = img_line.xys[mask]
        for kp in keypoints:
            cv2.circle(img, (int(kp[0]), int(kp[1])), 3, (255, 255, 0), 2)
        for idx, cam in enumerate(cfg['sfm_camera_names']):
            if re.search(cam, image_name):
                path = os.path.join(cfg["local_path"], cfg[f"all_viz_sfm_track_subdir_{cam}"], image_name.split("/")[-1])
                cv2.imwrite(path, img)
                break

    return True

def main(cfg, device_list):
    cfg["sfm_subdir"] = "sfm"
    cfg["colmap_exe"] = str(Path(__file__).parent / "colmap/build/src/exe/colmap")
    sfm_dir = os.path.join(cfg["local_path"], cfg["sfm_subdir"])
    os.makedirs(sfm_dir, exist_ok=True)

    cfg["all_sfm_subdir"] = os.path.join(cfg["sfm_subdir"], "all")
    cam_sfm_dir = os.path.join(cfg["local_path"], cfg[f"all_sfm_subdir"])
    os.makedirs(cam_sfm_dir, exist_ok=True)
    cfg["all_sfm_init_subdir"] = os.path.join(cfg["all_sfm_subdir"], "init")
    os.makedirs(os.path.join(cfg["local_path"], cfg["all_sfm_init_subdir"]), exist_ok=True)

    cfg["all_sfm_init_calib"] = os.path.join(cfg["all_sfm_init_subdir"], "calib.json")
    cfg["all_sfm_init_trajectory"] = os.path.join(cfg["all_sfm_init_subdir"], "trajectory.txt")

    cfg["all_sfm_init_cameras_txt"] = os.path.join(cfg["all_sfm_init_subdir"], "cameras.txt")
    cfg["all_sfm_init_images_txt"] = os.path.join(cfg["all_sfm_init_subdir"], "images.txt")
    cfg["all_sfm_init_points3D_txt"] = os.path.join(cfg["all_sfm_init_subdir"], "points3D.txt")

    cfg["all_local_feature_subpath"] = os.path.join(cfg["all_sfm_subdir"], "local_features.h5")
    cfg["all_local_match_subpath"] = os.path.join(cfg["all_sfm_subdir"], "local_matches.h5")
    cfg["all_pairs_sfm_txt"] = os.path.join(cfg["all_sfm_subdir"], "pairs_sfm.txt")

    cfg["all_sfm_opti_subdir"] = os.path.join(cfg["all_sfm_subdir"], "opti")
    os.makedirs(os.path.join(cfg["local_path"], cfg["all_sfm_opti_subdir"]), exist_ok=True)
    cfg["all_sfm_opti_calib"] = os.path.join(cfg["all_sfm_opti_subdir"], "calib.json")
    cfg["all_sfm_opti_trajectory"] = os.path.join(cfg["all_sfm_opti_subdir"], "trajectory.txt")
    cfg["all_sfm_opti_image_poses"] = os.path.join(cfg["all_sfm_opti_subdir"], "image_poses.txt")
    cfg["all_sfm_opti_key_poses_1"] = os.path.join(cfg["all_sfm_opti_subdir"], "key_poses_1.txt")
    cfg["all_sfm_opti_key_poses_2"] = os.path.join(cfg["all_sfm_opti_subdir"], "key_poses_2.txt")
    cfg["all_sfm_output_subdir"] = os.path.join(cfg["all_sfm_subdir"], "output")
    os.makedirs(os.path.join(cfg["local_path"], cfg["all_sfm_output_subdir"]), exist_ok=True)
    for idx, cam in enumerate(cfg['sfm_camera_names']):
        cfg[f"all_sfm_output_image_poses_{cam}"] = os.path.join(cfg["all_sfm_output_subdir"], f"image_poses_{cam}.txt")
        cfg[f"all_sfm_output_key_poses_1_{cam}"] = os.path.join(cfg["all_sfm_output_subdir"], f"key_poses_1_{cam}.txt")
        cfg[f"all_sfm_output_key_poses_2_{cam}"] = os.path.join(cfg["all_sfm_output_subdir"], f"key_poses_2_{cam}.txt")
    cfg["all_sfm_output_trajectory"] = os.path.join(cfg["all_sfm_output_subdir"], "trajectory.txt")

    cfg["all_viz_sfm_track_subdir"] = os.path.join(cfg["all_sfm_subdir"], "viz_sfm_track")
    os.makedirs(os.path.join(cfg["local_path"], cfg["all_viz_sfm_track_subdir"]), exist_ok=True)
    for idx, cam in enumerate(cfg['sfm_camera_names']):
        cfg[f"all_viz_sfm_track_subdir_{cam}"] = os.path.join(cfg["all_viz_sfm_track_subdir"], cam)
        os.makedirs(os.path.join(cfg["local_path"], cfg[f"all_viz_sfm_track_subdir_{cam}"]), exist_ok=True)


    params = []
    for idx, cam in enumerate(cfg['sfm_camera_names']):
        cfg[f"{cam}_sfm_subdir"] = os.path.join(cfg["sfm_subdir"], cam)
        cam_sfm_dir = os.path.join(cfg["local_path"], cfg[f"{cam}_sfm_subdir"])
        os.makedirs(cam_sfm_dir, exist_ok=True)
        cfg[f"{cam}_sfm_init_subdir"] = os.path.join(cfg[f"{cam}_sfm_subdir"], "init")
        os.makedirs(os.path.join(cfg["local_path"], cfg[f"{cam}_sfm_init_subdir"]), exist_ok=True)

        cfg[f"{cam}_sfm_init_calib"] = os.path.join(cfg[f"{cam}_sfm_init_subdir"], "calib.json")
        cfg[f"{cam}_sfm_init_trajectory"] = os.path.join(cfg[f"{cam}_sfm_init_subdir"], "trajectory.txt")

        cfg[f"{cam}_sfm_init_cameras_txt"] = os.path.join(cfg[f"{cam}_sfm_init_subdir"], "cameras.txt")
        cfg[f"{cam}_sfm_init_images_txt"] = os.path.join(cfg[f"{cam}_sfm_init_subdir"], "images.txt")
        cfg[f"{cam}_sfm_init_points3D_txt"] = os.path.join(cfg[f"{cam}_sfm_init_subdir"], "points3D.txt")

        device = device_list[idx % len(device_list)]
        cfg[f"{cam}_sfm_device"] = device
        cfg[f"{cam}_local_feature_conf"] = local_feature_conf
        cfg[f"{cam}_local_match_conf"] = local_match_conf
        cfg[f"{cam}_local_feature_subpath"] = os.path.join(cfg[f"{cam}_sfm_subdir"], "local_features.h5")

        cfg[f"{cam}_sfm_opti_subdir"] = os.path.join(cfg[f"{cam}_sfm_subdir"], "opti")
        os.makedirs(os.path.join(cfg["local_path"], cfg[f"{cam}_sfm_opti_subdir"]), exist_ok=True)
        cfg[f"{cam}_sfm_opti_calib"] = os.path.join(cfg[f"{cam}_sfm_opti_subdir"], "calib.json")
        cfg[f"{cam}_sfm_opti_trajectory"] = os.path.join(cfg[f"{cam}_sfm_opti_subdir"], "trajectory.txt")
        cfg[f"{cam}_sfm_opti_image_poses"] = os.path.join(cfg[f"{cam}_sfm_opti_subdir"], "image_poses.txt")
        cfg[f"{cam}_sfm_opti_key_poses_1"] = os.path.join(cfg[f"{cam}_sfm_opti_subdir"], "key_poses_1.txt")
        cfg[f"{cam}_sfm_opti_key_poses_2"] = os.path.join(cfg[f"{cam}_sfm_opti_subdir"], "key_poses_2.txt")
        cfg[f"{cam}_sfm_output_subdir"] = os.path.join(cfg[f"{cam}_sfm_subdir"], "output")
        os.makedirs(os.path.join(cfg["local_path"], cfg[f"{cam}_sfm_output_subdir"]), exist_ok=True)
        cfg[f"{cam}_num_pairs_sfm"] = 3
        cfg[f"{cam}_pair_rotation_threshold"] = 50
        cfg[f"{cam}_pairs_sfm_txt"] = os.path.join(cfg[f"{cam}_sfm_subdir"], "pairs_sfm.txt")
        cfg[f"{cam}_local_match_subpath"] = os.path.join(cfg[f"{cam}_sfm_subdir"], "local_matches.h5")
        cfg[f"{cam}_sfm_output_image_poses"] = os.path.join(cfg[f"{cam}_sfm_output_subdir"], "image_poses.txt")
        cfg[f"{cam}_sfm_output_key_poses_1"] = os.path.join(cfg[f"{cam}_sfm_output_subdir"], "key_poses_1.txt")
        cfg[f"{cam}_sfm_output_key_poses_2"] = os.path.join(cfg[f"{cam}_sfm_output_subdir"], "key_poses_2.txt")
        cfg[f"{cam}_sfm_output_trajectory"] = os.path.join(cfg[f"{cam}_sfm_output_subdir"], "trajectory.txt")

        cfg[f"{cam}_viz_feature_subdir"] = os.path.join(cfg[f"{cam}_sfm_subdir"], "viz_feature")
        os.makedirs(os.path.join(cfg["local_path"], cfg[f"{cam}_viz_feature_subdir"]), exist_ok=True)

        cfg[f"{cam}_viz_match_subdir"] = os.path.join(cfg[f"{cam}_sfm_subdir"], "viz_match")
        os.makedirs(os.path.join(cfg["local_path"], cfg[f"{cam}_viz_match_subdir"]), exist_ok=True)

        cfg[f"{cam}_viz_sfm_track_subdir"] = os.path.join(cfg[f"{cam}_sfm_subdir"], "viz_sfm_track")
        os.makedirs(os.path.join(cfg["local_path"], cfg[f"{cam}_viz_sfm_track_subdir"]), exist_ok=True)

        params.append([cfg, idx, cam])

    print(f"sfm main cfg: {cfg} !!!")
    local_cfg = os.path.join(cfg["local_path"], "config.json")
    utility.write_config(cfg, local_cfg)
    print(f"sfm main local_cfg: {local_cfg} !!!")

    # sfm_per_cam(params[0])
    # return
    processes = {}
    for idx, param in enumerate(params):
        processes[idx] = multiprocessing.Process(target=sfm_per_cam, args=[param])

    for _, proc in processes.items():
        proc.start()

    for _, proc in processes.items():
        proc.join()

    sfm_all_cam(cfg)

    print(f"sfm main cfg: {cfg} !!!")
    local_cfg = os.path.join(cfg["local_path"], "config.json")
    utility.write_config(cfg, local_cfg)
    print(f"sfm main local_cfg: {local_cfg} !!!")

    return True

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='sfm')

    parser.add_argument('--config', type=str, default='', help='config name')
    args = parser.parse_args()
    cfg = utility.read_json(args.config)
    print(cfg)
    device_list = []
    for i in range(torch.cuda.device_count()):
        device_list.append(f'cuda:{i}')
    torch.multiprocessing.set_start_method('spawn')
    res = main(cfg=cfg, device_list=device_list)
    print("res ", res)

