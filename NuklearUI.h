//
// Copyright (c) 2016 Rokas Kupstys
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
#pragma once


#include <Urho3D/Core/Object.h>

#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT 1
#define NK_INCLUDE_FONT_BAKING 1
#define NK_INCLUDE_DEFAULT_FONT 1
#define NK_INCLUDE_COMMAND_USERDATA 1
#define NK_INCLUDE_STANDARD_IO 1
#define NK_INCLUDE_FIXED_TYPES 1
#define NK_INCLUDE_DEFAULT_ALLOCATOR 1
#define GL_GLEXT_PROTOTYPES 1
#include "nuklear.h"
#include "nuklear_sdl_gl3.h"


class NuklearUI
    : public Urho3D::Object
{
    URHO3D_OBJECT(NuklearUI, Urho3D::Object);
public:
    NuklearUI(Urho3D::Context* ctx);
    virtual ~NuklearUI();

    nk_context* GetNkContext() const { return _nk_ctx; }
    operator nk_context*() const { return _nk_ctx; }

protected:
    void OnEndFrame(Urho3D::StringHash, Urho3D::VariantMap&);
    void OnRawEvent(Urho3D::StringHash, Urho3D::VariantMap& args);
    void OnPostUpdate(Urho3D::StringHash, Urho3D::VariantMap&);
    void OnEndRendering(Urho3D::StringHash, Urho3D::VariantMap&);

    struct GraphicsApiState
    {
        GLint last_program;
        GLint last_texture;
        GLint last_array_buffer;
        GLint last_element_array_buffer;
        GLint last_vertex_array;
        GLint last_blend_src;
        GLint last_blend_dst;
        GLint last_blend_equation_rgb;
        GLint last_blend_equation_alpha;
        GLint last_viewport[4];
        GLboolean last_enable_blend;
        GLboolean last_enable_cull_face;
        GLboolean last_enable_depth_test;
        GLboolean last_enable_scissor_test;
    };
    void GraphicsApiStateBackup(GraphicsApiState& state);
    void GraphicsApiStateRestore(GraphicsApiState& state);

    nk_context* _nk_ctx;
};


