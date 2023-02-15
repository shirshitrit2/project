//
// Created by Chen on 15/02/2023.
//

#include "Fruit.h"
#include "igl/AABB.h"

#include "Model.h"
Fruit::Fruit(const std::shared_ptr<cg3d::Model> &model, const std::string &color) : model(model), color(color) {
    if(color=="blue"){
        this->velocity = findVelocity(0.1);
    }
    if(color=="yellow"){
        this->velocity = findVelocity(0.1);
    }
    initTree();

}

void Fruit::initTree(){
    auto mesh = model->GetMeshList();
    Eigen::MatrixXd V1 = mesh[0]->data[0].vertices;
    Eigen::MatrixXi F1 = mesh[0]->data[0].faces;
    this->tree.init(V1,F1);
}

Eigen::Vector3f Fruit::findVelocity(float factor){

    float Vx1 = (rand() % range + offset);
    float Vz1 = (rand() % range + offset);
    Eigen::Vector3f Velocity = Eigen::Vector3f(Vx1,0,Vz1);
    Velocity.normalize();
    Velocity=Eigen::Vector3f(Velocity.x()*factor,0,Velocity.z()*factor);
    return Velocity;
}

const std::shared_ptr<cg3d::Model> &Fruit::getModel() const {
    return model;
}

const Eigen::Vector3f &Fruit::getVelocity() const {
    return velocity;
}

void Fruit::setVelocity(const Eigen::Vector3f &velocity) {
    Fruit::velocity = velocity;
}

const igl::AABB<Eigen::MatrixXd, 3> &Fruit::getTree() const {
    return tree;
}

const std::string &Fruit::getColor() const {
    return color;
}
