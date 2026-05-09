#include "Engine.h"
#include <chrono>
#include <thread>
#include <raylib.h>

static Engine* g_instance = nullptr;

Engine::Engine()
    : initialized(false)
    , screen_width(800)
    , screen_height(600)
    , fps(60)
    , window_title("RPG Engine")
    , current_world(nullptr)
    , displayed_world(nullptr)
    , pending_world(nullptr)
    , pending_show(false)
    , running(false)
    , simulate_running(false)
    , render_running(false)
    , shutdown_requested(false)
    , window_ready(false)
    , key_states(512, false)
    , mouse_button_states(8, false)
{
}

Engine::~Engine()
{
    Shutdown();
}

void Engine::Init(int width, int height, const char* title, float target_fps)
{
    if (initialized) {
        return;
    }

    screen_width = width;
    screen_height = height;
    fps = target_fps;
    window_title = title;

    initialized = true;
    running = true;
    shutdown_requested = false;

    if (simulate_thread.joinable()) {
        simulate_thread.join();
    }
    if (render_thread.joinable()) {
        render_thread.join();
    }

    StartRenderThread();
}

void Engine::StartSimulationThread()
{
    if (!simulate_running && initialized) {
        simulate_running = true;
        simulate_thread = std::thread(&Engine::SimulateLoop, this);
    }
}

void Engine::StartRenderThread()
{
    if (!render_running && initialized) {
        render_running = true;
        render_thread = std::thread(&Engine::RenderLoop, this);
    }
}

void Engine::Shutdown()
{
    if (!initialized) {
        return;
    }

    shutdown_requested = true;
    running = false;
    simulate_running = false;
    render_running = false;

    if (simulate_thread.joinable()) {
        simulate_thread.join();
    }
    if (render_thread.joinable()) {
        render_thread.join();
    }

    initialized = false;
}

void Engine::ShowWorld(World* world)
{
    std::lock_guard<std::mutex> lock(world_mutex);

    if (displayed_world) {
        displayed_world->SetActive(false);
    }

    pending_world = world;
    pending_show = true;
}

void Engine::WaitForClose()
{
    while (!shutdown_requested) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    Shutdown();
}

bool Engine::IsRunning()
{
    return window_ready && running && !shutdown_requested;
}

void Engine::ProcessEvents()
{
    if (!displayed_world || !displayed_world->IsActive()) {
        return;
    }

    for (int key = 0; key < 512; key++) {
        bool pressed = IsKeyDown(key);
        if (pressed != key_states[key]) {
            key_states[key] = pressed;
            displayed_world->ProcessKeyboard(key, pressed);
        }
    }

    for (int button = 0; button < 8; button++) {
        bool pressed = IsMouseButtonDown(button);
        if (pressed != mouse_button_states[button]) {
            mouse_button_states[button] = pressed;
            Vector2 mouse_pos = GetMousePosition();
            double cam_x, cam_y, cam_sx, cam_sy;
            displayed_world->GetCamera(&cam_x, &cam_y, &cam_sx, &cam_sy);
            double world_x = cam_x - cam_sx / 2 + mouse_pos.x * cam_sx / screen_width;
            double world_y = cam_y - cam_sy / 2 + mouse_pos.y * cam_sy / screen_height;
            displayed_world->ProcessMouse(world_x, world_y, button, pressed);
        }
    }
}

void Engine::RenderFrame()
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

    if (displayed_world && displayed_world->IsActive()) {
        displayed_world->Render(screen_width, screen_height);
    }

    EndDrawing();
}

void Engine::SimulateFrame()
{
    std::lock_guard<std::mutex> lock(world_mutex);
    if (displayed_world && displayed_world->IsActive() && !displayed_world->GetSimulateType()) {
        displayed_world->Simulate(GetFrameTime());
    }
}

void Engine::SimulateLoop()
{
    const double fixed_dt = 1.0 / 100.0;
    auto last_time = std::chrono::high_resolution_clock::now();
    double accumulator = 0.0;

    while (simulate_running) {
        auto current_time = std::chrono::high_resolution_clock::now();
        double frame_time = std::chrono::duration<double>(current_time - last_time).count();
        last_time = current_time;

        accumulator += frame_time;

        while (accumulator >= fixed_dt && simulate_running) {
            {
                std::lock_guard<std::mutex> lock(world_mutex);
                if (current_world && current_world->IsActive() && current_world->GetSimulateType()) {
                    current_world->Simulate(fixed_dt);
                }
            }
            accumulator -= fixed_dt;
        }

        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
}

void Engine::RenderLoop()
{
    InitWindow(screen_width, screen_height, window_title.c_str());
    SetTargetFPS((int)fps);
    window_ready = true;

    while (!shutdown_requested) {
        if (WindowShouldClose()) {
            shutdown_requested = true;
            break;
        }

        {
            std::lock_guard<std::mutex> lock(world_mutex);

            if (pending_show && pending_world != nullptr) {
                if (displayed_world) {
                    displayed_world->UnloadTextures();
                    displayed_world->SetActive(false);
                }

                current_world = pending_world;
                displayed_world = pending_world;
                pending_world = nullptr;
                pending_show = false;

                if (current_world) {
                    current_world->LoadTextures();
                    if (current_world->GetSimulateType()) {
                        StartSimulationThread();
                    }
                    current_world->SetActive(true);
                }
            }

            if (displayed_world && displayed_world->IsActive()) {
                ProcessEvents();

                if (current_world && !current_world->GetSimulateType()) {
                    current_world->Simulate(GetFrameTime());
                }

                BeginDrawing();
                ClearBackground(RAYWHITE);
                displayed_world->Render(screen_width, screen_height);
                EndDrawing();
            } else {
                BeginDrawing();
                ClearBackground(BLACK);
                EndDrawing();
            }
        }

        std::this_thread::sleep_for(std::chrono::microseconds(1000000 / (int)fps - 1000));
    }

    {
        std::lock_guard<std::mutex> lock(world_mutex);
        if (displayed_world) {
            displayed_world->UnloadTextures();
        }
    }

    CloseWindow();
}

Engine* Engine::GetInstance()
{
    if (!g_instance) {
        g_instance = new Engine();
    }
    return g_instance;
}
