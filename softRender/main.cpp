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
    std::string output_path = WORD_DIR + "output/blinn_phong.png";
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
    
    int esc_flag = 0;
    
//    光栅化器
    Rasterizer rasterizer(700, 700);
    
//    render loop
//    ------------------------------
//    "ESC"的ascii值为27， 即按esc则退出渲染
    while (esc_flag != 27) {
//        清空屏幕
        rasterizer.clear(color_clear, depth_color);
        
//        设置光栅化器的mvp matrix         
        rasterizer.set_model(get_model_matrix(angle_ry));
        rasterizer.set_view(get_view_matrix(camera_pos));
        rasterizer.set_projection(get_projection_matrix(45.0, 1, 0.1, 50.0));
        std::cout << "set mvp matrix successfully." << std::endl;
        
//        绘制三角形，将结果保存在rasterizer的frame_buffer和z_buffer : 核心步骤
        rasterizer.draw(model_triangles);
        
//        后续是利用opencv将frame_buffer的数据绘制到的显示框中
//        CV_<bit_depth>(S|U|F)C<number_of_channels>
//        1–bit_depth—比特数—代表8bite,16bites,32bites,64bites—举个例子吧–比如说,如
//        如果你现在创建了一个存储–灰度图片的Mat对象,这个图像的大小为宽100,高100,那么,现在这张
//        灰度图片中有10000个像素点，它每一个像素点在内存空间所占的空间大小是8bite,8位–所以它对
//        应的就是CV_8
//        2–S|U|F–S--代表—signed int—有符号整形
//        U–代表–unsigned int–无符号整形
//        F–代表–float---------单精度浮点型
//        3–C<number_of_channels>----代表—一张图片的通道数,比如:
//        1–灰度图片–grayImg—是–单通道图像
//        2–RGB彩色图像---------是–3通道图像
//        3–带Alph通道的RGB图像–是--4通道图像
        cv::Mat image(700, 700, CV_32FC3, rasterizer.get_frame_buffer().data());
//        函数原型
//        void convertTo(OutputArray dst, int rtype, double alpha, double beta);
//        第一个参数是存储转化后的图像
//        第二个参数是转化的数据类型，如果为-1，则类型与原图像类型一致
//        第三个参数是可选的比例因子，用于调整目标图像的像素值大小，默认值为1.0
//        第四个参数是添加到缩放值的可选增量，默认值为0。
        image.convertTo(image, CV_8UC3, 1.0f);
//        cvtcolor()函数是一个颜色空间转换函数，可以实现RGB颜色向HSV，HSI等颜色空间转换。也可以转换为灰度图
//        此处将RGB转换为BGR
        cv::cvtColor(image, image, cv::COLOR_RGB2BGR);

        cv::imshow("image", image);
        cv::imwrite(output_path, image);
        esc_flag = cv::waitKey(10);

        if (esc_flag == 'a' ) {
            angle_ry -= 0.1;
        } else if (esc_flag == 'd') {
            angle_ry += 0.1;
        }
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
//    这里最后一行第3位为1，目的在于，保存z值到w，原本距离越远，w就越小。后续用其他值除以w来计算深度值
    
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
