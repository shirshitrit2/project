//
// Created by Shir Shitrit on 27/02/2023.
//

#include "SkinnedSnake.h"
#include <igl/directed_edge_orientations.h>
#include <igl/directed_edge_parents.h>
#include <igl/forward_kinematics.h>
#include <igl/PI.h>
#include <igl/lbs_matrix.h>
#include <igl/deform_skeleton.h>
#include <igl/dqs.h>
#include <igl/readDMAT.h>
#include <igl/readOBJ.h>
#include <igl/readTGF.h>
#include <igl/opengl/glfw/Viewer.h>

#include <Eigen/Geometry>
#include <Eigen/StdVector>
#include <vector>
#include <algorithm>
#include <iostream>
#include <Eigen/src/Geometry/Transform.h>

SkinnedSnake::SkinnedSnake(std::shared_ptr<cg3d::Model> model, std::vector<Cyl> &cyls){

    this->model=model;
    this->cyls=cyls;

    firstdMeshList = model->GetMesh();
    latestMeshList= model->GetMesh();

    BE.resize(cyls.size(), 2);
    C.resize(cyls.size() + 1, 3);
    CT.resize(cyls.size() * 2, 3);
    vC.resize(cyls.size() + 1);
    vT.resize(cyls.size() + 1);
    vQ.resize(cyls.size() + 1, Eigen::Quaterniond::Identity());

    calculateW();
//    Eigen::Vector3d minV = model->GetMesh()->data[0].vertices.colwise().minCoeff();
//    Eigen::Vector3d maxV = model->GetMesh()->data[0].vertices.colwise().maxCoeff();

    ///// every joint is connected to the next one
    for(int i=0; i<cyls.size(); i++) {
        BE(i, 0) = i;
        BE(i, 1) = i+1;
    }

    Eigen::Vector3d pos(0, 0, 0);
    for (int i = 0; i < cyls.size() + 1; i++) {
        C.row(i) = pos;
        vT[i]=pos;
        pos += Eigen::Vector3d(0.8, 0, 0);
    }

}


void SkinnedSnake::calculateW(){
    int vertices = model->GetMesh()->data[0].vertices.rows();
    W.resize(vertices, cyls.size()+1);
    W.setZero();
    Eigen::Vector3d minV = model->GetMesh()->data[0].vertices.colwise().minCoeff();
    for (int i=0; i<cyls.size()+1; i++) {
        for(int j = 0; j < vertices; j++) {
            double curr = model->GetMesh()->data[0].vertices.row(j).z();
            if (curr >= minV.z() + 0.8 * i && curr <=  minV.z() + 0.8 * (i + 1)) {
                auto weight = abs(curr - (minV.z() + 0.8 * (i+1)));
                W(j, i) = weight;
                W(j, i+1) = 1 - weight;
                break;
            }
        }
    }
}


void SkinnedSnake::applySkin(){

    const int dim = C.cols();
    Eigen::MatrixXd T(BE.rows()*(dim+1),dim);
    for (int i = 0; i < cyls.size(); i++) {
        vT[i] = cyls[i].getCyl()->GetTranslation().cast<double>();
    }

    for(int e = 0;e<BE.rows();e++)
    {
        Eigen::Affine3d a = Eigen::Affine3d::Identity();
        a.translate(vT[e]);
        a.rotate(vQ[e]);
        T.block(e*(dim+1),0,dim+1,dim) =
                a.matrix().transpose().block(0,0,dim+1,dim);
    }

//    igl::dqs(latestMeshList->data[0].vertices,W,vQ,vT,U);

    igl::deform_skeleton(C,BE,T,CT,BET);
    Eigen::MatrixXd textureCoords = Eigen::MatrixXd::Zero(U.rows(),2);
    std::vector<cg3d::MeshData> nextLatestMeshList= {{U, firstdMeshList->data[0].faces, firstdMeshList->data[0].vertexNormals, textureCoords}};

    latestMeshList = {std::make_shared<Mesh>("latest mesh", nextLatestMeshList)};

}
