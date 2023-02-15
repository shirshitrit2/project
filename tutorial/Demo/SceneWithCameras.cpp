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


using namespace cg3d;

void SceneWithCameras::BuildImGui()
{
    int flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
    bool* pOpen = nullptr;

    ImGui::Begin("Menu", pOpen, flags);
    ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetWindowSize(ImVec2(0, 0), ImGuiCond_Always);
    if (ImGui::Button("Load object"))
        LoadObjectFromFileDialog();

    ImGui::Text("Camera: ");
    for (int i = 0; i < camList.size(); i++) {
        ImGui::SameLine(0);
        bool selectedCamera = camList[i] == camera;
        if (selectedCamera)
            ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
        if (ImGui::Button(std::to_string(i + 1).c_str()))
            SetCamera(i);
        if (selectedCamera)
            ImGui::PopStyleColor();
    }
    ImGui::SameLine();
    if (ImGui::Button("Center"))
        camera->SetTout(Eigen::Affine3f::Identity());
    if (pickedModel) {
        ImGui::Text("Picked model: %s", pickedModel->name.c_str());
        ImGui::SameLine();
        if (ImGui::Button("Drop"))
            pickedModel = nullptr;
        if (pickedModel) {
            if (ImGui::CollapsingHeader("Draw options", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Checkbox("Show wireframe", &pickedModel->showWireframe);
                if (pickedModel->showWireframe) {
                    ImGui::Text("Wireframe color:");
                    ImGui::SameLine();
                    ImGui::ColorEdit4("Wireframe color", pickedModel->wireframeColor.data(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
                }
                ImGui::Checkbox("Show faces", &pickedModel->showFaces);
                ImGui::Checkbox("Show textures", &pickedModel->showTextures);
                if (ImGui::Button("Scale down"))
                    pickedModel->Scale(0.9f);
                ImGui::SameLine();
                if (ImGui::Button("Scale up"))
                    pickedModel->Scale(1.1f);
            }
            if (ImGui::Button("Dump model mesh data")) {
                std::cout << "model name: " << pickedModel->name << std::endl;
                if (pickedModel->meshIndex > 0)
                    std::cout << "mesh index in use: " << pickedModel->meshIndex;
                for (auto& mesh: pickedModel->GetMeshList()) {
                    Eigen::IOFormat simple(Eigen::StreamPrecision, Eigen::DontAlignCols, ", ", ", ", "", "", "", "");
                    std::cout << "mesh name: " << mesh->name << std::endl;
                    for (int i = 0; i < mesh->data.size(); i++) {
                        if (mesh->data.size() > 1)
                            std::cout << "mesh #" << i + 1 << ":" << std::endl;
                        DumpMeshData(simple, mesh->data[i]);
                    }
                }
            }
            if (ImGui::Button("Dump model transformations")) {
                Eigen::IOFormat format(2, 0, ", ", "\n", "[", "]");
                const Eigen::Matrix4f& transform = pickedModel->GetAggregatedTransform();
                std::cout << "Tin:" << std::endl << pickedModel->Tin.matrix().format(format) << std::endl
                          << "Tout:" << std::endl << pickedModel->Tout.matrix().format(format) << std::endl
                          << "Transform:" << std::endl << transform.matrix().format(format) << std::endl
                          << "--- Transform Breakdown ---" << std::endl
                          << "Rotation:" << std::endl << Movable::GetTranslation(transform).matrix().format(format) << std::endl
                          << "Translation:" << std::endl << Movable::GetRotation(transform).matrix().format(format) << std::endl
                          << "Rotation x Translation:" << std::endl << Movable::GetTranslationRotation(transform).matrix().format(format)
                          << std::endl << "Scaling:" << std::endl << Movable::GetScaling(transform).matrix().format(format) << std::endl;
            }
        }
    }

    ImGui::End();
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


//void SceneWithCameras::colidingBalls(){
//    for(int i =0 ; i < fruits.size() ; i++){
//        for(int j =i+1 ; j <fruits.size() ; j++){
//            if(findSmallestBox(fruits[i],fruits[j])){
//                fruits[i].setVelocity(Eigen::Vector3f(-fruits[i].getVelocity().x(),0,-fruits[i].getVelocity().z()));
//                fruits[j].setVelocity(Eigen::Vector3f(-fruits[j].getVelocity().x(),0,-fruits[j].getVelocity().z()));
//            }
//        }
//    }
//}

void SceneWithCameras::colidingSnakeWithBall(){
    for(int i=0; i<fruits.size(); i++){
        if(findSmallestBox(snakeTree,fruits[i].getTree(), fruits[i])){
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
            fruits[i].getModel()->Translate(Eigen::Vector3f(-3,0,-3));
            fruits[i].setVelocity(Eigen::Vector3f(0,0,0));


        }
    }
}


void SceneWithCameras::Init(float fov, int width, int height, float near, float far)
{
    // create the basic elements of the scene
    AddChild(root = Movable::Create("root")); // a common (invisible) parent object for all the shapes
    auto program = std::make_shared<Program>("shaders/basicShader"); // TODO: TAL: replace with hard-coded basic program
    carbon = std::make_shared<Material>("carbon", program); // default material
    carbon->AddTexture(0, "textures/carbon.jpg", 2);

    // create the camera objects
    camList.resize(camList.capacity());
    camList[0] = Camera::Create("camera0", fov, float(width) / float(height), near, far);
    camList[1] = Camera::Create(" ", fov, float(width) / float(height), near, far);
    root->AddChild(camList[1] );
//    for (int i = 1; i < camList.size(); i++) {
//        auto camera = Camera::Create("", fov, double(width) / height, near, far);
//        auto model = ObjLoader::ModelFromObj(std::string("camera") + std::to_string(i), "data/camera.obj", carbon);
//        root->AddChild(camList[i] = CamModel::Create(*camera, *model));
//    }

//    camList[0]->Translate(10, Axis::Z);
    camList[0]->RotateByDegree(-90, Axis::X);
    camList[0]->Translate(20, Axis::Y);
    camList[0]->Translate(5, Axis::X);

    camList[1]->Translate(-3, Axis::X);
    camList[1]->RotateByDegree(-90, Axis::Y);
//    camList[2]->Translate(-8, Axis::Z);
//    camList[2]->RotateByDegree(180, Axis::Y);
//    camList[3]->Translate(3, Axis::X);
//    camList[3]->RotateByDegree(90, Axis::Y);
    camera = camList[0];

    auto bricks{std::make_shared<Material>("bricks", program)};
    auto grass{std::make_shared<Material>("grass", program)};
    auto daylight{std::make_shared<Material>("daylight", "shaders/cubemapShader")};

    bricks->AddTexture(0, "textures/bricks.jpg", 2);
    grass->AddTexture(0, "textures/grass.bmp", 2);
    daylight->AddTexture(0, "textures/cubemaps/Daylight Box_", 3);

    auto background{Model::Create("background", Mesh::Cube(), daylight)};
    AddChild(background);

//    cube1 = Model::Create("cube1", Mesh::Cube(), bricks);
//    cube2 = Model::Create("cube2", Mesh::Cube(), bricks);
//    cube1->Translate({-3, 0, -5});
//    cube2->Translate({3, 0, -5});
//    root->AddChildren({cube1, cube2});

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
    cyls.push_back( Model::Create("cylinder", Mesh::Cylinder(), grass));
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

    auto sphereMesh{ObjLoader::MeshFromObjFiles("sphereMesh", "data/sphere.obj")};
//    yellowSpheres.push_back( Model::Create("sphere1", sphereMesh, grass));
//    blueSpheres.push_back( Model::Create("sphere1", sphereMesh, bricks));
//    cylinder->AddChildren({sphere1, sphere2});
    Fruit a (Model::Create("sphere1", sphereMesh, grass), "blue") ;
    Fruit b (Model::Create("sphere1", sphereMesh, grass), "blue") ;
    Fruit c (Model::Create("sphere1", sphereMesh, grass), "blue") ;
    Fruit d (Model::Create("sphere1", sphereMesh, bricks), "yellow") ;
    Fruit e (Model::Create("sphere1", sphereMesh, bricks), "yellow") ;
    Fruit f (Model::Create("sphere1", sphereMesh, bricks), "yellow") ;

    fruits.push_back(a);
    fruits.push_back(b);
    fruits.push_back(c);
    fruits.push_back(d);
    fruits.push_back(e);
    fruits.push_back(f);


    float x1 = (rand() % range + offset)/10;
    float z1 = (rand() % range + offset)/10;
    float x2 = (rand() % range + offset)/10;
    float z2 = (rand() % range + offset)/10;
    float x3 = (rand() % range + offset)/10;
    float z3 = (rand() % range + offset)/10;
    float x4 = (rand() % range + offset)/10;
    float z4 = (rand() % range + offset)/10;
    float x5 = (rand() % range + offset)/10;
    float z5 = (rand() % range + offset)/10;
    float x6 = (rand() % range + offset)/10;
    float z6 = (rand() % range + offset)/10;

    fruits[0].getModel()->Translate(Eigen::Vector3f(x1,0,z1));
    fruits[1].getModel()->Translate(Eigen::Vector3f(x2,0,z2));
    fruits[2].getModel()->Translate(Eigen::Vector3f(x3,0,z3));
    fruits[3].getModel()->Translate(Eigen::Vector3f(x4,0,z4));
    fruits[4].getModel()->Translate(Eigen::Vector3f(x5,0,z5));
    fruits[5].getModel()->Translate(Eigen::Vector3f(x6,0,z6));




//    yellowVelocities.push_back(findVelocity(yellowVelocity));
//    blueVelocities.push_back(findVelocity(blueVelocity));
//    yellowSpheres[0]->showWireframe = true;
//    blueSpheres[0]->showWireframe = true;


    root->AddChild(fruits[0].getModel());
    root->AddChild(fruits[1].getModel());
    root->AddChild(fruits[2].getModel());
    root->AddChild(fruits[3].getModel());
    root->AddChild(fruits[4].getModel());
    root->AddChild(fruits[5].getModel());


//    currSphere= Model::Create("sphere1", sphereMesh, grass);
//    currSphere->Translate(Eigen::Vector3f(4,0,0));
//    currSphere->Scale(1.5f,Axis::X);

//    sphere1->isPickable = false;
//    sphere2->isPickable = false;
//    cylinder->Translate({0, -3, -5});
//    cylinder->showWireframe = true;


//    initTrees();
//    initTrees(blueTree, blueSpheres);


    background->Scale(60, Axis::XYZ);
    background->SetPickable(false);
    background->SetStatic();
}

void SceneWithCameras::Update(const Program& p, const Eigen::Matrix4f& proj, const Eigen::Matrix4f& view, const Eigen::Matrix4f& model)
{
    Scene::Update(p, proj, view, model);
    if (animate) {
        cube1->Rotate(0.003f, {1, 1, 0});
        cube2->Rotate(0.003f, {0, 1, 1});
        camList[0]->Rotate(0.001f, Axis::Y);
    }
//    std::cout<<"x"<<yellowSpheres[0]->GetTranslation().x()<<std::endl;
//    std::cout<<"z"<<yellowSpheres[0]->GetTranslation().x()<<std::endl;
    for(int i = 0;i <fruits.size(); i++)
    {
        std::shared_ptr<cg3d::Model> curModel = fruits[i].getModel();
        Eigen::Vector3f curVeloc = fruits[i].getVelocity();
        if(curModel->GetTranslation().x()> 30 ||curModel->GetTranslation().x()<-30){
            fruits[i].setVelocity(Eigen::Vector3f(-curVeloc.x(),0,curVeloc.z()));
        }
        if(curModel->GetTranslation().z()> 30 ||curModel->GetTranslation().z()<-30){
            fruits[i].setVelocity(Eigen::Vector3f(curVeloc.x(),0,-curVeloc.z()));
        }
        curModel->Translate(fruits[i].getVelocity());
    }
    colidingSnakeWithBall();

}

void SceneWithCameras::LoadObjectFromFileDialog()
{
    std::string filename = igl::file_dialog_open();
    if (filename.length() == 0) return;

    auto shape = Model::Create(filename, carbon);
}

void SceneWithCameras::KeyCallback(Viewport* _viewport, int x, int y, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        if (key == GLFW_KEY_SPACE)
            SetActive(!IsActive());

        // keys 1-9 are objects 1-9 (objects[0] - objects[8]), key 0 is object 10 (objects[9])
        if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9) {
            if (int index; (index = (key - GLFW_KEY_1 + 10) % 10) < camList.size())
                SetCamera(index);
        }
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
