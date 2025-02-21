#pragma once
#include <iostream>

class ProjectorCUDA {
public:
    ProjectorCUDA(float *points_w, size_t *point_size);
    ~ProjectorCUDA();
    void project(float *rotation, float *translation,
                 int *width, int *height, float *K,
                 float *depth_image, unsigned char *intensity_image);
    
    void filter_pts(float *projectionMatching, bool *hasPointProjected,
                    bool *isPointVisible, int *L);

private:
    // cuda
    float *points_w_;
    size_t *point_size_;
    // raster
    int *points_index;
    int *points_index_sorted;
    uint64_t *pixel_index, *pixel_index_sorted;
    float *points_depth;
    // host
    size_t size_;
};
