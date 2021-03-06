
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

#ifndef INSTANCED_MODEL
#define INSTANCED_MODEL

#define GLM_SWIZZLE //needed for .xyz(), just like in Camera class

#include "Modeling/Mesh/Mesh.hpp"
#include "Modeling/DataBuffers/OptionalDataBuffer.hpp"
#include <vector>
#include <memory>

typedef std::vector<std::shared_ptr<OptionalDataBuffer>> BufferList;

class InstancedModel
{
    public:
        InstancedModel(const std::shared_ptr<Mesh>& mesh);
        InstancedModel(const std::shared_ptr<Mesh>& mesh,
                       const glm::mat4& modelMatrix);
        InstancedModel(const std::shared_ptr<Mesh>& mesh,
                       const BufferList& optionalDBs);
        InstancedModel(const std::shared_ptr<Mesh>& mesh,
                       const glm::mat4& modelMatrix,
                       const BufferList& optionalDBs);
        InstancedModel(const std::shared_ptr<Mesh>& mesh,
                       const std::vector<glm::mat4>& modelMatrices,
                       const BufferList& optionalDBs);

        virtual void saveAs(GLuint programHandle);
        virtual void addInstance(const glm::mat4& instanceModelMatrix);
        virtual void render(GLuint programHandle);
        virtual void setModelMatrix(std::size_t index, const glm::mat4& matrix);
        void unify(const glm::mat4& under);
        glm::mat4 getModelMatrix(std::size_t index);
        void setVisible(bool visible);
        BufferList getOptionalDataBuffers();
        std::size_t getInstanceCount();
        void setAffectedByLight(bool value);
        bool isAffectedByLight();

    protected:
        void enableDataBuffers();
        void disableDataBuffers();

        std::shared_ptr<Mesh> mesh_;
        std::vector<glm::mat4> modelMatrices_;
        BufferList optionalDBs_;
        GLuint cachedHandle_;
        GLint matrixModelLocation_;
        bool isVisible_, lightApplicable_;
};

typedef std::shared_ptr<InstancedModel> InstancedModelPtr;

#endif
