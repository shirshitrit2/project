#include "SceneWithCameras.h"

#include <utility>
#include "igl/AABB.h"
#include "ObjLoader.h"
#include "AutoMorphingModel.h"
#include "SceneWithImGui.h"
//#include "CamModel.h"
#include "Visitor.h"
#include "Utility.h"
#include "imgui.h"
#include "file_dialog_open.h"
#include "GLFW/glfw3.h"
#include "../Example1/IglMeshLoader.h"
#include "Fruit.h"
#include "Display.h"
#include "Renderer.h"
#include<iostream>
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <chrono>
#include <thread>


using namespace cg3d;



void SceneWithCameras::BuildImGui()
{
    int flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
    bool* pOpen = nullptr;

    if(animate){
        ImGui::Begin("Menu", pOpen, flags);
        ImGui::SetWindowPos(ImVec2(280, 0), ImGuiCond_Always);
        ImGui::SetWindowSize(ImVec2(0, 0), ImGuiCond_Always);
//    if (ImGui::Button("Load object"))
//        LoadObjectFromFileDialog();


        ImGui::Text("Score:");
        std::string t = std::to_string(scoreCounter);
        char const *n_char = t.c_str();
        ImGui::SameLine(0);
        ImGui::Text(n_char);




        ImGui::Text("Camera: ");
        for (int i = 0; i < camList.size(); i++) {
            ImGui::SameLine(0);
            bool selectedCamera = camList[i] == camera;
            if (selectedCamera)
                ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
            if(i==0){
                if (ImGui::Button("Top view"))
                    SetCamera(i);
            }
            if(i==1){
                if (ImGui::Button("Snake view"))
                    SetCamera(i);
            }
//        if (ImGui::Button(std::to_string(i + 1).c_str()))
//            SetCamera(i);
            if (selectedCamera)
                ImGui::PopStyleColor();
        }
        ImGui::End();
    }
    else{

        if(win){
            ImGui::Begin("Menu", pOpen, flags);
            ImGui::SetWindowPos(ImVec2(220, 220), ImGuiCond_Always);
            ImGui::SetWindowSize(ImVec2(400, 400), ImGuiCond_Always);
            ImGui::Text("Level finished! ");
            if (ImGui::Button("quit")){
                glfwDestroyWindow(window);
                //std::terminate();
            }
            else if (ImGui::Button("Next level")){
                win=false;
                SetActive(!IsActive());
                init();
            }
            ImGui::End();
        }
        else if(lose){
            ImGui::Begin("Menu", pOpen, flags);
            ImGui::SetWindowPos(ImVec2(220, 220), ImGuiCond_Always);
            ImGui::SetWindowSize(ImVec2(400, 400), ImGuiCond_Always);
            ImGui::Text("Game over! ");
            ImGui::End();
        }
        else{
            ImGui::Begin("Menu", pOpen, flags);
            ImGui::SetWindowPos(ImVec2(220, 220), ImGuiCond_Always);
            ImGui::SetWindowSize(ImVec2(400, 400), ImGuiCond_Always);
            ImGui::Text("Welcome!");
            if (ImGui::Button("quit")){
                glfwDestroyWindow(window);
                //std::terminate();
            }
            else if (ImGui::Button("Start")){
                SetActive(!IsActive());
            }
            ImGui::End();
        }
    }


//    ImGui::SameLine();
//    if (ImGui::Button("Center"))
//        camera->SetTout(Eigen::Affine3f::Identity());
//    if (pickedModel) {
//        ImGui::Text("Picked model: %s", pickedModel->name.c_str());
//        ImGui::SameLine();
//        if (ImGui::Button("Drop"))
//            pickedModel = nullptr;
//        if (pickedModel) {
//            if (ImGui::CollapsingHeader("Draw options", ImGuiTreeNodeFlags_DefaultOpen)) {
//                ImGui::Checkbox("Show wireframe", &pickedModel->showWireframe);
//                if (pickedModel->showWireframe) {
//                    ImGui::Text("Wireframe color:");
//                    ImGui::SameLine();
//                    ImGui::ColorEdit4("Wireframe color", pickedModel->wireframeColor.data(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
//                }
//                ImGui::Checkbox("Show faces", &pickedModel->showFaces);
//                ImGui::Checkbox("Show textures", &pickedModel->showTextures);
//                if (ImGui::Button("Scale down"))
//                    pickedModel->Scale(0.9f);
//                ImGui::SameLine();
//                if (ImGui::Button("Scale up"))
//                    pickedModel->Scale(1.1f);
//            }
//            if (ImGui::Button("Dump model mesh data")) {
//                std::cout << "model name: " << pickedModel->name << std::endl;
//                if (pickedModel->meshIndex > 0)
//                    std::cout << "mesh index in use: " << pickedModel->meshIndex;
//                for (auto& mesh: pickedModel->GetMeshList()) {
//                    Eigen::IOFormat simple(Eigen::StreamPrecision, Eigen::DontAlignCols, ", ", ", ", "", "", "", "");
//                    std::cout << "mesh name: " << mesh->name << std::endl;
//                    for (int i = 0; i < mesh->data.size(); i++) {
//                        if (mesh->data.size() > 1)
//                            std::cout << "mesh #" << i + 1 << ":" << std::endl;
//                        DumpMeshData(simple, mesh->data[i]);
//                    }
//                }
//            }
//            if (ImGui::Button("Dump model transformations")) {
//                Eigen::IOFormat format(2, 0, ", ", "\n", "[", "]");
//                const Eigen::Matrix4f& transform = pickedModel->GetAggregatedTransform();
//                std::cout << "Tin:" << std::endl << pickedModel->Tin.matrix().format(format) << std::endl
//                          << "Tout:" << std::endl << pickedModel->Tout.matrix().format(format) << std::endl
//                          << "Transform:" << std::endl << transform.matrix().format(format) << std::endl
//                          << "--- Transform Breakdown ---" << std::endl
//                          << "Rotation:" << std::endl << Movable::GetTranslation(transform).matrix().format(format) << std::endl
//                          << "Translation:" << std::endl << Movable::GetRotation(transform).matrix().format(format) << std::endl
//                          << "Rotation x Translation:" << std::endl << Movable::GetTranslationRotation(transform).matrix().format(format)
//                          << std::endl << "Scaling:" << std::endl << Movable::GetScaling(transform).matrix().format(format) << std::endl;
//            }
//        }
//    }

//    ImGui::End();
}

void SceneWithCameras::DumpMeshData(const Eigen::IOFormat& simple, const MeshData& data)
{
    std::cout << "vertices mesh: " << data.vertices.format(simple) << std::endl;
    std::cout << "faces mesh: " << data.faces.format(simple) << std::endl;
    std::cout << "vertex normals mesh: " << data.vertexNormals.format(simple) << std::endl;
    std::cout << "texture coordinates mesh: " << data.textureCoords.format(simple) << std::endl;
}

SceneWithCameras::SceneWithCameras(std::string name, Display* display) : SceneWithImGui(std::move(name), display)
{
    ImGui::GetIO().IniFilename = nullptr;
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 5.0f;
}

void SceneWithCameras::SetCamera(int index)
{
    camera = camList[index];
    viewport->camera = camera;
}
//Eigen::Vector3f SceneWithCameras::findVelocity(float factor){
//
//    float Vx1 = (rand() % range + offset);
//    float Vz1 = (rand() % range + offset);
//    Eigen::Vector3f Velocity = Eigen::Vector3f(Vx1,0,Vz1);
//    Velocity.normalize();
//    Velocity=Eigen::Vector3f(Velocity.x()*factor,0,Velocity.z()*factor);
//    return Velocity;
//}
//
bool SceneWithCameras::findSmallestBox(igl::AABB<Eigen::MatrixXd,3> tree1,igl::AABB<Eigen::MatrixXd,3> tree2,Fruit f){

    if(isCollide(f)) {
//        return true;
        if (tree1.is_leaf() && tree2.is_leaf()) {
//            smallestbox1=tree1.m_box;
//            smallestbox2=tree2.m_box;
            return true;
        } else if (tree1.is_leaf() && !tree2.is_leaf())
            return findSmallestBox(tree1, *tree2.m_left,f) || findSmallestBox(tree1, *tree2.m_right,f);
        else if (!tree1.is_leaf() && tree2.is_leaf())
            return findSmallestBox(*tree1.m_left, tree2,f) || findSmallestBox(*tree1.m_right, tree2,f);
        else {
            return findSmallestBox(*tree1.m_right, *tree2.m_right,f) ||
                   findSmallestBox(*tree1.m_right, *tree2.m_left,f) ||
                   findSmallestBox(*tree1.m_left, *tree2.m_left,f) ||
                   findSmallestBox(*tree1.m_left, *tree2.m_right,f);
        }
    }
    return false;
}

//bool SceneWithCameras::findSmallestBox(Fruit f){
//    igl::AABB<Eigen::MatrixXd,3> tree1 = f.getTree();
//    igl::AABB<Eigen::MatrixXd,3> tree2 = snakeTree;
//    findSmallestBox(tree1,tree2,f);
//}

bool SceneWithCameras::isCollide(Fruit f){

    igl::AABB<Eigen::MatrixXd,3> tree1 = f.getTree();
    std::shared_ptr<cg3d::Model> model1 = f.getModel();
    std::shared_ptr<cg3d::Model> model2=cyls[0];
    igl::AABB<Eigen::MatrixXd,3> tree2=snakeTree;

    Eigen::AlignedBox<double,3> box1=tree1.m_box;
    Eigen::AlignedBox<double,3> box2=tree2.m_box;

//    double scale=1;
    double a0=box1.sizes()[0]*scale/2;
    double a1=box1.sizes()[1]*scale/2;
    double a2=box1.sizes()[2]*scale/2;

    double b0=box2.sizes()[0]*scale/2;
    double b1=box2.sizes()[1]*scale/2;
    double b2=box2.sizes()[2]*scale/2;

    Eigen::MatrixXd A=model1->GetRotation().cast<double>();
    Eigen::MatrixXd B=model2->GetRotation().cast<double>();

    Eigen::Vector3d A0=A*Eigen::Vector3d(1,0,0);
    Eigen::Vector3d A1=A*Eigen::Vector3d(0,1,0);
    Eigen::Vector3d A2=A*Eigen::Vector3d(0,0,1);


    Eigen::Vector3d B0=B*Eigen::Vector3d(1,0,0);
    Eigen::Vector3d B1=B*Eigen::Vector3d(0,1,0);
    Eigen::Vector3d B2=B*Eigen::Vector3d(0,0,1);

    Eigen::MatrixXd C=A.transpose()*B;

    Eigen::Vector4f center0={box1.center().x(),box1.center().y(),box1.center().z(),1};
    Eigen::Vector4f C0=model1->GetTransform()*center0;
    Eigen::Vector4f center1={box2.center().x(),box2.center().y(),box2.center().z(),1};
    Eigen::Vector4f C1=model2->GetTransform()*center1;


    Eigen::Vector3d newC0= {C0[0],C0[1],C0[2]};
    Eigen::Vector3d newC1= {C1[0],C1[1],C1[2]};

    Eigen::Vector3d D=newC1-newC0;

    if(abs(A0.dot(D)) > a0+(b0 * abs(C(0, 0))) + (b1 * abs(C(0, 1))) + (b2 * abs(C(0, 2)))){
        return false;
    }

    if(abs(A1.dot(D)) > a1+(b0 * abs(C(1, 0))) + (b1 * abs(C(1, 1))) + (b2 * abs(C(1, 2)))){
        return false;
    }

    if(abs(A2.dot(D)) > a2+(b0 * abs(C(2, 0))) + (b1 * abs(C(2, 1))) + (b2 * abs(C(2, 2)))){
        return false;
    }

    if(abs(B0.dot(D)) > (a0 * abs(C(0, 0))) + (a1 * abs(C(1, 0))) + (a2 * abs(C(2, 0))) + b0){
        return false;
    }

    if(abs(B1.dot(D)) > (a0 * abs(C(0, 1))) + (a1 * abs(C(1, 1))) + (a2 * abs(C(2, 1)))+b1){
        return false;
    }

    if(abs(B2.dot(D)) > (a0 * abs(C(0, 2))) + (a1 * abs(C(1, 2))) + (a2 * abs(C(2, 2)))+b2){
        return false;
    }

    if(abs((C(1, 0) * A2).dot(D) - (C(2, 0) * A1).dot(D)) > (a1 * abs(C(2, 0))) + (a2 * abs(C(1, 0))) + (b1 * abs(C(0, 2))) + (b2 * abs(C(0, 1))) )
    {
        return false;
    }

    if(abs((C(1, 1) * A2).dot(D) - (C(2, 1) * A1).dot(D)) > (a1 * abs(C(2, 1))) + (a2 * abs(C(1, 1))) + (b0 * abs(C(0, 2))) + (b2 * abs(C(0, 0))) )
    {
        return false;
    }

    if(abs((C(1, 2) * A2).dot(D) - (C(2, 2) * A1).dot(D)) > (a1 * abs(C(2, 2))) + (a2 * abs(C(1, 2))) + (b0 * abs(C(0, 1))) + (b1 * abs(C(0, 0))))
    {
        return false;
    }

    if(abs((C(2, 0) * A0).dot(D) - (C(0, 0) * A2).dot(D)) >(a0 * abs(C(2, 0))) + (a2 * abs(C(0, 0))) + (b1 * abs(C(1, 2))) + (b2 * abs(C(1, 1))) )
    {
        return false;
    }

    if(abs((C(2, 1) * A0).dot(D) - (C(0, 1) * A2).dot(D)) > (a0 * abs(C(2, 1))) + (a2 * abs(C(0, 1))) +(b0 * abs(C(1, 2))) + (b2 * abs(C(1, 0))) )
    {
        return false;
    }

    if(abs((C(2, 2) * A0).dot(D) - (C(0, 2) * A2).dot(D)) > (a0 * abs(C(2, 2))) + (a2 * abs(C(0, 2))) + (b0 * abs(C(1, 1))) + (b1 * abs(C(1, 0))))
    {
        return false;
    }

    if(abs((C(0, 0) * A1).dot(D) - (C(1, 0) * A0).dot(D)) > (a0 * abs(C(1, 0))) + (a1 * abs(C(0, 0))) +(b1 * abs(C(2, 2))) + (b2 * abs(C(2, 1))) )
    {
        return false;
    }

    if(abs((C(0, 1) * A1).dot(D) - (C(1, 1) * A0).dot(D)) > (a0 * abs(C(1, 1))) + (a1 * abs(C(0, 1))) + (b0 * abs(C(2, 2))) + (b2 * abs(C(2, 0))))
    {
        return false;
    }

    if(abs((C(0, 2) * A1).dot(D) - (C(1, 2) * A0).dot(D)) > (a0 * abs(C(1, 2))) + (a1 * abs(C(0, 2))) + (b0 * abs(C(2, 1))) + (b1 * abs(C(2, 0)))) {
        return false;
    }
    return true;


}
void SceneWithCameras::initSnakeTree(){
    auto mesh = cyls[0]->GetMeshList();
    Eigen::MatrixXd V1 = mesh[0]->data[0].vertices;
    Eigen::MatrixXi F1 = mesh[0]->data[0].faces;
    snakeTree.init(V1,F1);
}


//void SceneWithCameras::collidingBalls(){
//    for(int i =0 ; i < fruits.size() ; i++){
//        for(int j =i+1 ; j <fruits.size() ; j++){
//            if(findSmallestBox(fruits[i],fruits[j])){
//                fruits[i].setVelocity(Eigen::Vector3f(-fruits[i].getVelocity().x(),0,-fruits[i].getVelocity().z()));
//                fruits[j].setVelocity(Eigen::Vector3f(-fruits[j].getVelocity().x(),0,-fruits[j].getVelocity().z()));
//            }
//        }
//    }
//}

void SceneWithCameras::collidingSnakeWithBall(){
    for(int i=0; i<fruits.size(); i++){
        if(findSmallestBox(snakeTree,fruits[i].getTree(), fruits[i])){
            fruits[i].getModel()->Translate(Eigen::Vector3f(-3,0,-3));
            fruits[i].setVelocity(Eigen::Vector3f(0,0,0));
            //// Score balls.
            if(fruits[i].getColor()=="yellow"){
                scoreCounter=scoreCounter+10;
                std::cout<< scoreCounter <<std::endl;

            }
            //// Magnet balls.
            if(fruits[i].getColor()=="blue"){
                scale=1;
//                root->AddChild(currSphere);
            }
            //// Winning ball
            if(fruits[i].getColor()=="red"){
                SetActive(!IsActive());
                win=true;
            }
            //// Bomb ball
            if(fruits[i].getColor()=="black"){
                SetActive(!IsActive());
                lose=true;
            }
            //// Speed ball
            if(fruits[i].getColor()=="green"){
                speedFactor=speedFactor*10;
                speedTimer=10;
                std::cout<<"speed up"<<std::endl;

            }


        }
    }
}




void SceneWithCameras::Init(float fov, int width, int height, float near, float far)
{
    // create the basic elements of the scene
    AddChild(root = Movable::Create("root")); // a common (invisible) parent object for all the shapes
    auto program = std::make_shared<Program>("shaders/phongShader"); // TODO: TAL: replace with hard-coded basic program
    carbon = std::make_shared<Material>("carbon", program); // default material
    carbon->AddTexture(0, "textures/carbon.jpg", 2);
    auto material{ std::make_shared<Material>("material", program)}; // empty material

    //// Sound:

    //// Cameras:
    camList.resize(camList.capacity());
    camList[0] = Camera::Create("camera0", fov, float(width) / float(height), near, far);
    camList[1] = Camera::Create(" ", fov, float(width) / float(height), near, far);
    root->AddChild(camList[1] );
//    for (int i = 1; i < camList.size(); i++) {
//        auto camera = Camera::Create("", fov, double(width) / height, near, far);
//        auto model = ObjLoader::ModelFromObj(std::string("camera") + std::to_string(i), "data/camera.obj", carbon);
//        root->AddChild(camList[i] = CamModel::Create(*camera, *model));
//    }

    camList[0]->RotateByDegree(-90, Axis::X);
    camList[0]->Translate(20, Axis::Y);
    camList[0]->Translate(5, Axis::X);

    camList[1]->Translate(-3, Axis::X);
    camList[1]->RotateByDegree(-180, Axis::Y);
//    camList[2]->Translate(-8, Axis::Z);
//    camList[2]->RotateByDegree(180, Axis::Y);
//    camList[3]->Translate(3, Axis::X);
//    camList[3]->RotateByDegree(90, Axis::Y);
    camera = camList[0];

    //// Textures :
    auto bricks{std::make_shared<Material>("bricks", program)};
    auto grass{std::make_shared<Material>("grass", program)};
    auto daylight{std::make_shared<Material>("daylight", "shaders/cubemapShader")};

    bricks->AddTexture(0, "textures/bricks.jpg", 2);
    grass->AddTexture(0, "textures/grass.bmp", 2);
    daylight->AddTexture(0, "textures/cubemaps/Daylight Box_", 3);

    //// Background:
    auto background{Model::Create("background", Mesh::Cube(), daylight)};
    AddChild(background);


//    auto snake = Camera::Create("", fov, double(width) / height, near, far);
//    auto model = ObjLoader::ModelFromObj(std::string("snake") , "data/snake3.obj", carbon);
//    root->AddChild( CamModel::Create(*snake, *model));
//
//    auto snakeMesh{ObjLoader::MeshFromObjFiles<std::string>("snakeMesh", "data/snake1.obj", "data/snake2.obj")};
//    auto blank{std::make_shared<Material>("blank", program)};
//    auto snake{Model::Create("snake", snakeMesh, blank)};

//    auto morphFunc = [](Model* model, cg3d::Visitor* visitor) {
//        static float prevDistance = -1;
//        float distance = (visitor->view * visitor->norm * model->GetAggregatedTransform()).norm();
//        if (prevDistance != distance)
//            debug(model->name, " distance from camera: ", prevDistance = distance);
//        return distance > 3 ? 1 : 0;
//    };
//    auto autoSnake = AutoMorphingModel::Create(*snake, morphFunc);
//    autoSnake->showWireframe = true;
//    root->AddChild(autoSnake);
//    float scaleFactor = 0.5;

    //// Snake:
    cyls.push_back( Model::Create("first", Mesh::Cylinder(), grass));
    cyls[0]->Scale(scaleFactor,Axis::X);
    cyls[0]->SetCenter(Eigen::Vector3f(-0.8f*scaleFactor,0,0));
    root->AddChild(cyls[0]);
    for(int i = 1;i < 15; i++)
    {
        cyls.push_back( Model::Create("cylinder", Mesh::Cylinder(), grass));
        cyls[i]->Scale(scaleFactor,Axis::X);
        cyls[i]->Translate(1.6f*scaleFactor,Axis::X);
        cyls[i]->SetCenter(Eigen::Vector3f(-0.8f*scaleFactor,0,0));
        cyls[i-1]->AddChild(cyls[i]);
    }
    cyls[0]->Translate({0.8f*scaleFactor,0,0});

    initSnakeTree();

    //// Fruits:
    auto sphereMesh{ObjLoader::MeshFromObjFiles("sphereMesh", "data/sphere.obj")};
//    yellowSpheres.push_back( Model::Create("sphere1", sphereMesh, grass));
//    blueSpheres.push_back( Model::Create("sphere1", sphereMesh, bricks));
//    cylinder->AddChildren({sphere1, sphere2});

    //// Blue fruits- magnets
    for(int i=0; i<0; i++){
        Fruit f (Model::Create("sphere1", sphereMesh, material, Eigen::RowVector4f(0.0f,0.0f, 255.0f, 0.9f)), "blue");
        fruits.push_back(f);
    }
    //// Yellow fruits- score
    for(int i=0; i<0; i++){
        Fruit f (Model::Create("sphere1", sphereMesh, material, Eigen::RowVector4f(255.0f,255.0f, 105.0f, 0.9f)), "yellow");
        fruits.push_back(f);
    }
    //// Red fruits- win
    for(int i=0; i<15; i++){
        Fruit f (Model::Create("sphere1", sphereMesh, material, Eigen::RowVector4f(255.0f,0.0f, 0.0f, 0.9f)), "red");
        fruits.push_back(f);
    }
    //// Black fruits- bomb
    for(int i=0; i<0; i++){
        Fruit f (Model::Create("sphere1", sphereMesh, material, Eigen::RowVector4f(0.0f,0.0f, 0.0f, 0.9f)), "black");
        fruits.push_back(f);
    }
    //// Green fruits- speed
    for(int i=0; i<0; i++){
        Fruit f (Model::Create("sphere1", sphereMesh, material, Eigen::RowVector4f(0.0f,255.0f, 0.0f, 0.9f)), "green");
        fruits.push_back(f);
    }



    //// Placing the fruits randomly
    offset = -limits/2;
    range =  limits/2 - offset +1;

    for(int i=0;i<fruits.size();i++){
        float x= (rand() % range + offset);
        float y= 0;
        float z= (rand() % range + offset);

        fruits[i].getModel()->Translate(Eigen::Vector3f(x,y,z));

        while(isCollide(fruits[i])) {
            fruits[i].getModel()->Translate(Eigen::Vector3f(-x,-y,-z));
            float x= (rand() % range + offset);
            float y= 0; // should be random too
            float z= (rand() % range + offset);

            fruits[i].getModel()->Translate(Eigen::Vector3f(x,y,z));
        }
    }



//    yellowVelocities.push_back(findVelocity(yellowVelocity));
//    blueVelocities.push_back(findVelocity(blueVelocity));
//    yellowSpheres[0]->showWireframe = true;
//    blueSpheres[0]->showWireframe = true;

    for(int i=0;i<fruits.size();i++){
        root->AddChild(fruits[i].getModel());
    }



//    currSphere= Model::Create("sphere1", sphereMesh, grass);
//    currSphere->Translate(Eigen::Vector3f(4,0,0));
//    currSphere->Scale(1.5f,Axis::X);

//    sphere1->isPickable = false;
//    sphere2->isPickable = false;
//    cylinder->Translate({0, -3, -5});
//    cylinder->showWireframe = true;


//    initTrees();
//    initTrees(blueTree, blueSpheres);


    background->Scale(limits, Axis::XYZ);
    background->SetPickable(false);
    background->SetStatic();
}

void SceneWithCameras::Update(const Program& p, const Eigen::Matrix4f& proj, const Eigen::Matrix4f& view, const Eigen::Matrix4f& model)
{
    Scene::Update(p, proj, view, model);
    if (animate) {
        //// Check that balls doesn't move outside the box
        for (int i = 0; i < fruits.size(); i++) {
            std::shared_ptr<cg3d::Model> curModel = fruits[i].getModel();
            Eigen::Vector3f curVeloc = fruits[i].getVelocity();
            if (curModel->GetTranslation().x() > limits / 2 || curModel->GetTranslation().x() < -limits / 2) {
                fruits[i].setVelocity(Eigen::Vector3f(-curVeloc.x(), 0, curVeloc.z()));
            }
            if (curModel->GetTranslation().z() > limits / 2 || curModel->GetTranslation().z() < -limits / 2) {
                fruits[i].setVelocity(Eigen::Vector3f(curVeloc.x(), 0, -curVeloc.z()));
            }
            curModel->Translate(fruits[i].getVelocity());
        }
        collidingSnakeWithBall();
    }

    if(speedTimer>0){
        speedTimer--;
        if(speedTimer==0) {
            speedFactor = speedFactor / 10;
            std::cout<<"speed down"<<std::endl;

        }
    }
}

void SceneWithCameras::LoadObjectFromFileDialog()
{
    std::string filename = igl::file_dialog_open();
    if (filename.length() == 0) return;

    auto shape = Model::Create(filename, carbon);
}

void SceneWithCameras::KeyCallback(Viewport* _viewport, int x, int y, int key, int scancode, int action, int mods)
{
    Eigen::Matrix3f system=camera->GetRotation().transpose();
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {

        if (key == GLFW_KEY_SPACE)
            SetActive(!IsActive());

        // keys 1-9 are objects 1-9 (objects[0] - objects[8]), key 0 is object 10 (objects[9])
        if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9) {
            if (int index; (index = (key - GLFW_KEY_1 + 10) % 10) < camList.size())
                SetCamera(index);
        }
        if(key == GLFW_KEY_UP)
            cyls[0]->RotateInSystem(system, 0.1f, Axis::Y);
        if(key == GLFW_KEY_DOWN)
            cyls[0]->RotateInSystem(system, -0.1f, Axis::Y);
        if(key == GLFW_KEY_RIGHT)
            cyls[0]->RotateInSystem(system, -0.1f, Axis::Z);
        if(key == GLFW_KEY_LEFT)
            cyls[0]->RotateInSystem(system, 0.1f, Axis::Z);

    }

    SceneWithImGui::KeyCallback(nullptr, x, y, key, scancode, action, mods);
}

void SceneWithCameras::ViewportSizeCallback(Viewport* _viewport)
{
    for (auto& cam: camList)
        cam->SetProjection(float(_viewport->width) / float(_viewport->height));

    // note: we don't need to call Scene::ViewportSizeCallback since we are setting the projection of all the cameras
}

void SceneWithCameras::AddViewportCallback(Viewport* _viewport)
{
    viewport = _viewport;

    Scene::AddViewportCallback(viewport);
}



void SceneWithCameras::handleSound() {
    // Initialize OpenAL
    ALCdevice* device = alcOpenDevice(nullptr);
    ALCcontext* context = alcCreateContext(device, nullptr);
    alcMakeContextCurrent(context);

    // Load sound file
    ALuint buffer;
    alGenBuffers(1, &buffer);

    ALsizei size, frequency;
    ALenum format;
    ALvoid* data;


//    alutLoadWAVFile("sound.wav", &format, &data, &size, &frequency);
//    alBufferData(buffer, format, data, size, frequency);
//    alutUnloadWAV(format, data, size, frequency);

    // Create source and attach buffer
    ALuint source;
    alGenSources(1, &source);

    alSourcei(source, AL_BUFFER, buffer);
    alSourcei(source, AL_LOOPING, AL_TRUE); // Set looping to true

    // Play the sound
    alSourcePlay(source);

    // Wait for program to exit
    std::cout << "Press enter to exit." << std::endl;
    std::cin.get();

    // Clean up
    alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);

    alcMakeContextCurrent(nullptr);
    alcDestroyContext(context);
    alcCloseDevice(device);
}


