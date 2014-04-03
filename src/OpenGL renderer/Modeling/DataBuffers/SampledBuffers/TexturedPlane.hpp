
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

#ifndef TEXTURED_PLANE
#define TEXTURED_PLANE

#include "Image.hpp"
#include "../OptionalDataBuffer.hpp"
#include "Modeling/Mesh/Mesh.hpp"
#include "glm/glm.hpp"
#include <memory>

class TexturedPlane : public OptionalDataBuffer
{
    public:
        TexturedPlane(const std::shared_ptr<Image>& image);

        virtual void store(GLuint programHandle);
        virtual void enable();
        virtual void disable();

        static std::shared_ptr<Mesh> getMesh();

        virtual SnippetPtr getVertexShaderGLSL();
        virtual SnippetPtr getFragmentShaderGLSL();

    private:
        std::shared_ptr<Image> image_;
        GLuint planeTexture_;
        GLint textureCoordinates_;
};

#endif
