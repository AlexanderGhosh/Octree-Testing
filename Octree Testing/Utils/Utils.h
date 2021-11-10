#pragma once
#include <glm.hpp>
void multDirMatrix(const glm::mat4& x, const glm::vec3& src, glm::vec3& dst)
{
    const float a = src[0] * x[0][0] + src[1] * x[1][0] + src[2] * x[2][0];
    const float b = src[0] * x[0][1] + src[1] * x[1][1] + src[2] * x[2][1];
    const float c = src[0] * x[0][2] + src[1] * x[1][2] + src[2] * x[2][2];

    dst.x = a;
    dst.y = b;
    dst.z = c;
}

void multVecMatrix(const glm::mat4& x, const glm::vec3& src, glm::vec3& dst)
{
    const float a = src[0] * x[0][0] + src[1] * x[1][0] + src[2] * x[2][0] + x[3][0];
    const float b = src[0] * x[0][1] + src[1] * x[1][1] + src[2] * x[2][1] + x[3][1];
    const float c = src[0] * x[0][2] + src[1] * x[1][2] + src[2] * x[2][2] + x[3][2];
    float w = src[0] * x[0][3] + src[1] * x[1][3] + src[2] * x[2][3] + x[3][3];
    w = 1.0f / w;

    dst.x = a * w;
    dst.y = b * w;
    dst.z = c * w;
}