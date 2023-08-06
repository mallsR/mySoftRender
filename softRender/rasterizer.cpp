//
//  rasterizer.cpp
//  softRender
//
//  Created by xiaoR on 2023/8/5.
//

#include "rasterizer.hpp"

// 构造函数
Rasterizer::Rasterizer() {
    width = 700;
    height = 700;
}

Rasterizer::Rasterizer(int width, int height) {
    this->width = width;
    this->height = height;
}

// 清空深度缓存和颜色
void Rasterizer::clear(bool color_clear, bool depth_color) {
    if (color_clear) {
        std::fill(frame_buffer.begin(), frame_buffer.end(), Eigen::Vector3f{0, 0, 0});
    }
//    以float极小值进行填充
    if (depth_color) {
        std::fill(z_buffer.begin(), z_buffer.end(), __FLT_MIN__);
    }
}

void Rasterizer::set_model(Eigen::Matrix4f model) {
    this->model = model;
}

void Rasterizer::set_view(Eigen::Matrix4f view) {
    this->view = view;
}

void Rasterizer::set_projection(Eigen::Matrix4f projection) {
    this->projection = projection;
}

