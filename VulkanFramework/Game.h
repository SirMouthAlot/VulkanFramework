#ifndef __GAME_H__
#define __GAME_H__

#include "VulkanBackend.h"

class Game {
public:
    //Runs our application
    void Run();

private:
    void InitWindow();

    void MainLoop();
    
    void Cleanup();

    //Window variables
    GLFWwindow* m_window = nullptr;
    std::string m_windowName = "Vulkan";
    const int m_width = 800;
    const int m_height = 600;
};

#endif // !__GAME_H__