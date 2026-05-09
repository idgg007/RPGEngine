#include "Egine_dll/cheatsheet.h"
#include <iostream>
#include <stdbool.h>
#include <cstddef>
#include <thread>
#include <chrono>

void *g_world = NULL;
void *g_player = NULL;
bool g_can_jump = false;
bool g_left_pressed = false;
bool g_right_pressed = false;
const double GRAVITY = 800.0;
const double JUMP_FORCE = -500.0;
const double MOVE_SPEED = 200.0;

void HandleKeyboard(void *world, int key, bool pressed)
{
    double vx, vy;
    GetObjectV(g_player, &vx, &vy);

    if (key == KEY_SPACE || key == KEY_W || key == 87 || key == 119)
    {
        if (pressed && g_can_jump)
        {
            SetObjectV(g_player, vx, JUMP_FORCE);
            g_can_jump = false;
        }
    }
    else if (key == KEY_LEFT || key == KEY_A || key == 65 || key == 97)
    {
        g_left_pressed = pressed;
    }
    else if (key == KEY_RIGHT || key == KEY_D || key == 68 || key == 100)
    {
        g_right_pressed = pressed;
    }
}

void SimulateMid(void *world)
{
    double ax, ay;
    GetObjectA(g_player, &ax, &ay);

    g_can_jump = false;
    if (ay < GRAVITY - 50)
    {
        g_can_jump = true;
    }

    double vx, vy;
    GetObjectV(g_player, &vx, &vy);

    if (g_left_pressed && g_right_pressed)
    {
        vx = 0;
    }
    else if (g_left_pressed)
    {
        vx = -MOVE_SPEED;
    }
    else if (g_right_pressed)
    {
        vx = MOVE_SPEED;
    }

    SetObjectV(g_player, vx, vy);
}

int main()
{
    Init(800, 600, "Platformer Game");

    while (!IsRunning())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    g_world = CreateWorld();

    void *platform1 = CreateObject();
    SetObjectPos(platform1, 400, 526);
    SetObjectSize(platform1, 256, 32);
    CreateRectHitbox(platform1, 1000.0);
    SetObjectImage(platform1, "platform.png");
    AddEnvironment(g_world, platform1);

    void *platform2 = CreateObject();
    SetObjectPos(platform2, 150, 400);
    SetObjectSize(platform2, 128, 16);
    CreateRectHitbox(platform2, 1000.0);
    SetObjectImage(platform2, "platform.png");
    AddEnvironment(g_world, platform2);

    void *platform3 = CreateObject();
    SetObjectPos(platform3, 400, 300);
    SetObjectSize(platform3, 128, 16);
    CreateRectHitbox(platform3, 1000.0);
    SetObjectImage(platform3, "platform.png");
    AddEnvironment(g_world, platform3);

    void *platform4 = CreateObject();
    SetObjectPos(platform4, 650, 200);
    SetObjectSize(platform4, 128, 16);
    CreateRectHitbox(platform4, 1000.0);
    SetObjectImage(platform4, "platform.png");
    AddEnvironment(g_world, platform4);

    void *platform5 = CreateObject();
    SetObjectPos(platform5, 100, 200);
    SetObjectSize(platform5, 128, 16);
    CreateRectHitbox(platform5, 1000.0);
    SetObjectImage(platform5, "platform.png");
    AddEnvironment(g_world, platform5);

    g_player = CreateObject();
    SetObjectPos(g_player, 400, 450);
    SetObjectSize(g_player, 32, 32);
    CreateRectHitbox(g_player, 1.0);
    SetObjectImage(g_player, "player.png");
    AddCreature(g_world, g_player);

    SetBackground(g_world, "background.png", 400, 300, 800, 600);

    SetGravity(g_world, 0, GRAVITY);
    SetCameraPos(g_world, 400, 300, 800, 600);

    AddKeyboardSignal(g_world, HandleKeyboard);
    AddSimulateSignalMid(g_world, SimulateMid);

    ShowWorld(g_world);

    WaitForClose();

    return 0;
}