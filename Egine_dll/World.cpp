#include "World.h"
#include <algorithm>

World::World()
    : camera_x(0), camera_y(0)
    , camera_size_x(800), camera_size_y(600)
    , gravity_x(0), gravity_y(980)
    , world_speed(1.0)
    , simulate_in_thread(false)
    , next_id(0)
    , has_background(false)
    , background_pos_x(0), background_pos_y(0)
    , background_size_x(800), background_size_y(600)
    , active(false)
{
}

World::~World()
{
    RemoveAllObjects();
}

int World::AddObject(Object* obj, bool is_environment)
{
    std::lock_guard<std::mutex> lock(objects_mutex);
    WorldObject wobj;
    wobj.obj = obj;
    wobj.is_environment = is_environment;
    wobj.id = next_id++;
    objects.push_back(wobj);
    
    std::sort(objects.begin(), objects.end(), [](const WorldObject& a, const WorldObject& b) {
        return a.id < b.id;
    });
    
    return wobj.id;
}

bool World::ChangeObjectId(Object* obj, int new_id)
{
    std::lock_guard<std::mutex> lock(objects_mutex);
    
    auto it = std::find_if(objects.begin(), objects.end(), [obj](const WorldObject& wobj) {
        return wobj.obj == obj;
    });
    
    if (it == objects.end()) {
        return false;
    }
    
    if (new_id < 0 || new_id > next_id) {
        return false;
    }
    
    int old_id = it->id;
    if (old_id == new_id) {
        return true;
    }
    
    for (auto& wobj : objects) {
        if (wobj.id >= new_id && wobj.id < old_id) {
            wobj.id++;
        } else if (wobj.id > old_id && wobj.id <= new_id) {
            wobj.id--;
        }
    }
    
    it->id = new_id;
    
    std::sort(objects.begin(), objects.end(), [](const WorldObject& a, const WorldObject& b) {
        return a.id < b.id;
    });
    
    return true;
}

void World::RemoveAllObjects()
{
    std::lock_guard<std::mutex> lock(objects_mutex);
    for (auto& wobj : objects) {
        delete wobj.obj;
    }
    objects.clear();
    next_id = 0;
}

void World::SetCamera(double x, double y, double size_x, double size_y)
{
    camera_x = x;
    camera_y = y;
    camera_size_x = size_x;
    camera_size_y = size_y;
}

void World::MoveCamera(double x, double y)
{
    camera_x += x;
    camera_y += y;
}

void World::ChangeCameraSize(double size_x, double size_y)
{
    camera_size_x = size_x;
    camera_size_y = size_y;
}

void World::GetCamera(double* x, double* y, double* size_x, double* size_y)
{
    if (x) *x = camera_x;
    if (y) *y = camera_y;
    if (size_x) *size_x = camera_size_x;
    if (size_y) *size_y = camera_size_y;
}

void World::SetGravity(double gx, double gy)
{
    gravity_x = gx;
    gravity_y = gy;
}

void World::GetGravity(double* gx, double* gy)
{
    if (gx) *gx = gravity_x;
    if (gy) *gy = gravity_y;
}

void World::SetWorldSpeed(double speed)
{
    world_speed = speed;
}

double World::GetWorldSpeed()
{
    return world_speed;
}

void World::SetSimulateType(bool in_thread)
{
    simulate_in_thread = in_thread;
}

bool World::GetSimulateType()
{
    return simulate_in_thread;
}

void World::AddSignalBefore(std::function<void(World*)> signal)
{
    signals_before.push_back(signal);
}

void World::AddSignalMid(std::function<void(World*)> signal)
{
    signals_mid.push_back(signal);
}

void World::AddSignalAfter(std::function<void(World*)> signal)
{
    signals_after.push_back(signal);
}

void World::AddKeyboardSignal(std::function<void(World*, int, bool)> signal)
{
    keyboard_signals.push_back(signal);
}

void World::AddMouseSignal(std::function<void(World*, double, double, int, bool)> signal)
{
    mouse_signals.push_back(signal);
}

void World::Render(int screen_width, int screen_height)
{
    std::lock_guard<std::mutex> lock(objects_mutex);

    Rectangle camera = {
        (float)camera_x,
        (float)camera_y,
        (float)camera_size_x,
        (float)camera_size_y
    };
    Vector2 screen = { (float)screen_width, (float)screen_height };

    if (has_background) {
        double world_left = camera.x - camera.width / 2.0;
        double world_top = camera.y - camera.height / 2.0;
        double scale_x = screen.x / camera.width;
        double scale_y = screen.y / camera.height;
        double bg_left_x = background_pos_x - background_size_x / 2.0;
        double bg_top_y = background_pos_y - background_size_y / 2.0;
        double screen_x = (bg_left_x - world_left) * scale_x;
        double screen_y = (bg_top_y - world_top) * scale_y;
        double screen_w = background_size_x * scale_x;
        double screen_h = background_size_y * scale_y;
        float tex_scale = (float)(screen_h / background_texture.height);
        DrawTextureEx(background_texture, {(float)screen_x, (float)screen_y}, 0.0f, tex_scale, WHITE);
    } else {
        ClearBackground(RAYWHITE);
    }

    DrawText(TextFormat("Objects: %d", objects.size()), 10, 35, 20, BLUE);
    DrawText(TextFormat("Camera: %.0f, %.0f", camera_x, camera_y), 10, 60, 20, GREEN);

    for (auto it = objects.begin(); it != objects.end(); ++it) {
        if (it->is_environment) {
            it->obj->Draw(camera, screen);
        }
    }

    for (auto it = objects.begin(); it != objects.end(); ++it) {
        if (!it->is_environment) {
            it->obj->Draw(camera, screen);
        }
    }
}

void World::Simulate(double delta_time)
{
    std::lock_guard<std::mutex> lock(objects_mutex);
    
    double dt = delta_time * world_speed;
    
    for (auto& signal : signals_before) {
        signal(this);
    }
    
    for (auto& wobj : objects) {
        if (!wobj.is_environment) {
            wobj.obj->AddAcceleration(gravity_x, gravity_y);
        }
    }
    
    for (auto& env_obj : objects) {
        if (env_obj.is_environment) {
            for (auto& moving_obj : objects) {
                if (!moving_obj.is_environment) {
                    env_obj.obj->Hit(*moving_obj.obj, false);
                }
            }
        }
    }
    
    for (auto& signal : signals_mid) {
        signal(this);
    }
    
    for (auto& wobj : objects) {
        if (!wobj.is_environment) {
            wobj.obj->Move(dt);
        }
    }
    
    for (auto& signal : signals_after) {
        signal(this);
    }
}

void World::ProcessKeyboard(int key, bool pressed)
{
    for (auto& signal : keyboard_signals) {
        signal(this, key, pressed);
    }
}

void World::ProcessMouse(double world_x, double world_y, int button, bool pressed)
{
    for (auto& signal : mouse_signals) {
        signal(this, world_x, world_y, button, pressed);
    }
}

void World::LoadTextures()
{
    std::lock_guard<std::mutex> lock(objects_mutex);
    for (auto& wobj : objects) {
        wobj.obj->SetSituate(true);
    }

    if (has_background && !background_path.empty()) {
        Image img = LoadImage(background_path.c_str());
        if (img.data != nullptr) {
            background_texture = LoadTextureFromImage(img);
            UnloadImage(img);
        } else {
            has_background = false;
        }
    }
}

void World::UnloadTextures()
{
    std::lock_guard<std::mutex> lock(objects_mutex);
    for (auto& wobj : objects) {
        wobj.obj->SetSituate(false);
    }
    if (has_background) {
        UnloadTexture(background_texture);
        has_background = false;
    }
}

void World::SetBackground(const char* image_path, double pos_x, double pos_y, double size_x, double size_y)
{
    background_path = image_path;
    background_pos_x = pos_x;
    background_pos_y = pos_y;
    background_size_x = size_x;
    background_size_y = size_y;
    has_background = !background_path.empty();
}

bool World::HasBackground()
{
    return has_background;
}

void World::SetActive(bool a)
{
    active = a;
}

bool World::IsActive()
{
    return active;
}
