//
// Created by Shir Shitrit on 16/02/2023.
//

#include "AnimationVisitor.h"
#include "Visitor.h"
#include "DrawVisitor.h"
#include "Visitor.h"
#include "Scene.h"
#include "Movable.h"
#include "DebugHacks.h"

namespace cg3d
{

    void AnimationVisitor::Run(Scene* _scene, Camera* camera)
    {
        Visitor::Run(scene = _scene, camera);
    }

    void AnimationVisitor::Visit(Model* model) {
        Eigen::Matrix3f system= model->GetRotation();
        if(scene->IsActive()){
            if(model->name == std::string("first") ){
//                model->TranslateInSystem(system, Eigen::Vector3f(-0.1f*scene->speedFactor,0,0));

            }
        }


    }


}