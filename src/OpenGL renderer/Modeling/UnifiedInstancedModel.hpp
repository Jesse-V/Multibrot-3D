
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

#ifndef UNIFIED_INSTANCED_MODEL
#define UNIFIED_INSTANCED_MODEL

#define GLM_SWIZZLE //needed for .xyz(), just like in Camera class
#include "InstancedModel.hpp"

class UnifiedInstancedModel : public InstancedModel
{
    public:
        UnifiedInstancedModel(const std::shared_ptr<Mesh>& mesh);
        UnifiedInstancedModel(const std::shared_ptr<Mesh>& mesh,
                              const glm::mat4& modelMatrix);
        UnifiedInstancedModel(const std::shared_ptr<Mesh>& mesh,
                              const BufferList& optionalDBs);
        UnifiedInstancedModel(const std::shared_ptr<Mesh>& mesh,
                              const glm::mat4& modelMatrix,
                              const BufferList& optionalDBs);
        UnifiedInstancedModel(const std::shared_ptr<Mesh>& mesh,
                              const std::vector<glm::mat4>& modelMatrices,
                              const BufferList& optionalDBs);

        //overrides from InstancedModel
        void saveAs(GLuint programHandle);
        void render(GLuint programHandle);
        void setModelMatrix(std::size_t index, const glm::mat4& matrix);
        void addInstance(const glm::mat4& instanceModelMatrix);

    private:
        bool unified_ = false;
};

#endif
