#pragma once

struct cam
{
    float pos_x;
    float pos_y;
    float pos_z;

    float look_at_x;
    float look_at_y;
    float look_at_z;

    bool is_rotating = false;
    float last_mouse_pos_x;
    float last_mouse_pos_y;

    bool is_selecting = false;
};
struct vec3
{
    float x;
    float y;
    float z;
};
struct obj
{
    float x;
    float y;
    float z;
    float id;
    bool is_selected = false;
};
