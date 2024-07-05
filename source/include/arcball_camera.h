#ifndef ARCBALL_CAMERA_H
#define ARCBALL_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define ENABLE_ARCBALL_CAMERA

class ArcballCamera{
public:
    enum class MoveStyle {
        Arcball,
        Planar,
        None
    };

    ArcballCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 10.0f),
                  glm::vec3 lookat = glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f)) : m_worldUp(worldUp), m_position(position), m_frontPoint(lookat){
        m_backup = Backup(position, lookat, worldUp);

        glm::vec3 frontDir = getFrontDir();
        glm::vec3 rightDir = glm::normalize(glm::cross(frontDir, worldUp));
        glm::vec3 upDir = glm::normalize(glm::cross(rightDir, frontDir));
        m_rightPoint = m_position + rightDir;
        m_upPoint = m_position + upDir;
    }

    ArcballCamera(float posX, float posY, float posZ,
                  float lookX, float lookY, float lookZ,
                  float upX, float upY, float upZ)
        : ArcballCamera(glm::vec3(posX, posY, posZ), glm::vec3(lookX, lookY, lookZ), glm::vec3(upX, upY, upZ)){}

    ~ArcballCamera(){}

    glm::mat4 getViewMatrix() const {
        glm::vec3 front = getFrontDir();
        glm::vec3 right = getRightDir();
        glm::vec3 up = getUpDir();

        glm::mat4 result = glm::mat4(1.0f);
        result[0][0] = right.x;
        result[1][0] = right.y;
        result[2][0] = right.z;
        result[0][1] = up.x;
        result[1][1] = up.y;
        result[2][1] = up.z;
        result[0][2] = -front.x;
        result[1][2] = -front.y;
        result[2][2] = -front.z;
        result[3][0] = -glm::dot(right, m_position);
        result[3][1] = -glm::dot(up, m_position);
        result[3][2] = glm::dot(front, m_position);
        return result;
    }

    glm::vec3 getPosition() const {
        return m_position;
    }

    float getZoom() const {
        return m_zoom;
    }

    void reset(){
        *this = ArcballCamera(m_backup.position, m_backup.lookat, m_backup.worldUp);
    }

    void setDistance(float distance){
        glm::vec3 front = getFrontDir();
        glm::vec3 right = getRightDir();
        glm::vec3 up = getUpDir();
        m_position = m_frontPoint - distance * front;
        m_rightPoint = m_position + right;
        m_upPoint = m_position + up;
    }

    void setSensitive(float sensitive){
        m_sensitive = sensitive;
    }

    void processMouseScroll(float yoffset) {
        m_zoom -= yoffset;
        m_zoom = clamp(m_zoom, 1.0f, 45.0f);
    }

    void processMouseMovement(int viewWidth, int viewHeight, float xoffset, float yoffset, MoveStyle moveStyle = MoveStyle::Arcball) {
        auto pos = m_position;
        auto pivot = m_frontPoint;
        glm::vec3 front = getFrontDir();
        glm::vec3 right = getRightDir();
        glm::vec3 up = getUpDir();

        if (moveStyle == MoveStyle::Planar) {
            glm::vec3 offset = m_sensitive * glm::vec3(xoffset * right - yoffset * up);
            m_position = pos + offset;
            m_frontPoint = pivot + offset;
            m_rightPoint = m_position + right;
            m_upPoint = m_position + up;
        } else if (moveStyle == MoveStyle::Arcball) {
            float deltaAngleX = (2.0f * PI / viewWidth) * xoffset;
            float deltaAngleY = (PI / viewHeight) * yoffset;

            glm::vec3 newPosition = rotatePoint(pos, deltaAngleX, deltaAngleY); 
            glm::vec3 newRightPoint = rotatePoint(m_rightPoint, deltaAngleX, deltaAngleY);
            glm::vec3 newUpPoint = rotatePoint(m_upPoint, deltaAngleX, deltaAngleY);

            m_position = newPosition;
            m_rightPoint = newRightPoint;
            m_upPoint = newUpPoint;
        }
    }

private:

    static constexpr float PI = 3.14159265359f;

    glm::vec3 m_position;
    glm::vec3 m_frontPoint;
    glm::vec3 m_rightPoint;
    glm::vec3 m_upPoint;

    glm::vec3 m_worldUp;

    float m_sensitive = 0.01f;
    float m_zoom = 45.0f;

    struct Backup {
        glm::vec3 position;
        glm::vec3 lookat;
        glm::vec3 worldUp;
        Backup() {}
        Backup(glm::vec3 position, glm::vec3 lookat, glm::vec3 worldUp) : position(position), lookat(lookat), worldUp(worldUp) {}
    } m_backup;

    // 限制一个变量的范围
    float clamp(float value, float min, float max) const {
        if (value < min) return min;
        if (value > max) return max;
        return value;
    }

    glm::vec3 getFrontDir() const {
        return glm::normalize(m_frontPoint - m_position);
    }

    glm::vec3 getRightDir() const {
        return glm::normalize(m_rightPoint - m_position);
    }

    glm::vec3 getUpDir() const {
        return glm::normalize(m_upPoint - m_position);
    }

    glm::vec3 rotatePoint(glm::vec3 point, float angleX, float angleY) const {
        glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), angleX, getUpDir());
        glm::vec3 result = glm::vec3(rotationX * glm::vec4(point - m_frontPoint, 1.0f)) + m_frontPoint;

        glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), angleY, getRightDir());
        result = glm::vec3(rotationY * glm::vec4(result - m_frontPoint, 1.0f)) + m_frontPoint;
        return result;
    }
};

#endif // ARCBALL_CAMERA_H
