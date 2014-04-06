
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

#include "PointLight.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <sstream>
#include <iostream>

int PointLight::instanceID_ = 0;


PointLight::PointLight(const glm::vec3& position, const glm::vec3& color,
                    float radius, float power)
{
    instanceID_++;
    position_ = std::make_pair(position, VarData("lPosition" + std::to_string(instanceID_)));
    color_    = std::make_pair(color,    VarData("lColor"    + std::to_string(instanceID_)));
    radius_   = std::make_pair(radius,   VarData("lRadius"   + std::to_string(instanceID_)));
    power_    = std::make_pair(power,    VarData("lPower"    + std::to_string(instanceID_)));
}



void PointLight::sync(GLuint handle)
{
    if (position_.second.outOfSync_)
    {
        auto loc = find(position_.second, handle);
        glUniform3fv(loc, 1, glm::value_ptr(position_.first));
        position_.second.outOfSync_ = false;
    }

    if (color_.second.outOfSync_)
    {
        auto loc = find(color_.second, handle);
        glUniform3fv(loc, 1, glm::value_ptr(color_.first));
        color_.second.outOfSync_ = false;
    }

    if (radius_.second.outOfSync_)
    {
        auto loc = find(radius_.second, handle);
        glUniform1f(loc, radius_.first);
        radius_.second.outOfSync_ = false;
    }

    if (power_.second.outOfSync_)
    {
        auto loc = find(power_.second, handle);
        glUniform1f(loc, power_.first);
        power_.second.outOfSync_ = false;
    }
}



SnippetPtr PointLight::getVertexShaderGLSL()
{
    return std::make_shared<ShaderSnippet>(
        R".(
            //PointLight fields
        ).",
        R".(
            //PointLight methods
        ).",
        R".(
            //PointLight main method code
        )."
    );
}



SnippetPtr PointLight::getFragmentShaderGLSL()
{
    std::stringstream stream("");
    stream <<
        "const vec3 position = vec3(" << position_.first.x << "," <<
            position_.first.y << "," << position_.first.z << "), " <<
        "color = vec3(" << color_.first.x << "," <<
            color_.first.y << "," << color_.first.z << "); " <<
        "const float lRadius = " << radius_.first << ", " <<
        "lPower  = " << power_.first << "; ";

    return std::make_shared<ShaderSnippet>(
        R".(
            //PointLight fields
        ).",
        R".(
            //PointLight methods
        ).",
        R".(
            //PointLight main method
            {
                )." + stream.str() + R".(

                float d = length(vertexWorldFrag - position);
                if (d < lRadius)
                {
                    vec3 light = vec3((lRadius - d) * lPower / lRadius) * color;
                    colors.lightBlend = max(colors.lightBlend, light);
                }
            }
        )."
    );
}



GLint PointLight::find(const VarData& varData, GLuint handle)
{
    bool found = false;
    GLint location = (GLint)NULL;
    for (auto bridge : cache_)
    {
        if (bridge.handle_ == handle && bridge.varName_ == varData.name_)
        {
            location = bridge.location_;
            found = true;
            break;
        }
    }

    if (!found)
    {
        location = glGetUniformLocation(handle, varData.name_.c_str());
        cache_.push_back(VarBridge(handle, varData.name_, location));
    }

    return location;
}



void PointLight::setPosition(const glm::vec3& newPos)
{
    position_.first = newPos;
}



void PointLight::setColor(const glm::vec3& newColor)
{
    color_.first = newColor;
}



void PointLight::setPower(float power)
{
    power_.first = power;
}



void PointLight::setRadius(float radius)
{
    radius_.first = radius;
}



glm::vec3 PointLight::getPosition() const
{
    return position_.first;
}



glm::vec3 PointLight::getColor() const
{
    return color_.first;
}



float PointLight::getPower() const
{
    return power_.first;
}



float PointLight::getRadius() const
{
    return radius_.first;
}
