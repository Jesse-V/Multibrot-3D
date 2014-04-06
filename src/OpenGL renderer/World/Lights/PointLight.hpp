
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

#ifndef POINT_LIGHT
#define POINT_LIGHT

#include "NewLight.hpp"

class PointLight : public NewLight
{
    public:
        PointLight(
            const glm::vec3& position = glm::vec3(0),
            const glm::vec3& color = glm::vec3(1),
            float radius = 8, float power = 4
        );

        virtual void sync(GLuint handle);

        virtual SnippetPtr getVertexShaderGLSL();
        virtual SnippetPtr getFragmentShaderGLSL();

        void setPosition(const glm::vec3& newPos);
        void setColor(const glm::vec3& newColor);
        void setPower(float power);
        void setRadius(float radius);

        glm::vec3 getPosition() const;
        glm::vec3 getColor() const;
        float getPower() const;
        float getRadius() const;

    private:
        glm::vec3 position_, color_;
        float radius_, power_;
};

#endif
