
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

#ifndef VIEWER
#define VIEWER

/**
    The Viewer class performs high-level tasks for setting up and managing
    the application as a whole. The Viewer is a singleton, but when constructed
    it uses the FAHClientIO class to load trajectory information from FAHClient,
    constructs SlotViewers, and adds Lights and a Camera to the Scene. It also
    has an update and render method to implement a simple gameplay loop.
    It uses the update method to ask the SlotViewers to animate or step forward
    the amount of time since the last time the update method was called.
**/

#include "User.hpp"
#include "World/Scene.hpp"
#include "Modeling/UnifiedInstancedModel.hpp"
#include <memory>

typedef UnifiedInstancedModel ModelType;
typedef std::pair<int, std::shared_ptr<ModelType>> ColoredCube;

class Viewer
{
    public:
        void update(int deltaTime);
        void animate(int deltaTime);
        void render();
        void handleWindowReshape(int screenWidth, int screenHeight);
        std::shared_ptr<User> getUser();
        static Viewer& getInstance();

    private:
        Viewer();
        void reportFPS();
        void addModels();
        void addBellCurveBlocks();
        void addFractal();
        void addSkybox();
        std::vector<std::vector<int>> readGeometry(const std::string& filename);
        std::shared_ptr<Mesh> getInternalFacingCube();
        std::shared_ptr<Mesh> getExternalFacingCube();
        std::shared_ptr<Camera> createCamera();

    private:
        static Viewer* singleton_;

        std::shared_ptr<Scene> scene_;
        std::shared_ptr<User> user_;
        float timeSpentRendering_;
        int frameCount_;
        bool needsRerendering_;
};

#endif
