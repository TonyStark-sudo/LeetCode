import os
import numpy as np

def get_candidate_ground_points(img_seg_infos, img_name, key_points, img_size):
    img_seg_scale = 1.0
    if 'img_seg_scale' in img_seg_infos:
        img_seg_scale = img_seg_infos['img_seg_scale']
    img_seg_npy_file = f'{img_seg_infos["img_seg_npy_subdir"]}/{img_name}'
    if not os.path.exists(img_seg_npy_file):
        return None
    key_points_int = np.round(img_seg_scale*key_points).astype(int)
    seg_label = np.load(img_seg_npy_file)
    seg_label = np.round(seg_label).astype(int)
    points_clipped_x = np.clip(key_points_int[:, 0], 0, int(img_seg_scale*img_size[0]) - 1)
    points_clipped_y = np.clip(key_points_int[:, 1], 0, int(img_seg_scale*img_size[1]) - 1)
    key_points_int = np.stack((points_clipped_x, points_clipped_y), axis=-1)

    candidate_static_points = np.isin(seg_label[key_points_int[:, 1], key_points_int[:, 0]], [7, 8, 11, 13, 14, 15, 23, 24])
    return candidate_static_points

def get_candidate_dynamic_points(img_seg_infos, img_name, key_points, img_size):
    img_seg_scale = 1.0
    if 'img_seg_scale' in img_seg_infos:
        img_seg_scale = img_seg_infos['img_seg_scale']
    img_seg_npy_file = f'{img_seg_infos["img_seg_npy_subdir"]}/{img_name}'
    if not os.path.exists(img_seg_npy_file):
        return None
    key_points_int = np.round(img_seg_scale*key_points).astype(int)
    seg_label = np.load(img_seg_npy_file)
    seg_label = np.round(seg_label).astype(int)
    points_clipped_x = np.clip(key_points_int[:, 0], 0, int(img_seg_scale*img_size[0]) - 1)
    points_clipped_y = np.clip(key_points_int[:, 1], 0, int(img_seg_scale*img_size[1]) - 1)
    key_points_int = np.stack((points_clipped_x, points_clipped_y), axis=-1)

    candidate_static_points = (seg_label[key_points_int[:, 1], key_points_int[:, 0]] < 52) & (seg_label[key_points_int[:, 1], key_points_int[:, 0]] != 27)
    return candidate_static_points

def filter_candidate_dynamic_points(img_seg_infos, img_name, key_points):
    if img_seg_infos is None or len(img_seg_infos) == 0:
        return False
    if 'keypoints' not in key_points:
        return False
    candidate_static_points = get_candidate_dynamic_points(img_seg_infos, img_name, key_points['keypoints'], key_points['image_size'].tolist())
    if candidate_static_points is None:
        return False
    indices = np.where(candidate_static_points)[0]
    key_points['descriptors'] = key_points['descriptors'][:, indices]
    key_points['keypoints'] = key_points['keypoints'][indices, :]
    key_points['scores'] = key_points['scores'][indices]
    return True

def tag_candidate_dynamic_points(seg_image_file, key_points):
    if 'keypoints' not in key_points:
        return False
    candidate_static_points = get_candidate_dynamic_points(seg_image_file, key_points['keypoints'])
    print('tag')
