#ifndef WORLD_H
#define WORLD_H

#include "Object.h"
#include <vector>
#include <functional>
#include <mutex>
#include <raylib.h>

class World
{
private:
    struct WorldObject {
        Object* obj;
        bool is_environment;
        int id;
    };

    std::vector<WorldObject> objects;
    std::mutex objects_mutex;

    double camera_x, camera_y;
    double camera_size_x, camera_size_y;
    double gravity_x, gravity_y;
    double world_speed;
    bool simulate_in_thread;
    int next_id;

    std::vector<std::function<void(World*)>> signals_before;
    std::vector<std::function<void(World*)>> signals_mid;
    std::vector<std::function<void(World*)>> signals_after;
    std::vector<std::function<void(World*, int, bool)>> keyboard_signals;
    std::vector<std::function<void(World*, double, double, int, bool)>> mouse_signals;

    Texture2D background_texture;
    std::string background_path;
    bool has_background;
    double background_pos_x, background_pos_y;
    double background_size_x, background_size_y;

    bool active;

public:
    World();
    ~World();
    
    int AddObject(Object* obj, bool is_environment);
    bool ChangeObjectId(Object* obj, int new_id);
    void RemoveAllObjects();
    
    void SetCamera(double x, double y, double size_x, double size_y);
    void MoveCamera(double x, double y);
    void ChangeCameraSize(double size_x, double size_y);
    void GetCamera(double* x, double* y, double* size_x, double* size_y);
    
    void SetGravity(double gx, double gy);
    void GetGravity(double* gx, double* gy);
    
    void SetWorldSpeed(double speed);
    double GetWorldSpeed();
    
    void SetSimulateType(bool in_thread);
    bool GetSimulateType();
    
    void AddSignalBefore(std::function<void(World*)> signal);
    void AddSignalMid(std::function<void(World*)> signal);
    void AddSignalAfter(std::function<void(World*)> signal);
    void AddKeyboardSignal(std::function<void(World*, int, bool)> signal);
    void AddMouseSignal(std::function<void(World*, double, double, int, bool)> signal);
    
    void Render(int screen_width, int screen_height);
    void Simulate(double delta_time);
    
    void ProcessKeyboard(int key, bool pressed);
    void ProcessMouse(double world_x, double world_y, int button, bool pressed);
    
    void LoadTextures();
    void UnloadTextures();

    void SetBackground(const char* image_path, double pos_x, double pos_y, double size_x, double size_y);
    bool HasBackground();

    void SetActive(bool a);
    bool IsActive();
};

#endif
