//
// Created by Shir Shitrit on 22/02/2023.
//

#include "Cyl.h"

Cyl::Cyl(const std::shared_ptr<cg3d::Model> &cyl) : cyl(cyl) {}

 Eigen::Vector3f Cyl::getTranslation()  {
    Eigen::Vector3f curr= Translations.front();
    Translations.pop();
    return curr;
}

 Eigen::Vector2f Cyl::getRotation()  {
    Eigen::Vector2f curr= Rotations.front();
    Rotations.pop();
    return curr;
}

Eigen::Matrix4f Cyl::getTrransforms(){
    Eigen::Matrix4f curr= Transforms.front();
    Transforms.pop();
    return curr;
}


void Cyl::setTranslation(Eigen::Vector3f translation) {
    Translations.push(translation);
}

void Cyl::setRotation(Eigen::Vector2f rotation) {
    Rotations.push(rotation);
}

void Cyl::setTransform(Eigen::Matrix4f trans){
    Transforms.push(trans);
}


const std::shared_ptr<cg3d::Model> &Cyl::getCyl() const {
    return cyl;
}

bool Cyl::isRotationEmpty(){
    return Rotations.empty();
}
bool Cyl::isTranslationEmpty(){
    return Translations.empty();
}

bool Cyl::isTransformEmpty(){
    return  Transforms.empty();
}
