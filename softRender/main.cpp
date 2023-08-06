//
//  main.cpp
//  softRender
//
//  Created by xiaoR on 2023/8/2.
//

#include <iostream>

#include <opencv2/opencv.hpp>

//#include "model.hpp"
#include "Triangle.hpp"
#include "OBJ_Loader.hpp"
#include "rasterizer.hpp"

const float PI = 3.1415926;

//函数定义区域
//------------------
// 通过角度计算model matrix，寻求简单起见，旋转围绕y轴进行旋转
Eigen::Matrix4f get_model_matrix(float angle);
// 计算view matrix，移动camera，相当于反方向移动场景
Eigen::Matrix4f get_view_matrix(Eigen::Vector3f camera_pos);
// 计算projection matrix
// eye_fov：摄影机视角，aspect_ratio：宽高比，z_far/z_near : 远近平面的距离
Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio, float z_near, float z_far);


int main(int argc, const char * argv[]) {
//    加载模型
//    -------------------
    std::string WORD_DIR = "/Users/xiaor/Project/xCode/softRender/softRender/";
    std::string obj_path = WORD_DIR + "models/spot/spot_triangulated_good.obj";
    objl::Loader model;
    bool model_load_flag = model.LoadFile(obj_path);
    if(!model_load_flag) {
        std::cout << "load model failed." << std::endl;
        exit(1);
    }
    
    std::vector<Triangle * > model_triangles;
//    借助mesh读取所有三角形顶点，法线和纹理，后续依据三角形进行渲染
    for(auto mesh : model.LoadedMeshes)
    {
        for(int i = 0; i < mesh.Vertices.size(); i += 3)
        {
//            三角形对象存储的数据包括顶点、法线和纹理坐标
            Triangle * t = new Triangle();
            for(int j = 0;j < 3; ++j)
            {
//                奇次坐标存储vertex
                t->setVertex(j, Vector4f(mesh.Vertices[i+j].Position.X, mesh.Vertices[i+j].Position.Y, mesh.Vertices[i+j].Position.Z, 1.0));
                t->setNormal(j,  Vector3f(mesh.Vertices[i+j].Normal.X, mesh.Vertices[i+j].Normal.Y, mesh.Vertices[i+j].Normal.Z));
                t->setTexCoord(j,Vector2f(mesh.Vertices[i+j].TextureCoordinate.X, mesh.Vertices[i+j].TextureCoordinate.Y));
            }
            model_triangles.push_back(t);
        }
    }
    
//    读取模型三角形成功
    std::cout << "load triangles successfully." << std::endl;
    
    
//    绘图设置
//    --------------------------
//    定义摄影机位置 ： 采用左手坐标系定义
    Eigen::Vector3f camera_pos = {0, 0, 10};
//    默认清空颜色和深度缓冲
    bool color_clear = true, depth_color = true;
//    默认围绕y轴旋转的角度
    float angle_ry = 20.0;
    
    int esc = 0;
    
//    光栅化器
    Rasterizer rasterizer(700, 700);
    
//    render loop
//    ------------------------------
//    "ESC"的ascii值为27， 即按esc则退出渲染
    while (esc != 27) {
//        清空屏幕
        rasterizer.clear(color_clear, depth_color);
        
//        设置光栅化器的mvp matrix         
        rasterizer.set_model(get_model_matrix(angle_ry));
        rasterizer.set_view(get_view_matrix(camera_pos));
        rasterizer.set_projection(get_projection_matrix(45.0, 1, 0.1, 50.0));
        std::cout << "set mvp matrix successfully." << std::endl;
        
        break;
    }
    
    
    return 0;
}


// 函数具体实现区域
//--------------
Eigen::Matrix4f get_model_matrix(float angle) {
    angle = angle * PI / 180.0f;
    
    Eigen::Matrix4f rotation;
    rotation << cos(angle), 0, sin(angle), 0,
                0, 1, 0, 0,
                -sin(angle), 0, cos(angle), 0,
                0, 0, 0, 1;
    Eigen::Matrix4f scale;
    scale << 2.5, 0, 0, 0,
            0, 2.5, 0, 0,
            0, 0, 2.5, 0,
            0, 0, 0, 1;
    Eigen::Matrix4f translate;
    translate << 1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1;
    
    return translate * rotation * scale;
}


Eigen::Matrix4f get_view_matrix(Eigen::Vector3f camera_pos) {
    Eigen::Matrix4f view;

    view << 1,0,0,-camera_pos[0],
            0,1,0,-camera_pos[1],
            0,0,1,-camera_pos[2],
            0,0,0,1;
    
    return view;
}


Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio, float z_near, float z_far) {
//    透视投影矩阵
    Eigen::Matrix4f perspective;
    
    perspective << z_near, 0, 0, 0,
                0, z_near, 0, 0,
                0, 0, z_near + z_far, - z_near * z_far,
                0, 0, 1, 0;
    
//    正交投影 : 先平移，再缩放
    Eigen::Matrix4f orthographic;
    
//    计算上下左右边界
    float top_border = tan(eye_fov / 2) * z_near;
    float bottom_border = -top_border;
    float right_border = aspect_ratio * top_border;
    float left_border = -right_border;
    
//    右手坐标系计算正交投影矩阵
    Eigen::Matrix4f translate;
    translate << 1, 0, 0, - (left_border + right_border) / 2,
                0, 1, 0, - (top_border + bottom_border) / 2,
                0, 0, 1, - (z_near + z_far) / 2,
                0, 0, 0, 1;
    Eigen::Matrix4f scale;
    scale << 2.0 / (right_border - left_border) , 0, 0, 0,
                0, 2.0 / (top_border - bottom_border), 0, 0,
                0, 0, 2.0 / (z_near - z_far), 0,
                0, 0, 0, 1;
    orthographic = scale * translate;
    
//    最后的projection矩阵
    Eigen::Matrix4f projection = orthographic * perspective;
    
    return projection;
}
