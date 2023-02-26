#include <GL.h>
#include "DrawVisitor.h"
#include "Visitor.h"
#include "Scene.h"
#include "Movable.h"
#include "DebugHacks.h"


namespace cg3d
{
void DrawVisitor::Run(Scene* _scene, Camera* camera)
{
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glEnable(GL_DEPTH_TEST);

    if (drawOutline) {
        glEnable(GL_STENCIL_TEST);
        glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
    }

    Visitor::Run(scene = _scene, camera);
}

void DrawVisitor::Init()
{
    // clear and set up the depth and color buffers (and the stencil buffer if outline is enabled)
    unsigned int flags = GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT;
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    
    glClearColor(0, 0, 0, 0);

    // clear and set up the stencil buffer if outline is enabled
    if (drawOutline) {
        glStencilMask(0xFF);
        glClearStencil(0);
        flags |= GL_STENCIL_BUFFER_BIT;
    }

    glClear(flags);
}

void DrawVisitor::Visit(Scene* _scene)
{
    Visitor::Visit(_scene); // draw children first

    if (_scene->pickedModel && drawOutline)
        DrawOutline();
}

void DrawVisitor::Visit(Model* model)
{
    Visitor::Visit(model); // draw children first

    if (!model->isHidden) {
        Eigen::Matrix4f modelTransform = model->isStatic ? model->GetAggregatedTransform() : norm * model->GetAggregatedTransform();
        const Program* program = model->material->BindProgram();
        scene->Update(*program, proj, view, modelTransform);
        program->SetUniform4f("lightColor",  model->lightColor.x(),model->lightColor.y(),model->lightColor.z(),model->lightColor.w());
        program->SetUniform4f("Kai",  model->ambient.x(),model->ambient.y(),model->ambient.z(),model->ambient.w());
        program->SetUniform4f("Kdi",  model->diffuse.x(),model->diffuse.y(),model->diffuse.z(),model->diffuse.w());
        program->SetUniform4f("Ksi",  model->specular.x(),model->specular.y(),model->specular.z(),model->specular.w());
        program->SetUniform1f("specular_exponent", 5.0f);
        program->SetUniform4f("light_position", -50.0, -50.0f, 0.0f, 1.0f);
        // glEnable(GL_LINE_SMOOTH);
        glLineWidth(model->lineWidth);

        // enable writing to the stencil only when we draw the picked model (and we want to draw an outline)
        glStencilMask(drawOutline && scene->pickedModel && model == scene->pickedModel.get() ? 0xFF : 0x0);

        model->material->BindProgram(); // call BindProgram() again to rebind the textures because igl bind_mesh messes them up
        model->UpdateDataAndDrawMeshes(*program, model->showFaces, model->showTextures);

        if (model->showWireframe) {
            program = model->material->BindFixedColorProgram();
            scene->Update(*program, proj, view, modelTransform);
            program->SetUniform4fv("fixedColor", 1, &model->wireframeColor);
            model->UpdateDataAndDrawMeshes(*program, false, false);
        }
    }
}

void DrawVisitor::DrawOutline()
{
    auto& model = scene->pickedModel;
    auto program = model->material->BindFixedColorProgram();
    scene->Update(*program, proj, view, model->isStatic ? model->GetAggregatedTransform() : norm * model->GetAggregatedTransform());
    program->SetUniform4fv("fixedColor", 1, &outlineLineColor);

    // draw the picked model with thick lines only where previously the stencil wasn't touched (i.e. around the original model)
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glStencilMask(0x0);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
   glLineWidth(outlineLineWidth);
    model->UpdateDataAndDrawMeshes(*program, false, false);
}

} // namespace cg3d
