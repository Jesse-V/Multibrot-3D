
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

#ifndef TEXTURED_CUBE
#define TEXTURED_CUBE

#include "Image.hpp"
#include "../OptionalDataBuffer.hpp"
#include "glm/glm.hpp"
#include <memory>

class TexturedCube : public OptionalDataBuffer
{
    public:
        TexturedCube(
            const std::shared_ptr<Image>& positiveX,
            const std::shared_ptr<Image>& negativeX,
            const std::shared_ptr<Image>& positiveY,
            const std::shared_ptr<Image>& negativeY,
            const std::shared_ptr<Image>& positiveZ,
            const std::shared_ptr<Image>& negativeZ
        );
        void mapTo(GLenum target, const std::shared_ptr<Image>& img);

        virtual void store(GLuint programHandle);
        virtual void enable();
        virtual void disable();

        virtual SnippetPtr getVertexShaderGLSL();
        virtual SnippetPtr getFragmentShaderGLSL();

    private:
        std::shared_ptr<Image> positiveX_, negativeX_,
                               positiveY_, negativeY_,
                               positiveZ_, negativeZ_;
        GLuint cubeTexture_;
        GLint textureCoordinates_;
};

#endif
