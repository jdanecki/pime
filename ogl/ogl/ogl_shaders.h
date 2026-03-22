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
#include <fstream>
#include <iterator>
#include <string>
#include <SDL3/SDL.h>
#include <GL/gl.h>
#include "ogl_loader.h"

class OGL_Shaders
{
    static OGL_Shaders * instance;
    GLint proj_location_2d;
    GLint tex_location_2d;
    GLint color_location_2d;
    GLuint program_2d;
    GLuint compile_shader(GLenum type, std::string src)
    {
        GLuint shader = OGL_Loader::get_instance()->glCreateShader(type);
        const GLchar * string = src.c_str();
        GLint len = src.length();
        OGL_Loader::get_instance()->glShaderSource(shader, 1, &string, &len);
        OGL_Loader::get_instance()->glCompileShader(shader);
        GLint is_ok;
        OGL_Loader::get_instance()->glGetShaderiv(shader, GL_COMPILE_STATUS, &is_ok);
        SDL_assert(is_ok && "could not compile shader");
        return shader;
    }

    OGL_Shaders(std::string vert_shader_filename_2d, std::string frag_shader_filename_2d)
    {
        std::fstream vert_src_f_2d(vert_shader_filename_2d);
        if (!vert_src_f_2d)
            SDL_assert("Couldn't open vertex shader src" == 0);
        std::string vert_src_str = std::string(std::istreambuf_iterator<char>(vert_src_f_2d), std::istreambuf_iterator<char>());
        GLuint vert_shader_2d = compile_shader(GL_VERTEX_SHADER, vert_src_str);

        std::fstream frag_src_f(frag_shader_filename_2d);
        if (!frag_src_f)
            SDL_assert("Couldn't open fragment shader src" == 0);
        std::string frag_src_str = std::string(std::istreambuf_iterator<char>(frag_src_f), std::istreambuf_iterator<char>());
        GLuint frag_shader_2d = compile_shader(GL_FRAGMENT_SHADER, frag_src_str);

        program_2d = OGL_Loader::get_instance()->glCreateProgram();
        OGL_Loader::get_instance()->glAttachShader(program_2d, vert_shader_2d);
        OGL_Loader::get_instance()->glAttachShader(program_2d, frag_shader_2d);
        OGL_Loader::get_instance()->glLinkProgram(program_2d);
        GLint is_ok;
        OGL_Loader::get_instance()->glGetProgramiv(program_2d, GL_LINK_STATUS, &is_ok);
        SDL_assert(is_ok && "could not link program");
        OGL_Loader::get_instance()->glDeleteShader(vert_shader_2d);
        OGL_Loader::get_instance()->glDeleteShader(frag_shader_2d);
        proj_location_2d = OGL_Loader::get_instance()->glGetUniformLocation(program_2d, "uProj");
        color_location_2d = OGL_Loader::get_instance()->glGetUniformLocation(program_2d, "uColor");
        tex_location_2d = OGL_Loader::get_instance()->glGetUniformLocation(program_2d, "uTex");
    }

  public:
    GLuint get_program_2d()
    {
        return program_2d;
    }

    GLint get_proj_location_2d()
    {
        return proj_location_2d;
    }

    GLint get_color_location_2d()
    {
        return color_location_2d;
    }

    GLint get_tex_location_2d()
    {
        return tex_location_2d;
    }

    static OGL_Shaders * get_instance()
    {
        if (!instance)
            instance = new OGL_Shaders("shaders/2d.vert", "shaders/2d.frag");
        return instance;
    }
};
