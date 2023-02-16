//
// Created by Shir Shitrit on 16/02/2023.
//

#ifndef ENGINEREWORK_ANIMATIONVISITOR_H
#define ENGINEREWORK_ANIMATIONVISITOR_H
#include "Visitor.h"

namespace cg3d {


    class AnimationVisitor : public Visitor {
    public:
        void Run(Scene* scene, Camera* camera) override;
        void Visit(Model* model) override;
    private:
        Scene* scene;


    };


#endif //ENGINEREWORK_ANIMATIONVISITOR_H
}