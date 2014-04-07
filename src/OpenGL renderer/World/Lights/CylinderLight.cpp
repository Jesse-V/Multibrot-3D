
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

#include "CylinderLight.hpp"


//TODO: NOT IMPLEMENTED


void CylinderLight::setEmitting(bool emitting)
{
    Light::setEmittingInternal(emitting);
    //TODO: implement
}



SnippetPtr CylinderLight::getVertexShaderGLSL()
{
    return std::make_shared<ShaderSnippet>(
        R".(
            //CylinderLight fields
        ).",
        R".(
            //CylinderLight methods
        ).",
        R".(
            //CylinderLight main method code
        )."
    );
}



SnippetPtr CylinderLight::getFragmentShaderGLSL()
{
    return std::make_shared<ShaderSnippet>(
        R".(
            //CylinderLight fields
        ).",
        R".(
            //CylinderLight methods
        ).",
        R".(
            //CylinderLight main method
        )."
    );
}
