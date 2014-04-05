
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

#ifndef SCENE
#define SCENE

/**
    A Scene is basically a high-level container for Models, Lights, and a Camera.
    This allows for an application to use multiple Scenes for different
    environments or locations. Scene::render() will handle the management of
    rendering all Models with all the Lights, as seen by the given Camera. A
    Scene uses a multimap to create a 1-to-many mapping between Programs and
    Models. This is useful when many Models are the same, but they
    are drawn in different locations. They would all share the same Program and
    the same GPU-side data, and synchronizing data with their Program would
    affect them all simultaneously. This adds a bit of complexity, but is a
    very significant optimization: it gave an 9x speedup when it was first
    implemented, but this depends on the mapping of course.

**/

#include "Camera.hpp"
#include "Lights/Light.hpp"
#include "Modeling/Shading/Program.hpp"
#include "Modeling/InstancedModel.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <unordered_map>
#include <vector>

class Scene
{
    public:
        Scene(const std::shared_ptr<Camera>& camera);
        void addModel(const InstancedModelPtr& model);
        void addModel(const InstancedModelPtr& model, const ProgramPtr& program);
        void addLight(const std::shared_ptr<Light>& light);
        void setCamera(const std::shared_ptr<Camera>& camera);
        void setAmbientLight(const glm::vec3& rgb);
        float render();

        std::shared_ptr<Camera> getCamera();
        int getModelCount();
        LightList getLights();
        glm::vec3 getAmbientLight();

        virtual SnippetPtr getVertexShaderGLSL();
        virtual SnippetPtr getFragmentShaderGLSL();

        struct Renderable
        {
            Renderable(InstancedModelPtr m, ProgramPtr prog, GLint a, GLint v, GLint p) :
                model(m), program(prog), ambientLightUniform(a),
                viewUniform(v), projUniform(p)
            {}

            InstancedModelPtr model;
            ProgramPtr program;
            GLint ambientLightUniform, viewUniform, projUniform;
        };

    private:
        void syncLighting(GLuint programHandle, GLint ambientLightUniform);
        void doneSyncingLighting();

    private:
        std::vector<Renderable> renderables_;
        LightList lights_;
        std::shared_ptr<Camera> camera_;
        glm::vec3 ambientLight_;
        bool ambientLightUpdated_;
};


#endif
