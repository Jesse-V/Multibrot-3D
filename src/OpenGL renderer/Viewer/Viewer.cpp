
/******************************************************************************\
                     This file is part of Folding Atomata,
          a program that displays 3D views of Folding@home proteins.

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
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

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
    auto cubes = std::make_shared<InstancedModel>(getSkyboxMesh());

    std::ifstream file;
    file.open("geometry.dat", std::ifstream::in);
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

    for (std::size_t j = 0; j < geometry.size(); j++)
    {
        glm::vec3 min, max;

        if (geometry[j][0] == 4)
        {
            min = glm::vec3(geometry[j][1], geometry[j][2], geometry[j][3]);
            max = glm::vec3(geometry[j][4], geometry[j][5], geometry[j][6]);
        }
        else
            std::cout << "Non-box encountered!" << std::endl;

        auto matrix = glm::translate(glm::mat4(), min);
        matrix      = glm::scale(matrix, (max - min) / glm::vec3(16));
        cubes->addInstance(matrix);
    }

    scene_->addModel(cubes); //add to Scene and save
}



void Viewer::addSkybox()
{
    std::cout << "Creating skybox..." << std::endl;

    auto image = std::make_shared<Image>(
        Options::getInstance().getSkyboxPath());

    BufferList list = { std::make_shared<TexturedCube>(image, image,
        image, image, image, image) };
    auto matrix = glm::scale(glm::mat4(), glm::vec3(4096));
    auto model = std::make_shared<InstancedModel>(getSkyboxMesh(), matrix, list);
    scene_->addModel(model); //add to Scene and save

    std::cout << "... done creating skybox." << std::endl;
}



std::shared_ptr<Mesh> Viewer::getSkyboxMesh()
{
    static std::shared_ptr<Mesh> mesh = nullptr;

    if (mesh)
        return mesh;

    const std::vector<glm::vec3> VERTICES = {
        glm::vec3(-1, -1, -1),
        glm::vec3(-1, -1,  1),
        glm::vec3(-1,  1, -1),
        glm::vec3(-1,  1,  1),
        glm::vec3( 1, -1, -1),
        glm::vec3( 1, -1,  1),
        glm::vec3( 1,  1, -1),
        glm::vec3( 1,  1,  1)
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



std::shared_ptr<Camera> Viewer::createCamera()
{
    auto camera = std::make_shared<Camera>();
    camera->setPosition(glm::vec3(0, -50, 0));

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
