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

    GLint proj_location_3d;
    GLint tex_location_3d;
    GLint view_location_3d;
    GLint color_location_3d;
    GLint model_transform_location_3d;
    GLint scale_location_3d;
    GLint transform_location_3d;
    GLuint program_3d;

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

    GLuint load_shader_pair(std::string vert, std::string frag)
    {
        std::fstream vert_src_f(vert);
        if (!vert_src_f)
            SDL_assert("Couldn't open vertex shader src" == 0);
        std::string vert_src_str = std::string(std::istreambuf_iterator<char>(vert_src_f), std::istreambuf_iterator<char>());
        GLuint vert_shader = compile_shader(GL_VERTEX_SHADER, vert_src_str);

        std::fstream frag_src_f(frag);
        if (!frag_src_f)
            SDL_assert("Couldn't open fragment shader src" == 0);
        std::string frag_src_str = std::string(std::istreambuf_iterator<char>(frag_src_f), std::istreambuf_iterator<char>());
        GLuint frag_shader = compile_shader(GL_FRAGMENT_SHADER, frag_src_str);

        GLuint program = OGL_Loader::get_instance()->glCreateProgram();
        OGL_Loader::get_instance()->glAttachShader(program, vert_shader);
        OGL_Loader::get_instance()->glAttachShader(program, frag_shader);
        OGL_Loader::get_instance()->glLinkProgram(program);
        GLint is_ok;
        OGL_Loader::get_instance()->glGetProgramiv(program, GL_LINK_STATUS, &is_ok);
        SDL_assert(is_ok && "could not link program");
        OGL_Loader::get_instance()->glDeleteShader(vert_shader);
        OGL_Loader::get_instance()->glDeleteShader(frag_shader);
        return program;
    }

    OGL_Shaders(std::string vert_shader_filename_2d, std::string frag_shader_filename_2d, std::string vert_shader_filename_3d, std::string frag_shader_filename_3d)
    {
        OGL_Loader * gl = OGL_Loader::get_instance();
        program_2d = load_shader_pair(vert_shader_filename_2d, frag_shader_filename_2d);
        proj_location_2d = gl->glGetUniformLocation(program_2d, "uProj");
        color_location_2d = gl->glGetUniformLocation(program_2d, "uColor");
        tex_location_2d = gl->glGetUniformLocation(program_2d, "uTex");
        program_3d = load_shader_pair(vert_shader_filename_3d, frag_shader_filename_3d);
        proj_location_3d = gl->glGetUniformLocation(program_3d, "uProjection");
        tex_location_3d = gl->glGetUniformLocation(program_3d, "uTexture");
        view_location_3d = gl->glGetUniformLocation(program_3d, "uView");
        color_location_3d = gl->glGetUniformLocation(program_3d, "uColor");
        model_transform_location_3d = gl->glGetUniformLocation(program_3d, "uModelTransform");
        scale_location_3d = gl->glGetUniformLocation(program_3d, "uScale");
        transform_location_3d = gl->glGetUniformLocation(program_3d, "uTransform");
    }

  public:
    GLuint get_program_2d()
    {
        return program_2d;
    }
    GLuint get_program_3d()
    {
        return program_3d;
    }

#define GENERATE_GETTER(x)                                                                                                                                                                             \
    GLint get_##x()                                                                                                                                                                                    \
    {                                                                                                                                                                                                  \
        return x;                                                                                                                                                                                      \
    }

    GENERATE_GETTER(proj_location_2d)
    GENERATE_GETTER(tex_location_2d)
    GENERATE_GETTER(color_location_2d)

    GENERATE_GETTER(proj_location_3d)
    GENERATE_GETTER(tex_location_3d)
    GENERATE_GETTER(view_location_3d)
    GENERATE_GETTER(color_location_3d)
    GENERATE_GETTER(model_transform_location_3d)
    GENERATE_GETTER(scale_location_3d)
    GENERATE_GETTER(transform_location_3d)

    static OGL_Shaders * get_instance()
    {
        if (!instance)
            instance = new OGL_Shaders("shaders/2d.vert", "shaders/2d.frag", "shaders/3d.vert", "shaders/3d.frag");
        return instance;
    }
};
