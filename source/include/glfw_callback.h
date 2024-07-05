// 该文件中包含了glfw需要的回调函数，以及camera相关的交互函数 ：
//  -- glfw_error_callback
//  -- framebuffer_size_callback
//  -- mouse_callback
//  -- scroll_callback
//  -- processInput
// 你需要提前定义SCREEN_WIDTH 和 SCREEN_HEIGHT
// 你需要提前包含camera.h 或者 arcball_camera.h中的一种，且只能包含一种
// 你需要提前定义名为camera的相机类

#ifndef GLFW_CALLBACK_H
#define GLFW_CALLBACK_H

// comon for both camera types
float deltaTime = 0.0f;
float lastFrame = 0.0f;

float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.processMouseScroll(static_cast<float>(yoffset));
}

void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

/* ------------------------------------------ FPS camera specific --------------------------------------*/
#ifdef ENABLE_FPS_CAMERA

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.processMouseMovement(xoffset, yoffset);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS){
        camera.changeFreezedState();
    }

    float factor = 1.0f;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        factor = 2.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        camera.processKeyboard(Camera::MOVEDIRECTION::FORWARD, deltaTime, factor);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        camera.processKeyboard(Camera::MOVEDIRECTION::BACKWARD, deltaTime, factor);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        camera.processKeyboard(Camera::MOVEDIRECTION::LEFT, deltaTime, factor);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        camera.processKeyboard(Camera::MOVEDIRECTION::RIGHT, deltaTime, factor);
    }
}

#endif // ENABLE_FPS_CAMERA

/* ------------------------------------------ ARCBALL camera specific --------------------------------------*/

#ifdef ENABLE_ARCBALL_CAMERA

bool isPlanning = false;
bool isRotating = false;
bool isGuiFocused = false;

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn){
    if(isGuiFocused){
        return;
    }

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = lastX - xpos;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    ArcballCamera::MoveStyle moveStyle = ArcballCamera::MoveStyle::None;

    if (isPlanning) {
        moveStyle = ArcballCamera::MoveStyle::Planar;
    }
    
    if (isRotating) {
        moveStyle = ArcballCamera::MoveStyle::Arcball;
    }
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    if(width == 0 || height == 0) {
        return;
    }
    camera.processMouseMovement(width, height, xoffset, yoffset, moveStyle);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
        isPlanning = true;
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_RELEASE) {
        isPlanning = false;
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        isRotating = true;
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
        isRotating = false;
    }
}

#endif // ENABLE_ARCBALL_CAMERA

#endif // GLFW_CALLBACK_H
