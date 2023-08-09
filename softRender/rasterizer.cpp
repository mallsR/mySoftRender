//
//  rasterizer.cpp
//  softRender
//
//  Created by xiaoR on 2023/8/5.
//

#include "rasterizer.hpp"

// 定义光源的位置与强度
struct light{
    Eigen::Vector3f position;
    Eigen::Vector3f intensity;
};

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

// 获取元素的函数
std::vector<Eigen::Vector3f> Rasterizer::get_frame_buffer() {
    return frame_buffer;
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
        
//        第二部分：获取viewspace下的顶点位置，因为fragment shader是在 view space下进行着色的。
//        ---------------------------------------------------------------------------
        //                片元位置要换为3维的，方便插值
        Eigen::Vector3f view_vertex[3];
        for (int i = 0; i < 3; ++i) {
            view_vertex[i] = (view * model * triangle->v[i]).head<3>();
        }
        
//        第三部分：对法线进行变换
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
        
//        第四部分：将得到的三角形顶点和法线传入draw_triangle函数，进行三角形的绘制
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
        
        
        draw_triangle(new_triangle, view_vertex);
    }
}


void Rasterizer::draw_triangle(const Triangle & triangle, Eigen::Vector3f (&view_vertex)[3]) {
    //    第一部分：计算包围盒
    //    ----------------------------
    int left_border = std::min(std::min(triangle.v[0].x(), triangle.v[1].x()), triangle.v[2].x());
    int right_border = std::max(std::max(triangle.v[0].x(), triangle.v[1].x()), triangle.v[2].x());
    int bottom_border = std::min(std::min(triangle.v[0].y(), triangle.v[1].y()), triangle.v[2].y());
    int top_border = std::max(std::max(triangle.v[0].y(), triangle.v[1].y()), triangle.v[2].y());
//    std::cout << "box " <<std::endl;
//    std::cout << "left_border : " << left_border << std::endl;
//    std::cout << "right_border : " << right_border << std::endl;
//    std::cout << "bottom_border : " << bottom_border << std::endl;
//    std::cout << "top_border : " << top_border << std::endl;
    
//    第二部分：遍历包围盒，逐像素操作
//    ------------------------------
    Eigen::Vector4f screen_vertex[] = {
        Eigen::Vector4f(triangle.v[0].x(), triangle.v[0].y(), triangle.v[0].z(), 1.0),
        Eigen::Vector4f(triangle.v[1].x(), triangle.v[1].y(), triangle.v[1].z(), 1.0),
        Eigen::Vector4f(triangle.v[2].x(), triangle.v[2].y(), triangle.v[2].z(), 1.0),
    };
    for(int x = left_border; x <= right_border; ++x) {
        for (int y = bottom_border; y <= top_border; ++y) {
//            1. 利用three cross product判断是否在三角形内部
            bool success = pixel_in_triangle(screen_vertex, std::make_pair(x + 0.5, y + 0.5));
//            不在三角形内部则跳过
            if (!success) {
                continue;
            }
//            2. 重心坐标，计算alpha, beta, grama，用于后续插值
            auto[alpha, beta, gamma] = compute_barycentric_coordinates(screen_vertex, std::make_pair(x + 0.5, y + 0.5));
            
//            3. 计算当前像素的深度，进行深度缓冲
            float Z = 1.0 / (alpha / screen_vertex[0].w() + beta / screen_vertex[1].w() + gamma / screen_vertex[2].w());
            float zp = alpha * screen_vertex[0].z() / screen_vertex[0].w() + beta * screen_vertex[1].z() / screen_vertex[1].w() + gamma * screen_vertex[2].z() / screen_vertex[2].w();
            zp *= Z;
            
//            获取深度缓冲中当前位置的深度
            int curr_index = (height - y) * width + x;
//            更新深度缓冲
            if (curr_index < width * height && zp < z_buffer[curr_index]) {
                z_buffer[curr_index] = zp;
                
//                第三部分：插值属性 ： 之前已经对顶点和法向量做出变换
//                1. 颜色
                auto interpolate_color = interpolate(alpha, beta, gamma, triangle.color[0], triangle.color[1], triangle.color[2], 1.0);
//                2. 法线
                auto interpolate_normal = interpolate(alpha, beta, gamma, triangle.normal[0], triangle.normal[1], triangle.normal[2], 1.0);
//                3. 纹理坐标
                auto interpolate_texcoord = interpolate(alpha, beta, gamma, triangle.tex_coords[0], triangle.tex_coords[1], triangle.tex_coords[2], 1.0);
//                4. 着色片元的位置
                auto interpolate_fragcoord = interpolate(alpha, beta, gamma, view_vertex[0], view_vertex[1], view_vertex[2], 1.0);
                
//                着色部分
                auto frag_color = phong_fragment_shader(interpolate_color, interpolate_normal, interpolate_texcoord, interpolate_fragcoord);
                    
//                设置frame_buff的颜色
                int idx = (height - y) * width + x;
                frame_buffer[idx] = frag_color;
            }
        }
    }
}

bool Rasterizer::pixel_in_triangle(const Eigen::Vector4f (&screen_vertex)[3], std::pair<float, float> pos) {
    Eigen::Vector3f curr_pos(pos.first, pos.second, 1.0);
//    取x,y构建vertex
    Eigen::Vector3f vertex[3];
    for (int i = 0; i < 3; ++i) {
        vertex[i] = {screen_vertex[i].x(), screen_vertex[i].y(), 1.0};
    }

//    利用three cross product计算结果
    Eigen::Vector3f cross1 = (vertex[1] - vertex[0]).cross(curr_pos - vertex[0]);
    Eigen::Vector3f cross2 = (vertex[2] - vertex[1]).cross(curr_pos - vertex[1]);
    Eigen::Vector3f cross3 = (vertex[0] - vertex[2]).cross(curr_pos - vertex[2]);
    
//    打印输出
//    std::cout << "vertexs : " << std::endl;
//    std::cout << vertex[0] << std::endl;
//    std::cout << vertex[1] << std::endl;
//    std::cout << vertex[2] << std::endl;
//    std::cout << "curr_pos : " << std::endl;
//    std::cout << curr_pos << std::endl;
//
//    std::cout << "cross product : " << std::endl;
//    std::cout << cross1 << std::endl;
//    std::cout << cross2 << std::endl;
//    std::cout << cross3 << std::endl;
//    判断三个cross结果的第3维度符号是否相同
    if ((cross1.z() > 0 && cross2.z() > 0 && cross3.z()) || (cross1.z() < 0 && cross2.z() < 0 && cross3.z() < 0)) {
//        std::cout << "inside triangle." << std::endl;
        return true;
    } else {
//        std::cout << "outside triangle." << std::endl;
    }
    
    return false;
}

// 重心坐标
std::tuple<float, float, float> compute_barycentric_coordinates(const Eigen::Vector4f (&v)[3], std::pair<float, float> pos) {
    float x = pos.first, y = pos.second;
    float alpha = (x * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*y + v[1].x()*v[2].y() - v[2].x()*v[1].y()) / (v[0].x()*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*v[0].y() + v[1].x()*v[2].y() - v[2].x()*v[1].y());
    float beta = (x*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*y + v[2].x()*v[0].y() - v[0].x()*v[2].y()) / (v[1].x()*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*v[1].y() + v[2].x()*v[0].y() - v[0].x()*v[2].y());
    float gamma = (x*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*y + v[0].x()*v[1].y() - v[1].x()*v[0].y()) / (v[2].x()*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*v[2].y() + v[0].x()*v[1].y() - v[1].x()*v[0].y());
    return {alpha, beta, gamma};
}


// 插值属性
Eigen::Vector3f Rasterizer::interpolate(float alpha, float beta, float gamma, const Eigen::Vector3f& vertex1, const Eigen::Vector3f& vertex2, const Eigen::Vector3f& vertex3, float weight)
{
    return (alpha * vertex1 + beta * vertex2 + gamma * vertex3) / weight;
}

// blinn-phong 着色模型
Eigen::Vector3f Rasterizer::phong_fragment_shader(Eigen::Vector3f color, Eigen::Vector3f normal, Eigen::Vector2f texcoord, Eigen::Vector3f frag_pos) {
    Eigen::Vector3f frag_color = {0, 0, 0};
    
//    第一部分：光源、光照系数的设置
//    ---------------------------------
    auto first_light = light{{20, 20, 20}, {500, 500, 500}};
    auto second_light = light{{-20, 20, 0}, {500, 500, 500}};

//    ka: Ambient lighting
//    kd: Diffuse reflection
//    ks: Specular highlights
    Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    Eigen::Vector3f kd = color;
    Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);
    
//     只用于计算高光
    Eigen::Vector3f eye_pos{0, 0, 10};
    float p = 150;
//     环境光的能量
    Eigen::Vector3f ambient_light_intensity{10, 10, 10};
    
    std::vector<light> lights = {first_light, second_light};
    
    for (auto light : lights) {
//        光照的方向 : 光照方向是从物体着色点指向灯光
        Eigen::Vector3f light_direct = light.position - frag_pos;
        
//        视线的方向 : 视线方向是从物体指向人眼
        Eigen::Vector3f view_direct = eye_pos - frag_pos;
        
//        距离的平方
        float pow_distance = light_direct.dot(light_direct);
        
//        第二部分：环境光照
//        ---------------------------------
        auto La = ka.cwiseProduct(ambient_light_intensity);
        
//        第三部分：漫反射
//        ----------------------------------
        auto Ld = kd.cwiseProduct(light.intensity / pow_distance) * fmax(0.0f, normal.normalized().dot(light_direct.normalized()));
        
//        第四部分：高光
//        ----------------------------------
//        半程向量
        Eigen::Vector3f h = (light_direct.normalized() + view_direct.normalized()).normalized();
        auto Ls = ks.cwiseProduct(light.intensity / pow_distance) * pow(fmax(0.0f, normal.normalized().dot(h.normalized())), p);
        
//        整合结果
        frag_color += La + Ld + Ls;
    }
    return frag_color;
}
