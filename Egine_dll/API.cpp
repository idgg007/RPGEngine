#include <raylib.h>
#include "cheatsheet.h"
#include "Engine.h"
#include "World.h"
#include "Object.h"

extern "C" {

RPGAPI void Init(float width, float height, const char* title)
{
    Engine::GetInstance()->Init((int)width, (int)height, title, 60);
}

RPGAPI void InitWithFps(float width, float height, const char* title, float fps)
{
    Engine::GetInstance()->Init((int)width, (int)height, title, fps);
}

RPGAPI void Shutdown()
{
    Engine::GetInstance()->Shutdown();
}

RPGAPI bool IsRunning()
{
    return Engine::GetInstance()->IsRunning();
}

RPGAPI void ProcessEvents()
{
    Engine::GetInstance()->ProcessEvents();
}

RPGAPI void RenderFrame()
{
    Engine::GetInstance()->RenderFrame();
}

RPGAPI void SimulateFrame()
{
    Engine::GetInstance()->SimulateFrame();
}

RPGAPI void* CreateWorld()
{
    return new World();
}

RPGAPI void* CreateObject()
{
    return new Object();
}

RPGAPI int AddEnvironment(void* world, void* object)
{
    World* w = static_cast<World*>(world);
    Object* o = static_cast<Object*>(object);
    return w->AddObject(o, true);
}

RPGAPI int AddCreature(void* world, void* object)
{
    World* w = static_cast<World*>(world);
    Object* o = static_cast<Object*>(object);
    return w->AddObject(o, false);
}

RPGAPI bool ChangeObjectId(void* world, void* object, int new_id)
{
    World* w = static_cast<World*>(world);
    Object* o = static_cast<Object*>(object);
    return w->ChangeObjectId(o, new_id);
}

RPGAPI void ShowWorld(void* world)
{
    World* w = static_cast<World*>(world);
    Engine::GetInstance()->ShowWorld(w);
}

RPGAPI void WaitForClose()
{
    Engine::GetInstance()->WaitForClose();
}

RPGAPI void SetCameraPos(void* world, double x, double y, double size_x, double size_y)
{
    World* w = static_cast<World*>(world);
    w->SetCamera(x, y, size_x, size_y);
}

RPGAPI void MoveCamera(void* world, double x, double y)
{
    World* w = static_cast<World*>(world);
    w->MoveCamera(x, y);
}

RPGAPI void ChangeCameraSize(void* world, double size_x, double size_y)
{
    World* w = static_cast<World*>(world);
    w->ChangeCameraSize(size_x, size_y);
}

RPGAPI void SetGravity(void* world, double g_x, double g_y)
{
    World* w = static_cast<World*>(world);
    w->SetGravity(g_x, g_y);
}

RPGAPI void SetWorldSpeed(void* world, double speed)
{
    World* w = static_cast<World*>(world);
    w->SetWorldSpeed(speed);
}

RPGAPI void SetSimulateType(void* world, bool type)
{
    World* w = static_cast<World*>(world);
    w->SetSimulateType(type);
}

RPGAPI void SetObjectPos(void* object, double x, double y)
{
    Object* o = static_cast<Object*>(object);
    o->SetPosition(x, y);
}

RPGAPI void GetObjectPos(void* object, double* x, double* y)
{
    Object* o = static_cast<Object*>(object);
    o->GetPosition(x, y);
}

RPGAPI void SetObjectSize(void* object, double size_x, double size_y)
{
    Object* o = static_cast<Object*>(object);
    o->SetSize(size_x, size_y);
}

RPGAPI void GetObjectSize(void* object, double* size_x, double* size_y)
{
    Object* o = static_cast<Object*>(object);
    o->GetSize(size_x, size_y);
}

RPGAPI void AddHitBox(void* object, double x1, double y1, double x2, double y2, double hardness)
{
    Object* o = static_cast<Object*>(object);
    o->AddHitLine(x1, y1, x2, y2, hardness);
}

RPGAPI void CreateRectHitbox(void* object, double hardness)
{
    Object* o = static_cast<Object*>(object);
    o->CreateRectHitbox(hardness);
}

RPGAPI void AddObjectV(void* object, double v_x, double v_y)
{
    Object* o = static_cast<Object*>(object);
    o->AddVelocity(v_x, v_y);
}

RPGAPI void AddObjectA(void* object, double a_x, double a_y)
{
    Object* o = static_cast<Object*>(object);
    o->AddAcceleration(a_x, a_y);
}

RPGAPI void SetObjectV(void* object, double v_x, double v_y)
{
    Object* o = static_cast<Object*>(object);
    o->SetVelocity(v_x, v_y);
}

RPGAPI void GetObjectV(void* object, double* v_x, double* v_y)
{
    Object* o = static_cast<Object*>(object);
    o->GetVelocity(v_x, v_y);
}

RPGAPI void SetObjectA(void* object, double a_x, double a_y)
{
    Object* o = static_cast<Object*>(object);
    o->SetAcceleration(a_x, a_y);
}

RPGAPI void GetObjectA(void* object, double* a_x, double* a_y)
{
    Object* o = static_cast<Object*>(object);
    o->GetAcceleration(a_x, a_y);
}

RPGAPI int SetObjectImage(void* object, const char* image_path)
{
    Object* o = static_cast<Object*>(object);
    return o->AddImage(image_path);
}

RPGAPI void ChangeObjectShowImage(void* object, int image_id)
{
    Object* o = static_cast<Object*>(object);
    o->SetCurrentImage(image_id);
}

RPGAPI void AddSimulateSignalBefore(void* world, SimulateSignal signal)
{
    World* w = static_cast<World*>(world);
    w->AddSignalBefore([signal](World* w_ptr) {
        signal(static_cast<void*>(w_ptr));
    });
}

RPGAPI void AddSimulateSignalMid(void* world, SimulateSignal signal)
{
    World* w = static_cast<World*>(world);
    w->AddSignalMid([signal](World* w_ptr) {
        signal(static_cast<void*>(w_ptr));
    });
}

RPGAPI void AddSimulateSignalAfter(void* world, SimulateSignal signal)
{
    World* w = static_cast<World*>(world);
    w->AddSignalAfter([signal](World* w_ptr) {
        signal(static_cast<void*>(w_ptr));
    });
}

RPGAPI void AddKeyboardSignal(void* world, KeyboardSignal signal)
{
    World* w = static_cast<World*>(world);
    w->AddKeyboardSignal([signal](World* w_ptr, int key, bool pressed) {
        signal(static_cast<void*>(w_ptr), key, pressed);
    });
}

RPGAPI void AddMouseSignal(void* world, MouseSignal signal)
{
    World* w = static_cast<World*>(world);
    w->AddMouseSignal([signal](World* w_ptr, double x, double y, int button, bool pressed) {
        signal(static_cast<void*>(w_ptr), x, y, button, pressed);
    });
}

RPGAPI void SetBackground(void* world, const char* image_path, double x, double y, double size_x, double size_y)
{
    World* w = static_cast<World*>(world);
    w->SetBackground(image_path, x, y, size_x, size_y);
}

}
