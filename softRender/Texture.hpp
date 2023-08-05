//
//  Texture.hpp
//  softRender
//
//  Created by xiaoR on 2023/8/5.
//

#ifndef Texture_hpp
#define Texture_hpp

#include <eigen3/Eigen/Eigen>
#include <opencv2/opencv.hpp>
class Texture{
private:
    cv::Mat image_data; /* Mat 类存储图像 */

public:
    Texture(const std::string& name)
    {
//        加载图像
        image_data = cv::imread(name);
//        颜色空间转换为BGR
        cv::cvtColor(image_data, image_data, cv::COLOR_RGB2BGR);
        width = image_data.cols;
        height = image_data.rows;
    }

    int width, height;

//    获取纹理的 BGR 颜色
    Eigen::Vector3f getColor(float u, float v)
    {
        auto u_img = u * width;
        auto v_img = (1 - v) * height;
//        Mat类中的at方法对于获取图像矩阵某点的RGB值，坐标形式为(y, x)
//        cv::Vec3b: 类，用于表示包含三个 uchar 类型元素的向量。
        auto color = image_data.at<cv::Vec3b>(v_img, u_img);
        return Eigen::Vector3f(color[0], color[1], color[2]);
    }

};

#endif /* Texture_hpp */

