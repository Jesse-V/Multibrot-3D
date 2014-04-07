
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

#include "Scene.hpp"
#include "Modeling/Shading/ShaderManager.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>


Scene::Scene(const std::shared_ptr<Camera>& camera) :
    camera_(camera), lightManager_(std::make_shared<LightManager>())
{
    lightManager_->setAmbientLight(glm::vec3(1));
}



void Scene::addModel(const InstancedModelPtr& model)
{
    if (model->isAffectedByLight())
        addModel(model, ShaderManager::createProgram(model,
            getVertexShaderGLSL(), getFragmentShaderGLSL(), lightManager_));
    else
        addModel(model, ShaderManager::createProgram(model,
            getVertexShaderGLSL(), getFragmentShaderGLSL(), NULL));
}



void Scene::addModel(const InstancedModelPtr& model, const ProgramPtr& program)
{
    auto programHandle = program->getHandle();
    glUseProgram(programHandle);

    model->saveAs(programHandle);
    GLint ambLU = glGetUniformLocation(programHandle, "ambientLight");
    GLint viewU = glGetUniformLocation(programHandle, "viewMatrix");
    GLint projU = glGetUniformLocation(programHandle, "projMatrix");

    renderables_.push_back(Renderable(model, program, ambLU, viewU, projU));
}



void Scene::setCamera(const std::shared_ptr<Camera>& sceneCamera)
{
    camera_ = sceneCamera;
}



float Scene::render()
{
    using namespace std::chrono;
    auto start = steady_clock::now();

    camera_->startSync();
    for (auto renderable : renderables_)
    {
        GLuint handle = renderable.program->getHandle();
        glUseProgram(handle);
        camera_->sync(renderable.viewUniform, renderable.projUniform);
        lightManager_->sync(handle, renderable.ambientLightUniform);

        renderable.model->render(handle);
    }
    camera_->endSync();

    auto diff = duration_cast<microseconds>(steady_clock::now() - start).count();
    return diff / 1000.0f;
}



std::shared_ptr<Camera> Scene::getCamera()
{
    return camera_;
}



std::shared_ptr<LightManager> Scene::getLightManager()
{
    return lightManager_;
}



int Scene::getModelCount()
{
    return (int)renderables_.size();
}



SnippetPtr Scene::getVertexShaderGLSL()
{
    return std::make_shared<ShaderSnippet>(
        R".(
            // ********* VERTEX SHADER ********* \\

            //Scene fields
            attribute vec3 vertex; //position of the vertex
            uniform mat4 viewMatrix, projMatrix; //Camera & projection matrices
            uniform mat4 modelMatrix; //transforms model into world space
            varying vec3 vertexWorldFrag, vertexViewedFrag, vertexProjectedFrag;
        ).",
        R".(
            //Scene methods
        ).",
        R".(
            //Scene main method code
            vec4 vertexWorld     = modelMatrix * vec4(vertex, 1);
            vec4 vertexViewed    = viewMatrix  * vertexWorld;
            vec4 vertexProjected = projMatrix  * vertexViewed;

            gl_Position = vertexProjected;
            vertexWorldFrag = vertexWorld.xyz;
            vertexViewedFrag = vertexViewed.xyz;
            vertexProjectedFrag = vertexProjected.xyz;
        )."
    );
}



SnippetPtr Scene::getFragmentShaderGLSL()
{
    return std::make_shared<ShaderSnippet>(
        R".(
            // ********* FRAGMENT SHADER ********* \\

            //Scene fields
            uniform vec3 ambientLight;
            varying vec3 vertexWorldFrag, vertexViewedFrag, vertexProjectedFrag;

            struct Colors
            {
                vec3 material, lightBlend;
            };
        ).",
        R".(
            //Scene methods
        ).",
        R".(
            //Scene main method code
            Colors colors;
            colors.material = vec3(1);
            colors.lightBlend = vec3(1);
        )."
    );
}
