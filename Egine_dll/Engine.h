#ifndef ENGINE_H
#define ENGINE_H

#include "World.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>
#include <string>

class Engine
{
private:
    bool initialized;
    int screen_width;
    int screen_height;
    float fps;
    std::string window_title;

    World* current_world;
    World* displayed_world;
    World* pending_world;
    bool pending_show;
    std::mutex world_mutex;

    std::thread simulate_thread;
    std::thread render_thread;
    std::atomic<bool> running;
    std::atomic<bool> simulate_running;
    std::atomic<bool> render_running;
    std::atomic<bool> shutdown_requested;
    std::atomic<bool> window_ready;

    std::vector<bool> key_states;
    std::vector<bool> mouse_button_states;

    void SimulateLoop();
    void RenderLoop();
    void StartSimulationThread();
    void StartRenderThread();

public:
    Engine();
    ~Engine();

    void Init(int width, int height, const char* title, float target_fps = 60);
    void Shutdown();

    void ShowWorld(World* world);
    void WaitForClose();

    bool IsRunning();

    void ProcessEvents();
    void RenderFrame();
    void SimulateFrame();

    static Engine* GetInstance();
};

#endif
