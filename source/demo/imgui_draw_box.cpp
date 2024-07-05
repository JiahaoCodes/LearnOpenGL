#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "arcball_camera.h"
#include "box.h"
#include "shader_program.h"
#include "textures_loader.h"

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

ArcballCamera camera(glm::vec3(0.0f, 0.0f, 10.0f));

#include "glfw_callback.h"

//TODO =======================================Define GUI DATA -- start=========================================*/
struct GuiData {
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    float distance = 10.0f;
} guiData;

void showImGuiWindow(ImGuiIO& io){
    static float f = 0.0f;
    static int counter = 0;

    ImGui::Begin("你好，世界！");

    ImGui::Text("This is some useful text.");

    ImGui::SliderFloat("camera distance", &guiData.distance, 5.0f, 50.0f);
    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
    ImGui::ColorEdit3("clear color", (float*)&guiData.clear_color);

    if (ImGui::Button("按钮"))
        counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);
    
    isGuiFocused = io.WantCaptureMouse;
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();
}
//TODO =======================================Define GUI DATA -- end=========================================*/
// Main code
int main(int, char**)
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    const char* glsl_version = "#version 450";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSwapInterval(1);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImFont* font = io.Fonts->AddFontFromFileTTF("fonts/MiSans-Regular.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());

    //TODO =======================================Render With OpenGL -- start=========================================*/
    // 加载纹理
    unsigned int texture = loadTexture("images/box_texture.jpg");
    using ShaderType = ShaderProgram::ShaderType;
    ShaderProgram shaderProgram(
        {
            {ShaderType::VERTEX, "shaders/draw_box.vert"},
            {ShaderType::FRAGMENT, "shaders/draw_box.frag"}
        }
    );
    // 创建Box
    glm::vec3 boxPositon(0.0f, 0.0f, 0.0f);
    glm::vec3 boxExtent(1.0f, 1.0f, 1.0f);
    Box box(boxPositon, boxExtent);

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_DEPTH_TEST);

    //TODO =======================================Render With OpenGL -- end=========================================*/

    while (!glfwWindowShouldClose(window)) {

        if(glfwGetWindowAttrib(window, GLFW_ICONIFIED)){
            glfwWaitEvents();
            continue;
        }

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glfwPollEvents();
        processInput(window);

        /* -------------------------------------------start ImGui frame---------------------------------------*/
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        showImGuiWindow(io);
        /* -------------------------------------------start ImGui frame---------------------------------------*/

        //TODO =======================================origanize DrawCALL -- start=========================================*/
        // 清屏
        glClearColor(guiData.clear_color.x, guiData.clear_color.y, guiData.clear_color.z, guiData.clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 绑定纹理
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        camera.setDistance(guiData.distance);

        // 绑定着色器
        shaderProgram.use();
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        if(width == 0 || height == 0){
            width = 1;
            height = 1;
        }
        glm::mat4 projection = glm::perspective(glm::radians(camera.getZoom()), (float)width / (float)height, 0.1f, 100.0f);
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

        shaderProgram.setUniform("model", model);
        shaderProgram.setUniform("view", view);
        shaderProgram.setUniform("projection", projection);

        box.draw();
       //TODO =======================================origanize DrawCALL -- end=========================================*/

       /* -------------------------------------------RENDER IMGUI---------------------------------------*/
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
        /* -------------------------------------------RENDER IMGUI---------------------------------------*/
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}