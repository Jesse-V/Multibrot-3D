
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

#include "Bridge.hpp"
#include <iostream>
#include <typeinfo>


GLint Bridge::query(const PairedData& data, GLuint handle)
{
    bool found = false;
    GLint location = (GLint)NULL;
    for (auto link : cache_)
    {
        if (link.progHandle_ == handle && link.varName_ == data.name_)
        {
            location = link.location_;
            found = true;
            break;
        }
    }

    if (!found)
    {
        location = glGetUniformLocation(handle, data.name_.c_str());
        cache_.push_back(Link(data.name_, handle, location));
    }

    return location;
}



Bridge* Bridge::singleton_ = 0;

Bridge& Bridge::getInstance()
{
    try
    {
        if (singleton_)
            return *singleton_;

        std::cout << "Creating Bridge..." << std::endl;
        singleton_ = new Bridge();
        std::cout << "... done creating Bridge." << std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << std::endl;
        std::cerr << "Caught " << typeid(e).name() <<
            " during Bridge initiation: " << e.what() << std::endl;
    }

    return *singleton_;
}
