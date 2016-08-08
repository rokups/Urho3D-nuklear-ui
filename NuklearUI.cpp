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
#include <string.h>
#define NK_IMPLEMENTATION 1
#define NK_SDL_GL3_IMPLEMENTATION 1
#include "NuklearUI.h"
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Graphics/GraphicsEvents.h>
#define GL_GLEXT_PROTOTYPES 1
#include "nuklear/demo/sdl_opengl3/nuklear_sdl_gl3.h"

using namespace Urho3D;


struct NuklearUI::GraphicsApiState
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

NuklearUI::NuklearUI(Context* ctx)
    : Urho3D::Object(ctx)
{
    GraphicsApiState state = { };
    GraphicsApiStateBackup(state);
    _nk_ctx = nk_sdl_init(GetSubsystem<Graphics>()->GetWindow());
    GraphicsApiStateRestore(state);

    SubscribeToEvent(E_INPUTBEGIN, URHO3D_HANDLER(NuklearUI, OnInputBegin));
    SubscribeToEvent(E_SDLRAWINPUT, URHO3D_HANDLER(NuklearUI, OnRawEvent));
    SubscribeToEvent(E_INPUTEND, URHO3D_HANDLER(NuklearUI, OnInputEnd));
    SubscribeToEvent(E_ENDRENDERING, URHO3D_HANDLER(NuklearUI, OnEndRendering));

    nk_sdl_font_stash_begin(&_atlas);
}

void NuklearUI::FinalizeFonts()
{
    nk_sdl_font_stash_end();
}

NuklearUI::~NuklearUI()
{
    nk_sdl_shutdown();
}

void NuklearUI::OnInputBegin(StringHash, VariantMap&)
{
    nk_input_begin(_nk_ctx);
}

void NuklearUI::OnRawEvent(StringHash, VariantMap& args)
{
    nk_sdl_handle_event(static_cast<SDL_Event*>(args[SDLRawInput::P_SDLEVENT].Get<void*>()));
}

void NuklearUI::OnInputEnd(StringHash, VariantMap&)
{
    nk_input_end(_nk_ctx);
}

void NuklearUI::OnEndRendering(StringHash, VariantMap&)
{
    const int MAX_VERTEX_MEMORY = 512 * 1024;
    const int MAX_ELEMENT_MEMORY = 128 * 1024;
    GraphicsApiState state = { };
    GraphicsApiStateBackup(state);
    nk_sdl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY, MAX_ELEMENT_MEMORY);
    GraphicsApiStateRestore(state);
}

// Copyright (c) 2016 Yehonatan Ballas
void NuklearUI::GraphicsApiStateBackup(NuklearUI::GraphicsApiState& state)
{
    glGetIntegerv(GL_CURRENT_PROGRAM, &state.last_program);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &state.last_texture);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &state.last_array_buffer);
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &state.last_element_array_buffer);
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &state.last_vertex_array);
    glGetIntegerv(GL_BLEND_SRC, &state.last_blend_src);
    glGetIntegerv(GL_BLEND_DST, &state.last_blend_dst);
    glGetIntegerv(GL_BLEND_EQUATION_RGB, &state.last_blend_equation_rgb);
    glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &state.last_blend_equation_alpha);
    glGetIntegerv(GL_VIEWPORT, state.last_viewport);
    state.last_enable_blend = glIsEnabled(GL_BLEND);
    state.last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
    state.last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
    state.last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);
}

// Copyright (c) 2016 Yehonatan Ballas
void NuklearUI::GraphicsApiStateRestore(NuklearUI::GraphicsApiState& state)
{
    glUseProgram((GLuint)state.last_program);
    glBindTexture(GL_TEXTURE_2D, (GLuint)state.last_texture);
    glBindVertexArray((GLuint)state.last_vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, (GLuint)state.last_array_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (GLuint)state.last_element_array_buffer);
    glBlendEquationSeparate((GLenum)state.last_blend_equation_rgb, (GLenum)state.last_blend_equation_alpha);
    glBlendFunc((GLenum)state.last_blend_src, (GLenum)state.last_blend_dst);
    if (state.last_enable_blend)
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);
    if (state.last_enable_cull_face)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);
    if (state.last_enable_depth_test)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
    if (state.last_enable_scissor_test)
        glEnable(GL_SCISSOR_TEST);
    else
        glDisable(GL_SCISSOR_TEST);
    glViewport(state.last_viewport[0], state.last_viewport[1], (GLsizei)state.last_viewport[2],
               (GLsizei)state.last_viewport[3]);
}
