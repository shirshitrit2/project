#pragma once

#include "SceneWithImGui.h"
#include "CamModel.h"
#include "igl/AABB.h"
#include "Fruit.h"


class SceneWithCameras : public cg3d::SceneWithImGui
{
private:
    std::shared_ptr<Movable> root;
    std::shared_ptr<cg3d::Material> carbon;
    //bool animate = false;
    void BuildImGui() override;
    std::vector<std::shared_ptr<cg3d::Camera>> camList{2};
    std::shared_ptr<cg3d::Model> cube1, cube2, cylinder, sphere1, sphere2,snake;
    cg3d::Viewport* viewport = nullptr;
    std::vector<std::shared_ptr<cg3d::Model>> cyls;
//    std::vector<std::shared_ptr<cg3d::Model>> yellowSpheres;
//    std::vector<std::shared_ptr<cg3d::Model>> blueSpheres;
//    std::vector<Eigen::Vector3f> yellowVelocities;
//    std::vector<Eigen::Vector3f> blueVelocities;
//    float yellowVelocity = 0.05;
//    float blueVelocity =0.01 ;

    int offset = 0;
    int range =  0;
//    std::vector<igl::AABB<Eigen::MatrixXd,3>> yellowTree;
//    std::vector<igl::AABB<Eigen::MatrixXd,3>> blueTree;
    std::vector<Fruit> fruits;
    igl::AABB<Eigen::MatrixXd,3> snakeTree;
    float scaleFactor = 0.5;
    int scoreCounter=0;
    double scale=0.5;
    std::shared_ptr<cg3d::Model> currSphere;
    bool win=false;
    bool lose=false;
    int limits=60;
    int speedTimer=0;
    int level=0;



public:

    SceneWithCameras(std::string name, cg3d::Display* display);
    void Init(float fov, int width, int height, float near, float far);
    void Update(const cg3d::Program& program, const Eigen::Matrix4f& proj, const Eigen::Matrix4f& view, const Eigen::Matrix4f& model) override;
    void KeyCallback(cg3d::Viewport* _viewport, int x, int y, int key, int scancode, int action, int mods) override;
    void AddViewportCallback(cg3d::Viewport* _viewport) override;
    void ViewportSizeCallback(cg3d::Viewport* _viewport) override;
    bool isSnakeCollide(int i);

private:
    //inline bool IsActive() const { return animate; };
    //inline void SetActive(bool _isActive = true) { animate = _isActive; }
//    virtual bool isActive() const  ;
//
//    virtual void setActive(bool animate);;
    void LoadObjectFromFileDialog();
    void SetCamera(int index);
//    static std::shared_ptr<CamModel> CreateCameraWithModel(int width, int height, float fov, float near, float far, const std::shared_ptr<cg3d::Material>& material);
    static void DumpMeshData(const Eigen::IOFormat& simple, const cg3d::MeshData& data) ;
//    Eigen::Vector3f findVelocity(float factor);
    bool findSmallestBox(Fruit f);
    bool findSmallestBox(igl::AABB<Eigen::MatrixXd,3> tree1,igl::AABB<Eigen::MatrixXd,3> tree2,Fruit f);
    bool isCollide(Fruit f);
    void initSnakeTree();
    void collidingBalls();
    void collidingSnakeWithBall();
    void handleSound();

};

