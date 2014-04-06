
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
#include <sstream>


PointLight::PointLight(const glm::vec3& position, const glm::vec3& color,
                    float radius, float power) :
    position_(position), color_(color), radius_(radius), power_(power)
{}



void PointLight::sync(GLuint handle)
{

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
        "const vec3 position = vec3(" << position_.x << "," <<
            position_.y << "," << position_.z << "), " <<
        "color = vec3(" << color_.x << "," <<
            color_.y << "," << color_.z << "); " <<
        "const float lRadius = " << radius_ << ", " <<
        "lPower  = " << power_ << "; ";

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

                float d = sqrt(pow(vertexWorldFrag.x - position.x, 2) +
                    pow(vertexWorldFrag.y - position.y, 2) +
                    pow(vertexWorldFrag.z - position.z, 2));

                if (d < lRadius)
                {
                    vec3 light = vec3((lRadius - d) * lPower / lRadius) * color;
                    colors.lightBlend = max(colors.lightBlend, light);
                }
            }
        )."
    );
}



void PointLight::setPosition(const glm::vec3& newPos)
{
    position_ = newPos;
}



void PointLight::setColor(const glm::vec3& newColor)
{
    color_ = newColor;
}



void PointLight::setPower(float power)
{
    power_ = power;
}



void PointLight::setRadius(float radius)
{
    radius_ = radius;
}



glm::vec3 PointLight::getPosition() const
{
    return position_;
}



glm::vec3 PointLight::getColor() const
{
    return color_;
}



float PointLight::getPower() const
{
    return power_;
}



float PointLight::getRadius() const
{
    return radius_;
}
