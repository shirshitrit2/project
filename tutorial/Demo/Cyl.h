//
// Created by Shir Shitrit on 22/02/2023.
//

#ifndef ENGINEREWORK_CYL_H
#define ENGINEREWORK_CYL_H
#include <memory>
#include "Model.h"
#include <queue>


class Cyl {
public:
    explicit Cyl(const std::shared_ptr<cg3d::Model> &cyl);

     Eigen::Vector3f getTranslation() ;
     Eigen::Vector2f getRotation() ;
    Eigen::Matrix4f getTrransforms();

    void setTranslation(Eigen::Vector3f translation);
    void setRotation(Eigen::Vector2f rotation);
    void setTransform(Eigen::Matrix4f trans);
    bool isRotationEmpty();
    bool isTranslationEmpty();
    bool isTransformEmpty();
    const std::shared_ptr<cg3d::Model> &getCyl() const;

        private:
            std::shared_ptr<cg3d::Model> cyl;

        public:
    std::queue<Eigen::Vector3f> Translations;
    std::queue<Eigen::Vector2f> Rotations;
    std::queue<Eigen::Matrix4f> Transforms;




};


#endif //ENGINEREWORK_CYL_H
