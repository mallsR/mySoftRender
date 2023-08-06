#  SoftRender

## which is soft render
游戏引擎可以理解成一个功能异常强大复杂发软渲染器，软渲染器就是指在尽量不依赖第三方库的条件下，利用代码创建窗口，渲染出一个3d空间里的各种物体。
---

## Target
### functions:
读取一种模型文件（如obj）  
正交透视相机（投影）  
三角形遍历与属性设置  
模拟片元着色器（Blinn Phong着色模型）  
利用重心坐标插值三角形内部属性  
逐片元操作：设置颜色，深度值  
读取一种贴图文件（如bmp）  
纹理映射  
利用shadow map实现阴影绘制  
  
## Related knowledge
渲染管线流程    
画三角形算法（包围盒，逐像素判断，重心坐标）  
画3D模型  
正交矩阵和透视矩阵  
纹理映射与透视矫正  
光照模型（漫反射和镜面反射）  
shadow map阴影算法  
---

## Basic library:
1. opencv，用于读取图像，设置纹理
2. eigen，用于方便处理向量和矩阵的运算

## Step:
1. 借助opencv库读取图像，设置纹理  
    brew 安装 opencv2: 添加Header search path和Library search path  
2. class Texture  
    读取纹理贴图数据  
3. class Loader  
    加载模型顶点、法线和纹理
4. class Rasterizer
    . 几何阶段
        顶点着色器（mvp变换）
        曲面细分着色器
        几何着色器  
        剪裁  
        屏幕映射
    . 光栅化阶段
        三角形设置  
        三角形遍历  
        片元着色器  
        逐片元操作  
5. main函数
    . 加载模型  
    . 定义相机属性  
    . render loop  
        . 清空屏幕 ： 设置深度缓存以及fragment缓存  
        . 设置mvp matrix  

  
## Process:
基础类：  
Loader类,用于加载模型    
矩阵类，进行矩阵运算  
Texture类，读取纹理贴图数据  
Triangle类，用于保存三角形属性   
Rasterizer类，用于光栅化过程
Camera类，用于设置相机，更改视角
结构体：  
Vector 向量，读取模型数据基本类，包含顶点，法线，uv等信息  
Mesh，用于曲面细分，保存信息  

Transform类，用于平移，旋转，缩放变换      
Color类，描述颜色结构  
Light类，光  
Buffer类，用于创建各种缓冲，ZBuffer等  
Shader类，决定模型属性（颜色，贴图，光照，阴影等）  
Window类，创建窗口  
Renderer类，渲染窗口  
    

## Note:
1. 涉及较多传递值和地址，边界情况考虑  
