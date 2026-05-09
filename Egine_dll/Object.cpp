#include "Object.h"
#include <cmath>
#include <algorithm>

Object::Object()
    : is_displayed(false)
    , current_image_id(-1)
    , pos_x(0), pos_y(0)
    , size_x(0), size_y(0)
    , V_x(0), V_y(0)
    , a_x(0), a_y(0)
{
}

Object::~Object()
{
    UnloadImages();
}

void Object::SetSituate(bool flag)
{
    if (flag && !is_displayed) {
        for (const std::string& path : image_paths) {
            Image img = LoadImage(path.c_str());
            if (img.data != nullptr) {
                images.push_back(img);
                textures.push_back(LoadTextureFromImage(img));
            }
        }
    } else if (!flag && is_displayed) {
        for (auto& tex : textures) {
            UnloadTexture(tex);
        }
        textures.clear();
        for (auto& img : images) {
            UnloadImage(img);
        }
        images.clear();
    }
    is_displayed = flag;
}

void Object::SetObject(double x, double y, double width, double height)
{
    pos_x = x;
    pos_y = y;
    size_x = width;
    size_y = height;
}

void Object::SetPosition(double x, double y)
{
    pos_x = x;
    pos_y = y;
}

void Object::GetPosition(double* x, double* y)
{
    if (x) *x = pos_x;
    if (y) *y = pos_y;
}

void Object::SetSize(double width, double height)
{
    size_x = width;
    size_y = height;
}

void Object::GetSize(double* width, double* height)
{
    if (width) *width = size_x;
    if (height) *height = size_y;
}

void Object::Draw(const Rectangle& camera, Vector2 screen)
{
    double world_left = camera.x - camera.width / 2.0;
    double world_top = camera.y - camera.height / 2.0;

    double scale_x = screen.x / camera.width;
    double scale_y = screen.y / camera.height;

    double screen_left_x = (pos_x - size_x / 2.0 - world_left) * scale_x;
    double screen_top_y = (pos_y - size_y / 2.0 - world_top) * scale_y;
    double screen_w = size_x * scale_x;
    double screen_h = size_y * scale_y;

    if (is_displayed && current_image_id >= 0 && current_image_id < (int)textures.size()) {
        Texture2D tex = textures[current_image_id];
        float scale = (float)(screen_h / tex.height);
        float tex_w = tex.width * scale;
        float tex_h = tex.height * scale;
        float draw_x = (float)screen_left_x + (screen_w - tex_w) / 2.0f;
        float draw_y = (float)screen_top_y + (screen_h - tex_h);
        DrawTextureEx(tex, {draw_x, draw_y}, 0.0f, scale, WHITE);
    } else {
        DrawRectangle((int)screen_left_x, (int)screen_top_y, (int)screen_w, (int)screen_h, BLUE);
        DrawRectangleLines((int)screen_left_x, (int)screen_top_y, (int)screen_w, (int)screen_h, RED);
    }
}

void Object::SetVelocity(double vx, double vy)
{
    V_x = vx;
    V_y = vy;
}

void Object::GetVelocity(double* vx, double* vy)
{
    if (vx) *vx = V_x;
    if (vy) *vy = V_y;
}

void Object::AddVelocity(double vx, double vy)
{
    V_x += vx;
    V_y += vy;
}

void Object::SetAcceleration(double ax, double ay)
{
    a_x = ax;
    a_y = ay;
}

void Object::GetAcceleration(double* ax, double* ay)
{
    if (ax) *ax = a_x;
    if (ay) *ay = a_y;
}

void Object::AddAcceleration(double ax, double ay)
{
    a_x += ax;
    a_y += ay;
}

void Object::ResetAcceleration()
{
    a_x = 0;
    a_y = 0;
}

int Object::AddImage(const char* image_path)
{
    image_paths.push_back(image_path);
    if (current_image_id == -1) {
        current_image_id = 0;
    }
    return (int)image_paths.size() - 1;
}

void Object::SetCurrentImage(int image_id)
{
    if (image_id == -1) {
        current_image_id = -1;
    } else if (image_id >= 0 && image_id < (int)image_paths.size()) {
        current_image_id = image_id;
    }
}

void Object::UnloadImages()
{
    if (is_displayed) {
        for (auto& tex : textures) {
            UnloadTexture(tex);
        }
        textures.clear();
        for (auto& img : images) {
            UnloadImage(img);
        }
        images.clear();
    }
    image_paths.clear();
    current_image_id = -1;
}

static float Direction(Vector2 p1, Vector2 p2, Vector2 p3)
{
    return (p3.x - p1.x) * (p2.y - p1.y) - (p2.x - p1.x) * (p3.y - p1.y);
}

void Object::AddHitLine(double x1, double y1, double x2, double y2, double hardness)
{
    HitLine line;
    line.start = {(float)x1, (float)y1};
    line.end = {(float)x2, (float)y2};
    line.hardness = hardness;
    hitlines.push_back(line);
}

void Object::CreateRectHitbox(double hardness)
{
    double left = -size_x / 2.0;
    double right = size_x / 2.0;
    double top = -size_y / 2.0;
    double bottom = size_y / 2.0;
    AddHitLine(left, top, right, top, hardness);
    AddHitLine(right, top, right, bottom, hardness);
    AddHitLine(right, bottom, left, bottom, hardness);
    AddHitLine(left, bottom, left, top, hardness);
}

static double Distance(Vector2 a, Vector2 b)
{
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

static double Distance(Vector2 vec)
{
    return sqrt(vec.x * vec.x + vec.y * vec.y);
}

static bool LineIntersect(Vector2 p1, Vector2 p2, Vector2 p3, Vector2 p4, Vector2* intersect)
{
    float d1 = Direction(p3, p4, p1);
    float d2 = Direction(p3, p4, p2);
    float d3 = Direction(p1, p2, p3);
    float d4 = Direction(p1, p2, p4);
    
    if (((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0)) &&
        ((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0))) {
        float denom = (p2.x - p1.x) * (p4.y - p3.y) - (p2.y - p1.y) * (p4.x - p3.x);
        if (denom == 0) return false;
        float t = ((p1.x - p3.x) * (p4.y - p3.y) - (p1.y - p3.y) * (p4.x - p3.x));
        t /= denom;
        intersect->x = p1.x + t * (p2.x - p1.x);
        intersect->y = p1.y + t * (p2.y - p1.y);
        return true;
    }
    return false;
}

static Vector2 GetLineNormal(Vector2 p1, Vector2 p2)
{
    Vector2 dir = { p2.x - p1.x, p2.y - p1.y };
    Vector2 normal = { -dir.y, dir.x };
    double len = Distance(normal);
    if (len > 0) {
        normal.x /= (float)len;
        normal.y /= (float)len;
    }
    return normal;
}

static double PointToLineDistance(Vector2 p, Vector2 line_p1, Vector2 line_p2)
{
    double A = line_p2.y - line_p1.y;
    double B = line_p1.x - line_p2.x;
    double C = line_p2.x * line_p1.y - line_p1.x * line_p2.y;
    return fabs(A * p.x + B * p.y + C) / sqrt(A * A + B * B);
}

void Object::Hit(Object& other, bool is_elastic)
{
    double left = pos_x - size_x / 2.0;
    double right = pos_x + size_x / 2.0;
    double top = pos_y - size_y / 2.0;
    double bottom = pos_y + size_y / 2.0;

    double other_left = other.pos_x - other.size_x / 2.0;
    double other_right = other.pos_x + other.size_x / 2.0;
    double other_top = other.pos_y - other.size_y / 2.0;
    double other_bottom = other.pos_y + other.size_y / 2.0;

    if (right <= other_left || left >= other_right || bottom <= other_top || top >= other_bottom) {
        return;
    }

    double overlap_left = right - other_left;
    double overlap_right = other_right - left;
    double overlap_top = bottom - other_top;
    double overlap_bottom = other_bottom - top;

    double min_overlap = overlap_left;
    double normal_x = 1.0;
    double normal_y = 0.0;

    if (overlap_right < min_overlap) {
        min_overlap = overlap_right;
        normal_x = -1.0;
        normal_y = 0.0;
    }
    if (overlap_top < min_overlap) {
        min_overlap = overlap_top;
        normal_x = 0.0;
        normal_y = 1.0;
    }
    if (overlap_bottom < min_overlap) {
        min_overlap = overlap_bottom;
        normal_x = 0.0;
        normal_y = -1.0;
    }

    double force = min_overlap * 1000.0;

    if (is_elastic) {
        other.AddAcceleration(normal_x * force, normal_y * force);
    } else {
        double vel_dot = other.V_x * normal_x + other.V_y * normal_y;
        if (vel_dot < 0) {
            other.V_x -= vel_dot * normal_x;
            other.V_y -= vel_dot * normal_y;
            if (fabs(normal_y) > 0.5) {
                other.V_x *= 0.95;
            }
        }
        other.AddAcceleration(normal_x * force, normal_y * force);
    }
}

void Object::Move(double deltaTime)
{
    V_x += a_x * deltaTime / 2;
    V_y += a_y * deltaTime / 2;
    pos_x += V_x * deltaTime;
    pos_y += V_y * deltaTime;
    V_x += a_x * deltaTime / 2;
    V_y += a_y * deltaTime / 2;
    a_x = 0;
    a_y = 0;
}

void Object::ApplyVelocity(double deltaTime)
{
    pos_x += V_x * deltaTime;
    pos_y += V_y * deltaTime;
}
