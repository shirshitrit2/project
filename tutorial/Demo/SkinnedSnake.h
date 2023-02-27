//
// Created by Shir Shitrit on 27/02/2023.
//
#include "Model.h"
#include "Cyl.h"

#include <utility>
#ifndef ENGINEREWORK_SKINNEDSNAKE_H
#define ENGINEREWORK_SKINNEDSNAKE_H

using namespace cg3d;

class SkinnedSnake  {

public:
    SkinnedSnake(std::shared_ptr<cg3d::Model> cyl, std::vector<Cyl> &cyls);
    void applySkin();


private:
    /////functions
    void calculateW();
    ////skeleton
    Eigen::MatrixXd CT;
    Eigen::MatrixXi BET;
    /////rotation and translation of joints
    std::vector<Eigen::Quaterniond> vQ;
    std::vector<Eigen::Vector3d> vC;
    std::vector<Eigen::Vector3d> vT;

    Eigen::Quaterniond quat;
// W - weights matrix
// BE - Edges between joints
// C - joints positions
// P - parents
// M - weights per vertex per joint matrix
// U - new vertices position after skinning
    Eigen::MatrixXd W,C,U,M;
    Eigen::MatrixXi BE;
    Eigen::VectorXi P;
//    std::vector<RotationList > poses; // rotations of joints for animation
    double anim_t = 0.0;
    double anim_t_dir = 0.015;
    bool use_dqs = false;
    bool recompute = true;

    std::vector<Cyl> cyls;
    std::shared_ptr<cg3d::Model> model;
    std::shared_ptr<Mesh> latestMeshList;
    std::shared_ptr<Mesh> firstdMeshList;




};


#endif //ENGINEREWORK_SKINNEDSNAKE_H
