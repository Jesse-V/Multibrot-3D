
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

#include "TexturedPlane.hpp"
#include <iostream>
#include "Modeling/Shading/Program.hpp"
#include <string.h>


TexturedPlane::TexturedPlane(const std::shared_ptr<Image>& image) :
    image_(image)
{}



void TexturedPlane::store(GLuint programHandle)
{
    /*
        http://www.opengl-tutorial.org/beginners-tutorials/tutorial-5-a-textured-cube/
        https://www.opengl.org/wiki/Cubemap_Texture
        http://stackoverflow.com/questions/14505969/the-best-way-to-texture-a-cube-in-opengl
        https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_06
    */

    glGenTextures(1, &planeTexture_);
    glBindTexture(GL_TEXTURE_CUBE_MAP, planeTexture_);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glActiveTexture(GL_TEXTURE0);

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, image_->getWidth(),
        image_->getWidth(), 0, GL_BGR, GL_UNSIGNED_BYTE, image_->getImageData());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, image_->getWidth(),
        image_->getWidth(), 0, GL_BGR, GL_UNSIGNED_BYTE, image_->getImageData());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, image_->getWidth(),
        image_->getWidth(), 0, GL_BGR, GL_UNSIGNED_BYTE, image_->getImageData());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, image_->getWidth(),
        image_->getWidth(), 0, GL_BGR, GL_UNSIGNED_BYTE, image_->getImageData());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, image_->getWidth(),
        image_->getWidth(), 0, GL_BGR, GL_UNSIGNED_BYTE, image_->getImageData());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, image_->getWidth(),
        image_->getWidth(), 0, GL_BGR, GL_UNSIGNED_BYTE, image_->getImageData());

    textureCoordinates_ = glGetAttribLocation(programHandle, "UV");
    if (textureCoordinates_ == -1)
        throw std::runtime_error("Could not bind UV attribute");
    glEnableVertexAttribArray(textureCoordinates_);
}



void TexturedPlane::enable()
{
    glVertexAttribPointer(textureCoordinates_, 3, GL_FLOAT, GL_FALSE, 0, 0);
}



void TexturedPlane::disable()
{
    glDisableVertexAttribArray(textureCoordinates_);
}



std::shared_ptr<Mesh> TexturedPlane::getMesh()
{
    static std::shared_ptr<Mesh> mesh = nullptr;

    if (mesh)
        return mesh;

    const std::vector<glm::vec3> VERTICES = {
        glm::vec3(-0.5, -0.5, -0.5),
        glm::vec3(-0.5, -0.5,  0.5),
        glm::vec3(-0.5,  0.5, -0.5),
        glm::vec3(-0.5,  0.5,  0.5),
        glm::vec3( 0.5, -0.5, -0.5),
        glm::vec3( 0.5, -0.5,  0.5),
        glm::vec3( 0.5,  0.5, -0.5),
        glm::vec3( 0.5,  0.5,  0.5)
    };

    //visible from the inside only, so faces in
    const std::vector<GLuint> INDICES = {
        4, 5, 1, 0, //front
        2, 3, 7, 6, //back
        6, 4, 0, 2, //top
        3, 1, 5, 7, //bottom
        0, 1, 3, 2, //left
        6, 7, 5, 4, //right
    };

    auto vBuffer = std::make_shared<VertexBuffer>(VERTICES);
    auto iBuffer = std::make_shared<IndexBuffer>(INDICES, GL_QUADS);
    mesh = std::make_shared<Mesh>(vBuffer, iBuffer, GL_QUADS);
    return mesh;
}



//todo: some of this code belongs to SampledBuffer, base class's method could be called
SnippetPtr TexturedPlane::getVertexShaderGLSL()
{
    return std::make_shared<ShaderSnippet>(
        R".(
            //TexturedPlane fields
            attribute vec3 UV;
            varying vec3 UVf;
        ).",
        R".(
            //TexturedPlane methods
        ).",
        R".(
            //TexturedPlane main method code
            UVf = UV;
        )."
    );
}



SnippetPtr TexturedPlane::getFragmentShaderGLSL()
{
    return std::make_shared<ShaderSnippet>(
        R".(
            //TexturedPlane fields
            uniform samplerCube cubeSampler;
            varying vec3 UVf;
        ).",
        R".(
            //TexturedPlane methods
        ).",
        R".(
            //TexturedPlane main method code
            colors.material = textureCube(cubeSampler, UVf).rgb;
        )."
    );
}
