
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

#ifndef RADIAL_GRADIENT_LIGHT
#define RADIAL_GRADIENT_LIGHT

#include "Light.hpp"
#include "Modeling/DataBuffers/Bridge.hpp"

class RadialGradientLight : public Light
{
    public:
        RadialGradientLight(float radius);

        virtual void setEmitting(bool emitting);
        virtual void sync(GLuint handle);

        void setRadius(float newRadius);
        float getRadius() const;

        virtual SnippetPtr getVertexShaderGLSL();
        virtual SnippetPtr getFragmentShaderGLSL();

    private:
        static int instanceID_;
        std::pair<float, Bridge::PairedData> radius_;
};

#endif
