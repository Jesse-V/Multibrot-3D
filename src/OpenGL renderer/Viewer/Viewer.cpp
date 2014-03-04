
/******************************************************************************\
                     This file is part of Multibrot Renderer,
          a program that displays 3D views of the Multibrot fractal

                      Copyright (c) 2013, Jesse Victors

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see http://www.gnu.org/licenses/

                For information regarding this software email:
                                Jesse Victors
                         jvictors@jessevictors.com
\******************************************************************************/

#define _GLIBCXX_USE_NANOSLEEP

#include "Viewer.hpp"
#include "Modeling/DataBuffers/SampledBuffers/Image.hpp"
#include "Modeling/DataBuffers/SampledBuffers/TexturedCube.hpp"
#include "Modeling/DataBuffers/ColorBuffer.hpp"
#include "Options.hpp"
#include <thread>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <iostream>


Viewer::Viewer() :
    scene_(std::make_shared<Scene>(createCamera())),
    user_(std::make_shared<User>(scene_)),
    timeSpentRendering_(0), frameCount_(0), needsRerendering_(true)
{
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);

    addModels();
    user_->grabPointer();
    reportFPS();
}



void Viewer::reportFPS()
{
    std::thread fpsReporter([&]()
    {
        std::cout << "Note: FPS will be low when the camera is still." << std::endl;
        while (true)
        {
            std::this_thread::sleep_for(std::chrono::seconds(2));

            glm::vec3 cameraPos = scene_->getCamera()->getPosition();
            std::cout << frameCount_ / 2 << " FPS, spent " <<
                timeSpentRendering_ / 2 << " ms rendering. <" << cameraPos.x <<
                ", " << cameraPos.y << ", " << cameraPos.z << ">" << std::endl;

            frameCount_ = 0;
            timeSpentRendering_ = 0;
        }
    });

    fpsReporter.detach();
}



void Viewer::addModels()
{
    addBellCurveBlocks();
    addFractal();
}



void Viewer::addBellCurveBlocks()
{
    static const float SPREAD = 0.005, HEIGHT = 15;
    static const int RES = 25;

    auto dirt      = std::make_shared<Image>("images/dirt.png");
    auto grassTop  = std::make_shared<Image>("images/grass_top.png");

    auto side1 = std::make_shared<Image>("images/grass_side.png", false, false, true);
    auto side2 = std::make_shared<Image>("images/grass_side.png", true, false, true);
    auto side3 = std::make_shared<Image>("images/grass_side.png", false, true);
    auto side4 = std::make_shared<Image>("images/grass_side.png");

    BufferList list = { std::make_shared<TexturedCube>(side1, side2,
        side3, side4, grassTop, dirt) };

    auto blocks = std::make_shared<InstancedModel>(
                    getExternalFacingCube(), list);

    for (int x = -RES; x < RES; x++)
    {
        for (int y = -RES; y < RES; y++)
        {
            float dSq = x * x + y * y;
            float z = HEIGHT / pow(2.718282f, SPREAD * dSq);
            z = (int)z;

            auto loc = glm::vec3(x + RES, y + RES, -z) + glm::vec3(0.5f);
            auto matrix = glm::translate(glm::mat4(), loc);
            blocks->addInstance(matrix);
        }
    }

    scene_->addModel(blocks); //add to Scene and save
}



void Viewer::addFractal()
{
    static const auto SCALE = glm::vec3(1 / 64.0f);
    static const auto OFFSET = glm::vec3(25, 25, -4) / SCALE - glm::vec3(0.02f);
    static const auto BOX_SCALE = glm::vec3(0.99f);

    auto geometry = readGeometry("geometry.dat");
    auto boxModels = getBoxModels();

    long count = 0;
    for (auto rectangle : geometry)
    {
        glm::vec3 min = glm::vec3(rectangle[1], rectangle[2], rectangle[3]);
        glm::vec3 max = glm::vec3(rectangle[4], rectangle[5], rectangle[6]);
        glm::vec3 size = max - min;
        float minDimSize = std::min(size.x, std::min(size.y, size.z));

        for (auto model : boxModels)
        {
            if ((int)minDimSize == model.first)
            {
                auto matrix = glm::scale(glm::mat4(), SCALE);
                matrix      = glm::translate(matrix, min - glm::vec3(512) + OFFSET);
                matrix      = glm::scale(matrix, (max - min) * BOX_SCALE);
                matrix      = glm::translate(matrix, glm::vec3(0.5f));
                model.second->addInstance(matrix);
                count++;
                break;
            }
        }
    }

    std::cout << "Instance count: " << count << std::endl;

    for (auto model : boxModels)
        scene_->addModel(model.second); //add to Scene and save
}



std::vector<std::vector<int>> Viewer::readGeometry(const std::string& filename)
{
    std::ifstream file;
    file.open(filename, std::ifstream::in);
    if (file.fail())
        std::cout << "Unable to open geometry file!" << std::endl;

    std::string line;
    std::vector<std::vector<int>> geometry;
    while (getline(file, line))
    {
        std::istringstream is(line);
        geometry.push_back(
            std::vector<int>(std::istream_iterator<int>(is),
            std::istream_iterator<int>()
        ));
    }

    file.close();

    std::cout << "Read " << geometry.size() << " objects from file." << std::endl;

    return geometry;
}



std::vector<ColoredCube> Viewer::getBoxModels()
{
    std::vector<std::pair<int, glm::vec3>> boxColors;
    boxColors.push_back(std::make_pair(128, glm::vec3(1, 0, 0)));
    boxColors.push_back(std::make_pair(64, glm::vec3(0, 1, 0)));
    boxColors.push_back(std::make_pair(32, glm::vec3(0, 0, 1)));
    boxColors.push_back(std::make_pair(16, glm::vec3(0, 0.5, 1)));
    boxColors.push_back(std::make_pair(8, glm::vec3(0, 1, 0.5)));
    boxColors.push_back(std::make_pair(4, glm::vec3(0.05, 0.05, 0.05)));
    boxColors.push_back(std::make_pair(2, glm::vec3(1, 1, 1)));

    auto mesh = getExternalFacingCube();
    std::vector<ColoredCube> cubes;
    for (auto box : boxColors)
    {
        BufferList list = { std::make_shared<ColorBuffer>(box.second, 8) };
        auto model = std::make_shared<InstancedModel>(mesh, list);
        cubes.push_back(std::make_pair(box.first, model));
    }

    return cubes;
}



void Viewer::addSkybox()
{
    std::cout << "Creating skybox..." << std::endl;

    auto image = std::make_shared<Image>(
        Options::getInstance().getSkyboxPath());

    BufferList list = { std::make_shared<TexturedCube>(image, image,
        image, image, image, image) };
    auto matrix = glm::scale(glm::mat4(), glm::vec3(4096));
    auto model = std::make_shared<InstancedModel>(
                                getInternalFacingCube(), matrix, list);
    scene_->addModel(model); //add to Scene and save

    std::cout << "... done creating skybox." << std::endl;
}



std::shared_ptr<Mesh> Viewer::getInternalFacingCube()
{
    static std::shared_ptr<Mesh> mesh = nullptr;

    if (mesh)
        return mesh;

    const std::vector<glm::vec3> VERTICES = {
        glm::vec3(-0.5, -0.5, -0.5),
        glm::vec3(-0.5, -0.5,  0.5),
        glm::vec3(-0.5,  0.5, -0.5),
        glm::vec3(-0.5,  0.5,  0.5),
        glm::vec3( 0.5, -0.5, -0.5),
        glm::vec3( 0.5, -0.5,  0.5),
        glm::vec3( 0.5,  0.5, -0.5),
        glm::vec3( 0.5,  0.5,  0.5)
    };

    //visible from the inside only, so faces in
    const std::vector<GLuint> INDICES = {
        0, 1, 5, 4, //front
        6, 7, 3, 2, //back
        2, 0, 4, 6, //top
        7, 5, 1, 3, //bottom
        2, 3, 1, 0, //left
        4, 5, 7, 6  //right
    };

    auto vBuffer = std::make_shared<VertexBuffer>(VERTICES);
    auto iBuffer = std::make_shared<IndexBuffer>(INDICES, GL_QUADS);
    mesh = std::make_shared<Mesh>(vBuffer, iBuffer, GL_QUADS);
    return mesh;
}



std::shared_ptr<Mesh> Viewer::getExternalFacingCube()
{
    static std::shared_ptr<Mesh> mesh = nullptr;

    if (mesh)
        return mesh;

    const std::vector<glm::vec3> VERTICES = {
        glm::vec3(-0.5, -0.5, -0.5),
        glm::vec3(-0.5, -0.5,  0.5),
        glm::vec3(-0.5,  0.5, -0.5),
        glm::vec3(-0.5,  0.5,  0.5),
        glm::vec3( 0.5, -0.5, -0.5),
        glm::vec3( 0.5, -0.5,  0.5),
        glm::vec3( 0.5,  0.5, -0.5),
        glm::vec3( 0.5,  0.5,  0.5)
    };

    //visible from the inside only, so faces in
    const std::vector<GLuint> INDICES = {
        4, 5, 1, 0, //front
        2, 3, 7, 6, //back
        6, 4, 0, 2, //top
        3, 1, 5, 7, //bottom
        0, 1, 3, 2, //left
        6, 7, 5, 4, //right
    };

    auto vBuffer = std::make_shared<VertexBuffer>(VERTICES);
    auto iBuffer = std::make_shared<IndexBuffer>(INDICES, GL_QUADS);
    mesh = std::make_shared<Mesh>(vBuffer, iBuffer, GL_QUADS);
    return mesh;
}



std::shared_ptr<Camera> Viewer::createCamera()
{
    auto camera = std::make_shared<Camera>();
    camera->setPosition(glm::vec3(0, -5, 0));

    camera->lookAt(
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 1)
    );

    return camera;
}



void Viewer::update(int deltaTime)
{
    user_->update(deltaTime);
    //note: tests of whether or not the player is moving is done in render()
}



void Viewer::animate(int deltaTime)
{
    bool animationHappened = false;

    //TODO: make any animation calls, if any happened set flag to true

    if (animationHappened)
        needsRerendering_ = true;
}



void Viewer::render()
{
    if (!needsRerendering_ && !user_->isMoving())
        return;
    needsRerendering_ = false; //it was true, so reset it and then render

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    timeSpentRendering_ += scene_->render();
    frameCount_++;

    glutSwapBuffers();
}



void Viewer::handleWindowReshape(int newWidth, int newHeight)
{
    scene_->getCamera()->setAspectRatio(newWidth / (float)newHeight);
    user_->setWindowOffset(glutGet(GLUT_WINDOW_X), glutGet(GLUT_WINDOW_Y));
    //needsRerendering_ = true; //need to redraw after window update

    std::cout << "Windows updated to " << newWidth << " by " << newHeight <<
        ", a ratio of " << (newWidth / (float)newHeight) << std::endl;
}



std::shared_ptr<User> Viewer::getUser()
{
    return user_;
}



Viewer* Viewer::singleton_ = 0;

Viewer& Viewer::getInstance()
{
    try
    {
        if (singleton_)
            return *singleton_;

        std::cout << "Creating Viewer..." << std::endl;
        singleton_ = new Viewer();
        std::cout << "... done creating Viewer." << std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << std::endl;
        std::cerr << "Caught " << typeid(e).name() <<
            " during Viewer initiation: " << e.what() << std::endl;
        glutDestroyWindow(glutGetWindow());
    }

    return *singleton_;
}
