
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

#include "ShaderManager.hpp"
#include "Program.hpp"
#include <sstream>
#include <thread>
#include <iostream>


ProgramPtr ShaderManager::createProgram(
    const InstancedModelPtr& model, const SnippetPtr& sceneVertexShader,
    const SnippetPtr& sceneFragmentShader, const LightManagerPtr& lightMngr
)
{
    std::cout << "Creating vertex and fragment shaders for Model"
        << " with " << lightMngr->getLights().size() << " light(s)... ";

    auto buffers = model->getOptionalDataBuffers();
    auto vertexShaderStr = assembleVertexShaderStr(buffers,
                                              sceneVertexShader, lightMngr);
    auto fragmentShaderStr = assembleFragmentShaderStr(buffers,
                                              sceneFragmentShader, lightMngr);

    std::cout << "done." << std::endl;
    return cs5400::makeProgram(
        cs5400::makeVertexShaderStr(vertexShaderStr),
        cs5400::makeFragmentShaderStr(fragmentShaderStr)
    );
}



std::string ShaderManager::assembleVertexShaderStr(
    const BufferList& buffers, const SnippetPtr& sceneVertexShader,
    const LightManagerPtr& lightMngr
)
{
    auto vertexSnippets = assembleVertexSnippets(sceneVertexShader,
                                                 buffers, lightMngr);
    return buildShader(
        assembleFields(vertexSnippets),
        assembleMethods(vertexSnippets),
        assembleMainBodyCode(vertexSnippets)
    );
}



std::string ShaderManager::assembleFragmentShaderStr(
    const BufferList& buffers, const SnippetPtr& sceneFragmentShader,
    const LightManagerPtr& lightMngr
)
{
    auto fragmentSnippets = assembleFragmentSnippets(sceneFragmentShader,
                                                     buffers, lightMngr);
    return buildShader(
        assembleFields(fragmentSnippets),
        assembleMethods(fragmentSnippets),
        assembleMainBodyCode(fragmentSnippets) + R".(
            //final fragment shader main body code from ShaderManager
            if (colors.material == vec3(-1))
                colors.material = vec3(1);

            if (colors.lightBlend == vec3(-1))
                colors.lightBlend = vec3(1);

            //point light in spawn corner
            {
                const vec3 position = vec3(8, 8, 1);
                const vec3 color    = vec3(255, 228, 206) / vec3(255);
                const float lRadius = 16;
                const float lPower  = 2;

                float d = sqrt(pow(vertexWorldFrag.x - position.x, 2) +
                    pow(vertexWorldFrag.y - position.y, 2) +
                    pow(vertexWorldFrag.z - position.z, 2));

                if (d < lRadius)
                {
                    vec3 light = vec3((lRadius - d) * lPower / lRadius) * color;
                    colors.lightBlend = max(colors.lightBlend, light);
                }
            }

            //point light in far corner
            {
                const vec3 position = vec3(60 - 6, 60 - 6, 1);
                const vec3 color    = vec3(255, 228, 206) / vec3(255);
                const float lRadius = 16;
                const float lPower  = 2;

                float d = sqrt(pow(vertexWorldFrag.x - position.x, 2) +
                    pow(vertexWorldFrag.y - position.y, 2) +
                    pow(vertexWorldFrag.z - position.z, 2));

                if (d < lRadius)
                {
                    vec3 light = vec3((lRadius - d) * lPower / lRadius) * color;
                    colors.lightBlend = max(colors.lightBlend, light);
                }
            }

            //point light in right corner
            {
                const vec3 position = vec3(60 - 6, 8, 1);
                const vec3 color    = vec3(255, 228, 206) / vec3(255);
                const float lRadius = 16;
                const float lPower  = 2;

                float d = sqrt(pow(vertexWorldFrag.x - position.x, 2) +
                    pow(vertexWorldFrag.y - position.y, 2) +
                    pow(vertexWorldFrag.z - position.z, 2));

                if (d < lRadius)
                {
                    vec3 light = vec3((lRadius - d) * lPower / lRadius) * color;
                    colors.lightBlend = max(colors.lightBlend, light);
                }
            }

            //point light in left corner
            {
                const vec3 position = vec3(8, 60 - 6, 1);
                const vec3 color    = vec3(255, 228, 206) / vec3(255);
                const float lRadius = 16;
                const float lPower  = 2;

                float d = sqrt(pow(vertexWorldFrag.x - position.x, 2) +
                    pow(vertexWorldFrag.y - position.y, 2) +
                    pow(vertexWorldFrag.z - position.z, 2));

                if (d < lRadius)
                {
                    vec3 light = vec3((lRadius - d) * lPower / lRadius) * color;
                    colors.lightBlend = max(colors.lightBlend, light);
                }
            }

            //flashlight
            {
                float r = sqrt(pow(vertexProjectedFrag.x, 2) +
                    pow(vertexProjectedFrag.y, 2));

                float beamR = 3;
                if (r < beamR)
                {
                    vec3 light = vec3(beamR - r);
                    colors.lightBlend = max(colors.lightBlend, light);
                }
            }

            //fog
            {
                const float minD = 80;
                const float maxD = 120;
                float d = sqrt(pow(vertexProjectedFrag.x, 2) +
                    pow(vertexProjectedFrag.y, 2) +
                    pow(vertexProjectedFrag.z, 2));

                if (d >= minD)
                {
                    vec3 fog = vec3(1 - (d - minD) / (maxD - minD));
                    colors.lightBlend *= fog;
                }
            }

            vec3 lighting = ambientLight * colors.lightBlend;
            vec3 color = colors.material * lighting;
            gl_FragColor = vec4(color, 1);
        )."
    );
}



std::vector<SnippetPtr> ShaderManager::assembleVertexSnippets(
    const SnippetPtr& sceneVertexShader, const BufferList& buffers,
    const LightManagerPtr& lightMngr
)
{
    std::vector<SnippetPtr> vertexSnippets;
    vertexSnippets.reserve(1 + buffers.size() + lightMngr->getLights().size());
    vertexSnippets.push_back(sceneVertexShader);

    for (auto buffer : buffers)
        vertexSnippets.push_back(buffer->getVertexShaderGLSL());

    //TODO: light merging
    //if (lights.size() > 0) //only need one instance of light code
    //    vertexSnippets.push_back(lights[0]->getVertexShaderGLSL());

    return vertexSnippets;
}



std::vector<SnippetPtr> ShaderManager::assembleFragmentSnippets(
    const SnippetPtr& sceneFragmentShader, const BufferList& buffers,
    const LightManagerPtr& lightMngr
)
{
    std::vector<SnippetPtr> fragmentSnippets;
    fragmentSnippets.reserve(1 + buffers.size() + lightMngr->getLights().size());
    fragmentSnippets.push_back(sceneFragmentShader);

    for (auto buffer : buffers)
        fragmentSnippets.push_back(buffer->getFragmentShaderGLSL());

    //TODO: light merging
    //if (lights.size() > 0) //only need one instance of light code
    //    fragmentSnippets.push_back(lights[0]->getFragmentShaderGLSL());

    return fragmentSnippets;
}



std::string ShaderManager::assembleFields(const SnippetList& snippets)
{
    std::stringstream stream("");
    for (auto snippet : snippets)
        stream << snippet->getFields();
    return stream.str();
}



std::string ShaderManager::assembleMethods(const SnippetList& snippets)
{
    std::stringstream stream("");
    for (auto snippet : snippets)
        stream << snippet->getMethods();
    return stream.str();
}



std::string ShaderManager::assembleMainBodyCode(const SnippetList& snippets)
{
    std::stringstream stream("");
    for (auto snippet : snippets)
        stream << snippet->getMainBodyCode();
    return stream.str();
}



std::string ShaderManager::buildShader(const std::string& fields,
    const std::string& methods, const std::string& mainBodyCode
)
{
    //return fields + methods + mainBodyCode;
    return R".(
            #version 120
        )."
        + fields
        + "\n"
        + methods
        + R".(
            void main()
            {
        )."
        + mainBodyCode
        + R".(
            }
        ).";
}
