
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

#ifndef COLOR_BUFFER
#define COLOR_BUFFER

/**
    A ColorBuffer assigns a RGB color to each vertex in a Mesh.
    During rendering, the graphics card blends these colors together to
    produce a final fragment color on a face in between vertices.
    When the colors aren't uniform for all vertices, this can be used for a
    per-vertex lighting effect, which doesn't have to be done in real-time.
**/

#include "OptionalDataBuffer.hpp"
#include <glm/glm.hpp>
#include <memory>

class ColorBuffer : public OptionalDataBuffer
{
    public:
        ColorBuffer(const glm::vec3& color, std::size_t count);
        ColorBuffer(const std::vector<glm::vec3>& colors);
        std::vector<glm::vec3> getColors();

        virtual void store(GLuint programHandle);
        virtual void enable();
        virtual void disable();

        virtual SnippetPtr getVertexShaderGLSL();
        virtual SnippetPtr getFragmentShaderGLSL();

    private:
        std::vector<glm::vec3> colors_;
        GLuint colorBuffer_;
        GLint colorAttrib_;
};

typedef std::shared_ptr<ColorBuffer> ColorPtr;

#endif
