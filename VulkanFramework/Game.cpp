#include "Game.h"

void Game::Run()
{
    //Initializes window
    InitWindow();
    //Initializes vulkan
    VulkanBackend::GetInstance()->InitVulkan(m_window, m_width, m_height);
    //Our main loop, handles everything for the program.
    MainLoop();
    //Cleans up upon exit.
    Cleanup();
}

void Game::InitWindow()
{
    //Initializes glfw
    glfwInit();

    //Window settings
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    //Creates and stores the window pointer
    m_window = glfwCreateWindow(m_width, m_height, m_windowName.c_str(), nullptr, nullptr);
}

void Game::MainLoop()
{
    //While the window ***isn't*** closing
    while (!glfwWindowShouldClose(m_window)) 
    {
        //Check events (input etc)
        glfwPollEvents();
        DrawFrame();
    }

    VulkanBackend::GetInstance()->WaitForIdle();
}

void Game::DrawFrame()
{
    VulkanBackend::GetInstance()->DrawFrame();
}

void Game::Cleanup()
{
    VulkanBackend::GetInstance()->CleanupVulkan();
    
    //Destroys window
    if (m_window != nullptr)
    {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }

    //Closes GLFW
    glfwTerminate();
}
