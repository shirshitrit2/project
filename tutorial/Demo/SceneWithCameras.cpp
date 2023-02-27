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
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "SkinnedSnake.h"


using namespace cg3d;

//////////////////////////////////////////////////////////////////////
/////////////////////// Main functions: /////////////////////////////
/////////////////////////////////////////////////////////////////////

void SceneWithCameras::BuildImGui()
{
    int flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
    bool* pOpen = nullptr;

    //// Score menu:
    if(playing){
        ImGui::Begin("Menu", pOpen, flags);
        ImGui::SetWindowPos(ImVec2(250, 0), ImGuiCond_Always);
        ImGui::SetWindowSize(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::SetWindowFontScale(1.5f); // set font scale to 2x the default size
        ImGui::Text("Score:");
        std::string t = std::to_string(scoreCounter+levelSCore);
        char const *n_char = t.c_str();
        ImGui::SameLine(0);
        ImGui::Text(n_char);

        ////Powerups:
        if(scoreCounter+levelSCore>=100){
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f,0.0f,0.0f,1.0f)); // Set text color to yellow
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f,1.0f,0.0f,1.0f)); // Set text color to yellow
            if (ImGui::Button("Add winning ball (-100 points)")){
                Fruit f (Model::Create("sphere1", sphereMesh, material, Eigen::RowVector4f(0.7f,0.0f, 0.0f, 0.9f)), "red");
                f.getModel()->Scale(8);
                fruits.push_back(f);
                placeFruits(f);
                root->AddChild(f.getModel());
                levelSCore-=100;
            }
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
        }
        if(imunity){
            ImGui::TextColored(ImVec4(1,0,0,1),"IMMUNITY");
        }
        if(catchbombwithimunuty){
            if(bombBonusCounter<11){
                ImGui::TextColored(ImVec4(0.16f,0.83f,0.59f,1),"50+ BONUS");
            }
            if(bombBonusCounter==11){
                catchbombwithimunuty= false;
            }
        }
        if(magnetTimer>0){
            ImGui::Text("Magnet timer:");
            std::string t = std::to_string(magnetTimer/100);
            char const *n_char = t.c_str();
            ImGui::SameLine(0);
            ImGui::Text(n_char);
        }
        ////Cameras:
        ImGui::Text("Camera: ");
        for (int i = 0; i < camList.size(); i++) {
            ImGui::SameLine(0);
            bool selectedCamera = camList[i] == camera;
            if (selectedCamera)
                ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
            if(i==0){
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f,1.0f,1.0f,1.0f)); // Set text color to yellow
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.68f,0.68f,0.05f,1.0f)); // Set text color to yellow
                if (ImGui::Button("Top view"))
                    SetCamera(i);
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
            }
            if(i==1){
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f,1.0f,1.0f,1.0f)); // Set text color to yellow
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.68f,0.68f,0.05f,1.0f)); // Set text color to yellow
                if (ImGui::Button("Snake view"))
                    SetCamera(i);
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
            }
            if (selectedCamera)
                ImGui::PopStyleColor();
        }
        ImGui::End();
    }
    else{
        ////Level finished
        if(win){
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 0.9f, 0.4f, 1.0f)); // set blue color
            ImGui::Begin("Menu", pOpen, flags);
            ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
            ImGui::SetWindowSize(ImVec2(800, 800), ImGuiCond_Always);
            ImGui::SetWindowFontScale(3.0f); // set font scale to 2x the default size
            ImGui::SetCursorPosX(210); // set x position of text
            ImGui::SetCursorPosY(200); // set y position of text
            std::string t = std::to_string(level);
            char const *n_char = t.c_str();
            ImGui::TextColored(ImVec4(0.68f,0.68f,0.05f,1.0f),"Level ");
            ImGui::SameLine(0);
            ImGui::TextColored(ImVec4(0.68f,0.68f,0.05f,1.0f),n_char);
            ImGui::SameLine(0);
            ImGui::TextColored(ImVec4(0.68f,0.68f,0.05f,1.0f)," finished!");
            ImGui::SetWindowFontScale(2.0f); // set font scale to 2x the default size
            ImGui::SetCursorPosX(330); // set x position of text
            std::string t2 = std::to_string(scoreCounter+levelSCore);
            char const *n_char2 = t2.c_str();
            ImGui::TextColored(ImVec4(0.68f,0.68f,0.05f,1.0f),"Score: ");
            ImGui::SameLine(0);
            ImGui::TextColored(ImVec4(0.68f,0.68f,0.05f,1.0f),n_char2);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f,1.0f,1.0f,1.0f)); // Set text color to yellow
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.68f,0.68f,0.05f,1.0f)); // Set text color to yellow
            ImGui::SetCursorPosX(315);
            ImGui::SetWindowFontScale(2.0f); // set font scale to 2x the default size

            if (ImGui::Button("Next level")){
                win=false;
                reset(1);
                scoreCounter=scoreCounter+levelSCore;
                levelSCore=0;
                ready.play();
            }
            ImGui::SetCursorPosX(315);
            if (ImGui::Button("play again")){
                reset(0);
                levelSCore=0;
                ready.play();
            }
            ImGui::SetCursorPosX(360);
            if (ImGui::Button("quit")){
                SetActive(!IsActive());
                glfwDestroyWindow(window);
                mainSound.stop();
            }
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            ImGui::End();
            ImGui::PopStyleColor();
        }
            ////Game over:
        else if(lose){
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 0.9f, 0.4f, 1.0f)); // set blue color
            ImGui::Begin("Menu", pOpen, flags);
            ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
            ImGui::SetWindowSize(ImVec2(800, 800), ImGuiCond_Always);
            ImGui::SetWindowFontScale(4.0f); // set font scale to 2x the default size
            ImGui::SetCursorPosX(260); // set x position of text
            ImGui::SetCursorPosY(200); // set y position of text
            ImGui::TextColored(ImVec4(0.0f,0.0f,0.0f,1.0f),"Game over! ");
            ImGui::SetWindowFontScale(2.0f); // set font scale to 2x the default size
            ImGui::SetCursorPosX(330); // set x position of text
            std::string t2 = std::to_string(scoreCounter+levelSCore);
            char const *n_char2 = t2.c_str();
            ImGui::TextColored(ImVec4(0.68f,0.68f,0.05f,1.0f),"Score: ");
            ImGui::SameLine(0);
            ImGui::TextColored(ImVec4(0.68f,0.68f,0.05f,1.0f),n_char2);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f,1.0f,1.0f,1.0f)); // Set text color to yellow
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.68f,0.68f,0.05f,1.0f)); // Set text color to yellow
            ImGui::SetCursorPosX(320);

            if (ImGui::Button("play again")){
                reset(0);
//                scoreCounter=0;
                levelSCore=0;
                ready.play();
            }
            ImGui::SetCursorPosX(355);
            if (ImGui::Button("quit")){
                SetActive(!IsActive());
                glfwDestroyWindow(window);
                mainSound.stop();
            }
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            ImGui::End();
            ImGui::PopStyleColor();
        }
            ////Main menu:
        else if(level==1){
            ImGuiIO& io = ImGui::GetIO();
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 0.9f, 0.4f, 1.0f)); // set blue color
            ImGui::Begin("Menu", pOpen, flags);
            ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
            ImGui::SetWindowSize(ImVec2(800, 800), ImGuiCond_Always);
            ImGui::SetWindowFontScale(3.0f); // set font scale to 2x the default size
            ImGui::SetCursorPosX(320); // set x position of text
            ImGui::SetCursorPosY(200); // set y position of text
            ImGui::TextColored(ImVec4(0.68f,0.68f,0.05f,1.0f),"Welcome!");
            ImGui::SetWindowFontScale(2.0f); // set font scale to 2x the default size
            ImGui::SetCursorPosX(10); // set x position of text
            ImGui::SetCursorPosY(270);
            ImGui::TextColored(ImVec4(0.68f,0.68f,0.05f,1.0f),"In this game, you are a snake in an underwater world ");
            ImGui::SetCursorPosX(10); // set x position of text
            ImGui::TextColored(ImVec4(0.68f,0.68f,0.05f,1.0f),"full of colorful balls, each one has it's own power:");
            ImGui::SetCursorPosX(10); // set x position of text
            ImGui::TextColored(ImVec4(1.0f,0.0f,0.0f,1.0f),"Red- Win the level.");
            ImGui::SetCursorPosX(10); // set x position of text
            ImGui::TextColored(ImVec4(0.0f,0.0f,0.0f,1.0f),"Black- A bomb, will kill you and finish the game.");
            ImGui::SetCursorPosX(10); // set x position of text
            ImGui::TextColored(ImVec4(0.54f,0.03f,0.88f,1.0f),"Purple- 10 Points (100 points = new red ball).");
            ImGui::SetCursorPosX(10); // set x position of text
            ImGui::TextColored(ImVec4(0.0f,0.0f,1.0f,1.0f),"Blue- Magnet powerup, ");
            ImGui::SetCursorPosX(10); // set x position of text
            ImGui::TextColored(ImVec4(0.0f,0.0f,1.0f,1.0f),"your range of collecting balls wil increase.");
            ImGui::SetCursorPosX(10); // set x position of text
            ImGui::TextColored(ImVec4(0.0f,0.5f,0.0f,1.0f),"Green- Immunity powerup, ");
            ImGui::SetCursorPosX(10); // set x position of text
            ImGui::TextColored(ImVec4(0.0f,0.5f,0.0f,1.0f),"will switch bomb from killing to giving extra points.");
            ImGui::SetCursorPosX(200); // set x position of text
            ImGui::TextColored(ImVec4(0.68f,0.68f,0.05f,1.0f),"Be carefull of the walls!");

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f,1.0f,1.0f,1.0f)); // Set text color to yellow
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.68f,0.68f,0.05f,1.0f)); // Set text color to yellow
            ImGui::SetCursorPosX(350);
            ImGui::SetCursorPosY(600);

            ImGui::SetWindowFontScale(2.0f); // set font scale to 2x the default size

            if (ImGui::Button("Start")){
                SetActive(!IsActive());
                ready.play();
                playing=true;
                std::thread([&]() {
                    while (IsActive()) {
                        this->ourUpdate();
                        std::this_thread::sleep_for(std::chrono::milliseconds(30));
                    }
                }).detach();
            }
            ImGui::SetCursorPosX(355);

            if (ImGui::Button("quit")){
                SetActive(!IsActive());
                glfwDestroyWindow(window);
                mainSound.stop();
            }
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();

            ImGui::End();
            ImGui::PopStyleColor();
        }
    }

}


void SceneWithCameras::Init(float fov, int width, int height, float near, float far)
{
    AddChild(root = Movable::Create("root")); // a common (invisible) parent object for all the shapes
    carbon = std::make_shared<Material>("carbon", program);
    carbon->AddTexture(0, "textures/carbon.jpg", 2);
    material= std::make_shared<Material>("material", program); // empty material
    sponge = std::make_shared<Material>("carbon", program1);
    sponge->AddTexture(0, "textures/sponge4.jpeg", 2);

    //// Sounds:
    mainBuffer.loadFromFile("textures/spongebob.wav");
    biteBuffer.loadFromFile("textures/bite.wav");
    moneybuffer.loadFromFile("textures/krab.wav");
    readybuffer.loadFromFile("textures/ready.wav");
    victorybuffer.loadFromFile("textures/victory.wav");
    trybuffer.loadFromFile("textures/try.wav");

    mainSound.setBuffer(mainBuffer);
    bite.setBuffer(biteBuffer);
    money.setBuffer(moneybuffer);
    ready.setBuffer(readybuffer);
    victory.setBuffer(victorybuffer);
    tryagain.setBuffer(trybuffer);

    mainSound.setLoop(true);
    mainSound.setVolume(30);
    mainSound.play();

    //// Cameras:
    camList.resize(camList.capacity());
    camList[0] = Camera::Create("camera0", fov, float(width) / float(height), near, far);
    camList[1] = Camera::Create(" ", fov, float(width+100) / float(height), near, far);
    root->AddChild(camList[1] );

    camList[0]->RotateByDegree(-90, Axis::X);
    camList[0]->Translate(50, Axis::Y);
    camList[0]->Translate(5, Axis::X);

    camList[1]->Translate(0.8, Axis::Y);
    camList[1]->Translate(-0.2, Axis::X);

    camList[1]->RotateByDegree(90, Axis::Y);
    camera = camList[1];

    //// Textures :
//    auto bricks{std::make_shared<Material>("bricks", program)};
//    auto grass{std::make_shared<Material>("grass", program)};
    auto daylight{std::make_shared<Material>("daylight", "shaders/cubemapShader")};

//    bricks->AddTexture(0, "textures/box0.bmp", 2);
//    grass->AddTexture(0, "textures/box0.bmp", 2);
    daylight->AddTexture(0, "textures/cubemaps/Box_", 3);

    //// Background:
    auto background{Model::Create("background", Mesh::Cube(), daylight)};
    AddChild(background);

    //// Snake:
    ////first cyl:
    Cyl cur(Model::Create("first", Mesh::Cylinder(), sponge));
    cyls.push_back( cur);
    initSnakeRotation =cyls[0].getCyl()->GetRotation();
    cyls[0].getCyl()->Scale(1,Axis::X);
    root->AddChild(cyls[0].getCyl());
    ////1-16 cyls:
    for(int i = 1;i < 16; i++)
    {
        Cyl cur(Model::Create("cylinder", Mesh::Cylinder(), sponge));
        cyls.push_back( cur);
        cyls[i].getCyl()->Translate(1.6f,Axis::X);
        cyls[i].getCyl()->Rotate(cyls[i-1].getCyl()->GetRotation());
        cyls[i].getCyl()->Translate(cyls[i-1].getCyl()->GetTranslation());
        root->AddChild(cyls[i].getCyl());
    }
    cyls[0].getCyl()->AddChild(camList[1]);

    ////init tree for collision:
    initSnakeTree();

    //// Fruits:
    sphereMesh=ObjLoader::MeshFromObjFiles("sphereMesh", "data/sphere.obj");

    //// Blue fruits- magnets
    for(int i=0; i<3; i++){
        Fruit f (Model::Create("sphere1", sphereMesh, material, Eigen::RowVector4f(0.0f,1.0f, 5.0f, 0.9f)), "blue");
        f.getModel()->Scale(8);
        fruits.push_back(f);
    }
    //// Yellow fruits- score
    for(int i=0; i<10; i++){
        Fruit f (Model::Create("sphere1", sphereMesh, material, Eigen::RowVector4f(0.5f,0.0f,0.5f,1.0f)), "yellow");
        f.getModel()->Scale(8);
        fruits.push_back(f);
    }
    //// Red fruits- win
    for(int i=0; i<1; i++){
        Fruit f (Model::Create("sphere1", sphereMesh, material, Eigen::RowVector4f(0.7f,0.0f, 0.0f, 0.9f)), "red");
        f.getModel()->Scale(8);
        fruits.push_back(f);
    }
    //// Black fruits- bomb
    for(int i=0; i<1; i++){
        Fruit f (Model::Create("sphere1", sphereMesh, material, Eigen::RowVector4f(0.0f,0.0f, 0.0f, 0.9f)), "black", level*0.1);
        f.getModel()->Scale(8);
        fruits.push_back(f);
    }
    //// Green fruits- immunity
    for(int i=0; i<3; i++){
        Fruit f (Model::Create("sphere1", sphereMesh, material, Eigen::RowVector4f(0.0f,1.0f, 0.0f, 0.9f)), "green");
        f.getModel()->Scale(8);
        fruits.push_back(f);
    }


    //// Placing the fruits randomly

    for(int i=0;i<fruits.size();i++){
        placeFruits(fruits[i]);
    }

    //// Adding all fruits to the root:
    for(int i=0;i<fruits.size();i++){
        root->AddChild(fruits[i].getModel());
    }

    ////    Skinning
//    snakeMesh = {IglLoader::MeshFromFiles("snake", "textures/snake2.obj")};
//    snake= Model::Create("snake", snakeMesh, material);
//    skinnedSnake=std::make_shared<SkinnedSnake>(SkinnedSnake(snake, cyls));
//    skinnedSnake->applySkin();
//    root->AddChild(snake);

    background->Scale(limits, Axis::XYZ);
    background->SetPickable(false);
    background->SetStatic();

}

/// Update that's runs by thread every 30 milliseconds:
void SceneWithCameras::ourUpdate(){
    if(playing){

        if(end_counter==cyls.size()){
            turn= false;
            fullTurnAction= false;
        }
        if(start_counter==cyls.size()){
            turn=true;
        }

        ////move cyls[1]-cyls[n]
        if(turn || stopturn){////evryone is turning
            for(int i = (cyls.size()-1);i>0;i--){
                Eigen::Matrix4f trans=cyls[i-1].getCyl()->GetTransform();
                cyls[i].getCyl()->SetTransform(trans);
            }
        }else{
            if(start_counter>cyls.size()){////evryone goinig straight
                for(int i = (cyls.size()-1);i>0;i--){
                    Eigen::Matrix3f system= cyls[i].getCyl()->GetRotation();
                    cyls[i].getCyl()->Translate(system* Eigen::Vector3f(-1.5f*speedFactor,0,0));
                }
            }else{////some are turning and some are going straight
                for(int i = start_counter;i>0;i--){//// first i cyls - going round (start to 0)
                    Eigen::Matrix4f trans=cyls[i-1].getCyl()->GetTransform();
                    cyls[i].getCyl()->SetTransform(trans);
                }
                for(int i = start_counter+1;i<cyls.size();i++){ /////tail- going straight (start+1 to size)
                    Eigen::Matrix3f system= cyls[i].getCyl()->GetRotation();
                    cyls[i].getCyl()->Translate(system* Eigen::Vector3f(-1.5f*speedFactor,0,0));
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
            if (curModel->GetTranslation().x() > (limits / 2)-10 || curModel->GetTranslation().x() < (-limits / 2)+10) {
                fruits[i].setVelocity(Eigen::Vector3f(-curVeloc.x(), curVeloc.y(), curVeloc.z()));
            }
            if (curModel->GetTranslation().z() > (limits / 2)-10  || curModel->GetTranslation().z() < (-limits / 2)+10) {
                fruits[i].setVelocity(Eigen::Vector3f(curVeloc.x(), curVeloc.y(), -curVeloc.z()));
            }
            if (curModel->GetTranslation().y() > (limits / 2)-10  || curModel->GetTranslation().y() < (-limits / 2)+10) {
                fruits[i].setVelocity(Eigen::Vector3f(curVeloc.x(), -curVeloc.y(), curVeloc.z()));
            }
            if(fruits[i].getColor()=="red"){
                curModel->Translate(fruits[i].getVelocity()*level);////moving the red fruit- change by level

            }
            else{
                curModel->Translate(fruits[i].getVelocity());////moving the fruits

            }
        }


        ////check collision with balls
        collidingSnakeWithBall();


        ////check if the snack is going out of limit
        if (cyls[0].getCyl()->GetTranslation().x() > (limits / 2)-10  || cyls[0].getCyl()->GetTranslation().x() < (-limits / 2)+10) {
            playing= false;
            lose= true;
            tryagain.play();
        }
        if (cyls[0].getCyl()->GetTranslation().z() > (limits / 2)-10  || cyls[0].getCyl()->GetTranslation().z() < (-limits / 2)+10) {
            playing=false;
            lose=true;
            tryagain.play();
        }
        if (cyls[0].getCyl()->GetTranslation().y() > (limits / 2)-10 || cyls[0].getCyl()->GetTranslation().y() < (-limits / 2)+10) {
            playing=false;
            lose=true;
            tryagain.play();
        }

        //// Handle magnet boost timer
        if(magnetTimer>0){
            magnetTimer=magnetTimer-15;
            if(magnetTimer==0) {
                scale=0.5;
            }
        }

        //// Check self collision of the snake
        for(int i=14;i<cyls.size();i++){
            if(isSnakeCollide(i)){
                playing=false;
                lose=true;
                tryagain.play();
            }
        }

        ///// skinning
//        skinnedSnake->applySkin();
    }
}


void SceneWithCameras::reset(int j){
    level=level+j;
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

    for(int i = cyls.size()-1;i > 0; i--) {
        Eigen::Matrix4f trans=cyls[0].getCyl()->GetTransform();
        cyls[i].getCyl()->SetTransform(trans);
        cyls[i].getCyl()->Rotate(cyls[i].getCyl()->GetRotation().transpose());
        cyls[i].getCyl()->Translate(1.6f , Axis::X);
        cyls[i].getCyl()->Translate(cyls[i - 1].getCyl()->GetTranslation());
    }

    ///add bomb
    for(int i=0; i<j; i++){
        Fruit f (Model::Create("sphere1", sphereMesh, material, Eigen::RowVector4f(0.0f,0.0f, 0.0f, 0.9f)), "black");
        f.getModel()->Scale(8);
        fruits.push_back(f);
        placeFruits(f);
        root->AddChild(f.getModel());
    }

    playing=true;
    lose=false;
    win=false;
    fullTurnAction= false;
    ////Reset powerups
    imunity= false;
    magnetTimer=0;
    scale=0.5;

}


//////////////////////////////////////////////////////////////////////
/////////////////////// Side functions: /////////////////////////////
/////////////////////////////////////////////////////////////////////


bool SceneWithCameras::findSmallestBox(igl::AABB<Eigen::MatrixXd,3> tree1,igl::AABB<Eigen::MatrixXd,3> tree2,Fruit f){

    if(isCollide(f)) {
        if (tree1.is_leaf() && tree2.is_leaf()) {
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


void SceneWithCameras::initSnakeTree(){
    auto mesh = cyls[0].getCyl()->GetMeshList();
    Eigen::MatrixXd V1 = mesh[0]->data[0].vertices;
    Eigen::MatrixXi F1 = mesh[0]->data[0].faces;
    snakeTree.init(V1,F1);
}

///catching a ball:
void SceneWithCameras::collidingSnakeWithBall(){
    for(int i=0; i<fruits.size(); i++){
        if(findSmallestBox(snakeTree,fruits[i].getTree(), fruits[i])){
            placeFruits(fruits[i]);
            bite.setVolume(100);
            addCyl();

            //// Score balls.
            if(fruits[i].getColor()=="yellow"){
                money.play();
                levelSCore=levelSCore+10;
            }
            //// Magnet balls.
            if(fruits[i].getColor()=="blue"){
                scale=2;
                magnetTimer=3000;
                bite.play();
            }
            //// Winning ball
            if(fruits[i].getColor()=="red"){
                playing= false;
                win=true;
                victory.play();
            }
            //// Bomb ball
            if(fruits[i].getColor()=="black"){
                if(!imunity){
                    playing= false;
                    lose=true;
                    tryagain.play();
                } else{
                    money.play();
                    imunity= false;
                    levelSCore+=50;
                    catchbombwithimunuty = true;
                    bombBonusCounter =0;
                }
            }
            //// imunity
            if(fruits[i].getColor()=="green"){
                imunity=true;
                bite.play();
            }
        }
    }
}

void SceneWithCameras::Update(const Program& p, const Eigen::Matrix4f& proj, const Eigen::Matrix4f& view, const Eigen::Matrix4f& model)
{
    Scene::Update(p, proj, view, model);
}

void SceneWithCameras::KeyCallback(Viewport* _viewport, int x, int y, int key, int scancode, int action, int mods) {
    Eigen::Matrix3f system = cyls[0].getCyl()->GetRotation().transpose();
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {

        if (key == GLFW_KEY_SPACE){
            addCyl();
        }

        // keys 1-9 are objects 1-9 (objects[0] - objects[8]), key 0 is object 10 (objects[9])
        if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9) {
            if (int index; (index = (key - GLFW_KEY_1 + 10) % 10) < camList.size())
                SetCamera(index);
        }
        if (key == GLFW_KEY_UP) {
            if (fullTurnAction) {
                stopturn = true;
                while (!cyls[0].isTranslationEmpty())
                    cyls[0].getTranslation();
                while (!cyls[0].isRotationEmpty())
                    cyls[0].getRotation();
            }
            if ( playing) {
                loadTurn(false, true);
            }
        }
        if (key == GLFW_KEY_DOWN) {
            if (fullTurnAction) {
                stopturn = true;
                while (!cyls[0].isTranslationEmpty())
                    cyls[0].getTranslation();
                while (!cyls[0].isRotationEmpty())
                    cyls[0].getRotation();
            }
            if ( playing) {
                loadTurn(true, true);
            }
        }
        if (key == GLFW_KEY_RIGHT) {
            if (fullTurnAction) {
                stopturn = true;
                while (!cyls[0].isTranslationEmpty())
                    cyls[0].getTranslation();
                while (!cyls[0].isRotationEmpty())
                    cyls[0].getRotation();
            }
            if (playing) {
                loadTurn(false, false);
            }
        }
        if (key == GLFW_KEY_LEFT) {
            if (fullTurnAction) {
                stopturn = true;
                while (!cyls[0].isTranslationEmpty())
                    cyls[0].getTranslation();
                while (!cyls[0].isRotationEmpty())
                    cyls[0].getRotation();
            }
            if (playing) {
                loadTurn(true, false);
            }
        }
    }
    SceneWithImGui::KeyCallback(nullptr, x, y, key, scancode, action, mods);
}

//// Gives cyl[0] list of movements to do in the next updates (using Bezier)
void SceneWithCameras::loadTurn(bool direction, bool isAxisZ) {
    float dir = -1.0;
    if (direction) {
        dir = 1.0;
    }
    float timeIntervals = 40.0;
    float time = 1.0f / (timeIntervals * fac);
    if (isAxisZ) { ////down
        cyls[0].setTranslation(Eigen::Vector3f(5, 5, 5));
        for (float i = 1.0f; i <= (timeIntervals * fac) / 10; i = i + 1.0f) {
            Eigen::Vector3f p1 = cyls[0].getCyl()->GetTranslation();
            Eigen::Vector3f p2 = p1 + Eigen::Vector3f(-2 * fac, 0, 0);
            Eigen::Vector3f p3 = p1 + Eigen::Vector3f(-2 * fac, dir * 0.1 * fac, 0);
            float cur_time = i / (timeIntervals * fac);
            cyls[0].setTranslation(getBezierPosition(cur_time, p1, p2, p3) - p1);
            cyls[0].setRotation(Eigen::Vector2f(3.0, 1.57079633f * time * dir));
        }
        cyls[0].setTranslation(Eigen::Vector3f(7, 7, 7));

    } else {////right
        cyls[0].setTranslation(Eigen::Vector3f(5, 5, 5));
        for (float i = 1.0f; i <= (timeIntervals * fac) / 10; i = i + 1.0f) {
            Eigen::Vector3f p1 = cyls[0].getCyl()->GetTranslation();
            Eigen::Vector3f p2 = p1 + Eigen::Vector3f(-2 * fac, 0, 0);
            Eigen::Vector3f p3 = p1 + Eigen::Vector3f(-2 * fac, 0, dir * 0.1 * fac);
            float cur_time = i / (timeIntervals * fac);
            Eigen::Vector3f trans = getBezierPosition(cur_time, p1, p2, p3) - p1;
            cyls[0].setTranslation(trans);
            cyls[0].setRotation(Eigen::Vector2f(2.0, 1.57079633f * time * dir));
        }
        cyls[0].setTranslation(Eigen::Vector3f(7, 7, 7));
    }

}

//// Calculates Bezier curve's positions:
Eigen::Vector3f SceneWithCameras::getBezierPosition(float time, Eigen::Vector3f p0, Eigen::Vector3f p1, Eigen::Vector3f p2) {
    if (time < 0 || time > 1)
        throw std::invalid_argument("Time out of bounds for curve");
    Eigen::Vector3f pos = p1 + (pow((1 - time), 2) * (p0 - p1)) + (pow(time, 2) * (p2 - p1));
    return pos;
}

////Adding a new cylinder to the snake:
void SceneWithCameras::addCyl(){
    Cyl cur(Model::Create("cylinder", Mesh::Cylinder(), sponge));
    cyls.push_back( cur);
    cyls[cyls.size()-1].getCyl()->Rotate(cyls[cyls.size()-2].getCyl()->GetRotation());
    cyls[cyls.size()-1].getCyl()->Translate(cyls[cyls.size()-2].getCyl()->GetTranslation());
    cyls[cyls.size()-1].getCyl()->Translate(1.6f,Axis::X);
    root->AddChild(cyls[cyls.size()-1].getCyl());
}

    //// Place fruits randomly:
    void SceneWithCameras::placeFruits(Fruit f) {
        offset = -(limits / 2 )+20;
        range = (limits / 2) -20  - offset + 1;
        scale = 2;
        float x = (rand() % range + offset);
        float y = (rand() % range + offset);
        float z = (rand() % range + offset);

        f.getModel()->Translate(Eigen::Vector3f(x, y, z));

        while (isCollide(f)) {
            f.getModel()->Translate(Eigen::Vector3f(-x, -y, -z));
            float x = (rand() % range + offset);
            float y = (rand() % range + offset); // should be random too
            float z = (rand() % range + offset);

            f.getModel()->Translate(Eigen::Vector3f(x, y, z));
        }
        scale = 0.5;
    }



////Snake with a ball:
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


////Self collision of snake:
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

    double a0=box1.sizes()[0]*0.5/2;
    double a1=box1.sizes()[1]*0.5/2;
    double a2=box1.sizes()[2]*0.5/2;

    double b0=box2.sizes()[0]*0.5/2;
    double b1=box2.sizes()[1]*0.5/2;
    double b2=box2.sizes()[2]*0.5/2;

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



void SceneWithCameras::ViewportSizeCallback(Viewport *_viewport) {
    for (auto &cam: camList)
        cam->SetProjection(float(_viewport->width) / float(_viewport->height));

    // note: we don't need to call Scene::ViewportSizeCallback since we are setting the projection of all the cameras
}

void SceneWithCameras::AddViewportCallback(Viewport *_viewport) {
    viewport = _viewport;

    Scene::AddViewportCallback(viewport);
}

void SceneWithCameras::LoadObjectFromFileDialog()
{
    std::string filename = igl::file_dialog_open();
    if (filename.length() == 0) return;

    auto shape = Model::Create(filename, carbon);
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

void SceneWithCameras::DumpMeshData(const Eigen::IOFormat& simple, const MeshData& data)
{
    std::cout << "vertices mesh: " << data.vertices.format(simple) << std::endl;
    std::cout << "faces mesh: " << data.faces.format(simple) << std::endl;
    std::cout << "vertex normals mesh: " << data.vertexNormals.format(simple) << std::endl;
    std::cout << "texture coordinates mesh: " << data.textureCoords.format(simple) << std::endl;
}