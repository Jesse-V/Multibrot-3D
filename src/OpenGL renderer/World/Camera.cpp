
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

#include "Camera.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <memory>
#include <sstream>
#include <stdexcept>
#include <iostream>


Camera::Camera() :
    projectionUpdated_(false), viewUpdated_(false)
{
    reset();
}



// Reset the camera back to its defaults
void Camera::reset()
{
    auto startPos = glm::vec3(0, -1, 0);
    setPosition(startPos);
    lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1));
    translate(-startPos);

    fieldOfView_   = 45.0f;         // frustrum viewing aperture
    aspectRatio_   = 4.0f / 3.0f;   // frustrum view angling
    nearFieldClip_ = 0.005f;        // clip anything closer than this
    farFieldClip_  = 65536.0f;         // clip anything farther than this
    updateProjectionMatrix();
}



void Camera::startSync()
{
    if (viewUpdated_)
    {
        temporaryViewMatrix_ = calculateViewMatrix();
        syncView_ = true;
        viewUpdated_ = false;
    }

    if (projectionUpdated_)
    {
        temporaryProjMatrix_ = getProjectionMatrix();
        syncProjection_ = true;
        projectionUpdated_ = false;
    }
}



void Camera::sync(GLint viewMatrixUniform, GLint projMatrixUniform)
{
    if (viewMatrixUniform < 0 || projMatrixUniform < 0)
        throw std::runtime_error("Unable to find Camera uniform variables!");

    //assemble view matrix and sync if it has updated
    if (syncView_)
        glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE,
                                        glm::value_ptr(temporaryViewMatrix_));

    //sync projection matrix if it has updated
    if (syncProjection_)
        glUniformMatrix4fv(projMatrixUniform, 1, GL_FALSE,
                                        glm::value_ptr(temporaryProjMatrix_));
}



void Camera::endSync()
{
    syncView_ = viewUpdated_;
    syncProjection_ = projectionUpdated_;
}



// Set the camera to an arbitrary location without changing orientation
void Camera::setPosition(const glm::vec3& newPosition)
{
    if (newPosition == lookingAt_)
        throw std::runtime_error("Cannot be where we're looking at!");

    position_ = newPosition;
    viewUpdated_ = true;
}



// Set the orientation of the camera without changing its position_
void Camera::lookAt(const glm::vec3& gazePoint, const glm::vec3& upVector)
{
    if (gazePoint == position_)
        throw std::runtime_error("Cannot look at same point as position!");

    lookingAt_ = gazePoint;
    upVector_ = upVector;
    viewUpdated_ = true;
}



void Camera::translateX(float magnitude)
{
    translate(glm::vec3(magnitude, 0, 0));
}



void Camera::translateY(float magnitude)
{
    translate(glm::vec3(0, magnitude, 0));
}



void Camera::translateZ(float magnitude)
{
    translate(glm::vec3(0, 0, magnitude));
}



//translate the camera along X/Y/Z
void Camera::translate(const glm::vec3& delta)
{
    position_ += delta;
    lookingAt_ += delta;
    viewUpdated_ = true;
}



void Camera::moveForward(float magnitude)
{
    translate(calculateLookDirection() * magnitude);
}



void Camera::moveRight(float magnitude)
{
    glm::vec3 orientation = calculateLookDirection();
    glm::vec3 tangental = glm::normalize(glm::cross(orientation, upVector_));
    translate(tangental * magnitude);
}



void Camera::moveUp(float magnitude)
{
    translate(glm::normalize(upVector_) * magnitude);
}



// Pitch the camera along the axis orthogonal to the up and look vectors
void Camera::pitch(float theta)
{
    glm::vec3 lookVector = lookingAt_ - position_;
    glm::vec3 tangental = glm::cross(lookVector, upVector_);
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(), theta, tangental);

    upVector_  = (rotationMatrix * glm::vec4(upVector_,  1)).xyz();
    lookVector = (rotationMatrix * glm::vec4(lookVector, 1)).xyz();
    lookingAt_ = position_ + lookVector;
    viewUpdated_ = true;
}



// Yaw the camera typically around the up vector.
// If aroundUpVector is false, the camera yaws around (0, 0, 1), the global up
void Camera::yaw(float theta, bool aroundUpVector)
{
    glm::vec3 vectorOfRotation = aroundUpVector ? upVector_ : glm::vec3(0, 0, 1);
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(), theta, vectorOfRotation);

    glm::vec3 lookVector = lookingAt_ - position_;
    lookVector = (rotationMatrix * glm::vec4(lookVector, 1)).xyz();
    upVector_  = (rotationMatrix * glm::vec4(upVector_,  1)).xyz();
    lookingAt_ = position_ + lookVector;
    viewUpdated_ = true;
}



//rolls the camera by rotating the up vector around the look direction
void Camera::roll(float theta)
{
    glm::vec3 orientation = calculateLookDirection();
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(), theta, orientation);
    upVector_ = (rotationMatrix * glm::vec4(upVector_, 1)).xyz();
    viewUpdated_ = true;
}



//returns true if the pitch attempt failed due to constraints
bool Camera::constrainedPitch(float theta)
{
    auto oldUpVector = upVector_;
    auto oldLookingAt = lookingAt_;

    pitch(theta);

    /*
        revert the pitch if any of the conditions are true:
        (orientation.z < 0 && upVector_.z < 0) ==> if looking down upside-down
        (orientation.z > 0 && upVector_.z < 0) ==> if looking up but tilted back
    */

    if (upVector_.z < 0 && (float)fabs(calculateLookDirection().z) > 0.00001f) // != 0
    {
        upVector_ = oldUpVector;
        lookingAt_ = oldLookingAt;
        viewUpdated_ = true;
        //std::cout << "Camera pitch constrained. Reverted request." << std::endl;
        return true;
    }

    return false;
}



bool Camera::constrainedRoll(float theta)
{
    auto oldUpVector = upVector_;
    roll(theta);

    if (upVector_.z < 0)
    {
        upVector_ = oldUpVector;
        viewUpdated_ = true;
        std::cout << "Camera roll constrained. Reverted request." << std::endl;
        return true;
    }

    return false;
}



void Camera::setFieldOfView(float degrees)
{
    fieldOfView_ = degrees;
    updateProjectionMatrix();
}



void Camera::setAspectRatio(float ratio)
{
    aspectRatio_ = ratio;
    updateProjectionMatrix();
}



void Camera::setNearFieldClipDistance(float distance)
{
    nearFieldClip_ = distance;
    updateProjectionMatrix();
}



void Camera::setFarFieldClipDistance(float distance)
{
    farFieldClip_ = distance;
    updateProjectionMatrix();
}



// Sets the perspective of the camera
void Camera::setPerspective(
    float fieldOfViewDegrees, float aspectRatio,
    float nearClipDistance, float farClipDistance
)
{
    aspectRatio_   = aspectRatio;
    nearFieldClip_ = nearClipDistance;
    farFieldClip_  = farClipDistance;
    setFieldOfView(fieldOfViewDegrees);
}



void Camera::updateProjectionMatrix()
{
    projection_ = glm::perspective(fieldOfView_, aspectRatio_,
                                   nearFieldClip_, farFieldClip_);
    projectionUpdated_ = true;
}



//accessors:

glm::vec3 Camera::getPosition() const
{
    return position_;
}



glm::vec3 Camera::getLookingAt() const
{
    return lookingAt_;
}



glm::vec3 Camera::calculateLookDirection() const
{
    return glm::normalize(lookingAt_ - position_);
}



glm::vec3 Camera::getUpVector() const
{
    return upVector_;
}



glm::mat4 Camera::calculateViewMatrix() const
{
    return glm::lookAt(getPosition(), getLookingAt(), getUpVector());
}



float Camera::getFOV() const
{
    return fieldOfView_;
}



float Camera::getAspectRatio() const
{
    return aspectRatio_;
}



float Camera::getNearFieldClip() const
{
    return nearFieldClip_;
}



float Camera::getFarFieldClip() const
{
    return farFieldClip_;
}



glm::mat4 Camera::getProjectionMatrix() const
{
    return projection_;
}



std::string Camera::toString() const
{
    std::stringstream ss;

    ss << "Look@: <" << lookingAt_.x << ", " << lookingAt_.y <<
                    ", " << lookingAt_.z << "> ";
    ss << "UpV: <"   << upVector_.x << ", " << upVector_.y <<
                    ", " << upVector_.z << "> ";
    ss << "Pos: <"   << position_.x << ", " << position_.y <<
                    ", " << position_.z << ">";

    return ss.str();
}
