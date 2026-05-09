#ifndef CHEATSHEET_H
#define CHEATSHEET_H

#ifdef _WIN32
    #ifdef MYLIBRARY_EXPORTS
        #define RPGAPI __declspec(dllexport)
    #else
        #define RPGAPI __declspec(dllimport)
    #endif
#else
    #define RPGAPI __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*SimulateSignal)(void* world);
typedef void (*KeyboardSignal)(void* world, int key, bool pressed);
typedef void (*MouseSignal)(void* world, double x, double y, int button, bool pressed);

RPGAPI void Init(float width, float height, const char* title);
RPGAPI void InitWithFps(float width, float height, const char* title, float fps);
RPGAPI void Shutdown();
RPGAPI bool IsRunning();
RPGAPI void ProcessEvents();
RPGAPI void RenderFrame();
RPGAPI void SimulateFrame();

RPGAPI void* CreateWorld();
RPGAPI void* CreateObject();

RPGAPI int AddEnvironment(void* world, void* object);
RPGAPI int AddCreature(void* world, void* object);
RPGAPI bool ChangeObjectId(void* world, void* object, int new_id);

RPGAPI void ShowWorld(void* world);
RPGAPI void WaitForClose();

RPGAPI void SetCameraPos(void* world, double x, double y, double size_x, double size_y);
RPGAPI void MoveCamera(void* world, double x, double y);
RPGAPI void ChangeCameraSize(void* world, double size_x, double size_y);

RPGAPI void SetGravity(void* world, double g_x, double g_y);
RPGAPI void SetWorldSpeed(void* world, double speed);
RPGAPI void SetSimulateType(void* world, bool type);

RPGAPI void SetObjectPos(void* object, double x, double y);
RPGAPI void GetObjectPos(void* object, double* x, double* y);
RPGAPI void SetObjectSize(void* object, double size_x, double size_y);
RPGAPI void GetObjectSize(void* object, double* size_x, double* size_y);
RPGAPI void AddHitBox(void* object, double x1, double y1, double x2, double y2, double hardness);
RPGAPI void CreateRectHitbox(void* object, double hardness);
RPGAPI void AddObjectV(void* object, double v_x, double v_y);
RPGAPI void AddObjectA(void* object, double a_x, double a_y);
RPGAPI void SetObjectV(void* object, double v_x, double v_y);
RPGAPI void GetObjectV(void* object, double* v_x, double* v_y);
RPGAPI void SetObjectA(void* object, double a_x, double a_y);
RPGAPI void GetObjectA(void* object, double* a_x, double* a_y);
RPGAPI int SetObjectImage(void* object, const char* image_path);
RPGAPI void ChangeObjectShowImage(void* object, int image_id);

RPGAPI void AddSimulateSignalBefore(void* world, SimulateSignal signal);
RPGAPI void AddSimulateSignalMid(void* world, SimulateSignal signal);
RPGAPI void AddSimulateSignalAfter(void* world, SimulateSignal signal);
RPGAPI void AddKeyboardSignal(void* world, KeyboardSignal signal);
RPGAPI void AddMouseSignal(void* world, MouseSignal signal);

RPGAPI void SetBackground(void* world, const char* image_path, double x, double y, double size_x, double size_y);

#define KEY_NULL 0
#define KEY_SPACE 32
#define KEY_ESCAPE 256
#define KEY_ENTER 257
#define KEY_TAB 258
#define KEY_BACKSPACE 259
#define KEY_INSERT 260
#define KEY_DELETE 261
#define KEY_RIGHT 262
#define KEY_LEFT 263
#define KEY_DOWN 264
#define KEY_UP 265
#define KEY_PAGE_UP 266
#define KEY_PAGE_DOWN 267
#define KEY_HOME 268
#define KEY_END 269
#define KEY_CAPS_LOCK 280
#define KEY_SCROLL_LOCK 281
#define KEY_NUM_LOCK 282
#define KEY_PRINT_SCREEN 283
#define KEY_PAUSE 284
#define KEY_F1 290
#define KEY_F2 291
#define KEY_F3 292
#define KEY_F4 293
#define KEY_F5 294
#define KEY_F6 295
#define KEY_F7 296
#define KEY_F8 297
#define KEY_F9 298
#define KEY_F10 299
#define KEY_F11 300
#define KEY_F12 301
#define KEY_LEFT_SHIFT 340
#define KEY_LEFT_CONTROL 341
#define KEY_LEFT_ALT 342
#define KEY_RIGHT_SHIFT 344
#define KEY_RIGHT_CONTROL 345
#define KEY_RIGHT_ALT 346
#define KEY_A 65
#define KEY_B 66
#define KEY_C 67
#define KEY_D 68
#define KEY_E 69
#define KEY_F 70
#define KEY_G 71
#define KEY_H 72
#define KEY_I 73
#define KEY_J 74
#define KEY_K 75
#define KEY_L 76
#define KEY_M 77
#define KEY_N 78
#define KEY_O 79
#define KEY_P 80
#define KEY_Q 81
#define KEY_R 82
#define KEY_S 83
#define KEY_T 84
#define KEY_U 85
#define KEY_V 86
#define KEY_W 87
#define KEY_X 88
#define KEY_Y 89
#define KEY_Z 90
#define KEY_0 48
#define KEY_1 49
#define KEY_2 50
#define KEY_3 51
#define KEY_4 52
#define KEY_5 53
#define KEY_6 54
#define KEY_7 55
#define KEY_8 56
#define KEY_9 57
#define MOUSE_BUTTON_LEFT 0
#define MOUSE_BUTTON_RIGHT 1
#define MOUSE_BUTTON_MIDDLE 2

#ifdef __cplusplus
}
#endif

#endif
