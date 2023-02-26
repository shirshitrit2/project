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
#include <SFML/Audio.hpp>



using namespace cg3d;



void SceneWithCameras::BuildImGui()
{
    int flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
    bool* pOpen = nullptr;

    if(playing){
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

        if(imunity){
            ImGui::TextColored(ImVec4(1,0,0,1),"IMUNITY");

        }
        if(catchbombwithimunuty){
            if(bombBonusCounter<11){
                ImGui::TextColored(ImVec4(0.16f,0.83f,0.59f,1),"50+ BONUS");
            }
            if(bombBonusCounter==11){
                catchbombwithimunuty= false;
            }
        }

        if(speedTimer>0){
            ImGui::Text("Turbo timer:");
            std::string t = std::to_string(speedTimer/100);
            char const *n_char = t.c_str();
            ImGui::SameLine(0);
            ImGui::Text(n_char);
        }
        if(magnetTimer>0){
            ImGui::Text("Magnet timer:");
            std::string t = std::to_string(magnetTimer/100);
            char const *n_char = t.c_str();
            ImGui::SameLine(0);
            ImGui::Text(n_char);
        }




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
//            SetActive(!IsActive());

            if (ImGui::Button("Next level")){

                win=false;
                level++;
                resetPlay = true;
//                SetActive(!IsActive());


//                auto program = std::make_shared<Program>("shaders/phongShader"); // TODO: TAL: replace with hard-coded basic program
//
//                auto material{ std::make_shared<Material>("material", program)}; // empty material
//                auto sphereMesh{ObjLoader::MeshFromObjFiles("sphereMesh", "data/sphere.obj")};
//                Fruit f (Model::Create("sphere1", sphereMesh, material, Eigen::RowVector4f(0.0f,0.0f, 0.0f, 0.9f)), "black", level*0.1);
//                f.getModel()->Scale(8);
//                fruits.push_back(f);
//                placeFruits(f);
//
//////                Init(45.0f,800,800,0.1f,120.0f);
//                cyls[0].getCyl()->SetCenter(Eigen::Vector3f(0,0,0));
//                for(int i = 14;i > 0; i--) {
////                    cyls[i].getCyl().
//                    cyls[i].getCyl()->Translate(1.6f * scaleFactor, Axis::X);
//                    cyls[i].getCyl()->Rotate(cyls[i - 1].getCyl()->GetRotation());
//                    cyls[i].getCyl()->Translate(cyls[i - 1].getCyl()->GetTranslation());
//                }
////               root->RemoveChild(cyls[0].getCyl());
//
//                SetActive(!IsActive());
//                std::thread([&]() {
//                    while (IsActive()) {
//                        this->ourUpdate();
//                        std::this_thread::sleep_for(std::chrono::milliseconds(30));
//                    }
//                }).detach();

            }
            else if (ImGui::Button("quit")){
                glfwDestroyWindow(window);
                mainSound.stop();

                //std::terminate();
            }
            ImGui::End();
        }
        else if(lose){
            ImGui::Begin("Menu", pOpen, flags);
            ImGui::SetWindowPos(ImVec2(220, 220), ImGuiCond_Always);
            ImGui::SetWindowSize(ImVec2(400, 400), ImGuiCond_Always);
            ImGui::Text("Game over! ");
            if (ImGui::Button("quit")){
                glfwDestroyWindow(window);
                mainSound.stop();

                //std::terminate();
            }
            ImGui::End();

        }
        else if(level==1){
            ImGui::Begin("Menu", pOpen, flags);
            ImGui::SetWindowPos(ImVec2(220, 220), ImGuiCond_Always);
            ImGui::SetWindowSize(ImVec2(400, 400), ImGuiCond_Always);
            ImGui::Text("Welcome!");
            if (ImGui::Button("Start")){
                SetActive(!IsActive());
                playing=true;
                std::thread([&]() {
                    while (IsActive()) {
                        this->ourUpdate();
                        std::this_thread::sleep_for(std::chrono::milliseconds(30));
                    }
                }).detach();
//                Init(45.0f,800,800,0.1f,120.0f);
            }
            else if (ImGui::Button("quit")){
                glfwDestroyWindow(window);
                mainSound.stop();
                //std::terminate();
            }
            ImGui::End();
        }
    }


}

void SceneWithCameras::reset(){
//
/////MAKE GLOBAL meterial ans sphere mesh
//    auto material{ std::make_shared<Material>("material", program)};
//    auto sphereMesh{ObjLoader::MeshFromObjFiles("sphereMesh", "data/sphere.obj")};
//    Fruit f (Model::Create("sphere1", sphereMesh, material, Eigen::RowVector4f(0.0f,0.0f, 0.0f, 0.9f)), "black", level*0.1);
//    f.getModel()->Scale(8);
//    fruits.push_back(f);
//    placeFruits(f);
//    root->AddChild(f.getModel());

////                Init(45.0f,800,800,0.1f,120.0f);

    Eigen::Matrix3f system= cyls[0].getCyl()->GetRotation();
    while(!cyls[0].isTranslationEmpty()){
        cyls[0].getTranslation();
    }

    while(!cyls[0].isRotationEmpty()){
        cyls[0].getRotation();
    }

    turn=false;
    cyls[0].getCyl()->Translate(Eigen::Vector3f(-cyls[0].getCyl()->GetTranslation().x(),-cyls[0].getCyl()->GetTranslation().y(),-cyls[0].getCyl()->GetTranslation().z()));
    cyls[0].getCyl()->Rotate(cyls[0].getCyl()->GetRotation().transpose());

    for(int i = 14;i > 0; i--) {
//        cyls[i].getCyl().Tr
        Eigen::Matrix4f trans=cyls[0].getCyl()->GetTransform();
        cyls[i].getCyl()->SetTransform(trans);
        cyls[i].getCyl()->Rotate(cyls[i].getCyl()->GetRotation().transpose());
        cyls[i].getCyl()->Translate(1.6f * scaleFactor, Axis::X);
//        cyls[i].getCyl()->Rotate(cyls[i - 1].getCyl()->GetRotation());
        cyls[i].getCyl()->Translate(cyls[i - 1].getCyl()->GetTranslation());
    }
    playing=true;
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


bool SceneWithCameras::isCollide(Fruit f){

    igl::AABB<Eigen::MatrixXd,3> tree1 = f.getTree();
    std::shared_ptr<cg3d::Model> model1 = f.getModel();
    std::shared_ptr<cg3d::Model> model2=cyls[0].getCyl();
    igl::AABB<Eigen::MatrixXd,3> tree2=snakeTree;

    Eigen::AlignedBox<double,3> box1=tree1.m_box;
    Eigen::AlignedBox<double,3> box2=tree2.m_box;

//    double scale=1;
    double a0=box1.sizes()[0]*16*scale/2;
    double a1=box1.sizes()[1]*16*scale/2;
    double a2=box1.sizes()[2]*16*scale/2;

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

bool SceneWithCameras::isSnakeCollide(int i){

    std::shared_ptr<cg3d::Model> model1=cyls[i].getCyl();
    igl::AABB<Eigen::MatrixXd,3> tree1;
    auto mesh = cyls[i].getCyl()->GetMeshList();
    Eigen::MatrixXd V1 = mesh[0]->data[0].vertices;
    Eigen::MatrixXi F1 = mesh[0]->data[0].faces;
    tree1.init(V1,F1);

    std::shared_ptr<cg3d::Model> model2=cyls[0].getCyl();
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
    auto mesh = cyls[0].getCyl()->GetMeshList();
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
//            fruits[i].getModel()->Translate(Eigen::Vector3f(-3,0,-3));
//            fruits[i].setVelocity(Eigen::Vector3f(0,0,0));
            placeFruits(fruits[i]);
            bite.play();
            bite.setVolume(100);
            //// Score balls.
            if(fruits[i].getColor()=="yellow"){
                scoreCounter=scoreCounter+10;
                std::cout<< scoreCounter <<std::endl;

            }
            //// Magnet balls.
            if(fruits[i].getColor()=="blue"){
                scale=1;
                magnetTimer=3000;
//                root->AddChild(currSphere);
            }
            //// Winning ball
            if(fruits[i].getColor()=="red"){
//                SetActive(!IsActive());
                playing= false;
                init();
                win=true;
            }
            //// Bomb ball
            if(fruits[i].getColor()=="black"){
                if(!imunity){
                    playing= false;
//                    SetActive(!IsActive());
                    lose=true;
                } else{
                    imunity= false;
                    scoreCounter+=50;
                    catchbombwithimunuty = true;
                    bombBonusCounter =0;
                }

            }
            //// imunity
            if(fruits[i].getColor()=="green"){
//                speedFactor=speedFactor*10;
//                speedTimer=3000;
//                std::cout<<"speed up"<<std::endl;
                imunity=true;


            }

        }
    }
}




void SceneWithCameras::Init(float fov, int width, int height, float near, float far)
{
    // create the basic elements of the scene
    AddChild(root = Movable::Create("root")); // a common (invisible) parent object for all the shapes
    program = std::make_shared<Program>("shaders/phongShader"); // TODO: TAL: replace with hard-coded basic program
    carbon = std::make_shared<Material>("carbon", program); // default material
    carbon->AddTexture(0, "textures/carbon.jpg", 2);
    auto material{ std::make_shared<Material>("material", program)}; // empty material

    //// Sound:
//    sf::SoundBuffer buffer;

    mainBuffer.loadFromFile("textures/spongebob.wav");
    if (!biteBuffer.loadFromFile("textures/bite.wav"))
    {
        std::cout << "Error: Failed to load sound file." << std::endl;
    }

    mainSound.setBuffer(mainBuffer);
    bite.setBuffer(biteBuffer);
    mainSound.setLoop(true);
    mainSound.play();
    mainSound.setVolume(30);

//
//    while (sound.getStatus() == sf::Sound::Playing)
//    {
//        // Wait until the sound finishes playing
//    }

    //// Cameras:
    camList.resize(camList.capacity());
    camList[0] = Camera::Create("camera0", fov, float(width) / float(height), near, far);
    camList[1] = Camera::Create(" ", fov, float(width) / float(height), near, far);
    root->AddChild(camList[1] );

    camList[0]->RotateByDegree(-90, Axis::X);
    camList[0]->Translate(50, Axis::Y);
    camList[0]->Translate(5, Axis::X);

    camList[1]->Translate(-0.8, Axis::X);
    camList[1]->RotateByDegree(90, Axis::Y);
    camera = camList[1];

    //// Textures :
    auto bricks{std::make_shared<Material>("bricks", program)};
    auto grass{std::make_shared<Material>("grass", program)};
    auto daylight{std::make_shared<Material>("daylight", "shaders/cubemapShader")};

    bricks->AddTexture(0, "textures/box0.bmp", 2);
    grass->AddTexture(0, "textures/box0.bmp", 2);
    daylight->AddTexture(0, "textures/cubemaps/Box_", 3);

    //// Background:
    auto background{Model::Create("background", Mesh::Cube(), daylight)};
    AddChild(background);


    //// Snake:
    Cyl cur(Model::Create("first", Mesh::Cylinder(), grass));
    cyls.push_back( cur);
    initSnakeRotation =cyls[0].getCyl()->GetRotation();
    cyls[0].getCyl()->Scale(scaleFactor,Axis::X);
//    cyls[0].getCyl()->Scale(scaleFactor);

//    cyls[0].getCyl()->SetCenter(Eigen::Vector3f(-0.8f*scaleFactor,0,0));
    root->AddChild(cyls[0].getCyl());
    for(int i = 1;i < 15; i++)
    {
        Cyl cur(Model::Create("cylinder", Mesh::Cylinder(), grass));
        cyls.push_back( cur);
//        cyls[i].getCyl()->Scale(scaleFactor,Axis::X);
        cyls[i].getCyl()->Translate(1.6f*scaleFactor,Axis::X);
//        cyls[i].getCyl()->Translate(Eigen::Vector3f(0.8f*scaleFactor,0,0));
        cyls[i].getCyl()->Rotate(cyls[i-1].getCyl()->GetRotation());
        cyls[i].getCyl()->Translate(cyls[i-1].getCyl()->GetTranslation());
        root->AddChild(cyls[i].getCyl());
    }
//    cyls[0].getCyl()->Translate({0.8f*scaleFactor,0,0});
    cyls[0].getCyl()->AddChild(camList[1]);

    initSnakeTree();

    //// Fruits:
    auto sphereMesh{ObjLoader::MeshFromObjFiles("sphereMesh", "data/sphere.obj")};
//    yellowSpheres.push_back( Model::Create("sphere1", sphereMesh, grass));
//    blueSpheres.push_back( Model::Create("sphere1", sphereMesh, bricks));
//    cylinder->AddChildren({sphere1, sphere2});

    //// Blue fruits- magnets
    for(int i=0; i<0; i++){
        Fruit f (Model::Create("sphere1", sphereMesh, material, Eigen::RowVector4f(0.0f,0.1f, 5.0f, 0.9f)), "blue");
        f.getModel()->Scale(8);
        fruits.push_back(f);
    }
    //// Yellow fruits- score
    for(int i=0; i<0; i++){
        Fruit f (Model::Create("sphere1", sphereMesh, material, Eigen::RowVector4f(0.6f,1.0f, 0.0f, 0.9f)), "yellow");
        f.getModel()->Scale(8);
        fruits.push_back(f);
    }
    //// Red fruits- win
    for(int i=0; i<0; i++){
        Fruit f (Model::Create("sphere1", sphereMesh, material, Eigen::RowVector4f(9.0f,0.1f, 0.0f, 0.9f)), "red");
        f.getModel()->Scale(8);
        fruits.push_back(f);
    }
    //// Black fruits- bomb
    for(int i=0; i<10; i++){
        Fruit f (Model::Create("sphere1", sphereMesh, material, Eigen::RowVector4f(0.0f,0.0f, 0.0f, 0.9f)), "black", level*0.1);
        f.getModel()->Scale(8);
        fruits.push_back(f);
    }
    //// Green fruits- immunity
    for(int i=0; i<0; i++){
        Fruit f (Model::Create("sphere1", sphereMesh, material, Eigen::RowVector4f(0.0f,1.0f, 0.0f, 0.9f)), "green");
        f.getModel()->Scale(8);
        fruits.push_back(f);
    }



    //// Placing the fruits randomly
//    offset = -limits/2;
//    range =  limits/2 - offset +1;
//    scale=1;

    for(int i=0;i<fruits.size();i++){
        placeFruits(fruits[i]);
    }
//        scale=0.5;


    for(int i=0;i<fruits.size();i++){
        root->AddChild(fruits[i].getModel());
    }


    background->Scale(limits, Axis::XYZ);
    background->SetPickable(false);
    background->SetStatic();



}


void SceneWithCameras::ourUpdate(){
    if(resetPlay){
        resetPlay=false;
        reset();
    }else{
        if(end_counter==15){
            turn= false;
            fullTurnAction= false;
        }
        if(start_counter==15){
            turn=true;
        }
        ////move cyls[1]-cyls[n]
        if(turn){////evryone is turning
            for(int i = (cyls.size()-1);i>0;i--){
                Eigen::Matrix4f trans=cyls[i-1].getCyl()->GetTransform();
                cyls[i].getCyl()->SetTransform(trans);
            }
        }else{
            if(start_counter>15){////evryone goinig strait
                for(int i = (cyls.size()-1);i>0;i--){
                    Eigen::Matrix3f system= cyls[i].getCyl()->GetRotation();
                    cyls[i].getCyl()->Translate(system* Eigen::Vector3f(-1.5f*speedFactor,0,0));
                }
            }else{////some are turning and some are going strait
                for(int i = start_counter+1;i<cyls.size();i++){ /////tail- going straight
                    Eigen::Matrix3f system= cyls[i].getCyl()->GetRotation();
                    cyls[i].getCyl()->Translate(system* Eigen::Vector3f(-1.5f*speedFactor,0,0));
                }
                for(int i = start_counter;i>0;i--){//// first i cyls - going round
                    Eigen::Matrix4f trans=cyls[i-1].getCyl()->GetTransform();
                    cyls[i].getCyl()->SetTransform(trans);
                }
            }

        }

        end_counter++;
        start_counter++;
        bombBonusCounter++;
////move cyls[0]
        if(cyls[0].isTranslationEmpty()){
            Eigen::Matrix3f system= cyls[0].getCyl()->GetRotation();
            cyls[0].getCyl()->Translate(system* Eigen::Vector3f(-1.5f*speedFactor,0,0));

        } else{
            Eigen::Vector3f trans = cyls[0].getTranslation();
            Eigen::Matrix3f system= cyls[0].getCyl()->GetRotation();
            if(trans == Eigen::Vector3f(5,5,5)){////check if its first iteration of a turn
                start_counter=1;////starts counter for turning cyls
                fullTurnAction=true;
                cyls[0].getCyl()->Translate(system*cyls[0].getTranslation());////geting the real first translation and rotation
                Eigen::Vector2f next_rot=cyls[0].getRotation();
                if(next_rot[0]==3.0){
                    cyls[0].getCyl()->Rotate(next_rot[1],Axis::Z);
                }
                else{
                    cyls[0].getCyl()->Rotate(next_rot[1],Axis::Y);
                }

            }else if(trans ==Eigen::Vector3f(7,7,7)){////check if its thr end of a turn for cyls[0]
                end_counter=0;////starts counter for stoping cyls

            }else{
                cyls[0].getCyl()->Translate(system*trans);////if its not the end and not the start move acording to the trans and rot
                Eigen::Vector2f next_rot=cyls[0].getRotation();
                if(next_rot[0]==3.0){
                    cyls[0].getCyl()->Rotate(next_rot[1],Axis::Z);
                }
                else{
                    cyls[0].getCyl()->Rotate(next_rot[1],Axis::Y);
                }
            }


        }
        //// Check that balls doesn't move outside the box
        for (int i = 0; i < fruits.size(); i++) {
            std::shared_ptr<cg3d::Model> curModel = fruits[i].getModel();
            Eigen::Vector3f curVeloc = fruits[i].getVelocity();
            if (curModel->GetTranslation().x() > limits / 2 || curModel->GetTranslation().x() < -limits / 2) {
                fruits[i].setVelocity(Eigen::Vector3f(-curVeloc.x(), curVeloc.y(), curVeloc.z()));
            }
            if (curModel->GetTranslation().z() > limits / 2 || curModel->GetTranslation().z() < -limits / 2) {
                fruits[i].setVelocity(Eigen::Vector3f(curVeloc.x(), curVeloc.y(), -curVeloc.z()));
            }
            if (curModel->GetTranslation().y() > limits / 2 || curModel->GetTranslation().y() < -limits / 2) {
                fruits[i].setVelocity(Eigen::Vector3f(curVeloc.x(), -curVeloc.y(), curVeloc.z()));
            }
            if(fruits[i].getColor()=="red"){
                curModel->Translate(fruits[i].getVelocity()*level);////moving the red fruit- change by level

            }
            else{
                curModel->Translate(fruits[i].getVelocity());////moving the fruits

            }
        }


        ////check colision with balls
        collidingSnakeWithBall();


        ////check if the snack is going out of limit
        if (cyls[0].getCyl()->GetTranslation().x() > limits / 2 || cyls[0].getCyl()->GetTranslation().x() < -limits / 2) {
            SetActive(!IsActive());
            playing= false;
            lose= true;
        }
        if (cyls[0].getCyl()->GetTranslation().z() > limits / 2 || cyls[0].getCyl()->GetTranslation().z() < -limits / 2) {
            SetActive(!IsActive());
            playing=false;
            lose=true;
        }
        if (cyls[0].getCyl()->GetTranslation().y() > limits / 2 || cyls[0].getCyl()->GetTranslation().y() < -limits / 2) {
            SetActive(!IsActive());
            playing=false;
            lose=true;
        }


        //// Handle speed boost timer
        if(speedTimer>0){
            speedTimer=speedTimer-15;
            if(speedTimer==0) {
                speedFactor = speedFactor / 10;
                std::cout<<"speed down"<<std::endl;

            }
        }

        //// Handle magnet boost timer
        if(magnetTimer>0){
            magnetTimer=magnetTimer-15;
            if(magnetTimer==0) {
                scale=0.5;
                std::cout<<"magnet down"<<std::endl;

            }
        }

        //// Check self collision of the snake
        for(int i=1;i<cyls.size();i++){
            if(isSnakeCollide(i)){
//            lose=true;
            }
        }






    }


}

void SceneWithCameras::Update(const Program& p, const Eigen::Matrix4f& proj, const Eigen::Matrix4f& view, const Eigen::Matrix4f& model)
{
    Scene::Update(p, proj, view, model);

}

void SceneWithCameras::LoadObjectFromFileDialog()
{
    std::string filename = igl::file_dialog_open();
    if (filename.length() == 0) return;

    auto shape = Model::Create(filename, carbon);
}

void SceneWithCameras::KeyCallback(Viewport* _viewport, int x, int y, int key, int scancode, int action, int mods)
{
    Eigen::Matrix3f system=cyls[0].getCyl()->GetRotation().transpose();
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {

        if (key == GLFW_KEY_SPACE)
            SetActive(!IsActive());

        // keys 1-9 are objects 1-9 (objects[0] - objects[8]), key 0 is object 10 (objects[9])
        if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9) {
            if (int index; (index = (key - GLFW_KEY_1 + 10) % 10) < camList.size())
                SetCamera(index);
        }
        if(key == GLFW_KEY_UP){
            if(!fullTurnAction){
                loadTurn(false,true);}
        }
//            cyls[0]->RotateInSystem(system, -0.1f, Axis::Z);
        if(key == GLFW_KEY_DOWN){
            if(!fullTurnAction){
                loadTurn(true,true);}
        }
//            cyls[0]->RotateInSystem(system, 0.1f, Axis::Z);
        if(key == GLFW_KEY_RIGHT){
            if(!fullTurnAction){
                loadTurn(false, false);}

        }
//            cyls[0]->RotateInSystem(system, -0.1f, Axis::Y);
        if(key == GLFW_KEY_LEFT){
            if(!fullTurnAction){
                loadTurn(true, false);}

        }
//            cyls[0]->RotateInSystem(system, 0.1f, Axis::Y);

    }

    SceneWithImGui::KeyCallback(nullptr, x, y, key, scancode, action, mods);
}



void SceneWithCameras::loadTurn(bool direction, bool isAxisZ){
    float dir=-1.0;
    if(direction){
        dir=1.0;
    }
    float timeIntervals=40.0;
    float time = 1.0f/(timeIntervals*fac);

    if(isAxisZ){ ////down
//        Eigen::Vector3f p1= cyls[0].getCyl()->GetTranslation();
//        Eigen::Vector3f p2 = p1+Eigen::Vector3f(-2*fac,0,0);
//        Eigen::Vector3f p3 = p1+Eigen::Vector3f(-2*fac,dir*0.1*fac,0);
//        cyls[0].setTranslation(getPosition(time,p1,p2,p3)-p1);
//        cyls[0].setRotation(Eigen::Vector2f (3.0,1.57079633f*time*dir));
        cyls[0].setTranslation(Eigen::Vector3f(5,5,5));

        for(float i=1.0f;i<=(timeIntervals*fac);i=i+1.0f){
            Eigen::Vector3f p1= cyls[0].getCyl()->GetTranslation();
            Eigen::Vector3f p2 = p1+Eigen::Vector3f(-2*fac,0,0);
            Eigen::Vector3f p3 = p1+Eigen::Vector3f(-2*fac,dir*0.1*fac,0);
            float cur_time = i/(timeIntervals*fac);
            cyls[0].setTranslation(getPosition(cur_time,p1,p2,p3)-p1);
            cyls[0].setRotation(Eigen::Vector2f (3.0,1.57079633f*time*dir));
        }
        cyls[0].setTranslation(Eigen::Vector3f(7,7,7));

    } else{////right
//
        cyls[0].setTranslation(Eigen::Vector3f(5,5,5));

        for(float i=1.0f;i<=(timeIntervals*fac);i=i+1.0f){
            Eigen::Vector3f p1= cyls[0].getCyl()->GetTranslation();
            Eigen::Vector3f p2 = p1+Eigen::Vector3f(-2*fac,0,0);
            Eigen::Vector3f p3 = p1+Eigen::Vector3f(-2*fac,0,dir*0.1*fac);
            float cur_time = i/(timeIntervals*fac);
            Eigen::Vector3f trans =getPosition(cur_time,p1,p2,p3)-p1;
            cyls[0].setTranslation(trans);
            cyls[0].setRotation(Eigen::Vector2f (2.0,1.57079633f*time*dir));
        }
        cyls[0].setTranslation(Eigen::Vector3f(7,7,7));
    }
//    std::queue<Eigen::Vector3f> copy=cyls[0].Translations;
//
//    while(!copy.empty()){
//        std::cout << copy.front() << " "<< std::endl;
//        copy.pop();
//    }
}


Eigen::Vector3f SceneWithCameras::getPosition(float time, Eigen::Vector3f p0, Eigen::Vector3f p1,Eigen::Vector3f p2) {
    if(time < 0 || time > 1)
        throw std::invalid_argument("Time out of bounds for curve");
    Eigen::Vector3f pos = p1 + ( pow((1-time),2)*(p0-p1) ) + ( pow(time,2)*(p2-p1) );
//    pos += pow((1-time), 2)  * p1;
//    pos+=2 * pow((1-time), 1) * pow(time, 1) * p2;
    return pos;
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

void SceneWithCameras::placeFruits(Fruit f){
    offset = -limits/2;
    range =  limits/2 - offset +1;
    scale=1;
    float x= (rand() % range + offset);
    float y= (rand() % range + offset);
    float z= (rand() % range + offset);

    f.getModel()->Translate(Eigen::Vector3f(x,y,z));

    while(isCollide(f)) {
        f.getModel()->Translate(Eigen::Vector3f(-x,-y,-z));
        float x= (rand() % range + offset);
        float y= (rand() % range + offset); // should be random too
        float z= (rand() % range + offset);

        f.getModel()->Translate(Eigen::Vector3f(x,y,z));
    }
    scale=0.5;
}



