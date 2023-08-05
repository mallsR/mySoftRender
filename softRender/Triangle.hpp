//
//  Triangle.hpp
//  softRender
//
//  Created by xiaoR on 2023/8/5.
//

#ifndef Triangle_hpp
#define Triangle_hpp

#include <eigen3/Eigen/Eigen>
#include "Texture.hpp"

using namespace Eigen;
class Triangle{

public:
//    存储三角形三个顶点坐标
    Vector4f v[3];
//    以下为每个顶点的属性 ： 颜色、纹理和法线
//    颜色为RBG表示
    Vector3f color[3];
    Vector2f tex_coords[3];
    Vector3f normal[3];
    
//    定义Texture类的指针，用于后续访问纹理
    Texture *tex= nullptr;
    Triangle();

    Eigen::Vector4f a() const { return v[0]; }
    Eigen::Vector4f b() const { return v[1]; }
    Eigen::Vector4f c() const { return v[2]; }

    void setVertex(int ind, Vector4f ver); /* 设置第i个顶点 */
    void setNormal(int ind, Vector3f n);    /* 设置第i个法线 */
    void setColor(int ind, float r, float g, float b);  /* 设置第i个点的颜色 */

    void setNormals(const std::array<Vector3f, 3>& normals);
    void setColors(const std::array<Vector3f, 3>& colors);
    void setTexCoord(int ind,Vector2f uv );  /* 设置第i个点的纹理 */
    std::array<Vector4f, 3> toVector4() const;
};





#endif /* Triangle_hpp */
