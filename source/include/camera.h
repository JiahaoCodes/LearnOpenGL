#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define ENABLE_FPS_CAMERA

class Camera{
public:

    enum class MOVEDIRECTION{
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
    };

    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;

    glm::vec3 worldUp;

    float yaw;
    float pitch;

    float speed;
    float sensitivity;
    float zoom;

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = -90.0f, float pitch = 0.0f) :position(position), front(glm::vec3(0.0f, 0.0f, -1.0f)), 
           up(up), worldUp(glm::vec3(0.0f, 1.0f, 0.0f)), yaw(yaw), pitch(pitch), speed(5.0f), sensitivity(0.1f), zoom(45.0f) {
        m_backup = Backup(position, up, yaw, pitch);
        updateCameraVectors(); 
    }

    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ,float yaw, float pitch)
        : Camera(glm::vec3(posX, posY, posZ), glm::vec3(upX, upY, upZ), yaw, pitch) {}

    glm::mat4 getViewMatrix() const {
        return glm::lookAt(position, position + front, up);
    }

    glm::vec3 getPosition() const {
        return position;
    }

    float getZoom() const {
        return zoom;
    }

    void reset() {
        *this = Camera(m_backup.position, m_backup.up, m_backup.yaw, m_backup.pitch);
    }

    void processKeyboard(MOVEDIRECTION direction, float deltaTime, float factor = 1.0f) {
        if (freezed) return;
        float velocity = speed * deltaTime * factor;
        if (direction == MOVEDIRECTION::FORWARD)
            position += front * velocity;
        if (direction == MOVEDIRECTION::BACKWARD)
            position -= front * velocity;
        if (direction == MOVEDIRECTION::LEFT)
            position -= right * velocity;
        if (direction == MOVEDIRECTION::RIGHT)
            position += right * velocity;
    }

    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true) {
        if (freezed) return;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (constrainPitch) {
            pitch = clamp(pitch, -89.0f, 89.0f);
        }

        updateCameraVectors();
    }

    void processMouseScroll(float yoffset) {
        if (freezed) return;
        zoom -= yoffset;
        zoom = clamp(zoom, 1.0f, 45.0f);
    }

    void freeze() {
        freezed = true;
    }

    void unfreeze() {
        freezed = false;
    }

    void changeFreezedState() {
        freezed = !freezed;
    }

private:

    bool freezed = false;

    struct Backup {
        glm::vec3 position;
        glm::vec3 up;
        float yaw;
        float pitch;
        Backup() {}
        Backup(glm::vec3 position, glm::vec3 up, float yaw, float pitch) :position(position), up(up), yaw(yaw), pitch(pitch) {}
    } m_backup;

    // 限制一个变量的范围
    float clamp(float value, float min, float max) const {
        if (value < min) return min;
        if (value > max) return max;
        return value;
    }

    void updateCameraVectors() {
        glm::vec3 newFront;
        newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        newFront.y = sin(glm::radians(pitch));
        newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        this->front = glm::normalize(newFront);
        this->right = glm::normalize(glm::cross(this->front, this->worldUp));
        this->up = glm::normalize(glm::cross(this->right, this->front));
    }
};

#endif // CAMERA_H
