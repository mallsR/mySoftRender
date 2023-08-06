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

// 设置mvp matrix
void Rasterizer::set_model(Eigen::Matrix4f model) {
    this->model = model;
}

void Rasterizer::set_view(Eigen::Matrix4f view) {
    this->view = view;
}

void Rasterizer::set_projection(Eigen::Matrix4f projection) {
    this->projection = projection;
}

void Rasterizer::draw(std::vector<Triangle *> model_triangles) {
//    遍历三角形进行绘制
    for(auto triangle : model_triangles) {
//        第一部分：对顶点坐标进行变换
//        ---------------------------------
//        对三角形顶点进行mvp变换 ： 到clip space
        Eigen::Matrix4f mvp = projection * view * model;
        Eigen::Vector4f clip_vertex[] = {
            mvp * triangle->v[0],
            mvp * triangle->v[1],
            mvp * triangle->v[2]
        };
        
//        对clip_vertex进行透视除法，得到NDC空间坐标
        for(auto vertex : clip_vertex) {
            vertex.x() /= vertex.w();
            vertex.y() /= vertex.w();
            vertex.z() /= vertex.w();
        }
        
//        视口变换，得到screen space
        float f1 = (50 - 0.1) / 2.0;
        float f2 = (50 + 0.1) / 2.0;
        Eigen::Vector4f screen_vertex[3];
        for(unsigned int i = 0; i < 3; ++i) {
            screen_vertex[i] = Eigen::Vector4f(
                                   (clip_vertex[i].x() + 1.0) * width / 2.0,
                                   (clip_vertex[i].y() + 1.0) * height / 2.0,
                                   clip_vertex[i].z() * f1 + f2,
                                    clip_vertex[i].w()
                                    );
        }
        
//        第二部分：对法线进行变换
//        -----------------------------------
        // 由于fragment shader的计算都是在世界空间坐标中进行的，所以需要将Normal也转换到世界空间坐标。
        // 但由于Normal的特殊性，不能直接使用model matrix进行变换，一个合适的方式是为其设置Normal matrix，先求逆，再转置
        // 计算过程见 ： http://www.lighthouse3d.com/tutorials/glsl-tutorial/the-normal-matrix/
        Eigen::Matrix4f normal_matrix = (view * model).inverse().transpose();
//        保存顶点法线
        Eigen::Vector4f normal[] = {
            normal_matrix * Eigen::Vector4f(triangle->v[0].x(), triangle->v[0].y(), triangle->v[0].z(), 1.0f),
            normal_matrix * Eigen::Vector4f(triangle->v[1].x(), triangle->v[1].y(), triangle->v[1].z(), 1.0f),
            normal_matrix * Eigen::Vector4f(triangle->v[2].x(), triangle->v[2].y(), triangle->v[2].z(), 1.0f)
        };
        
//        第三部分：将得到的三角形顶点和法线传入draw_triangle函数，进行三角形的绘制
//        --------------------------------------------------------------
        Triangle new_triangle = *triangle;
        
//        装载vertex
        for(unsigned int i = 0; i < 3; ++i) {
            new_triangle.setVertex(i, screen_vertex[i]);
        }
        
//        装载法线：注意法线是三维vector3f
        for(unsigned int i = 0; i < 3; ++i) {
            new_triangle.setNormal(i, normal[i].head<3>());
        }
        
//        设置颜色
        new_triangle.setColor(0, 148, 121.0, 92.0);
        new_triangle.setColor(1, 148, 121.0, 92.0);
        new_triangle.setColor(2, 148, 121.0, 92.0);
        
        draw_triangle(new_triangle);
    }
}


void Rasterizer::draw_triangle(const Triangle & triangle) {
    
}
