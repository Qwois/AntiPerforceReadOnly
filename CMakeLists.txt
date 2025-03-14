cmake_minimum_required(VERSION 3.15)
project(ImGuiApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--subsystem,windows")

include_directories(
    external/glfw/include
    external/imgui
)

link_directories(external/glfw)

add_executable(ImGuiApp 
    src/main.cpp 
    src/gui.cpp 
    external/imgui/imgui.cpp 
    external/imgui/imgui_draw.cpp 
    external/imgui/imgui_widgets.cpp 
    external/imgui/imgui_tables.cpp 
    external/imgui/imgui_demo.cpp 
    external/imgui/imgui_impl_glfw.cpp 
    external/imgui/imgui_impl_opengl3.cpp
)

target_link_libraries(ImGuiApp PRIVATE 
    glfw3 
    opengl32 
    gdi32
)

