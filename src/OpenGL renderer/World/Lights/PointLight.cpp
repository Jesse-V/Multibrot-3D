
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

int PointLight::instanceID_ = 0;


PointLight::PointLight(const glm::vec3& position, const glm::vec3& color,
                    float radius, float power)
{
    instanceID_++;
    position_ = std::make_pair(position,
        Bridge::PairedData("PointLightPosition" + std::to_string(instanceID_)));
    color_    = std::make_pair(color,
        Bridge::PairedData("PointLightColor"    + std::to_string(instanceID_)));
    radius_   = std::make_pair(radius,
        Bridge::PairedData("PointLightRadius"   + std::to_string(instanceID_)));
    power_    = std::make_pair(power,
        Bridge::PairedData("PointLightPower"    + std::to_string(instanceID_)));
}



void PointLight::setEmitting(bool emitting)
{
    Light::setEmittingInternal(emitting);

    if (emitting)
        setRadius(backupRadius_);
    else
    {
        backupRadius_ = radius_.first;
        setRadius(0);
    }
}



void PointLight::sync(GLuint handle)
{
    if (position_.second.outOfSync_)
    {
        auto loc = Bridge::getInstance().query(position_.second, handle);
        glUniform3fv(loc, 1, glm::value_ptr(position_.first));
        position_.second.outOfSync_ = false;
    }

    if (color_.second.outOfSync_)
    {
        auto loc = Bridge::getInstance().query(color_.second, handle);
        glUniform3fv(loc, 1, glm::value_ptr(color_.first));
        color_.second.outOfSync_ = false;
    }

    if (radius_.second.outOfSync_)
    {
        auto loc = Bridge::getInstance().query(radius_.second, handle);
        glUniform1f(loc, radius_.first);
        radius_.second.outOfSync_ = false;
    }

    if (power_.second.outOfSync_)
    {
        auto loc = Bridge::getInstance().query(power_.second, handle);
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
    const auto pos = position_.second.name_;
    const auto col = color_.second.name_;
    const auto r = radius_.second.name_;
    const auto pr = power_.second.name_;

    std::stringstream fields("");
    fields << "uniform vec3 " << pos << ", " << col <<
        "; uniform float " << r << ", " << pr << ";";

    return std::make_shared<ShaderSnippet>(
        R".(
            //PointLight fields
            )." + fields.str() + R".(
        ).",
        R".(
            //PointLight methods
        ).",
        R".(
            //PointLight main method
            {
                float d = length(vertexWorldFrag - )." + pos + R".();
                if (d < )." + r + R".()
                {
                    vec3 light = vec3(()." + r + R".( - d) * )." + pr + R".( / )." + r + R".() * )." + col + R".(;
                    colors.lightBlend = max(colors.lightBlend, light);
                }
            }
        )."
    );
}



void PointLight::setPosition(const glm::vec3& newPos)
{
    position_.first = newPos;
    position_.second.outOfSync_ = true;
}



void PointLight::setColor(const glm::vec3& newColor)
{
    color_.first = newColor;
    color_.second.outOfSync_ = true;
}



void PointLight::setPower(float power)
{
    power_.first = power;
    power_.second.outOfSync_ = true;
}



void PointLight::setRadius(float radius)
{
    radius_.first = radius;
    radius_.second.outOfSync_ = true;
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
