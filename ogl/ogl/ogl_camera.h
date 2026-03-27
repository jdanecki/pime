/*
    pime-ogl - an OpenGL implementation of a pime client
    Copyright (C) 2026 Piotr Danecki <i3riced@mailfence.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once
#include "../core/world_params.h"
#include <SDL3/SDL.h>
#include <GL/gl.h>
#include <cmath>
#include "ogl_shaders.h"
#include "ogl_loader.h"

class OGL_Camera
{
    float fov;
    float znear, zfar;

    float * load_perspective(float fovy_deg, float aspect, float znear, float zfar)
    {
        float fovy = fovy_deg * (float)M_PI / 180.0f;
        float f = 1.0f / tanf(fovy * 0.5f);
        float nf = 1.0f / (znear - zfar);

        // Column-major order for glMultMatrixf (OpenGL expects column-major)
        float * m = new float[16];
        m[0] = f / aspect;
        m[1] = 0;
        m[2] = 0;
        m[3] = 0;
        m[4] = 0;
        m[5] = f;
        m[6] = 0;
        m[7] = 0;
        m[8] = 0;
        m[9] = 0;
        m[10] = (zfar + znear) * nf;
        m[11] = -1;
        m[12] = 0;
        m[13] = 0;
        m[14] = (2.0f * zfar * znear) * nf;
        m[15] = 0;
        return m;
        // FIXME: do not leak memory
    }

  public:
    void get_forward_vector(float * x, float * y, float * z)
    {
        float yaw_rad = -yaw * M_PI / 180.0f;
        float pitch_rad = -pitch * M_PI / 180.0f;
        *x = -cos(pitch_rad) * sin(yaw_rad);
        *y = -sin(pitch_rad);
        *z = -cos(pitch_rad) * cos(yaw_rad);
    }

    OGL_Camera() : x(128 * CHUNK_SIZE + 8.5), y(2), z(128 * CHUNK_SIZE + 8.5), pitch(0), yaw(0), fov(90), znear(0.1), zfar(1000)
    {
    }

    float x, y, z;
    float vy;
    float pitch, yaw;

    void rotate_by(float yaw, float pitch)
    {
        this->yaw += yaw;
        this->pitch += pitch;

        if (this->pitch > 90)
            this->pitch = 90;
        if (this->pitch < -90)
            this->pitch = -90;

        if (this->yaw < 0)
            this->yaw += 360;
        if (this->yaw >= 360)
            this->yaw -= 360;
    }

    void begin_camera(SDL_Window * window)
    {
        int w, h;
        OGL_Loader * gl = OGL_Loader::get_instance();
        OGL_Shaders * sh = OGL_Shaders::get_instance();
        gl->glUseProgram(sh->get_program_3d());

        SDL_GetWindowSize(window, &w, &h);
        glViewport(0, 0, w, (h > 0 ? h : 1));

        gl->glUniform3f(sh->get_transform_location_3d(), -x, -y, -z);

        gl->glUniformMatrix4fv(sh->get_proj_location_3d(), 1, GL_FALSE, load_perspective(90, (float)w / (float)(h > 0 ? h : 1), 0.1f, 2000.0f));

        float cos_yaw = cosf(-yaw * M_PI / 180);
        float sin_yaw = sinf(-yaw * M_PI / 180);
        float cos_pitch = cosf(pitch * M_PI / 180);
        float sin_pitch = sinf(pitch * M_PI / 180);

        float view[16] = {cos_yaw, sin_yaw * sin_pitch, sin_yaw * cos_pitch, 0, 0, cos_pitch, -sin_pitch, 0, -sin_yaw, cos_yaw * sin_pitch, cos_yaw * cos_pitch, 0, 0, 0, 0, 1};
        gl->glUniformMatrix4fv(sh->get_view_location_3d(), 1, GL_FALSE, view);
    }

    const char * get_direction_string()
    {
        int dir = int(round((yaw) / 90.0)) % 4;
        switch (dir)
        {
            case 0:
                return "North (-Z)";
            case 1:
                return "East (+X)";
            case 2:
                return "South (+Z)";
            case 3:
                return "West (-X)";
        }
        return "Unknown?";
    }
};
