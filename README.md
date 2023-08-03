#  SoftRender

which is soft render
游戏引擎可以理解成一个功能异常强大复杂发软渲染器，软渲染器就是指在尽量不依赖第三方库的条件下，利用代码创建窗口，渲染出一个3d空间里的各种物体。

Target
functions:
绘制基本元素，线，面，立方体等
正交透视相机（投影）
纹理映射
读取一种模型文件（如obj）
读取一种贴图文件（如bmp）
基本光照计算着色模型，漫反射模型和镜面反射模型，包括不同类型光源，如点光源和直射光
模拟实现顶点着色器和片源着色器
利用shadow map实现阴影绘制

Related knowledge
渲染管线流程
画线算法
画三角形算法
画3D模型
正交矩阵和透视矩阵
纹理映射与透视矫正
光照模型（漫反射和镜面反射）
shadow map阴影算法

Process:
基础类：
Model类，用于加载模型
Vector向量类，用于描述位置，法线等
矩阵类，进行矩阵运算
Transform类，用于平移，旋转，缩放变换
Vertex类，读取模型数据基本类，包含顶点，法线，uv等信息
Mesh类，Vertex数组集合
Color类，描述颜色结构
Texture类，读取纹理贴图数据
Light类，光
Camera类，相机
Buffer类，用于创建各种缓冲，ZBuffer等
Shader类，决定模型属性（颜色，贴图，光照，阴影等）
Window类，创建窗口
Renderer类，渲染窗口

Basic library:
1. opencv，用于绘制窗口
2. eigen，用于方便处理向量和矩阵的运算

Step:
1. 借助opencv库绘制窗口
    brew 安装 opencv2: 添加Header search path和Library search path
2. 



Note:
涉及较多传递值和地址，边界情况考虑
