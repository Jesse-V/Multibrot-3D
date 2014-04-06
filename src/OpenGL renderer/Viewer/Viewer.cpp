
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
#include "World/Lights/Fog.hpp"
#include "Modeling/DataBuffers/SampledBuffers/Image.hpp"
#include "Modeling/DataBuffers/SampledBuffers/TexturedCube.hpp"
#include "Modeling/DataBuffers/SampledBuffers/TexturedPlane.hpp"
#include "Modeling/DataBuffers/ColorBuffer.hpp"
#include "glm/gtx/transform.hpp"
#include <thread>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <iostream>
#include <math.h>


Viewer::Viewer() :
    scene_(std::make_shared<Scene>(createCamera())),
    user_(std::make_shared<User>(scene_)),
    timeSpentRendering_(0), frameCount_(0), needsRerendering_(true)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    scene_->getLightManager()->addLight(std::make_shared<Fog>());

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

/*
    auto grassTop  = std::make_shared<Image>("images/grass_top.png");
    BufferList list = { std::make_shared<TexturedPlane>(grassTop) };
    auto plane = std::make_shared<InstancedModel>(TexturedPlane::getMesh(), list);
    plane->addInstance(glm::scale(glm::mat4(), glm::vec3(1.0f)));
    scene_->addModel(plane);*/
}



void Viewer::addBellCurveBlocks()
{
    static const float SPREAD = 0.004f, HEIGHT = 15.0f;
    static const int RES = 30;

    auto dirt      = std::make_shared<Image>("images/dirt.png");
    auto grassTop  = std::make_shared<Image>("images/grass_top.png");

    std::string sideFile = "images/grass_side.png";
    auto side1 = std::make_shared<Image>(sideFile, false, false, true);
    auto side2 = std::make_shared<Image>(sideFile, true, false, true);
    auto side3 = std::make_shared<Image>(sideFile, false, true);
    auto side4 = std::make_shared<Image>(sideFile);

    BufferList list = { std::make_shared<TexturedCube>(side1, side2,
        side3, side4, grassTop, dirt) };

    auto blocks = std::make_shared<InstancedModel>(
                    TexturedCube::getExternalFacingMesh(), list);

    long count = 0;
    for (int x = -RES; x <= RES; x++)
    {
        for (int y = -RES; y <= RES; y++)
        {
            float dSq = x * x + y * y;
            float z = HEIGHT / (float)pow(2.718282f, SPREAD * dSq);
            z = (int)z;

            if (z >= 11)
                z = 11;

            auto loc = glm::vec3(x + RES, y + RES, -z) + glm::vec3(0.5f);
            auto matrix = glm::translate(glm::mat4(), loc);
            blocks->addInstance(matrix);
            count++;
        }
    }

    std::cout << "TexturedCube count: " << count << std::endl;
    scene_->addModel(blocks); //add to Scene and save
}



void Viewer::addFractal()
{
    static const auto SCALE = glm::vec3(1 / 64.0f);
    static const auto POS = glm::vec3(30, 30, -3);
    static const auto BOX_SCALE = glm::vec3(1.0f);

    std::vector<std::pair<int, std::shared_ptr<std::vector<glm::mat4>>>> boxTypes;
    for (int j = 128; j >= 1; j /= 2)
        boxTypes.push_back(std::make_pair(j, std::make_shared<std::vector<glm::mat4>>()));

    long count = 0;
    auto geometry = readGeometry("geometry.dat");
    for (auto rectangle : geometry)
    {
        glm::vec3 min = glm::vec3(rectangle[1], rectangle[2], rectangle[3]);
        glm::vec3 max = glm::vec3(rectangle[4], rectangle[5], rectangle[6]);
        glm::vec3 size = max - min;
        float minDimSize = std::min(size.x, std::min(size.y, size.z));

        for (auto boxType : boxTypes)
        {
            if ((int)minDimSize == boxType.first)
            {
                auto matrix = glm::scale(glm::mat4(), SCALE);
                matrix      = glm::translate(matrix, min - glm::vec3(512));
                matrix      = glm::scale(matrix, (max - min) * BOX_SCALE);
                matrix      = glm::translate(matrix, glm::vec3(0.5f));
                boxType.second->push_back(matrix);
                count++;
                break;
            }
        }
    }

    std::cout << "Instance count: " << count << std::endl;
    for (auto boxType : boxTypes)
        std::cout << boxType.first << ", " << boxType.second->size() << std::endl;

    auto mesh = TexturedCube::getExternalFacingMesh();
    for (auto boxType : boxTypes)
    {
        if (boxType.second->size() == 0)
            continue;

        std::vector<glm::vec3> vertexColors;
        auto vertices = mesh->getVertexBuffer()->getVertices();
        for (auto modelMatrix : *boxType.second)
        {
            for (auto vertex : vertices)
            {
                float r = 0, g = 0, b = 0;
                auto newV = (modelMatrix * glm::vec4(vertex, 1)).xyz();
                auto radius = (float)sqrt(std::pow(newV.y, 2) + std::pow(newV.z, 2));

                r = g = (radius - 2.4f) / 3.5f, b = 1;
                g = std::max(g, (-newV.x - 6.9f) / 1.5f);

                vertexColors.push_back(glm::vec3(r, g, b));
            }
        }

        BufferList list = { std::make_shared<ColorBuffer>(vertexColors) };
        auto model = std::make_shared<InstancedModel>(mesh, *boxType.second, list);


        model->unify(glm::rotate(glm::translate(POS), 0.0f, glm::vec3(0, 1, 0)));
        boxTypes_.push_back(model);
        scene_->addModel(model); //add to Scene and save
    }
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



void Viewer::addSkybox()
{
    std::cout << "Creating skybox..." << std::endl;

    auto image = std::make_shared<Image>(
        "/usr/share/FoldingAtomata/images/gradient.png");

    BufferList list = { std::make_shared<TexturedCube>(image, image,
        image, image, image, image) };
    auto matrix = glm::scale(glm::mat4(), glm::vec3(4096));
    auto model = std::make_shared<InstancedModel>(
                                TexturedCube::getInternalFacingMesh(), matrix, list);
    scene_->addModel(model); //add to Scene and save

    std::cout << "... done creating skybox." << std::endl;
}



std::shared_ptr<Camera> Viewer::createCamera()
{
    auto camera = std::make_shared<Camera>();

    camera->translate(glm::vec3(11, 11, 9));
    camera->yaw(-45.0f);
    camera->pitch(-25.0f);

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

    for (auto boxType : boxTypes_)
    {
        auto matrix = boxType->getModelMatrix(0);
        matrix = glm::rotate(matrix, 0.15f, glm::vec3(1, 0, 0));
        boxType->setModelMatrix(0, matrix);
    }

    animationHappened = true;

    if (animationHappened)
        needsRerendering_ = true;
}



void Viewer::render()
{
    if (!needsRerendering_ && !user_->isMoving())
        return;
    needsRerendering_ = false; //it was true, so reset it and then render

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearColor(255 / 255.0f, 249 / 255.0f, 253 / 255.0f, 1);

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
