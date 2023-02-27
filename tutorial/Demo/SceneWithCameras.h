#pragma once

#include "SceneWithImGui.h"
#include "CamModel.h"
#include "igl/AABB.h"
#include "Fruit.h"
#include "Cyl.h"
#include "imgui.h"
#include "SkinnedSnake.h"
#include <SFML/Audio.hpp>



class SceneWithCameras : public cg3d::SceneWithImGui
{

private:

    std::shared_ptr<Movable> root;
    std::shared_ptr<cg3d::Material> sponge,carbon;
    std::vector<std::shared_ptr<cg3d::Camera>> camList{2};
    std::shared_ptr<cg3d::Model> cylinder,snake,currSphere;
    std::shared_ptr<cg3d::Program> program=std::make_shared<cg3d::Program>("shaders/phongShader");;
    std::shared_ptr<cg3d::Program> program1 = std::make_shared<cg3d::Program>("shaders/basicShader"); //
    std::shared_ptr<cg3d::Material> material;
    std::shared_ptr<cg3d::Mesh> sphereMesh, snakeMesh;

    cg3d::Viewport* viewport = nullptr;

    std::vector<Cyl> cyls;
    std::vector<Fruit> fruits;

    igl::AABB<Eigen::MatrixXd,3> snakeTree;

    bool win=false;
    bool lose=false;
    bool stopturn=false;
    bool fullTurnAction=false;
    bool turn =false;
    bool playing=false;
    bool imunity = false;
    bool catchbombwithimunuty= false;

    int limits=200;
    int magnetTimer=0;
    int level=1;
    int end_counter=180;
    int start_counter= 180;
    int offset = 0;
    int range =  0;
    int scoreCounter=0;
    int bombBonusCounter =112;
    int levelSCore=0;


    float fac=0.5;
    double scale=0.5;

    sf::SoundBuffer mainBuffer;
    sf::SoundBuffer biteBuffer, moneybuffer, trybuffer, readybuffer, victorybuffer;
    sf::Sound bite, money, tryagain, ready, victory ,mainSound;

    Eigen::Matrix3f initSnakeRotation;
    ImTextureID texture_id;
    std::shared_ptr<SkinnedSnake> skinnedSnake;


public:
    SceneWithCameras(std::string name, cg3d::Display* display);
    void Init(float fov, int width, int height, float near, float far);
    void Update(const cg3d::Program& program, const Eigen::Matrix4f& proj, const Eigen::Matrix4f& view, const Eigen::Matrix4f& model) override;
    void KeyCallback(cg3d::Viewport* _viewport, int x, int y, int key, int scancode, int action, int mods) override;
    void AddViewportCallback(cg3d::Viewport* _viewport) override;
    void ViewportSizeCallback(cg3d::Viewport* _viewport) override;
    bool isSnakeCollide(int i);
    void loadTurn(bool direction, bool isAxisZ);
    void addCyl();

    Eigen::Vector3f getBezierPosition(float time, Eigen::Vector3f p0, Eigen::Vector3f p1,Eigen::Vector3f p2);
private:
    void LoadObjectFromFileDialog();
    void SetCamera(int index);
    static void DumpMeshData(const Eigen::IOFormat& simple, const cg3d::MeshData& data) ;
    bool findSmallestBox(igl::AABB<Eigen::MatrixXd,3> tree1,igl::AABB<Eigen::MatrixXd,3> tree2,Fruit f);
    bool isCollide(Fruit f);
    void initSnakeTree();
    void collidingSnakeWithBall();
    void placeFruits(Fruit f);
    void ourUpdate();
    void reset(int j);
    void BuildImGui() override;

};

