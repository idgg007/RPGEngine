#ifndef OBJECT_H
#define OBJECT_H

#include <raylib.h>
#include <vector>
#include <string>

class Object
{
private:
    std::vector<std::string> image_paths;
    std::vector<Image> images;
    std::vector<Texture> textures;
    bool is_displayed;
    int current_image_id;
    double pos_x, pos_y;
    double size_x, size_y;
    double V_x, V_y;
    double a_x, a_y;

    struct HitLine {
        Vector2 start;
        Vector2 end;
        double hardness;
    };
    std::vector<HitLine> hitlines;

public:
    Object();
    ~Object();

    void SetSituate(bool flag);
    void SetObject(double x, double y, double width, double height);
    void SetPosition(double x, double y);
    void GetPosition(double* x, double* y);
    void SetSize(double width, double height);
    void GetSize(double* width, double* height);
    
    void Draw(const Rectangle& camera, Vector2 screen);
    
    void SetVelocity(double vx, double vy);
    void GetVelocity(double* vx, double* vy);
    void AddVelocity(double vx, double vy);
    
    void SetAcceleration(double ax, double ay);
    void GetAcceleration(double* ax, double* ay);
    void AddAcceleration(double ax, double ay);
    void ResetAcceleration();
    
    int AddImage(const char* image_path);
    void SetCurrentImage(int image_id);
    void UnloadImages();
    
    void AddHitLine(double x1, double y1, double x2, double y2, double hardness);
    void CreateRectHitbox(double hardness);
    void Hit(Object& other, bool is_elastic);
    
    void Move(double deltaTime);
    void ApplyVelocity(double deltaTime);
};

#endif
