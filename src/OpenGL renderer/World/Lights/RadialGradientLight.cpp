
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

#include "RadialGradientLight.hpp"
#include <sstream>

int RadialGradientLight::instanceID_ = 0;


RadialGradientLight::RadialGradientLight(float radius)
{
    instanceID_++;
    radius_ = std::make_pair(radius,
        Bridge::PairedData("RGLightRadius" + std::to_string(instanceID_)));
}



void RadialGradientLight::setEmitting(bool emitting)
{
    Light::setEmittingInternal(emitting);
    if (emitting)
        setRadius(3.0f);
    else
        setRadius(0.0f);
}


void RadialGradientLight::sync(GLuint handle)
{
    if (radius_.second.outOfSync_)
    {
        auto loc = Bridge::getInstance().query(radius_.second, handle);
        glUniform1f(loc, radius_.first);
        radius_.second.outOfSync_ = false;
    }
}



void RadialGradientLight::setRadius(float newRadius)
{
    radius_.first = newRadius;
    radius_.second.outOfSync_ = true;
}



float RadialGradientLight::getRadius() const
{
    return radius_.first;
}



SnippetPtr RadialGradientLight::getVertexShaderGLSL()
{
    return std::make_shared<ShaderSnippet>(
        R".(
            //RadialGradientLight fields
        ).",
        R".(
            //RadialGradientLight methods
        ).",
        R".(
            //RadialGradientLight main method code
        )."
    );
}



SnippetPtr RadialGradientLight::getFragmentShaderGLSL()
{
    const auto r = radius_.second.name_;

    std::stringstream fields("");
    fields << "uniform float " << r << ";";

    return std::make_shared<ShaderSnippet>(
        R".(
            //RadialGradientLight fields
            )." + fields.str() + R".(
        ).",
        R".(
            //RadialGradientLight methods
        ).",
        R".(
            //RadialGradientLight main method
            {
                float beamRadius = )." + r + R".(;
                float r = length(vertexViewedFrag.xy);
                if (r < beamRadius)
                {
                    vec3 light = vec3(beamRadius - r);
                    colors.lightBlend = max(colors.lightBlend, light);
                }
            }
        )."
    );
}
