#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"


namespace implemented
{
    class DroneCamera
    {
     public:
        DroneCamera()
        {
            position    = glm::vec3(0, 2, 5);
            forward     = glm::vec3(0, 0, -1);
            up          = glm::vec3(0, 1, 0);
            globalUp    = glm::vec3(0, 1, 0);
            right       = glm::vec3(1, 0, 0);
            distanceToTarget = 3;
        }

        DroneCamera(const glm::vec3 &position, const glm::vec3 &center, const glm::vec3 &up)
        {
            Set(position, center, up);
        }

        ~DroneCamera()
        { }

        // Update camera
        void Set(const glm::vec3 &position, const glm::vec3 &center, const glm::vec3 &up)
        {
            this->position = position;
            forward     = glm::normalize(center - position);
            right       = glm::cross(forward, up);
            this->up    = glm::cross(right, forward);
        }

        void MoveForward(float distance)
        {
            glm::vec3 dir = glm::normalize(glm::vec3(forward.x, 0, forward.z));
            position += dir * distance;
        }

        void MoveRight(float distance)
        {
            position += distance * right;
        }

        void TranslateForward(float distance)
        {
            position += distance * forward;
        }

        void TranslateUpward(float distance)
        {
            position += distance * globalUp;
        }

        void TranslateRight(float distance)
        {
            glm::vec3 vec = glm::normalize(glm::vec3(right.x, 0, right.z));
            position += distance * vec;
        }

        void RotateThirdPerson_OX(float angle)
        {
            TranslateForward(distanceToTarget);
            forward = glm::vec3(glm::rotate(glm::mat4(1.0f), angle, right) * glm::vec4(forward, 1));
            forward = glm::normalize(forward);

            up = glm::cross(right, forward);
            up = glm::normalize(up);
            TranslateForward(-distanceToTarget);
        }

        void RotateThirdPerson_OY(float angle)
        {
            TranslateForward(distanceToTarget);
            forward = glm::vec3(glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0)) * glm::vec4(forward, 1));
            forward = glm::normalize(forward);

            right = glm::vec3(glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0)) * glm::vec4(right, 1));
            right = glm::normalize(right);

            up = glm::cross(right, forward);
            up = glm::normalize(up);
            TranslateForward(-distanceToTarget);
        }

        void RotateThirdPerson_OZ(float angle)
        {
            TranslateForward(distanceToTarget);

            right = glm::rotate(glm::mat4(1.0f), angle, forward) * glm::vec4(right, 1);
            right = glm::normalize(right);
            up = glm::cross(right, forward);
            up = glm::normalize(up);

            TranslateForward(-distanceToTarget);
        }

        glm::mat4 GetViewMatrix()
        {
            // Returns the view matrix
            return glm::lookAt(position, position + forward, up);
        }

        glm::vec3 GetTargetPosition()
        {
            return position + forward * distanceToTarget;
        }

     public:
        float distanceToTarget;
        glm::vec3 position;
        glm::vec3 forward;
        glm::vec3 right;
        glm::vec3 up;
        glm::vec3 globalUp;
    };
}   // namespace implemented
