
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

#include "LightManager.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <GL/glew.h>
#include <iostream>


LightManager::LightManager()
{
    ambientLightUpdated_ = true; //TODO, figure this out
}



void LightManager::addLight(const std::shared_ptr<NewLight>& light)
{
    lights_.push_back(light);
    std::cout << "Successfully added a Light." << std::endl;
}



LightList LightManager::getLights()
{
    return lights_;
}



void LightManager::sync(GLuint handle, GLint ambientLightUniform)
{
    if (ambientLightUpdated_)
        glUniform3fv(ambientLightUniform, 1, glm::value_ptr(ambientLight_));

    for (auto light : lights_)
        light->sync(handle);
}



void LightManager::setAmbientLight(const glm::vec3& rgb)
{
    ambientLight_ = rgb;
    ambientLightUpdated_ = true;
}



glm::vec3 LightManager::getAmbientLight()
{
    return ambientLight_;
}



SnippetPtr LightManager::getVertexShaderGLSL()
{
    return std::make_shared<ShaderSnippet>(
        R".(
            //LightManager fields
        ).",
        R".(
            //LightManager methods
        ).",
        R".(
            //LightManager main method code
        )."
    );
}



SnippetPtr LightManager::getFragmentShaderGLSL()
{
    return std::make_shared<ShaderSnippet>(
        R".(
            //LightManager fields
        ).",
        R".(
            //LightManager methods
        ).",
        R".(
            //LightManager main method
        )."
    );
}
