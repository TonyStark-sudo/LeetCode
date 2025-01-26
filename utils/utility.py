import sys
import os
import re
import numpy as np
import math
from pyquaternion import Quaternion
import json
import h5py
import cv2

def list2quat_wxyz(quat_wxyz):
    q = Quaternion(quat_wxyz[0], quat_wxyz[1], quat_wxyz[2], quat_wxyz[3], dtype=np.float64)
    q = q.normalised
    return q

def list2quat_xyzw(quat_xyzw):
    q = Quaternion(quat_xyzw[3], quat_xyzw[0], quat_xyzw[1], quat_xyzw[2], dtype=np.float64)
    q = q.normalised
    return q

def parse_trans(json_trans):
    p = np.array([0.0] * 3)
    p[0] = json_trans["translation"]["x"]
    p[1] = json_trans["translation"]["y"]
    p[2] = json_trans["translation"]["z"]
    q = Quaternion(json_trans["rotation"]["w"], json_trans["rotation"]["x"], json_trans["rotation"]["y"], json_trans["rotation"]["z"], dtype=np.float64)
    q = q.normalised
    return p, q

def multiply(p1, q1, p2, q2):
    q1 = q1.normalised
    q2 = q2.normalised
    p = p1 + q1.rotate(p2)
    q = q1 * q2
    return p, q

def inverse(p, q):
    q = q.normalised
    pp = -1.0 * (q.inverse.rotate(p))
    qq = q.inverse
    return pp, qq

def deg2rad(degree):
    return degree * math.pi / 180.0

def rad2deg(rad):
    return rad * 180.0 / math.pi

def get_utm_zone_id(lat, lon):
    if lat > 84 or lat < -84:
        return -1
    else:
        return math.floor((lon + 180) / 6) + 1

def convert_rot_from_utm_to_enu(lat, lon, q_utm):
    utm_id = get_utm_zone_id(lat, lon)
    gamma = math.atan(math.tan(deg2rad(lon - (6 * utm_id - 183))) * math.sin(deg2rad(lat)))
    R = q_utm.rotation_matrix
    euler = [0.0, 0.0, 0.0]
    euler[0] = math.atan2(-R[0, 1], R[1, 1])
    euler[1] = math.asin(R[2, 1])
    euler[2] = math.atan2(-R[2, 0], R[2, 2])
    euler[0] -= gamma
    q_enu = Quaternion.exp(Quaternion(0, 0, 0, euler[0] / 2.0)) * Quaternion.exp(Quaternion(0, euler[1] / 2.0, 0, 0)) * Quaternion.exp(Quaternion(0, 0, euler[2] / 2.0, 0))
    return q_enu

def convert_rot_from_enu_to_utm(lat, lon, q_enu):
    utm_id = get_utm_zone_id(lat, lon)
    gamma = math.atan(math.tan(deg2rad(lon - (6 * utm_id - 183))) * math.sin(deg2rad(lat)))
    R = q_enu.rotation_matrix
    euler = [0.0, 0.0, 0.0]
    euler[0] = math.atan2(-R[0, 1], R[1, 1])
    euler[1] = math.asin(R[2, 1])
    euler[2] = math.atan2(-R[2, 0], R[2, 2])
    euler[0] += gamma
    q_utm = Quaternion.exp(Quaternion(0, 0, 0, euler[0] / 2.0)) * Quaternion.exp(Quaternion(0, euler[1] / 2.0, 0, 0)) * Quaternion.exp(Quaternion(0, 0, euler[2] / 2.0, 0))
    return q_utm

def read_json(json_file):
    json_data = json.load(open(json_file, "r"))
    return json_data

def write_json(cfg, json_file):
    with open(json_file, "w") as f:
        f.write(json.dumps(cfg))

def write_txt(path, data_list):
    with open(path, "w") as f:
        s = ""
        for line in data_list:
            linestr = [str(x) for x in line]
            linestr = " ".join(linestr) + "\n"
            s += linestr
        f.write(s)

def write_config_demo(json_file):
    cfg = dict()
    cfg["s3_path"] = "s3://tf-23q3-shared-data/parsed_data/car_9/20230805/ppl_bag_20230805_165518_det/v2_230818_010714"
    cfg["local_root"] = "/data/data_zsp"
    cfg["csv_frame_id_range"] = [1288, 1888]
    cfg["lidar_names"] = ["fuser_lidar"]
    cfg["lidar_ids"] = [[1, 3]]
    cfg["camera_names"] = ["cam_front_120", "cam_back_120"]
    cfg["downsample_dts"] = [0.1, 0.1]
    with open(json_file, "w", encoding="utf-8") as f:
        f.write(json.dumps(cfg, indent=4))

def write_config(cfg, json_file):
    with open(json_file, "w", encoding="utf-8") as f:
        f.write(json.dumps(cfg, indent=4))

def get_txt_list(path):
    lines = open(path,'r').readlines()
    line_lists = [line.strip().split(" ") for line in lines]
    return line_lists

def save_cfg(cfg, str_txt):
    print(f"{str_txt} cfg: {cfg} !!!")
    local_cfg = os.path.join(cfg["local_path"], "config.json")
    write_config(cfg, local_cfg)
    print(f"{str_txt} local_cfg: {local_cfg} !!!")

def recursive(file1, file2):
    if not hasattr(file1, "keys"):
        return
    for key in file1.keys():
        if key in file2:
            recursive(file1[key], file2[key])
        else:
            file1.copy(key, file2)

def merge_h5py(files, output_file):
    with h5py.File(output_file, "w") as out:
        for file in files:
            with h5py.File(file, 'r') as f:
                recursive(f, out)


def read_pfm(filename):
    file = open(filename, 'rb')
    color = None
    width = None
    height = None
    scale = None
    endian = None

    header = file.readline().decode('utf-8').rstrip()
    if header == 'PF':
        color = True
    elif header == 'Pf':
        color = False
    else:
        raise Exception('Not a PFM file.')

    dim_match = re.match(r'^(\d+)\s(\d+)\s$', file.readline().decode('utf-8'))
    if dim_match:
        width, height = map(int, dim_match.groups())
    else:
        raise Exception('Malformed PFM header.')

    scale = float(file.readline().rstrip())
    if scale < 0:  # little-endian
        endian = '<'
        scale = -scale
    else:
        endian = '>'  # big-endian

    data = np.fromfile(file, endian + 'f')
    shape = (height, width, 3) if color else (height, width)

    data = np.reshape(data, shape)
    data = np.flipud(data)
    file.close()
    return data, scale


def save_pfm(filename, image, scale=1):
    file = open(filename, "wb")
    color = None

    image = np.flipud(image)

    if image.dtype.name != 'float32':
        raise Exception('Image dtype must be float32.')

    if len(image.shape) == 3 and image.shape[2] == 3:  # color image
        color = True
    elif len(image.shape) == 2 or len(image.shape) == 3 and image.shape[2] == 1:  # greyscale
        color = False
    else:
        raise Exception('Image must have H x W x 3, H x W x 1 or H x W dimensions.')

    file.write('PF\n'.encode('utf-8') if color else 'Pf\n'.encode('utf-8'))
    file.write('{} {}\n'.format(image.shape[1], image.shape[0]).encode('utf-8'))

    endian = image.dtype.byteorder

    if endian == '<' or endian == '=' and sys.byteorder == 'little':
        scale = -scale

    file.write(('%f\n' % scale).encode('utf-8'))

    image.tofile(file)
    file.close()

def trans2mat(p, q):
    trans = np.eye(4)
    trans[0:3, 0:3] = q.rotation_matrix
    trans[0:3, 3] = p
    return trans

def mat2trans(mat):
    p = mat[:3,3]
    q = Quaternion._from_matrix(mat[:3,:3])
    return p, q

def undistort_img(img, camera_info):
    K = camera_info["K"]
    D = camera_info["D"]

    K = np.array(K).reshape(3, 3)
    D = np.array(D).reshape(-1, 1)

    distortion_model = camera_info["distortion_model"]
    if distortion_model == "pinhole":
        undist_img = cv2.undistort(img, K, D, None, None)
        return undist_img
    elif distortion_model == "fisheye":
        # imsize = (img.shape[1], img.shape[0])
        # map1, map2 = cv2.fisheye.initUndistortRectifyMap(K, D, np.eye(3), K, imsize, cv2.CV_16SC2)
        # undist_img = cv2.remap(img, map1, map2, interpolation=cv2.INTER_LINEAR, borderMode=cv2.BORDER_CONSTANT)
        undist_img = cv2.fisheye.undistortImage(img, K, D=D, Knew=K)
        return undist_img

    return None

def resize_img(img, camera_info):
    if camera_info["resolution"][0] != camera_info["resize"][0] or camera_info["resolution"][1] != camera_info["resize"][1]:
        size_new = (int(camera_info["resize"][0]), int(camera_info["resize"][1]))
        img_new = cv2.resize(img, size_new)
        return img_new
    return img

def project_points(pointsi, camera_info):
    pts_idx = np.where(pointsi[:,2] > 0.1)
    pointsi = pointsi[pts_idx]

    K = camera_info["K"]
    D = camera_info["D"]

    K = np.array(K).reshape(3, 3)
    D = np.array(D).reshape(-1, 1)
    points = pointsi[:, 0:3]
    if camera_info["resolution"][0] != camera_info["resize"][0] or camera_info["resolution"][1] != camera_info["resize"][1]:
        fx = float(camera_info["resize"][0]) / float(camera_info["resolution"][0])
        fy = float(camera_info["resize"][1]) / float(camera_info["resolution"][1])
        K[0] *= fx
        K[1] *= fy

    if camera_info["undistortion"]:
        dis_pts = cv2.projectPoints(points.reshape(1, -1, 3), rvec=np.eye(3), tvec=np.zeros(3), cameraMatrix=K, distCoeffs=None)[0].reshape(-1, 2)
    else:
        distortion_model = camera_info["distortion_model"]
        if distortion_model == "pinhole":
            dis_pts = cv2.projectPoints(points.reshape(1, -1, 3), rvec=np.eye(3), tvec=np.zeros(3), cameraMatrix=K, distCoeffs=D)[0].reshape(-1, 2)
        elif distortion_model == "fisheye":
            dis_pts = cv2.fisheye.projectPoints(points.reshape(1, -1, 3), rvec=np.zeros(3), tvec=np.zeros(3), K=K, D=D)[0].reshape(-1, 2)
        else:
            print("project error !!!")
            return None

    idx = np.where((dis_pts[:,0] > 0) & (dis_pts[:,0] < camera_info["resize"][0]) & (dis_pts[:,1] > 0) & (dis_pts[:,1] < camera_info["resize"][1]))

    if pointsi.shape[1] >= 4:
        dis_ptsi = np.hstack([dis_pts, pointsi[:, 3:].reshape(pointsi.shape[0], -1)])
    else:
        dis_ptsi = dis_pts

    return pointsi[idx], dis_ptsi[idx]

def project_points_with_mask(pointsi, camera_info):
    mask1 = pointsi[:,2] > 0.1
    pointsi = pointsi[mask1]

    K = camera_info["K"]
    D = camera_info["D"]

    K = np.array(K).reshape(3, 3)
    D = np.array(D).reshape(-1, 1)
    points = pointsi[:, 0:3]
    if camera_info["resolution"][0] != camera_info["resize"][0] or camera_info["resolution"][1] != camera_info["resize"][1]:
        fx = float(camera_info["resize"][0]) / float(camera_info["resolution"][0])
        fy = float(camera_info["resize"][1]) / float(camera_info["resolution"][1])
        K[0] *= fx
        K[1] *= fy

    if camera_info["undistortion"]:
        dis_pts = cv2.projectPoints(points.reshape(1, -1, 3), rvec=np.eye(3), tvec=np.zeros(3), cameraMatrix=K, distCoeffs=None)[0].reshape(-1, 2)
    else:
        distortion_model = camera_info["distortion_model"]
        if distortion_model == "pinhole":
            dis_pts = cv2.projectPoints(points.reshape(1, -1, 3), rvec=np.eye(3), tvec=np.zeros(3), cameraMatrix=K, distCoeffs=D)[0].reshape(-1, 2)
        elif distortion_model == "fisheye":
            dis_pts = cv2.fisheye.projectPoints(points.reshape(1, -1, 3), rvec=np.zeros(3), tvec=np.zeros(3), K=K, D=D)[0].reshape(-1, 2)
        else:
            print("project error !!!")
            return None

    mask2 = (dis_pts[:,0] > 0) & (dis_pts[:,0] < camera_info["resize"][0]) & (dis_pts[:,1] > 0) & (dis_pts[:,1] < camera_info["resize"][1])

    if pointsi.shape[1] >= 4:
        dis_ptsi = np.hstack([dis_pts, pointsi[:, 3:].reshape(pointsi.shape[0], -1)])
    else:
        dis_ptsi = dis_pts

    idx = np.where(mask1)[0][mask2]
    mask = np.zeros_like(mask1, dtype=bool)
    mask[idx] = True

    return pointsi[mask2], dis_ptsi[mask2], mask

def transform_points(p, q, ptsi):
    if len(ptsi) == 0:
        return ptsi
    pts = ptsi[:, 0:3]
    trans = trans2mat(p, q)
    pts_homo = np.hstack([pts, np.ones((pts.shape[0], 1))])
    pts2 = trans @ pts_homo.T
    pts2 = pts2[:3,:].T
    pts2 = np.hstack([pts2, ptsi[:,3:].reshape(ptsi.shape[0], -1)])

    return pts2

def get_tum_pose_from_line(line):
    p = np.array([np.float64(x) for x in line[1:4]])
    q = [np.float64(x) for x in line[4:8]]
    q = list2quat_xyzw(q)
    return [line[0], p, q]

def interpolate_poses(timestamps, poses, dt):
    interp_poses = []
    i, j = 0, 0
    while i < len(timestamps) and j + 1 < len(poses):
        if float(poses[j+1][0]) + dt < float(timestamps[i]):
            j += 1
            continue
        elif float(poses[j][0]) + dt > float(timestamps[i]):
            i += 1
            continue

        s = (float(timestamps[i]) - float(poses[j][0]) - dt) / (float(poses[j+1][0]) - float(poses[j][0]))
        p = (1.0 - s) * poses[j][1] + s * poses[j+1][1]
        q = slerp_quaternion(poses[j][2], poses[j+1][2], amount=s)
        interp_poses.append([timestamps[i], p, q])
        i += 1
    return interp_poses

def slerp_quaternion(q0, q1, amount=0.5):
    q0._fast_normalise()
    q1._fast_normalise()

    dot = np.dot(q0.q, q1.q)

    if dot < 0.0:
        q0.q = -q0.q
        dot = -dot

    # sin_theta_0 can not be zero
    if dot > 0.9995:
        qr = Quaternion(q0.q + amount * (q1.q - q0.q))
        qr._fast_normalise()
        return qr

    theta_0 = np.arccos(dot)  # Since dot is in range [0, 0.9995], np.arccos() is safe
    sin_theta_0 = np.sin(theta_0)

    theta = theta_0 * amount
    sin_theta = np.sin(theta)

    s0 = np.cos(theta) - dot * sin_theta / sin_theta_0
    s1 = sin_theta / sin_theta_0
    qr = Quaternion((s0 * q0.q) + (s1 * q1.q))
    qr._fast_normalise()
    return qr

def interpolate_poses_all(timestamps, poses, dt):
    interp_poses = []
    j = 0
    if len(poses) == 0:
        return interp_poses
    for i, timestamp in enumerate(timestamps):
        while j + 1 < len(poses):
            if np.abs(float(poses[j+1][0]) + dt - float(timestamp)) < np.abs(float(poses[j][0]) + dt - float(timestamp)):
                j += 1
            else:
                break
        first_j = j
        second_j = j
        if j > 0 and j + 1 < len(poses):
            if float(timestamps[i]) > float(poses[j][0]) + dt and float(timestamps[i]) < float(poses[j+1][0]) + dt:
                second_j = j + 1
            elif float(timestamps[i]) > float(poses[j-1][0]) + dt and float(timestamps[i]) < float(poses[j][0]) + dt:
                first_j = j - 1
            elif np.abs(float(poses[j+1][0]) + dt - float(timestamp)) < np.abs(float(poses[j-1][0]) + dt - float(timestamp)):
                second_j = j + 1
            else:
                first_j = j - 1
        elif j > 0:
            first_j = j - 1
        elif j + 1 < len(poses):
            second_j = j + 1

        if first_j == second_j:
            interp_poses.append([timestamps[i], poses[first_j][1], poses[first_j][2]])
            print("warning only one poses !!!")
            continue
        s = (float(timestamps[i]) - float(poses[first_j][0]) - dt) / (float(poses[second_j][0]) - float(poses[first_j][0]))
        if s < 0.0 or s > 1.0:
            print(f"warning s is {s} (should be between 0 ~ 1) !!!")
        p = (1.0 - s) * poses[first_j][1] + s * poses[second_j][1]
        q = slerp_quaternion(poses[first_j][2], poses[second_j][2], amount=s)
        interp_poses.append([timestamps[i], p, q])
    return interp_poses

def get_synced_rpose(first_poses, second_poses, synced_map):
    first_rposes, second_poses2 = [], []
    i, j, k = 0, 0, 0
    while i < len(first_poses) and j < len(second_poses) and k < len(synced_map):
        while i < len(first_poses) and k < len(synced_map):
            if first_poses[i][0] != synced_map[k][0]:
                if float(first_poses[i][0]) < float(synced_map[k][0]):
                    i += 1
                    continue
                else:
                    k += 1
                    continue
            else:
                break

        if i >= len(first_poses) or k >= len(synced_map):
            break

        while j < len(second_poses):
            if second_poses[j][0] != synced_map[k][1]:
                if float(second_poses[j][0]) < float(synced_map[k][1]):
                    j += 1
                    continue
                else:
                    i += 1
                    k += 1
                    continue
            else:
                break

        if i >= len(first_poses) or k >= len(synced_map) or j >= len(second_poses):
            break
        pc = np.array([np.float64(x) for x in second_poses[j][1:4]])
        qc = [np.float64(x) for x in second_poses[j][4:8]]
        qc = list2quat_xyzw(qc)
        pl = np.array([np.float64(x) for x in first_poses[i][1:4]])
        ql = [np.float64(first_poses[i][7])]
        ql = [np.float64(x) for x in first_poses[i][4:8]]
        ql = list2quat_xyzw(ql)
        pcl, qcl = multiply(*inverse(pc, qc), pl, ql)
        first_rposes.append([first_poses[i][0], pcl, qcl])
        second_poses2.append(second_poses[j])
        i += 1
        j += 1
        k += 1

    return first_rposes, second_poses2