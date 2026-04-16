#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "network/ChatClient.hpp"
#include "GLFW/glfw3.h"
#include <iostream>

enum class AppState {LOGIN, CHAT};

int main() {
    if (!glfwInit()) return 1;
    const char* glsl_version = "#version 130";
    GLFWwindow* window = glfwCreateWindow(1280, 720, "SocketSync Chat", nullptr, nullptr);
    if (!window) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ChatClient client;
    AppState currentState = AppState::LOGIN;
    char username[32] = "";
    char messageBuffer[256] = "";
    char ipBuffer[32] = "127.0.0.2";
    int portNumber = 8080;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (currentState == AppState::LOGIN) {
            ImGui::Begin("Login", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::InputText("Username", username, IM_ARRAYSIZE(username));
            ImGui::InputText("Server IP", ipBuffer, IM_ARRAYSIZE(ipBuffer));
            ImGui::InputInt ("Port", &portNumber );

            if (ImGui::Button("Connect")) {
                if (client.connectToServer(ipBuffer, portNumber)) {
                    std::string joinMsg = std::string(username) + "has joined the chat!\n";
                    client.sendMessage(joinMsg);
                    currentState = AppState::CHAT;
                }
            }
            ImGui::End();
        }
        else if (currentState == AppState::CHAT) {
            ImGui::Begin("Chat Room");

            ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), false);
            for (const auto& msg: client.getMessages()) {
                ImGui::Text("%s", msg.c_str());
            }
            ImGui::EndChild();
            ImGui::Separator();

            if (ImGui::InputText("##Message", messageBuffer, IM_ARRAYSIZE(messageBuffer), ImGuiInputTextFlags_EnterReturnsTrue)
                || ImGui::Button("Send")) {
                std::string fullMsg = std::string(username) + ": "+messageBuffer;
                client.sendMessage(fullMsg);
                messageBuffer[0]='\0';
            }
            ImGui::End();
        }

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0,0,display_w,display_h);
        glClearColor(0.1f,0.1f,0.1f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

