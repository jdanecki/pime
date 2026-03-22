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
#include <SDL3/SDL.h>
#include <GL/gl.h>

#define GL_LOAD_FUNCTION(type, function_name)                                                                                                                                                          \
    function_name = (type)SDL_GL_GetProcAddress(#function_name);                                                                                                                                       \
    SDL_assert(function_name != nullptr);

class OGL_Loader
{
    static OGL_Loader * instance;

    OGL_Loader()
    {

        GL_LOAD_FUNCTION(PFNGLCREATESHADERPROC, glCreateShader)
        GL_LOAD_FUNCTION(PFNGLSHADERSOURCEPROC, glShaderSource)
        GL_LOAD_FUNCTION(PFNGLCOMPILESHADERPROC, glCompileShader)
        GL_LOAD_FUNCTION(PFNGLGETSHADERIVPROC, glGetShaderiv)
        GL_LOAD_FUNCTION(PFNGLCREATEPROGRAMPROC, glCreateProgram)
        GL_LOAD_FUNCTION(PFNGLATTACHSHADERPROC, glAttachShader)
        GL_LOAD_FUNCTION(PFNGLLINKPROGRAMPROC, glLinkProgram)
        GL_LOAD_FUNCTION(PFNGLGETPROGRAMIVPROC, glGetProgramiv)
        GL_LOAD_FUNCTION(PFNGLDELETESHADERPROC, glDeleteShader)
        GL_LOAD_FUNCTION(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays)
        GL_LOAD_FUNCTION(PFNGLGENBUFFERSPROC, glGenBuffers)
        GL_LOAD_FUNCTION(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray)
        GL_LOAD_FUNCTION(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray)
        GL_LOAD_FUNCTION(PFNGLBINDBUFFERPROC, glBindBuffer)
        GL_LOAD_FUNCTION(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer)
        GL_LOAD_FUNCTION(PFNGLVERTEXATTRIBIPOINTERPROC, glVertexAttribIPointer)
        GL_LOAD_FUNCTION(PFNGLBUFFERDATAPROC, glBufferData)
        GL_LOAD_FUNCTION(PFNGLUSEPROGRAMPROC, glUseProgram)
        GL_LOAD_FUNCTION(PFNGLUNIFORMMATRIX4FVPROC, glUniformMatrix4fv)
        GL_LOAD_FUNCTION(PFNGLUNIFORM1IPROC, glUniform1i)
        GL_LOAD_FUNCTION(PFNGLUNIFORM4FPROC, glUniform4f)
        GL_LOAD_FUNCTION(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation)
    }

  public:
    PFNGLCREATESHADERPROC glCreateShader;
    PFNGLSHADERSOURCEPROC glShaderSource;
    PFNGLCOMPILESHADERPROC glCompileShader;
    PFNGLGETSHADERIVPROC glGetShaderiv;
    PFNGLCREATEPROGRAMPROC glCreateProgram;
    PFNGLATTACHSHADERPROC glAttachShader;
    PFNGLLINKPROGRAMPROC glLinkProgram;
    PFNGLGETPROGRAMIVPROC glGetProgramiv;
    PFNGLDELETESHADERPROC glDeleteShader;
    PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
    PFNGLGENBUFFERSPROC glGenBuffers;
    PFNGLUSEPROGRAMPROC glUseProgram;
    PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
    PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
    PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
    PFNGLBINDBUFFERPROC glBindBuffer;
    PFNGLVERTEXATTRIBIPOINTERPROC glVertexAttribIPointer;
    PFNGLBUFFERDATAPROC glBufferData;
    PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
    PFNGLUNIFORM1IPROC glUniform1i;
    PFNGLUNIFORM4FPROC glUniform4f;
    PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;

    static OGL_Loader * get_instance()
    {
        if (!instance)
            instance = new OGL_Loader();
        return instance;
    }
};
