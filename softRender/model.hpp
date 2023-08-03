//
//  model.hpp
//  softRender
//
//  Created by xiaoR on 2023/8/3.
//

#ifndef model_hpp
#define model_hpp

#pragma once

#include <iostream>
#include <stdio.h>

#include <eigen3/Eigen/Eigen>

#include <string>
#include <vector>

using namespace std;

// 定义顶点，坐标，法线和纹理
struct Vertex{
    Vertex(){
//        position = {0.0f, 0.0f, 0.0f};
//        normal = {0.0f, 0.0f, 0.0f};
//        texture = {0.0f, 0.0f, 0.0f};
    }
    Eigen::Vector3f position;
    Eigen::Vector3f normal;
    Eigen::Vector2f texture;
};

// 定义材质
struct Material{
    Material(){
//        Ka = {0.0f, 0.0f, 0.0f};
//        Kd = {0.0f, 0.0f, 0.0f};
//        Ks = {0.0f, 0.0f, 0.0f};
    }
//    ambient coefficient
    Eigen::Vector3f Ka;
//    diffuse coefficient
    Eigen::Vector3f Kd;
//    specular coefficient
    Eigen::Vector3f Ks;
};

// 定义模型部位
struct Mesh{
    Mesh(){
        mesh_name = "";
    }
//    mesh name
    string mesh_name;
//    vertex
    vector<Vertex> vertices;
//    indices
    vector<unsigned int> indices;
};

class Model{
public:
    vector<Mesh> model_meshes;
    vector<Vertex> model_vertices;
    vector<Material> model_materials;
    vector<unsigned int> model_indices;
};

#endif /* model_hpp */
