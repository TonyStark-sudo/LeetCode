import os
import cv2
import h5py
import json
import shutil
import argparse
from tqdm import tqdm
from pathlib import Path

import numpy as np

from . import logger
from pyquaternion import Quaternion
from .utils.io import get_keypoints, get_matches
from .utils.parsers import names_to_pair
from .filter_key_points import get_candidate_ground_points
from typing import Dict, Optional


def generate_superglue_ret(matches, scores, valid_flags, pts_num):
    matches = matches[valid_flags]
    scores = scores[valid_flags]
    sg_matches = np.full((pts_num, 2), -1)
    sg_matches[:, 0] = np.arange(pts_num)
    sg_scores = np.full(pts_num, -1.0)
    
    sg_matches[matches[:, 0].astype(int), 1] = matches[:, 1]
    sg_scores[matches[:, 0].astype(int)] = scores
    
    return sg_matches[:,1], sg_scores

def check_match(image_width, image_height, pt0, pt1, check_pos):
    main_pt = np.array([0.5*image_width, 0.5*image_height, 1.0])
    ref_pt0 = pt0 - main_pt
    if ref_pt0[1] < 25:
        return True
    ref_pt1 = pt1 - main_pt
    delta_p = np.linalg.norm(ref_pt0) - np.linalg.norm(ref_pt1)
    if abs(ref_pt0[1]-ref_pt1[1])<1:
        return False
    return True

def compute_endpoints(image_width, image_height, line_param):
    end_pts = []
    if abs(line_param[1])>0.0001:
        y = -(line_param[0]+line_param[2])/line_param[1]
        if 1<y and y<image_height-1:
            end_pts.append([1.0, y])

        y = -(line_param[0]*(image_width-2.0)+line_param[2])/line_param[1]
        if 1<y and y<image_height-1:
            end_pts.append([image_width-2.0, y])
    
    if abs(line_param[0])>0.0001:
        x = -(line_param[1]+line_param[2])/line_param[0]
        if 1<x and x<image_width-1:
            end_pts.append([x, 1.0])

        x = -(line_param[1]*(image_height-2.0)+line_param[2])/line_param[1]
        if 1<x and x<image_width-1:
            end_pts.append([x, image_height-2.0])
    if len(end_pts)<2:
        return None
    return end_pts

def draw_image_pt(image_file, pt, resize_ratio=0.5):
    if not image_file.exists():
        logger.error(f"{str(image_file)} not exist!")
        return None
    image = cv2.imread(str(image_file), -1)
    if image is None:
        logger.error(f"bad image {str(image_file)}!")
        return None
    cv2.circle(image, (int(pt[0]+0.5), int(pt[1]+0.5)), radius=2, color=(255, 255, 0), thickness=-1)
    cv2.circle(image, (int(pt[0]+0.5), int(pt[1]+0.5)), radius=10, color=(255, 255, 0), thickness=1)
    height, width = image.shape[:2]
    new_size = (int(resize_ratio*width), int(resize_ratio*height))
    image = cv2.resize(image, new_size, interpolation=cv2.INTER_LINEAR)
    return image

def draw_match_pts(image_root, cameras, image0, pt0, image1, pt1):
    camera0 = cameras[image0.camera_id]
    image_name0 = image0.name
    camera1 = cameras[image1.camera_id]
    image_name1 = image1.name
    image_with_pt0 = draw_image_pt(image_root / image_name0, pt0)
    if image_with_pt0 is None:
        return None
    image_with_pt1 = draw_image_pt(image_root / image_name1, pt1)
    if image_with_pt1 is None:
        return None
    combined_image = np.hstack((image_with_pt0, image_with_pt1))
    cv2.imshow('combined_image', combined_image)
    cv2.waitKey()
    

def draw_epipolar(image_file, camera, pt, epipolar, err, resize_ratio=0.5):
    if not image_file.exists():
        logger.error(f"{str(image_file)} not exist!")
        return None
    image = cv2.imread(str(image_file), -1)
    if image is None:
        logger.error(f"bad image {str(image_file)}!")
        return None
    end_points = compute_endpoints(camera.width, camera.height, epipolar)
    if end_points is None:
        return None
    cv2.line(image, (int(end_points[0][0]+0.5), int(end_points[0][1]+0.5)), (int(end_points[1][0]+0.5), int(end_points[1][1]+0.5)), color=(0, 255, 0), thickness=1)
    cv2.circle(image, (int(pt[0]+0.5), int(pt[1]+0.5)), radius=2, color=(255, 255, 0), thickness=-1)
    cv2.circle(image, (int(pt[0]+0.5), int(pt[1]+0.5)), radius=10, color=(255, 255, 0), thickness=1)
    cv2.putText(image, f'{err}', (20, 40), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)
    height, width = image.shape[:2]
    new_size = (int(resize_ratio*width), int(resize_ratio*height))
    image = cv2.resize(image, new_size, interpolation=cv2.INTER_LINEAR)
    return image

def draw_epipolars(image_root, cameras, image0, pt0, epipolar_0, err_0, image1, pt1, epipolar_1, err_1):
    camera0 = cameras[image0.camera_id]
    image_name0 = image0.name
    camera1 = cameras[image1.camera_id]
    image_name1 = image1.name
    image_epipolar0 = draw_epipolar(image_root / image_name0, camera0, pt0, epipolar_0, err_0)
    if image_epipolar0 is None:
        return None
    image_epipolar1 = draw_epipolar(image_root / image_name1, camera1, pt1, epipolar_1, err_1)
    if image_epipolar1 is None:
        return None
    combined_image = np.hstack((image_epipolar0, image_epipolar1))
    cv2.imshow('combined_image', combined_image)
    cv2.waitKey()

def load_camera_extrinsic(cfg, cameras_extrinsic, camera_name):
    json_data = json.load(open(os.path.join(cfg["local_path"], cfg["calib_subdir"], f"camera_params/{camera_name}_extrinsic.json"), "r"))
    p_c_r = np.array([0.0] * 3)
    p_c_r[0] = json_data["transform"]["translation"]["x"]
    p_c_r[1] = json_data["transform"]["translation"]["y"]
    p_c_r[2] = json_data["transform"]["translation"]["z"]
    q_c_r = Quaternion(json_data["transform"]["rotation"]["w"], json_data["transform"]["rotation"]["x"], json_data["transform"]["rotation"]["y"], json_data["transform"]["rotation"]["z"], dtype=np.float64)
    q_c_r = q_c_r.normalised
    cameras_extrinsic[camera_name] = {
        'q_c_r': q_c_r,
        'p_c_r': p_c_r,
    }

def get_camera_extrinsic(cfg, cameras_extrinsic, camera_name):
    if camera_name not in cameras_extrinsic:
        load_camera_extrinsic(cfg, cameras_extrinsic, camera_name)
    if camera_name not in cameras_extrinsic:
        return None
    return cameras_extrinsic[camera_name]

def get_camera_intrinsic(camera):
    if camera.model == "PINHOLE":
        return np.array([[camera.params[0], 0, camera.params[1]],  # fx, 0, cx
                         [0, camera.params[2], camera.params[3]],  # 0, fy, cy
                         [0, 0, 1]])  # 0, 0, 1
    elif camera.model == "SIMPLE_PINHOLE":
        return np.array([[camera.params[0], 0, 0],
                         [0, camera.params[0], 0],
                         [0, 0, 1]])
    else:
        raise ValueError(f"Unsupported camera model: {camera.model}")

def parse_features(image_ids, features_path):
    image_keypoints = {}
    for image_name, image_id in tqdm(image_ids.items()):
        keypoints = get_keypoints(features_path, image_name)
        keypoints += 0.5  # COLMAP origin
        image_keypoints[image_name] = keypoints
    return image_keypoints

def parse_matches(image_ids, pairs_txt, matches_path, min_match_score: Optional[float] = None,):

    with open(pairs_txt, "r") as f:
        pairs = [p.split() for p in f.readlines()]

    matched_ids = set()
    matches = {}
    scores = {}
    for name0, name1 in tqdm(pairs):
        id0, id1 = image_ids[name0], image_ids[name1]
        if len({(id0, id1), (id1, id0)} & matched_ids) > 0:
            continue
        cur_matches, cur_scores = get_matches(matches_path, name0, name1)
        if min_match_score:
            cur_matches = cur_matches[cur_scores > min_match_score]
        matched_ids |= {(id0, id1), (id1, id0)}
        matches[f'{name0} {name1}'] = cur_matches
        scores[f'{name0} {name1}'] = cur_scores

    return matches, scores

def write_match(matches, scores, save_file, pairs_txt):
    if not save_file.parent.exists():
        save_file.parent.mkdir(parents=True, exist_ok=True)
    if not pairs_txt.parent.exists():
        pairs_txt.parent.mkdir(parents=True, exist_ok=True)
    with open(str(pairs_txt), 'w') as file:
        file.write('\n'.join(matches.keys()))
    
    with h5py.File(str(save_file), "a", libver="latest") as fd:
        for pair_name, pairs in matches.items():
            name0, name1 = pair_name.split()
            name_pair = names_to_pair(name0, name1)
            if name_pair in fd:
                del fd[name_pair]
            grp = fd.create_group(name_pair)
            grp.create_dataset("matches0", data=pairs)
            grp.create_dataset("matching_scores0", data=scores[pair_name])

def skew_symmetric(t):
    return np.array([[0, -t[2], t[1]],
                     [t[2], 0, -t[0]],
                     [-t[1], t[0], 0]])

def get_rel_pose(R0, t0, R1, t1):
    R_01 = R0 @ R1.T
    t_01 = t0 - R_01 @ t1
    return R_01, t_01

def compute_E(cameras, image0, image1):
    R_cam0_enu = image0.qvec2rotmat()
    t_cam0_enu = image0.tvec
    R_cam1_enu = image1.qvec2rotmat()
    t_cam1_enu = image1.tvec
    R_cam0_cam1, t_cam0_cam1 = get_rel_pose(R_cam0_enu, t_cam0_enu, R_cam1_enu, t_cam1_enu)
    
    K0 = get_camera_intrinsic(cameras[image0.camera_id])
    K1 = get_camera_intrinsic(cameras[image1.camera_id])
    K0_inv = np.linalg.inv(K0)
    K1_inv = np.linalg.inv(K1)
    
    return K1_inv.T @ R_cam0_cam1.T @ skew_symmetric(t_cam0_cam1) @ K0_inv

def ipm_and_reproject(cfg, cameras, src_cam_extrinsic, src_image, src_pt, dist_image):
    K_src = get_camera_intrinsic(cameras[src_image.camera_id])
    K_dist = get_camera_intrinsic(cameras[dist_image.camera_id])
    src_pt = np.append(src_pt, [1.0])
    cam_src_pt = np.linalg.inv(K_src) @ src_pt
    q_cam_rfu = src_cam_extrinsic['q_c_r']
    t_cam_rfu = src_cam_extrinsic['p_c_r']
    q_rfu_cam = q_cam_rfu.inverse
    t_rfu_cam = -1.0 * q_rfu_cam.rotate(t_cam_rfu)
    rfu_src_pt = q_rfu_cam.rotate(cam_src_pt)
    alpha = -t_rfu_cam[2]/rfu_src_pt[2]
    rfu_src_pt_3d = alpha * rfu_src_pt
    cam_src_pt_3d = q_cam_rfu.rotate(rfu_src_pt_3d) + t_cam_rfu
    
    R_cam0_enu = src_image.qvec2rotmat()
    t_cam0_enu = src_image.tvec
    R_cam1_enu = dist_image.qvec2rotmat()
    t_cam1_enu = dist_image.tvec
    R_cam1_cam0, t_cam1_cam0 = get_rel_pose(R_cam1_enu, t_cam1_enu, R_cam0_enu, t_cam0_enu)
    cam_dist_pt_3d = R_cam1_cam0 @ cam_src_pt_3d + t_cam1_cam0
    
    cam_dist_pt_3d = cam_dist_pt_3d / cam_dist_pt_3d[2]
    dist_pt = K_dist @ cam_dist_pt_3d
    return dist_pt
    pass

def reject_with_ipm(cfg, cameras, image_ids, images, image_keypoints, matches, scores):
    cams_extrinsic = {}
    for pair_name, pairs in tqdm(matches.items()):
        name0, name1 = pair_name.split()
        cam0 = name0.split("/")[1].split("_data")[0]
        cam1 = name1.split("/")[1].split("_data")[0]
        id0 = image_ids[name0]
        id1 = image_ids[name1]
        image_size_0 = np.array([cameras[images[id0].camera_id].width, cameras[images[id0].camera_id].height])
        pair_pts0 = image_keypoints[name0][pairs[:, 0]]
        pair_pts1 = image_keypoints[name1][pairs[:, 1]]
        
        img_seg_infos = {}
        if 'img_seg_subdir' in cfg:
            img_seg_infos['img_seg_scale'] = cfg['img_seg_scale']
            img_seg_infos['img_seg_npy_subdir'] = f"{cfg['local_path']}/{cfg[f'{cam0}_img_seg_npy_subdir']}"
        else:
            img_seg_infos = None
        
        candidate_ground_pts0 = get_candidate_ground_points(img_seg_infos, name0.split('/')[-1].replace('.jpg', '.npy'), pair_pts0, image_size_0)
        if candidate_ground_pts0 is None or np.count_nonzero(candidate_ground_pts0) == 0:
            continue
        
        indices = np.where(candidate_ground_pts0)[0]
        for ground_pt_id in indices:
            ground_pt = pair_pts0[ground_pt_id]
            cam0_extrinsic = get_camera_extrinsic(cfg, cams_extrinsic, cam0)
            if cam0_extrinsic is None:
                continue
            com_pt1 = ipm_and_reproject(cfg, cameras, cam0_extrinsic, images[id0], ground_pt, images[id1])
            pt1 = pair_pts1[ground_pt_id]
            

def reject_with_tri(cameras, image_ids, images, image_keypoints, matches, scores, image_root=None):
    to_deleted = []
    for pair_name, pairs in tqdm(matches.items(), desc="reject_with_epipolar"):
        name0, name1 = pair_name.split()
        id0 = image_ids[name0]
        id1 = image_ids[name1]
        image0 = images[id0]
        image1 = images[id1]
        pair_pts0 = image_keypoints[name0][pairs[:, 0]]
        pair_pts1 = image_keypoints[name1][pairs[:, 1]]
        ones = np.ones((pair_pts0.shape[0], 1))
        pair_pts0 = np.concatenate((pair_pts0, ones), axis=1)
        pair_pts1 = np.concatenate((pair_pts1, ones), axis=1)
        K0 = get_camera_intrinsic(cameras[image0.camera_id])
        K1 = get_camera_intrinsic(cameras[image1.camera_id])
        K0_inv = np.linalg.inv(K0)
        K1_inv = np.linalg.inv(K1)
        cam_pair_pts0 = pair_pts0 @ K0_inv.T
        cam_pair_pts1 = pair_pts1 @ K1_inv.T
        
        R0 = image0.qvec2rotmat()
        t0 = image0.tvec
        R1 = image1.qvec2rotmat()
        t1 = image1.tvec
        R_01, t_01 = get_rel_pose(R0, t0, R1, t1)
        
        delta_cam_p01 = cam_pair_pts0 - cam_pair_pts1
        part1 = t_01[0] - cam_pair_pts1[:, 0] * t_01[0]
        z = part1/delta_cam_p01[:,0]
        ess_than_zero = z > 1.0
        is_inf = ~np.isinf(z)
        valid_flag = ess_than_zero & is_inf
        if np.sum(valid_flag) == 0:
            to_deleted.append(pair_name)
        # for i in np.where(valid_flag)[0]:
        #     pt0 = np.append(pair_pts0[i], 1.0)
        #     pt1 = np.append(pair_pts1[i], 1.0)
        #     draw_match_pts(image_root, cameras, image0, pt0, image1, pt1)
        #     print('a')
        # print('a')
        # if valid_flag.count(False) == 0:
        #     continue
        # valid_flag = np.array(valid_flag)
        matches[pair_name], scores[pair_name] = generate_superglue_ret(matches[pair_name], scores[pair_name], np.array(valid_flag), image_keypoints[name0].shape[0])
    
    if len(to_deleted) > 0:
        matches = {key: value for key, value in matches.items() if key not in to_deleted}
        scores = {key: value for key, value in scores.items() if key not in to_deleted}
    

def reject_with_epipolar(cameras, image_ids, images, image_keypoints, matches, scores, image_root=None):
    to_deleted = []
    id_10 = int(0.1*len(image_ids)+0.5)
    id_80 = int(0.8*len(image_ids)+0.5)
    for pair_name, pairs in tqdm(matches.items(), desc="reject_with_epipolar"):
        name0, name1 = pair_name.split()
        id0 = image_ids[name0]
        id1 = image_ids[name1]
        keypoints0 = image_keypoints[name0]
        keypoints1 = image_keypoints[name1]
        if pairs.shape[0] == 0:
            to_deleted.append(pair_name)
            continue
        if (id0<id_10 and id1<id_10) or \
            (id0>id_80 or id1>id_80):
            valid_flag = [True]*len(pairs)
            matches[pair_name], scores[pair_name] = generate_superglue_ret(matches[pair_name], scores[pair_name], np.array(valid_flag), keypoints0.shape[0])
            continue
        E = compute_E(cameras, images[id0], images[id1])
        valid_flag = []
        for pair in pairs:
            pt0 = np.append(keypoints0[pair[0]], 1.0)
            pt1 = np.append(keypoints1[pair[1]], 1.0)
            epipolar_0 = pt1.T @ E
            magnitude = np.linalg.norm(epipolar_0[:2])
            epipolar_0 = epipolar_0/magnitude
            err_0 = abs(epipolar_0.T @ pt0)
            epipolar_1 = E @ pt0
            magnitude = np.linalg.norm(epipolar_1[:2])
            epipolar_1 = epipolar_1/magnitude
            err_1 = abs(epipolar_1.T @ pt1)
            if err_0<20 or err_1<20:
                valid_flag.append(True)
            else:
                valid_flag.append(False)
                # draw_epipolars(image_root, cameras, images[id0], pt0, epipolar_0, f'{id0}_{err_0}', images[id1], pt1, epipolar_1, f'{id1}_{err_1}')
        if valid_flag.count(True) < 5:
            to_deleted.append(pair_name)
            continue
        matches[pair_name], scores[pair_name] = generate_superglue_ret(matches[pair_name], scores[pair_name], np.array(valid_flag), keypoints0.shape[0])
    if len(to_deleted) > 0:
        matches = {key: value for key, value in matches.items() if key not in to_deleted}
        scores = {key: value for key, value in scores.items() if key not in to_deleted}
    return matches, scores


def reject_with_optical_flow(cameras, image_ids, images, image_keypoints, matches, scores, image_root):
    to_deleted = []
    for pair_name, pairs in tqdm(matches.items(), desc="start reject_with_optical_flow"):
        name0, name1 = pair_name.split()
        pair_pts0 = image_keypoints[name0][pairs[:, 0]]
        pair_pts1 = image_keypoints[name1][pairs[:, 1]]
        image0 = cv2.imread(str(image_root / name0), 0)
        image1 = cv2.imread(str(image_root / name1), 0)
        
        p1, st1, err1 = cv2.calcOpticalFlowPyrLK(image0, image1, pair_pts0.astype(np.float32), None)
        if p1 is None:
            to_deleted.append(pair_name)
            continue
        p1 = p1.astype(np.float16)
        for i in range(st1.shape[0]):
            pt_error = np.linalg.norm(pair_pts1[i] - p1[i])
            if pt_error > 3:
                st1[i] = 0
        matches[pair_name] = matches[pair_name][np.where(st1.flatten() == 1)[0]][:,1]
        scores[pair_name] = scores[pair_name][np.where(st1.flatten() == 1)[0]]

    if len(to_deleted) > 0:
        matches = {key: value for key, value in matches.items() if key not in to_deleted}
        scores = {key: value for key, value in scores.items() if key not in to_deleted}


def main(cfg, ext=".bin"):
    local_path = Path(cfg['local_path'])
    model = local_path / cfg['all_sfm_init_subdir']
    feature_path = local_path / cfg['all_local_feature_subpath']
    pairs_txt = local_path / cfg['all_pairs_sfm_txt']
    matches_path = local_path / cfg['all_local_match_subpath']

    if ext == ".bin":
        from .utils.read_write_model import read_images_binary, read_cameras_binary
        images = read_images_binary(model / "images.bin")
        cameras = read_cameras_binary(model / "cameras.bin")
    else:
        from .utils.read_write_model import read_images_text, read_cameras_text
        images = read_images_text(model / "images.txt")
        cameras = read_cameras_text(model / "cameras.txt")
    image_ids = {image.name: i for i, image in images.items()}
    image_keypoints = parse_features(image_ids, feature_path)
    matches, scores = parse_matches(image_ids, pairs_txt, matches_path)
    
    matches, scores = reject_with_epipolar(cameras, image_ids, images, image_keypoints, matches, scores, local_path)
    output_path = Path(str(matches_path).replace('.h5', '_ori.h5'))
    shutil.move(str(matches_path), str(output_path))
    write_match(matches, scores, matches_path, pairs_txt)
    


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    # parser.add_argument("--image_root", required=True, type=Path)
    # parser.add_argument("--model", required=True, type=Path)
    # parser.add_argument("--pairs_txt", required=True, type=Path)
    # parser.add_argument("--feature_path", required=True, type=Path)
    # parser.add_argument("--match_path", required=True, type=Path)
    # parser.add_argument("--output", required=True, type=Path)
    # args = parser.parse_args()
    # main(**args.__dict__)
    
    # local_path = Path('/home/weichuan/data/auto_labeling_datas/rogs_batch_run/ppl_bag_20231129_150824_det_25450_25640')
    # img_seg_infos = {}
    # img_seg_infos['img_seg_scale'] = 0.5
    # img_seg_infos['img_seg_npy_subdir'] = local_path / 'img_seg/cam_front_120/npy'
    # main(local_path, img_seg_infos, '.txt')
    
    model = Path('/home/weichuan/data/auto_labeling_datas/rogs_batch_run/ppl_bag_20231129_150824_det_25450_25640/sfm/all/opti')
    from utils.read_write_model import read_points3D_binary, read_images_binary
    images = read_images_binary(model / "images.bin")
    points3D = read_points3D_binary(model / "points3D.bin")
    # image_ids = {image.name: i for i, image in images.items()}
    matches = {}
    scores = {}
    # for pt3d_id, pt3d in points3D.items():
    #     if len(pt3d.image_ids)>10:
    #         continue
    #     image_ids = pt3d.image_ids
    #     sorted_list = np.sort(image_ids)
    #     max_gap = np.max(np.diff(sorted_list))
    #     if max_gap > 5:
    #         continue
    #     point2D_idxs = pt3d.point2D_idxs
    #     for i in range(len(image_ids)-1):
    #         j = i+1
    #         image_i = images[image_ids[i]]
    #         image_j = images[image_ids[j]]
    #         pair_name = f'{image_i.name} {image_j.name}'
    #         pt_id_pair = np.array([point2D_idxs[i], point2D_idxs[j]])
    #         if image_ids[i] > image_ids[j]:
    #             pair_name = f'{image_j.name} {image_i.name}'
    #             pt_id_pair = np.array([point2D_idxs[j], point2D_idxs[i]])
    #         if pair_name not in matches:
    #             matches[pair_name] = np.array([pt_id_pair])
    #             scores[pair_name] = np.array([1.0])
    #         else:
    #             matches[pair_name] = np.vstack((matches[pair_name], pt_id_pair))
    #             scores[pair_name] = np.append(scores[pair_name], 1.0)
    #         print('a')
    #     print('a')
    # print('a')
    # matches_path = Path('/home/weichuan/data/auto_labeling_datas/rogs_batch_run/ppl_bag_20231129_150824_det_25450_25640/sfm/all/local_matches.h5')
    # output_patch = Path(str(matches_path).replace('.h5', '_new.h5'))
    # write_match(matches, scores, output_patch)
    
    
    
    
    
    parser = argparse.ArgumentParser()
    parser.add_argument('--config', type=str, default='/home/weichuan/data/auto_labeling_datas/rogs_batch_run/ppl_bag_20231129_150824_det_25450_25640/config.json', help='config name')
    args = parser.parse_args()
    cfg = json.load(open(args.config, "r"))
    main(cfg=cfg, ext='.txt')
    
