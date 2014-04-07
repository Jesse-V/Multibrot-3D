
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

#ifndef BRIDGE
#define BRIDGE

#include <GL/glew.h>
#include <vector>
#include <string>

class Bridge
{
    public:
        struct PairedData
        {
            PairedData() :
                outOfSync_(true)
            {}

            PairedData(std::string name) :
                name_(name), outOfSync_(true)
            {}

            std::string name_;
            bool outOfSync_;
        };

        static Bridge& getInstance();
        GLint query(const PairedData& data, GLuint handle);

    private:
        static Bridge* singleton_;

        struct Link
        {
            Link(std::string name, GLuint handle, GLint loc) :
                varName_(name), progHandle_(handle), location_(loc)
            {}

            std::string varName_;
            GLuint progHandle_;
            GLint location_;
        };

        std::vector<Link> cache_;
        //std::unordered_map<std::pair<GLuint, std::string>, GLint> cache_;
};

#endif
