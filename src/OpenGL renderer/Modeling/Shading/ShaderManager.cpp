
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
    if (lightMngr == NULL)
        std::cout << "Creating vertex & fragment shaders for light-immune model... ";
    else
        std::cout << "Creating vertex and fragment shaders for model"
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
    vertexSnippets.reserve(2 + buffers.size());
    vertexSnippets.push_back(sceneVertexShader);

    for (auto buffer : buffers)
        vertexSnippets.push_back(buffer->getVertexShaderGLSL());

    if (lightMngr != NULL)
        vertexSnippets.push_back(lightMngr->getVertexShaderGLSL());

    return vertexSnippets;
}



std::vector<SnippetPtr> ShaderManager::assembleFragmentSnippets(
    const SnippetPtr& sceneFragmentShader, const BufferList& buffers,
    const LightManagerPtr& lightMngr
)
{
    std::vector<SnippetPtr> fragmentSnippets;
    fragmentSnippets.reserve(2 + buffers.size());
    fragmentSnippets.push_back(sceneFragmentShader);

    for (auto buffer : buffers)
        fragmentSnippets.push_back(buffer->getFragmentShaderGLSL());

    if (lightMngr != NULL)
        fragmentSnippets.push_back(lightMngr->getFragmentShaderGLSL());

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
