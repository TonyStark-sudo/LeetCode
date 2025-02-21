#include "project_cuda.h"
#include <cub/cub.cuh>
#include <cub/device/device_radix_sort.cuh>

__global__ void identify_pixel_ranges(uint64_t* pixel_index, uint2* ranges,
                                        size_t *point_size, uint64_t threshold) {
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
	if (idx >= *point_size)
		return;
    uint32_t pixel = pixel_index[idx] >> 32;
    if (pixel > threshold) {
        return;
    }
	if (idx == 0) {
		ranges[pixel].x = 0;
    }
	else {
		uint32_t pre_pixel = pixel_index[idx - 1] >> 32;
		if (pixel != pre_pixel) {
			ranges[pre_pixel].y = idx;
			ranges[pixel].x = idx;
		}
	}
	if (idx == *point_size - 1) {
		ranges[pixel].y = *point_size;
    }
}

__global__ void init_ranges(uint2* ranges, int *width, int *height) {
    int idx = threadIdx.x + blockDim.x * blockIdx.x;
    if (idx >= *width * *height) {
        return;
    }
    ranges[idx].x = 0;
    ranges[idx].y = 0;
}

__global__ void project_cuda(float *rotation, float *translation,
                             int *width, int *height, float *K,
                             float *points, size_t * point_size,
                             uint64_t *pixel_index, int *points_index, float *points_depth) {
    // TODO: pixel_inde points_index points_depth need reset
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    if (idx >= *point_size) {
        return;
    }
    // 可见idx是线性增加的，每隔四个存储空间是一个点的x, y, z, i
    // float3 是 CUDA 提供的一种 内置向量类型，用于存储 三维浮点数（x, y, z）
    float3 p_m = { points[4 * idx], points[4 * idx + 1], points[4 * idx + 2] };
    // to calculate p_c
    float3 p_c = {0, 0, 0};
    p_c.x = rotation[0] * p_m.x + rotation[1] * p_m.y + rotation[2] * p_m.z + translation[0];
    p_c.y = rotation[3] * p_m.x + rotation[4] * p_m.y + rotation[5] * p_m.z + translation[1];
    p_c.z = rotation[6] * p_m.x + rotation[7] * p_m.y + rotation[8] * p_m.z + translation[2];
    
    // 对相机系下的三维点进行过滤，不处理深度120米开外的点，和到相机光心距离130米之外的点
    if (p_c.z > 120 || p_c.z < 0 || (p_c.x * p_c.x + p_c.y * p_c.y + p_c.z + p_c.z) > 130 * 130) {
        return;
    }
    float3 p_cuni = {p_c.x / p_c.z, p_c.y / p_c.z, 1.0};
    float u = K[0] * p_cuni.x + K[1] * p_cuni.y + K[2];
    float v = K[3] * p_cuni.x + K[4] * p_cuni.y + K[5];
    
    // 强制类型转换将浮点型u, v转换成整型x, y
    int x = static_cast<int>(u);
    int y = static_cast<int>(v);

    // 对像素平面的像素坐标进行过滤，不处理在图像画幅之外的点
    if (x < 0 || x >= *width || y < 0 || y >= *height) {
        return;
    }

    // 得到x, y像素点的一维索引
    uint64_t pixel_index_i = static_cast<uint64_t>(y * *width + x);

    // 将pixel_index_i左移32位，低32位用来存储其他信息
    pixel_index_i <<= 32;

    // 低 32 位存储 p_c.z（当前点的深度值)
    pixel_index_i |= *((uint32_t*)&(p_c.z));

    // ************重要***********
    // 将像素(x, y)的一维索引存储到 pixel_index[idx(可以看做世界坐标系三维点的id)]
    pixel_index[idx] = pixel_index_i;

    // 将有效的idx(可以看做世界坐标系三维点的id)存储到points_index
    points_index[idx] = idx;
    
    // 将有效的idx(可以看做世界坐标系三维点的id)的深度值存储到points_depth
    points_depth[idx] = p_c.z;
}

__global__ void render(float* __restrict__ depth_image, unsigned char* __restrict__ intensity_image,
                       int* __restrict__ width, int* __restrict__ height,
                       float* __restrict__ points, uint2* __restrict__ ranges,
                       int* __restrict__ points_index, float* __restrict__ points_depth) {
    int pixel_x = blockIdx.x * blockDim.x + threadIdx.x;
    int pixel_y = blockIdx.y * blockDim.y + threadIdx.y;
    if (pixel_x >= *width || pixel_y >= *height) {
        return;
    }
    int pixel_index = pixel_x + pixel_y * *width;
    uint2 range = ranges[pixel_index];
    if (range.y > range.x) {
        // float min_depth = 120.0;
        // float best_intensity = 0;
        // for (int k = range.x; k < range.y; k++) {
        //      // sum += *translation - bev_points[3 * points_index[k] + 2];
        //     float depth = points_depth[points_index[k]];
        //     if (depth < min_depth) {
        //         min_depth = depth;
        //         best_intensity = points[4 * points_index[k] + 3];
        //     }
        // }
        // depth_image[pixel_index] = min_depth;
        // intensity_image[pixel_index] = static_cast<unsigned char>(best_intensity);
        depth_image[pixel_index] = points_depth[points_index[range.x]];
        intensity_image[pixel_index] = static_cast<unsigned char>(points[4 * points_index[range.x] + 3]);
    }
    else {
        depth_image[pixel_index] = 0.0;
        intensity_image[pixel_index] = 0;
    }
}

ProjectorCUDA::ProjectorCUDA(float *points_w, size_t *point_size) : size_(*point_size) {
    
    // 给类的成员函数分配Device(GPU)内存

    cudaMalloc((void**)&points_w_, *point_size * sizeof(float) * 4);
    cudaMalloc((void**)&point_size_, sizeof(size_t));
    cudaMalloc((void**)&pixel_index, size_ * sizeof(uint64_t));
    cudaMalloc((void**)&points_index, size_ * sizeof(int));
    cudaMalloc((void**)&pixel_index_sorted, size_ * sizeof(uint64_t));
    cudaMalloc((void**)&points_index_sorted, size_ * sizeof(int));
    cudaMalloc((void**)&points_depth, size_ * sizeof(float));

    // 将数据复制到GPU内存(显存)中,其中points_w_是成员变量，在显存中所占的存储空间为*point_size * sizeof(float) * 4

    cudaMemcpy(points_w_, points_w, *point_size * sizeof(float) * 4, cudaMemcpyHostToDevice);
    cudaMemcpy(point_size_, point_size, sizeof(size_t), cudaMemcpyHostToDevice);
}


void ProjectorCUDA::project(float *h_rotation, float *h_translation,
             int *h_width, int *h_height, float *h_K,
             float *h_depth_image, unsigned char *h_intensity_image) {
    // rasterization
    // pose and intrinsic
    // 初始化显存中的变量
    cudaMemset(points_index, 0, size_ * sizeof(int));
    cudaMemset(points_index_sorted, 0, size_ * sizeof(int));
    cudaMemset(points_depth, 0, size_ * sizeof(float));

    // pixel_index为什么初始化成w * h + 1, size是所有3D点的数量？
    cudaMemset(pixel_index, (*h_width * *h_height) + 1, size_ * sizeof(uint64_t));
    cudaMemset(pixel_index_sorted, 0, size_ * sizeof(uint64_t));
    uint2* ranges;
    float *rotation, *translation;
    int *width, *height;
    float *K;
    float *depth_image;
    unsigned char *intensity_image;

    cudaMalloc((void**)&ranges, (*h_width) * (*h_width) * sizeof(uint2));

    cudaMalloc((void**)&rotation, 9 * sizeof(float));
    cudaMalloc((void**)&translation, 3 * sizeof(float));
    cudaMalloc((void**)&width, sizeof(int));
    cudaMalloc((void**)&height, sizeof(int));
    cudaMalloc((void**)&K, 9 * sizeof(float));
    cudaMalloc((void**)&depth_image, (*h_width) * (*h_height) * sizeof(float));
    cudaMalloc((void**)&intensity_image, (*h_width) * (*h_height) * sizeof(unsigned char));

    cudaMemcpy(rotation, h_rotation, 9 * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(translation, h_translation, 3 * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(width, h_width, sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(height, h_width, sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(K, h_K, 9 * sizeof(float), cudaMemcpyHostToDevice);

    project_cuda<<< (size_ + 255) / 256, 256 >>>(rotation, translation,
                                                 width, height, K,
                                                 points_w_, point_size_,
                                                 pixel_index, points_index, points_depth);

    cudaDeviceSynchronize();
    void *sort_storage_tmp = nullptr;
    size_t sort_storage_size = 0;
    cub::DeviceRadixSort::SortPairs(sort_storage_tmp, sort_storage_size, pixel_index, pixel_index_sorted, points_index, points_index_sorted, size_);
    cudaMalloc(&sort_storage_tmp, sort_storage_size);
    cub::DeviceRadixSort::SortPairs(sort_storage_tmp, sort_storage_size, pixel_index, pixel_index_sorted, points_index, points_index_sorted, size_);
    cudaDeviceSynchronize();
    // test<<< 1, 1 >>>(pixel_index_sorted, points_index_sorted);
    cudaFree(sort_storage_tmp);
    init_ranges<<< (*h_width * *h_height + 255) / 256 , 256>>>(ranges, width, height);
    identify_pixel_ranges<<< (size_ + 255) / 256, 256 >>> (pixel_index_sorted, ranges, point_size_, *h_width * *h_height);
    dim3 block_dim(16, 16);
    dim3 grid_dim((*h_width + block_dim.x - 1) / block_dim.x, (*h_height + block_dim.y - 1) / block_dim.y);

    render<<< grid_dim, block_dim >>>(depth_image, intensity_image,
                              width, height, points_w_, ranges, points_index_sorted, points_depth);

    cudaDeviceSynchronize();

    cudaMemcpy(h_depth_image, depth_image, (*h_height * *h_width) * sizeof(float), cudaMemcpyDeviceToHost);
    cudaMemcpy(h_intensity_image, intensity_image, (*h_height * *h_width) * sizeof(unsigned char), cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();

    // cudaFree(pixel_index);
    // cudaFree(points_index);
    // cudaFree(pixel_index_sorted);
    // cudaFree(points_index_sorted);
    // cudaFree(points_depth);
    cudaFree(ranges);
    cudaFree(rotation);
    cudaFree(translation);
    cudaFree(width);
    cudaFree(height);
    cudaFree(K);
    cudaFree(depth_image);
    cudaFree(intensity_image);
    cudaMemset(depth_image, 0, (*h_height * *h_width) * sizeof(float));
    cudaMemset(intensity_image, 0, (*h_height * *h_width) * sizeof(unsigned char));
}

void ProjectorCUDA::filter_pts(float *projectionMatching, bool *hasPointProjected, 
                               bool *isPointVisible, int *L) {
    // 先弄清楚需要哪些成员变量，可以在这里进行赋值
    cudaMemset();
    float 

}

ProjectorCUDA::~ProjectorCUDA() {
    cudaFree(points_w_);
    cudaFree(point_size_);
    cudaFree(pixel_index);
    cudaFree(points_index);
    cudaFree(pixel_index_sorted);
    cudaFree(points_index_sorted);
    cudaFree(points_depth);
}

__global__ void ComputePointVisibility_cuda(bool *isPointVisible, 
                                           float *projectionMatching,
                                           bool *hasPointProjected, 
                                           int *width, int *height, int *L)
{
    int idx = threadIdx.x + blockIdx.x * blockDim.x;

    if (idx >= *point_size) {
        return;
    }

    int idx = blockIdx.x * blockDim.x + threadIdx.x;

    if (idx >= H * W) return;  // If the thread index exceeds the total number of pixels, return.

    int i = idx / W;  // Row index
    int j = idx % W;  // Column index

    if (hasPointProjected[idx])
    {
        Eigen::Vector2d Xpx(i, j);
        Eigen::Vector3d P = projectionMatching[idx];

        int umin = max(0, i - L); int vmin = max(0, j - L);
        int umax = min(H - 1, i + L); int vmax = min(W - 1, j + L);

        double solidAnglesPerSector[8];
        bool isSectorEmpty[8];
        
        // Initialize the solidAnglesPerSector with large values and mark all sectors as empty.
        for (int k = 0; k < 8; ++k)
        {
            solidAnglesPerSector[k] = std::numeric_limits<double>::max();
            isSectorEmpty[k] = true;
        }

        for (int u = umin; u <= umax; ++u)
        {
            for (int v = vmin; v <= vmax; ++v)
            {
                if (u == i && v == j) continue;

                int neighborIdx = u * W + v;
                if (hasPointProjected[neighborIdx])
                {
                    Eigen::Vector2d Ypx(u, v) - Xpx;
                    double polar2DAngle = std::atan2(Ypx(1), Ypx(0)) + 3.14159265359;
                    int sectorIdx = floor(4.0 * polar2DAngle / 3.14159265359);

                    Eigen::Vector3d Q = projectionMatching[neighborIdx];
                    Eigen::Vector3d PQ = Q - P;
                    Eigen::Vector3d PO = -1.0 * P;
                    double angle = std::abs(SignedAngle(PO, PQ));

                    if (angle < solidAnglesPerSector[sectorIdx])
                    {
                        solidAnglesPerSector[sectorIdx] = angle;
                        isSectorEmpty[sectorIdx] = false;
                    }
                }
            }
        }

        double sumAngle = 0;
        unsigned int nbrNonEmptySector = 0;
        unsigned int nbrSectorNonVisible = 0;
        for (int sectorIdx = 0; sectorIdx < 8; ++sectorIdx)
        {
            if (!isSectorEmpty[sectorIdx])
            {
                sumAngle += solidAnglesPerSector[sectorIdx];
                nbrNonEmptySector++;
                if (solidAnglesPerSector[sectorIdx] < 0.25)
                {
                    nbrSectorNonVisible++;
                }
            }
        }

        if (sumAngle < 2.0)
        {
            isPointVisible[idx] = false;
        }
    }
}

void LaunchComputePointVisibilityCUDA(bool* isPointVisible,
                                      const Eigen::Vector3d* projectionMatching,
                                      const bool* hasPointProjected, 
                                      int H, int W, int L)
{
    int blockSize = 256;  // Number of threads per block
    int numBlocks = (H * W + blockSize - 1) / blockSize;  // Number of blocks needed

    // Launch the kernel to compute visibility
    ComputePointVisibilityCUDA<<<numBlocks, blockSize>>>(isPointVisible, projectionMatching, hasPointProjected, H, W, L);

    // Wait for kernel to finish
    cudaDeviceSynchronize();
}
