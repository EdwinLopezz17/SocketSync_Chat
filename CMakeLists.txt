cmake_minimum_required(VERSION 4.2)
project(SocketSync_Chat)

set(CMAKE_CXX_STANDARD 20)

find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

include_directories(include)

add_executable(SocketSync_Server src/server/main_server.cpp)
target_link_libraries(SocketSync_Server PRIVATE Threads::Threads)

add_executable(ChatClient_CLI
        src/client_cli/main_cli.cpp
        src/network/ChatClient.cpp
)
target_link_libraries(ChatClient_CLI PRIVATE Threads::Threads)

set(IMGUI_SOURCES
        external/imgui/imgui.cpp
        external/imgui/imgui_draw.cpp
        external/imgui/imgui_widgets.cpp
        external/imgui/imgui_tables.cpp
        external/imgui/backends/imgui_impl_glfw.cpp
        external/imgui/backends/imgui_impl_opengl3.cpp
)

add_executable(ChatClient_GUI
        src/client_gui/main_gui.cpp
        src/network/ChatClient.cpp
        ${IMGUI_SOURCES}
)

target_include_directories(ChatClient_GUI PRIVATE
        include
        external/imgui
        external/imgui/backends
)

target_link_libraries(ChatClient_GUI PRIVATE
        Threads::Threads
        glfw
        OpenGL::GL
)