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
};
