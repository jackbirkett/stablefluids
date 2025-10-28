#include "vendor/imgui.h"
#include "vendor/imgui_impl_glfw.h"
#include "vendor/imgui_impl_opengl3.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include "shader.h"
#include "fluidsolver.h"

const unsigned int SCREEN_WIDTH = 1080;
const unsigned int SCREEN_HEIGHT = 1080;
const unsigned int N = 128;
const unsigned int SIZE = (N + 2) * (N + 2);
float dt = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f; // time of last frame
std::vector<float> u(SIZE), v(SIZE), u_prev(SIZE), v_prev(SIZE), dens(SIZE), dens_prev(SIZE), data(3 * N * N);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, const double& mx, const double& my, const double& prevMouseX, const double& prevMouseY);

int main()
{
    if (!glfwInit())
    {
        std::cout << "GLFW failed to be initialized." << std::endl;
        glfwTerminate();
        return -1;
    }

    // OpenGL version 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Fluid Simulation", NULL, NULL);
   
    if (!window)
    {
        std::cout << "GLFW failed to create window." << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW failed to be initialized." << std::endl;
        return -1;
    }

    float vertices[] =
    {
        //  x     y       u     v
        -1.0f, -1.0f,   0.0f, 0.0f,
        -1.0f,  1.0f,   0.0f, 1.0f,  
         1.0f,  1.0f,   1.0f, 1.0f,
         1.0f, -1.0f,   1.0f, 0.0f 
    };

    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    Shader fluidShader("vertexshader.glsl", "fragmentshader.glsl");

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    float visc = 0.0001f;
    float diff = 0.0001f;
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, N, N, 0, GL_RGB, GL_FLOAT, &data[0]);
    glGenerateMipmap(GL_TEXTURE_2D);

    double prevMouseX = 0, prevMouseY = 0;

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    float color[] = { 1.0f, 1.0f, 1.0f };

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        dt = currentFrame - lastFrame;
        lastFrame = currentFrame;

        double mx, my;
        glfwGetCursorPos(window, &mx, &my);

        if(!io.WantCaptureMouse)
            processInput(window, mx, my, prevMouseX, prevMouseY);

        prevMouseX = mx;
        prevMouseY = my;

        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        fluidShader.bind();
        glBindVertexArray(VAO);

        fluidShader.setUniform1i("fluidTexture", 0);

        std::swap(u_prev, u);
        std::swap(v_prev, v);

        diffuse(N, 1, u, u_prev, visc, dt);
        diffuse(N, 2, v, v_prev, visc, dt);
        project(N, u, v, u_prev, v_prev);
        
        std::swap(u_prev, u);
        std::swap(v_prev, v);

        advect(N, 1, u, u_prev, u_prev, v_prev, dt);
        advect(N, 2, v, v_prev, u_prev, v_prev, dt);
        project(N, u, v, u_prev, v_prev);
        
        std::swap(dens_prev, dens);
        diffuse(N, 0, dens, dens_prev, diff, dt);
        std::swap(dens_prev, dens);
        advect(N, 0, dens, dens_prev, u, v, dt);

        for (unsigned int i = 1; i <= N; i++) 
        {
            for (unsigned int j = 1; j <= N; j++)
            {
                int pixel_index = ((i - 1) + N * (j - 1)) * 3;
                data[pixel_index + 0] = dens[getIndex(i, j)] * color[0]; // red
                data[pixel_index + 1] = dens[getIndex(i, j)] * color[1]; // green
                data[pixel_index + 2] = dens[getIndex(i, j)] * color[2]; // blue
            }
        }

        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, N, N, GL_RGB, GL_FLOAT, &data[0]);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        ImGui::Begin("Controls");                          
        ImGui::Text("FPS: %.1f", io.Framerate);
        ImGui::SliderFloat("Kinematic Viscosity", &visc, 0.0001f, 0.01f, "");
        ImGui::SliderFloat("Diffusivity", &diff, 0.0001f, 0.01f, "");
        ImGui::ColorEdit3("Fluid Color", color);
        if (ImGui::Button("Clear")) 
        {
            for (unsigned int i = 0; i < SIZE; i++)
                u[i] = 0.0f, v[i] = 0.0f, u_prev[i] = 0.0f, v_prev[i] = 0.0f, dens[i] = 0.0f, dens_prev[i] = 0.0f;
        }

        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window, const double& mouseX, const double& mouseY, const double& prevMouseX, const double& prevMouseY)
{
    int winWidth, winHeight;
    glfwGetWindowSize(window, &winWidth, &winHeight);

    // map mouse to grid
    int gridX = (int)((mouseX / winWidth) * N);
    int gridY = (int)(((winHeight - mouseY) / winHeight) * N);

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) 
    {
        addSource(gridX, gridY, dens, 100.0f);
        // velocity based on mouse drag
        float dx = (float)(mouseX - prevMouseX);
        float dy = (float)(mouseY - prevMouseY);
        addSource(gridX, gridY, u, dx * 3.0f);
        addSource(gridX, gridY, v, dy * -3.0f);
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}