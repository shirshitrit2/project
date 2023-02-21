//
// Created by Chen on 15/02/2023.
//

#ifndef ENGINEREWORK_FRUIT_H
#define ENGINEREWORK_FRUIT_H
#include "igl/AABB.h"
#include "Model.h"

class Fruit {
private:
    std::shared_ptr<cg3d::Model> model;
    Eigen::Vector3f velocity;
    igl::AABB<Eigen::MatrixXd,3> tree;
    std::string color;
    int offset = -99;
    int range =  99 - offset +1;

public:
    Fruit(const std::shared_ptr<cg3d::Model> &model, const std::string &color);
    Fruit(const std::shared_ptr<cg3d::Model> &model, const std::string &color, const float velocityFactor);

    const std::shared_ptr<cg3d::Model> &getModel() const;

    const Eigen::Vector3f &getVelocity() const;

    void setVelocity(const Eigen::Vector3f &velocity);

    const igl::AABB<Eigen::MatrixXd, 3> &getTree() const;

    const std::string &getColor() const;


private:
    Eigen::Vector3f findVelocity(float factor);
    void initTree();
};

#endif //ENGINEREWORK_FRUIT_H
