import numpy as np
import struct
import lzf
import copy
# import open3d
from pypcd4 import PointCloud

def save_lidar(points, path, type, attr, rgb=None):
    if type == "pcd":
        if attr == "xyzi":
            pc = PointCloud.from_xyzi_points(points.astype(np.float32).copy())
            pc.save(path)
        elif attr == "xyz":
            pc = PointCloud.from_xyz_points(points.astype(np.float32).copy())
            pc.save(path)
        elif attr == "xyzit":
            # pc = PointCloud.from_xyzit_points(points.astype(np.float64).copy())
            fields = ("x", "y", "z", "intensity", "timestamp")
            types = (np.float32, np.float32, np.float32, np.float32, np.float64)
            pc = PointCloud.from_points(points, fields, types)
            pc.save(path)
        elif attr == "xyzrgb":
            rgb_float = PointCloud.encode_rgb(rgb)
            pclrgb = np.hstack([points, rgb_float.reshape(-1,1)])
            pc = PointCloud.from_xyzrgb_points(pclrgb.astype(np.float32).copy())
            pc.save(path)
        else:
            print(f"save lidar pcd attr {attr} fail !!!")
        # elif attr == "xyzrgb":
        #     new_pcd = open3d.geometry.PointCloud()
        #     new_pcd.points = open3d.utility.Vector3dVector(points[:, 0:3])
        #     new_pcd.colors = open3d.utility.Vector3dVector(points[:, 3:6])
        #     open3d.io.write_point_cloud(path, new_pcd)
    else:
        np.save(path, points)

def read_lidar(path, type, attr):
    if type == "pcd":
        pc = PointCloud.from_path(path)
        pcl = pc.numpy()
        if attr == "xyz":
            return pcl[:,0:3]
        if attr == "xyzi":
            return pcl[:,0:4]
        if attr == "xyzit":
            return pcl[:,0:5]
        if attr == "xyzrgb":
            rgb_uint8 = PointCloud.decode_rgb(pcl[:,-1])
            return pcl[:,0:3], rgb_uint8
        else:
            return pcl
    else:
        pc = np.load(path)
        return pc

def merge_pcd(paths, type, attr, outpath):
    if type == "pcd":
        pcs = None
        for path in paths:
            pc = PointCloud.from_path(path)
            if pcs is None:
                pcs = pc
            else:
                pcs = pcs + pc
        if pcs is not None:
            pcs.save(outpath)
            print(f"merge_pcd outpath {outpath} !!!")
    else:
        print(f"merge pcd type {type} not surpported yet !!!")