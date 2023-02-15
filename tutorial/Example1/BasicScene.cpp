#include "BasicScene.h"
#include <Eigen/src/Core/Matrix.h>
#include <edges.h>
#include <memory>
#include <per_face_normals.h>
#include <read_triangle_mesh.h>
#include <utility>
#include <vector>
#include "GLFW/glfw3.h"
#include "Mesh.h"
#include "PickVisitor.h"
#include "Renderer.h"
#include "ObjLoader.h"
#include "IglMeshLoader.h"

#include "igl/per_vertex_normals.h"
#include "igl/per_face_normals.h"
#include "igl/unproject_onto_mesh.h"
#include "igl/edge_flaps.h"
#include "igl/loop.h"
#include "igl/upsample.h"
#include "igl/AABB.h"
#include "igl/parallel_for.h"
#include "igl/shortest_edge_and_midpoint.h"
#include "igl/circulation.h"
#include "igl/edge_midpoints.h"
#include "igl/collapse_edge.h"
#include "igl/edge_collapse_is_valid.h"
#include "igl/write_triangle_mesh.h"

// #include "AutoMorphingModel.h"

using namespace cg3d;
#define CYL_SIZE 1.6

void BasicScene::Init(float fov, int width, int height, float near, float far)
{
    camera = Camera::Create( "camera", fov, float(width) / height, near, far);
    
    AddChild(root = Movable::Create("root")); // a common (invisible) parent object for all the shapes
    auto daylight{std::make_shared<Material>("daylight", "shaders/cubemapShader")}; 
    daylight->AddTexture(0, "textures/cubemaps/Daylight Box_", 3);
    auto background{Model::Create("background", Mesh::Cube(), daylight)};
    AddChild(background);
    background->Scale(120, Axis::XYZ);
    background->SetPickable(false);
    background->SetStatic();

 
    auto program = std::make_shared<Program>("shaders/phongShader");
    auto program1 = std::make_shared<Program>("shaders/pickingShader");
    
    auto material{ std::make_shared<Material>("material", program)}; // empty material
    auto material1{ std::make_shared<Material>("material", program1)}; // empty material
//    SetNamedObject(cube, Model::Create, Mesh::Cube(), material, shared_from_this());
 
    material->AddTexture(0, "textures/box0.bmp", 2);
    auto sphereMesh{IglLoader::MeshFromFiles("sphere_igl", "data/sphere.obj")};
    auto cylMesh{IglLoader::MeshFromFiles("cyl_igl","data/xcylinder.obj")};
    //auto cubeMesh{IglLoader::MeshFromFiles("cube_igl","data/cube_old.obj")};
    sphere1 = Model::Create( "sphere",sphereMesh, material);    
    //cube = Model::Create( "cube", cubeMesh, material);
    
    //Axis
    Eigen::MatrixXd vertices(6,3);
    vertices << -1,0,0,1,0,0,0,-1,0,0,1,0,0,0,-1,0,0,1;
    Eigen::MatrixXi faces(3,2);
    faces << 0,1,2,3,4,5;
    Eigen::MatrixXd vertexNormals = Eigen::MatrixXd::Ones(6,3);
    Eigen::MatrixXd textureCoords = Eigen::MatrixXd::Ones(6,2);
    std::shared_ptr<Mesh> coordsys = std::make_shared<Mesh>("coordsys",vertices,faces,vertexNormals,textureCoords);
//    axis.push_back(Model::Create("axis",coordsys,material1));
//    axis[0]->mode = 1;
//    axis[0]->Scale(4,Axis::XYZ);
//     axis[0]->lineWidth = 5;
//    root->AddChild(axis[0]);
    float scaleFactor = 1; 
    cyls.push_back( Model::Create("cyl",cylMesh, material));
    cyls[0]->Scale(scaleFactor,Axis::X);
    cyls[0]->SetCenter(Eigen::Vector3f(-0.8f*scaleFactor,0,0));
    root->AddChild(cyls[0]);
   
    for(int i = 1;i < 3; i++)
    { 
        cyls.push_back( Model::Create("cyl", cylMesh, material));
        cyls[i]->Scale(scaleFactor,Axis::X);   
        cyls[i]->Translate(1.6f*scaleFactor,Axis::X);
        cyls[i]->SetCenter(Eigen::Vector3f(-0.8f*scaleFactor,0,0));
        cyls[i-1]->AddChild(cyls[i]);
    }
    cyls[0]->Translate({0.8f*scaleFactor,0,0});

    auto morphFunc = [](Model* model, cg3d::Visitor* visitor) {
      return model->meshIndex;//(model->GetMeshList())[0]->data.size()-1;
    };
    //autoCube = AutoMorphingModel::Create(*cube, morphFunc);

  
    sphere1->showWireframe = true;
    //autoCube->Translate({-6,0,0});
    //autoCube->Scale(1.5f);
//    sphere1->Translate({-2,0,0});

    //autoCube->showWireframe = true;
    camera->Translate(22, Axis::Z);
    root->AddChild(sphere1);

//    root->AddChild(cyl);
    //root->AddChild(autoCube);
    // points = Eigen::MatrixXd::Ones(1,3);
    // edges = Eigen::MatrixXd::Ones(1,3);
    // colors = Eigen::MatrixXd::Ones(1,3);
    
    // cyl->AddOverlay({points,edges,colors},true);
    //cube->mode =1   ;
    //auto mesh = cube->GetMeshList();

    //autoCube->AddOverlay(points,edges,colors);
    // mesh[0]->data.push_back({V,F,V,E});
//    int num_collapsed;

  // Function to reset original mesh and data structures
    //V = mesh[0]->data[0].vertices;
    //F = mesh[0]->data[0].faces;
   // igl::read_triangle_mesh("data/cube.off",V,F);
    //igl::edge_flaps(F,E,EMAP,EF,EI);
    //std::cout<< "vertices: \n" << V <<std::endl;
    //std::cout<< "faces: \n" << F <<std::endl;
    //
    //std::cout<< "edges: \n" << E.transpose() <<std::endl;
    //std::cout<< "edges to faces: \n" << EF.transpose() <<std::endl;
    //std::cout<< "faces to edges: \n "<< EMAP.transpose()<<std::endl;
    //std::cout<< "edges indices: \n" << EI.transpose() <<std::endl;

}

void BasicScene::Update(const Program& program, const Eigen::Matrix4f& proj, const Eigen::Matrix4f& view, const Eigen::Matrix4f& model)
{
    Scene::Update(program, proj, view, model);
    program.SetUniform4f("lightColor", 0.8f, 0.3f, 0.0f, 0.5f);
    program.SetUniform4f("Kai", 1.0f, 0.3f, 0.6f, 1.0f);
    program.SetUniform4f("Kdi", 0.5f, 0.5f, 0.0f, 1.0f);
    program.SetUniform1f("specular_exponent", 5.0f);
    program.SetUniform4f("light_position", 0.0, 15.0f, 0.0, 1.0f);

    if(IK){
        if(next_rotations.empty()){
            Eigen::Vector3f l = Eigen::Vector3f(1.6f,0,0);
            Eigen::Vector3f E = GetCylPos(2);
            D = sphere1->GetRotation()*sphere1->GetTranslation();
            float dist = (D-E).norm();
            if(dist > 0.05f){
                Eigen::Vector3f  R ;
                if(arm_index==0){
                    R=Eigen::Vector3f(0,0,0);

                }else{
                    int index=(arm_index-1);
                    R= GetCylPos(index);
                }
                Eigen::Vector3f RD=D-R;
                Eigen::Vector3f RE=E-R;
                N_vec= RE.cross(RD);
                float lenght_RD = sqrt(pow(RD.x(),2)+pow(RD.y(),2)+pow(RD.z(),2));
                float lenght_RE = sqrt(pow(RE.x(),2)+pow(RE.y(),2)+pow(RE.z(),2));
                float cos_a =((RD).x() * (RE).x()+(RD).y() * (RE).y()+(RD).z() * (RE).z())/(lenght_RD*lenght_RE);
                float a= acos(cos_a);
                float a_frac = a/100;
                for (int i =0 ; i<100 ; i++){
                    next_rotations.push(a_frac);
                }
    //            cyls[arm_index]->Rotate(a,N);
    //            IK=false;
                //Eigen::Vector3f position= GetCylPos(arm_index);
                if(arm_index == 0){
                    arm_index = 2;
                }
                else{
                    arm_index = arm_index-1;
                }
            }else{
                IK=false;
                std::cout<<"finished"<<std::endl;
                while (!next_rotations.empty()){
                    next_rotations.pop();
                }
            }

        }else{
            float next_a=next_rotations.front();
            next_rotations.pop();
            cyls[arm_index]->Rotate(next_a,N_vec);
        }


    }
}

void BasicScene::MouseCallback(Viewport* viewport, int x, int y, int button, int action, int mods, int buttonState[])
{
    // note: there's a (small) chance the button state here precedes the mouse press/release event

    if (action == GLFW_PRESS) { // default mouse button press behavior
        PickVisitor visitor;
        visitor.Init();
        renderer->RenderViewportAtPos(x, y, &visitor); // pick using fixed colors hack
        auto modelAndDepth = visitor.PickAtPos(x, renderer->GetWindowHeight() - y);
        renderer->RenderViewportAtPos(x, y); // draw again to avoid flickering
        pickedModel = modelAndDepth.first ? std::dynamic_pointer_cast<Model>(modelAndDepth.first->shared_from_this()) : nullptr;
        pickedModelDepth = modelAndDepth.second;
        camera->GetRotation().transpose();
        xAtPress = x;
        yAtPress = y;

        // if (pickedModel)
        //     debug("found ", pickedModel->isPickable ? "pickable" : "non-pickable", " model at pos ", x, ", ", y, ": ",
        //           pickedModel->name, ", depth: ", pickedModelDepth);
        // else
        //     debug("found nothing at pos ", x, ", ", y);

        if (pickedModel && !pickedModel->isPickable)
            pickedModel = nullptr; // for non-pickable models we need only pickedModelDepth for mouse movement calculations later

        if (pickedModel)
            pickedToutAtPress = pickedModel->GetTout();
        else
            cameraToutAtPress = camera->GetTout();
    }
}

void BasicScene::ScrollCallback(Viewport* viewport, int x, int y, int xoffset, int yoffset, bool dragging, int buttonState[])
{
    // note: there's a (small) chance the button state here precedes the mouse press/release event
    auto system = camera->GetRotation().transpose();
    if (pickedModel) {
        if(pickedModel==cyls[1] ||pickedModel==cyls[2]){
            cyls[0] ->TranslateInSystem(system, {0, 0, -float(yoffset)});
            pickedToutAtPress = pickedModel->GetTout();
        }
        else{
            pickedModel->TranslateInSystem(system, {0, 0, -float(yoffset)});
            pickedToutAtPress = pickedModel->GetTout();
        }
    } else {
        camera->TranslateInSystem(system, {0, 0, -float(yoffset)});
        cameraToutAtPress = camera->GetTout();
    }
}

void BasicScene::CursorPosCallback(Viewport* viewport, int x, int y, bool dragging, int* buttonState)
{
    if (dragging) {
        auto system = camera->GetRotation().transpose() * GetRotation();
        auto moveCoeff = camera->CalcMoveCoeff(pickedModelDepth, viewport->width);
        auto angleCoeff = camera->CalcAngleCoeff(viewport->width);
        if (pickedModel) {
            //pickedModel->SetTout(pickedToutAtPress);
            if (buttonState[GLFW_MOUSE_BUTTON_RIGHT] != GLFW_RELEASE)
                pickedModel->TranslateInSystem(system, {-float(xAtPress - x) / moveCoeff, float(yAtPress - y) / moveCoeff, 0});
            if (buttonState[GLFW_MOUSE_BUTTON_MIDDLE] != GLFW_RELEASE)
                pickedModel->RotateInSystem(system, float(xAtPress - x) / angleCoeff, Axis::Z);
            if (buttonState[GLFW_MOUSE_BUTTON_LEFT] != GLFW_RELEASE) {
                pickedModel->RotateInSystem(system, float(xAtPress - x) / angleCoeff, Axis::Y);
                pickedModel->RotateInSystem(system, float(yAtPress - y) / angleCoeff, Axis::X);
            }
        } else {
           // camera->SetTout(cameraToutAtPress);
            if (buttonState[GLFW_MOUSE_BUTTON_RIGHT] != GLFW_RELEASE)
                root->TranslateInSystem(system, {-float(xAtPress - x) / moveCoeff/10.0f, float( yAtPress - y) / moveCoeff/10.0f, 0});
            if (buttonState[GLFW_MOUSE_BUTTON_MIDDLE] != GLFW_RELEASE)
                root->RotateInSystem(system, float(x - xAtPress) / 180.0f, Axis::Z);
            if (buttonState[GLFW_MOUSE_BUTTON_LEFT] != GLFW_RELEASE) {
                root->RotateInSystem(system, float(x - xAtPress) / angleCoeff, Axis::Y);
                root->RotateInSystem(system, float(y - yAtPress) / angleCoeff, Axis::X);
            }
        }
        xAtPress =  x;
        yAtPress =  y;
    }
}

void BasicScene::KeyCallback(Viewport* viewport, int x, int y, int key, int scancode, int action, int mods)
{
    auto system = camera->GetRotation().transpose();

    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) // NOLINT(hicpp-multiway-paths-covered)
        {
            case GLFW_KEY_SPACE:
                IK_algoritm();
                break;
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GLFW_TRUE);
                break;
            case GLFW_KEY_UP:
                if(pickedIndex==0){
                    cyls[pickedIndex]->RotateInSystem(system, 0.1f, Axis::X);
                }else{
                    cyls[pickedIndex]->RotateInSystem(cyls[pickedIndex-1]->GetRotation(), 0.1f, Axis::X);
                }

                break;
            case GLFW_KEY_DOWN:
                if(pickedIndex==0){
                    cyls[pickedIndex]->RotateInSystem(system, -0.1f, Axis::X);
                }else{
                    cyls[pickedIndex]->RotateInSystem(cyls[pickedIndex-1]->GetRotation(), -0.1f, Axis::X);
                }
                break;
            case GLFW_KEY_LEFT:
                if(pickedIndex==0){
                    cyls[pickedIndex]->RotateInSystem(system, 0.1f, Axis::Y);
                }else{
                    cyls[pickedIndex]->RotateInSystem(cyls[pickedIndex-1]->GetRotation(), 0.1f, Axis::Y);
                }
                break;
            case GLFW_KEY_RIGHT:
                if(pickedIndex==0){
                    cyls[pickedIndex]->RotateInSystem(system, -0.1f, Axis::Y);
                }else{
                    cyls[pickedIndex]->RotateInSystem(cyls[pickedIndex-1]->GetRotation(), 0.1f, Axis::Y);
                }
                break;
            case GLFW_KEY_W:
                camera->TranslateInSystem(system, {0, 0.1f, 0});
                break;
            case GLFW_KEY_S:
                camera->TranslateInSystem(system, {0, -0.1f, 0});
                break;
            case GLFW_KEY_A:
                camera->TranslateInSystem(system, {-0.1f, 0, 0});
                break;
            case GLFW_KEY_B:
                camera->TranslateInSystem(system, {0, 0, 0.1f});
                break;
            case GLFW_KEY_F:
                camera->TranslateInSystem(system, {0, 0, -0.1f});
                break;
            case GLFW_KEY_P:
                if(pickedModel){
                    // Create a rotation matrix
                    Eigen::Vector3f euler_angles = cyls[pickedIndex]->GetRotation().eulerAngles(2,0,2);
//                    Eigen::Matrix3Xf A1;
                    Eigen::Matrix3d A1;
                    A1 << cos(euler_angles[0]), -sin(euler_angles[0]), 0,
                            sin(euler_angles[0]),  cos(euler_angles[0]), 0,
                            0,       0,      1;

                    Eigen::Matrix3d A2;
                    A2 << 1,0, 0,
                            0,  cos(euler_angles[1]), -sin(euler_angles[1]),
                            0,       sin(euler_angles[1]),      cos(euler_angles[1]);

                    Eigen::Matrix3d A3;
                    A3 << cos(euler_angles[2]), -sin(euler_angles[2]), 0,
                            sin(euler_angles[2]),  cos(euler_angles[2]), 0,
                            0,       0,      1;
                    std::cout<<"A1: "<<std::endl;
                    printMat(A1);
                    std::cout<<"A2: "<<std::endl;
                    printMat(A2);
                    std::cout<<"A3: "<<std::endl;
                    printMat(A3);
                }
                break;
            case GLFW_KEY_T:
                for(int i =0 ; i <3 ; i++){
                    std::cout<<"point position cyl "<< i << std::endl;
                    print_vector(GetCylPos(i));
                }
                break;
            case GLFW_KEY_D:
                D= sphere1->GetRotation()*sphere1->GetTranslation();
                print_vector(D);
                break;
            case GLFW_KEY_N:
                if(pickedIndex == cyls.size()-1){
                    pickedIndex = 0;
//                    pickedModel=cyls[0];
                }else{
                    if(pickedIndex < cyls.size()-1){
                        pickedIndex = pickedIndex + 1;
//                        pickedModel = cyls[pickedIndex];
                    }
                }
                break;
            case GLFW_KEY_1:
                if( pickedIndex > 0)
                  pickedIndex--;
                break;
            case GLFW_KEY_2:
                if(pickedIndex < cyls.size()-1)
                    pickedIndex++;
                break;
            case GLFW_KEY_3:
                if( tipIndex >= 0)
                {
                  if(tipIndex == cyls.size())
                    tipIndex--;
                  sphere1->Translate(GetSpherePos());
                  tipIndex--;
                }
                break;
            case GLFW_KEY_4:
                if(tipIndex < cyls.size())
                {
                    if(tipIndex < 0)
                      tipIndex++;
                    sphere1->Translate(GetSpherePos());
                    tipIndex++;
                }
                break;
        }
    }
}

Eigen::Vector3f BasicScene::GetSpherePos()
{
      Eigen::Vector3f l = Eigen::Vector3f(1.6f,0,0);
      Eigen::Vector3f res;
      res = cyls[tipIndex]->GetRotation()*l;   
      return res;  
}
Eigen::Vector3f BasicScene::GetCylPos(int i)
{
    Eigen::Vector3f l = Eigen::Vector3f(1.6f,0,0);
    Eigen::Vector3f res={0,0,0};
    for( int j = 0 ; j<=i  ;j++  ){
        Eigen::Vector3f res1 =cyls[j]->GetRotation()*l;
        res = res + res1;
    }
    return res;
}
void BasicScene::print_vector(Eigen::Vector3f vec){
    std::cout << "(" << vec.x() <<"," << vec.y() << "," << vec.z() << ")" << std::endl;
}
void BasicScene::printMat(const Eigen::Matrix3d & mat){
    std::cout << " matrix:" << std::endl;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
            std::cout << mat(j, i) << " ";
        std::cout << std::endl;
    }
}

void BasicScene::IK_algoritm(){
    IK = !IK;
    float arm_lenght = 1.6f*3.0f;
    Eigen::Vector3f sphere_position = sphere1->GetRotation()*sphere1->GetTranslation();
    float dist =  sphere_position.norm();
//    std::cout<<"dist:"<<dist<<std::endl;
    if(dist > arm_lenght){
        std::cout<<"to far"<<std::endl;
        IK=false;
    }
    else{
        arm_index=2;
    }
}