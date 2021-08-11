#version 450 core

layout(location = 0) in vec4 i_pos; // contain position, radius, and thickness
layout(location = 1) in vec4 i_col;

layout(location = 0) out vec4 fs_pos;
layout(location = 1) out vec4 fs_col;

void main()
{
    
    fs_col = i_col;
    fs_rad = i_rad;
}
