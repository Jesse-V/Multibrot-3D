
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

#include "UnifiedInstancedModel.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <stdexcept>
#include <iostream>


UnifiedInstancedModel::UnifiedInstancedModel(const std::shared_ptr<Mesh>& mesh) :
    InstancedModel(mesh)
{}



UnifiedInstancedModel::UnifiedInstancedModel(const std::shared_ptr<Mesh>& mesh,
                                             const glm::mat4& modelMatrix) :
    InstancedModel(mesh, modelMatrix)
{}



UnifiedInstancedModel::UnifiedInstancedModel(const std::shared_ptr<Mesh>& mesh,
                                             const BufferList& optionalDBs) :
    InstancedModel(mesh, optionalDBs)
{}



UnifiedInstancedModel::UnifiedInstancedModel(const std::shared_ptr<Mesh>& mesh,
                const glm::mat4& modelMatrix, const BufferList& optionalDBs) :
    InstancedModel(mesh, modelMatrix, optionalDBs)
{}



UnifiedInstancedModel::UnifiedInstancedModel(const std::shared_ptr<Mesh>& mesh,
               const std::vector<glm::mat4>& modelMatrices,
               const BufferList& optionalDBs) :
    InstancedModel(mesh, modelMatrices, optionalDBs)
{}



void UnifiedInstancedModel::addInstance(const glm::mat4& instanceModelMatrix)
{
    if (unified_)
        throw std::logic_error("Can't addInstance, already unified!");
    else
        InstancedModel::addInstance(instanceModelMatrix);
}



void UnifiedInstancedModel::saveAs(GLuint programHandle)
{
    auto vertices   = mesh_->getVertexBuffer()->getVertices();
    auto indexes    = mesh_->getIndexBuffer()->getIndices();
    auto renderMode = mesh_->getRenderingMode();

    std::vector<glm::vec3> newVertices;
    std::vector<GLuint> newIndices;

    for (std::size_t instance = 0; instance < modelMatrices_.size(); instance++)
    {
        auto modelMatrix = modelMatrices_[instance];

        for (auto vertex : vertices)
            newVertices.push_back((modelMatrix * glm::vec4(vertex, 1)).xyz());

        for (auto index : indexes)
            newIndices.push_back(index + instance * vertices.size());
    }

    auto vBuffer = std::make_shared<VertexBuffer>(newVertices);
    auto iBuffer = std::make_shared<IndexBuffer>(newIndices, renderMode);
    mesh_ = std::make_shared<Mesh>(vBuffer, iBuffer, renderMode);

    modelMatrices_.clear();
    modelMatrices_.push_back(glm::mat4());

    unified_ = true;
    InstancedModel::saveAs(programHandle);
}



void UnifiedInstancedModel::render(GLuint programHandle)
{
    if (cachedHandle_ != programHandle) //bit of adaptable caching
    {
        cachedHandle_ = programHandle;
        matrixModelLocation_ = glGetUniformLocation(programHandle, "modelMatrix");
    }

    if (isVisible_)
    {
        enableDataBuffers();
        glUniformMatrix4fv(matrixModelLocation_, 1, GL_FALSE,
            glm::value_ptr(modelMatrices_[0]));
        mesh_->draw();
    }
}



void UnifiedInstancedModel::setModelMatrix(std::size_t index, const glm::mat4& matrix)
{
    if (unified_)
        throw std::logic_error("Can't set modelMatrix, already unified!");
    else
        InstancedModel::setModelMatrix(index, matrix);
}
