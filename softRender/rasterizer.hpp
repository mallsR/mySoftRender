//
//  rasterizer.hpp
//  softRender
//
//  Created by xiaoR on 2023/8/5.
//

#ifndef rasterizer_hpp
#define rasterizer_hpp

#include <eigen3/Eigen/Eigen>
#include <climits>

#include "Triangle.hpp"

// 光栅化器
// 用于清空屏幕，
// 几何阶段：顶点着色器、曲面细分着色器，几何着色器（包含MVP）、剪裁、屏幕映射；
// 光栅化阶段：三角形设置、三角形设置、片元着色器、逐片元操作
//--------------------------------------------------------------
class Rasterizer{
public:
    Rasterizer();
    Rasterizer(int width, int height);

//    清空屏幕
    void clear(bool color_clear, bool depth_color);
    
//    设置mvp
    void set_model(Eigen::Matrix4f model);
    void set_view(Eigen::Matrix4f view);
    void set_projection(Eigen::Matrix4f projection);
    
//    获取元素
    std::vector<Eigen::Vector3f> get_frame_buffer();
    
//    绘制模型 ：通过遍历三角形进行绘制，进而绘制模型
    void draw(std::vector<Triangle *> model_triangles);
//    重心坐标绘制三角形
    void draw_triangle(const Triangle & triangle, Eigen::Vector3f (&view_vertex)[3]);
    
//    判断是否在三角形内部
    bool pixel_in_triangle(const Eigen::Vector4f (&screen_vertex)[3], std::pair<float, float> pos);
//    计算重心坐标，并保存在tuple（c++11）中
    std::tuple<float, float, float> compute_barycentric_coordinates(const Eigen::Vector4f (&v)[3], std::pair<float, float> pos);
//    属性插值函数
    Eigen::Vector3f interpolate(float alpha, float beta, float gamma, const Eigen::Vector3f& vertex1, const Eigen::Vector3f& vertex2, const Eigen::Vector3f& vertex3, float weight);
    
//    着色 : 默认采用blinn_phong着色模型
    Eigen::Vector3f phong_fragment_shader(Eigen::Vector3f color, Eigen::Vector3f normal, Eigen::Vector2f texcoord, Eigen::Vector3f frag_pos);
    
private:
//    屏幕宽高
    int width, height;
    
//  model, view, projection matrix， 用于mvp变换
    Eigen::Matrix4f model;
    Eigen::Matrix4f view;
    Eigen::Matrix4f projection;
    
//    保存深度和颜色，用于fragment shader
    std::vector<Eigen::Vector3f> frame_buffer;
    std::vector<float> z_buffer;
    
};

#endif /* rasterizer_hpp */
