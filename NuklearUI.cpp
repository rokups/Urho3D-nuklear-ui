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
#define NK_IMPLEMENTATION 1
#include <string.h>
#include <SDL/SDL.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Graphics/GraphicsEvents.h>
#include "NuklearUI.h"
#undef NK_IMPLEMENTATION

using namespace Urho3D;

struct nk_sdl_vertex
{
    float position[2];
    float uv[2];
    nk_byte col[4];
};

void NuklearUI::ClipboardCopy(nk_handle usr, const char* text, int len)
{
    String str(text, (unsigned int)len);
    SDL_SetClipboardText(str.CString());
}

void NuklearUI::ClipboardPaste(nk_handle usr, struct nk_text_edit *edit)
{
    const char *text = SDL_GetClipboardText();
    if (text) nk_textedit_paste(edit, text, nk_strlen(text));
    (void)usr;
}

NuklearUI::NuklearUI(Context* ctx)
    : Urho3D::Object(ctx)
    , _graphics(GetSubsystem<Graphics>())
{
    nk_init_default(&_nk_ctx, 0);
    _nk_ctx.clip.copy = &ClipboardCopy;
    _nk_ctx.clip.paste = &ClipboardPaste;
    _nk_ctx.clip.userdata = nk_handle_ptr(0);

    nk_buffer_init_default(&_commands);
    _index_buffer = new Urho3D::IndexBuffer(_graphics->GetContext());
    _vertex_buffer = new Urho3D::VertexBuffer(_graphics->GetContext());
    Urho3D::Texture2D* nullTex = new Urho3D::Texture2D(_graphics->GetContext());
    nullTex->SetNumLevels(1);
    unsigned whiteOpaque = 0xffffffff;
    nullTex->SetSize(1, 1, Urho3D::Graphics::GetRGBAFormat());
    nullTex->SetData(0, 0, 0, 1, 1, &whiteOpaque);
    _null_texture.texture.ptr = nullTex;

    SubscribeToEvent(E_INPUTBEGIN, URHO3D_HANDLER(NuklearUI, OnInputBegin));
    SubscribeToEvent(E_SDLRAWINPUT, URHO3D_HANDLER(NuklearUI, OnRawEvent));
    SubscribeToEvent(E_INPUTEND, URHO3D_HANDLER(NuklearUI, OnInputEnd));
    SubscribeToEvent(E_ENDRENDERING, URHO3D_HANDLER(NuklearUI, OnEndRendering));

    nk_font_atlas_init_default(&_atlas);
    nk_font_atlas_begin(&_atlas);
}

void NuklearUI::FinalizeFonts()
{
    const void* image;
    int w, h;
    image = nk_font_atlas_bake(&_atlas, &w, &h, NK_FONT_ATLAS_RGBA32);

    Urho3D::SharedPtr<Urho3D::Texture2D> fontTex(new Urho3D::Texture2D(_graphics->GetContext()));
    fontTex->SetNumLevels(1);
    fontTex->SetSize(w, h, Urho3D::Graphics::GetRGBAFormat());
    fontTex->SetData(0, 0, 0, w, h, image);

    // Remember the created texture for cleanup
    _font_textures.Push(fontTex);

    nk_font_atlas_end(&_atlas, nk_handle_ptr(fontTex), &_null_texture);
    if (_atlas.default_font)
        nk_style_set_font(&_nk_ctx, &_atlas.default_font->handle);
}

NuklearUI::~NuklearUI()
{
    nk_font_atlas_clear(&_atlas);
    nk_free(&_nk_ctx);
}

void NuklearUI::OnInputBegin(StringHash, VariantMap&)
{
    nk_input_begin(&_nk_ctx);
}

void NuklearUI::OnRawEvent(StringHash, VariantMap& args)
{
    auto evt = static_cast<SDL_Event*>(args[SDLRawInput::P_SDLEVENT].Get<void*>());
    auto ctx = &_nk_ctx;
    //keybindings
    if (evt->type == SDL_KEYUP || evt->type == SDL_KEYDOWN)
    {
        /* key events */
        int down = evt->type == SDL_KEYDOWN;
        const Uint8* state = SDL_GetKeyboardState(0);
        SDL_Keycode sym = evt->key.keysym.sym;
        if (sym == SDLK_RSHIFT || sym == SDLK_LSHIFT)
            nk_input_key(ctx, NK_KEY_SHIFT, down);
        else if (sym == SDLK_DELETE)
            nk_input_key(ctx, NK_KEY_DEL, down);
        else if (sym == SDLK_RETURN)
            nk_input_key(ctx, NK_KEY_ENTER, down);
        else if (sym == SDLK_TAB)
            nk_input_key(ctx, NK_KEY_TAB, down);
        else if (sym == SDLK_BACKSPACE)
            nk_input_key(ctx, NK_KEY_BACKSPACE, down);
        else if (sym == SDLK_HOME)
        {
            nk_input_key(ctx, NK_KEY_TEXT_START, down);
            nk_input_key(ctx, NK_KEY_SCROLL_START, down);
        }
        else if (sym == SDLK_END)
        {
            nk_input_key(ctx, NK_KEY_TEXT_END, down);
            nk_input_key(ctx, NK_KEY_SCROLL_END, down);
        }
        else if (sym == SDLK_PAGEDOWN)
            nk_input_key(ctx, NK_KEY_SCROLL_DOWN, down);
        else if (sym == SDLK_PAGEUP)
            nk_input_key(ctx, NK_KEY_SCROLL_UP, down);
        else if (sym == SDLK_z)
            nk_input_key(ctx, NK_KEY_TEXT_UNDO, down && state[SDL_SCANCODE_LCTRL]);
        else if (sym == SDLK_r)
            nk_input_key(ctx, NK_KEY_TEXT_REDO, down && state[SDL_SCANCODE_LCTRL]);
        else if (sym == SDLK_c)
            nk_input_key(ctx, NK_KEY_COPY, down && state[SDL_SCANCODE_LCTRL]);
        else if (sym == SDLK_v)
            nk_input_key(ctx, NK_KEY_PASTE, down && state[SDL_SCANCODE_LCTRL]);
        else if (sym == SDLK_x)
            nk_input_key(ctx, NK_KEY_CUT, down && state[SDL_SCANCODE_LCTRL]);
        else if (sym == SDLK_b)
            nk_input_key(ctx, NK_KEY_TEXT_LINE_START, down && state[SDL_SCANCODE_LCTRL]);
        else if (sym == SDLK_e)
            nk_input_key(ctx, NK_KEY_TEXT_LINE_END, down && state[SDL_SCANCODE_LCTRL]);
        else if (sym == SDLK_UP)
            nk_input_key(ctx, NK_KEY_UP, down);
        else if (sym == SDLK_DOWN)
            nk_input_key(ctx, NK_KEY_DOWN, down);
        else if (sym == SDLK_LEFT)
        {
            if (state[SDL_SCANCODE_LCTRL])
                nk_input_key(ctx, NK_KEY_TEXT_WORD_LEFT, down);
            else
                nk_input_key(ctx, NK_KEY_LEFT, down);
        }
        else if (sym == SDLK_RIGHT)
        {
            if (state[SDL_SCANCODE_LCTRL])
                nk_input_key(ctx, NK_KEY_TEXT_WORD_RIGHT, down);
            else
                nk_input_key(ctx, NK_KEY_RIGHT, down);
        }
        else
            return /*0*/;
        return /*1*/;
    }
    else if (evt->type == SDL_MOUSEBUTTONDOWN || evt->type == SDL_MOUSEBUTTONUP)
    {
        /* mouse button */
        int down = evt->type == SDL_MOUSEBUTTONDOWN;
        const int x = evt->button.x, y = evt->button.y;
        if (evt->button.button == SDL_BUTTON_LEFT)
            nk_input_button(ctx, NK_BUTTON_LEFT, x, y, down);
        if (evt->button.button == SDL_BUTTON_MIDDLE)
            nk_input_button(ctx, NK_BUTTON_MIDDLE, x, y, down);
        if (evt->button.button == SDL_BUTTON_RIGHT)
            nk_input_button(ctx, NK_BUTTON_RIGHT, x, y, down);
        return /*1*/;
    }
    else if (evt->type == SDL_MOUSEMOTION)
    {
        if (ctx->input.mouse.grabbed)
        {
            int x = (int)ctx->input.mouse.prev.x, y = (int)ctx->input.mouse.prev.y;
            nk_input_motion(ctx, x + evt->motion.xrel, y + evt->motion.yrel);
        }
        else
            nk_input_motion(ctx, evt->motion.x, evt->motion.y);
        return /*1*/;
    }
    else if (evt->type == SDL_TEXTINPUT)
    {
        nk_glyph glyph = {};
        memcpy(glyph, evt->text.text, NK_UTF_SIZE);
        nk_input_glyph(ctx, glyph);
        return /*1*/;
    }
    else if (evt->type == SDL_MOUSEWHEEL)
    {
        nk_input_scroll(ctx, (float)evt->wheel.y);
        return /*1*/;
    }
}

void NuklearUI::OnInputEnd(StringHash, VariantMap&)
{
    nk_input_end(&_nk_ctx);
}

void NuklearUI::OnEndRendering(StringHash, VariantMap&)
{
    const int MAX_VERTEX_MEMORY = 512 * 1024;
    const int MAX_ELEMENT_MEMORY = 128 * 1024;

    _graphics->SetViewport(Urho3D::IntRect(0, 0, _graphics->GetWidth(), _graphics->GetHeight()));
    _graphics->SetBlendMode(Urho3D::BLEND_ALPHA);
    _graphics->SetCullMode(Urho3D::CULL_NONE);
    _graphics->SetDepthTest(Urho3D::CMP_ALWAYS);
    _graphics->SetFillMode(Urho3D::FILL_SOLID);
    _graphics->SetColorWrite(true);
    _graphics->SetDepthWrite(false);

    // Max. vertex / index count is not assumed to change later
    if (_vertex_buffer->GetVertexCount() == 0)
    {
        Urho3D::PODVector< Urho3D::VertexElement > elems;
        elems.Push(Urho3D::VertexElement(Urho3D::TYPE_VECTOR2, Urho3D::SEM_POSITION));
        elems.Push(Urho3D::VertexElement(Urho3D::TYPE_VECTOR2, Urho3D::SEM_TEXCOORD));
        elems.Push(Urho3D::VertexElement(Urho3D::TYPE_UBYTE4_NORM, Urho3D::SEM_COLOR));

        _vertex_buffer->SetSize(MAX_VERTEX_MEMORY / sizeof(nk_sdl_vertex), elems, true);
    }

    if (_index_buffer->GetIndexCount() == 0)
        _index_buffer->SetSize(MAX_ELEMENT_MEMORY / sizeof(unsigned short), false, true);

    void* vertexData = _vertex_buffer->Lock(0, _vertex_buffer->GetVertexCount(), true);
    void* indexData = _index_buffer->Lock(0, _index_buffer->GetIndexCount(), true);

    if (vertexData && indexData)
    {
        struct nk_convert_config config;
        static const struct nk_draw_vertex_layout_element vertex_layout[] = {
            {NK_VERTEX_POSITION, NK_FORMAT_FLOAT, NK_OFFSETOF(struct nk_sdl_vertex, position)},
            {NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, NK_OFFSETOF(struct nk_sdl_vertex, uv)},
            {NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8, NK_OFFSETOF(struct nk_sdl_vertex, col)},
            {NK_VERTEX_LAYOUT_END}};
        NK_MEMSET(&config, 0, sizeof(config));
        config.vertex_layout = vertex_layout;
        config.vertex_size = sizeof(struct nk_sdl_vertex);
        config.vertex_alignment = NK_ALIGNOF(struct nk_sdl_vertex);
        config.null = _null_texture;
        config.circle_segment_count = 22;
        config.curve_segment_count = 22;
        config.arc_segment_count = 22;
        config.global_alpha = 1.0f;
        config.shape_AA = NK_ANTI_ALIASING_ON;
        config.line_AA = NK_ANTI_ALIASING_ON;

        {
            struct nk_buffer vbuf, ebuf;
            nk_buffer_init_fixed(&vbuf, vertexData, (nk_size)MAX_VERTEX_MEMORY);
            nk_buffer_init_fixed(&ebuf, indexData, (nk_size)MAX_ELEMENT_MEMORY);
            nk_convert(&_nk_ctx, &_commands, &vbuf, &ebuf, &config);
        }

        Urho3D::IntVector2 viewSize = _graphics->GetViewport().Size();
        Urho3D::Vector2 invScreenSize(1.0f / (float)viewSize.x_, 1.0f / (float)viewSize.y_);
        Urho3D::Vector2 scale(2.0f * invScreenSize.x_, -2.0f * invScreenSize.y_);

        Urho3D::Matrix4 projection(Urho3D::Matrix4::IDENTITY);
        projection.m00_ = scale.x_;
        projection.m03_ = -1.0f;
        projection.m11_ = scale.y_;
        projection.m13_ = 1.0f;
        projection.m22_ = 1.0f;
        projection.m23_ = 0.0f;
        projection.m33_ = 1.0f;

        Urho3D::ShaderVariation* diffTextureVS = _graphics->GetShader(Urho3D::VS, "NuklearUI", "");
        Urho3D::ShaderVariation* diffTexturePS = _graphics->GetShader(Urho3D::PS, "NuklearUI", "");
        _graphics->SetShaders(diffTextureVS, diffTexturePS);
        _graphics->SetVertexBuffer(_vertex_buffer);
        _graphics->SetIndexBuffer(_index_buffer);
        _graphics->SetShaderParameter(Urho3D::VSP_VIEWPROJ, projection);

        _vertex_buffer->Unlock();
        _index_buffer->Unlock();

        unsigned offset = 0;
        const struct nk_draw_command* cmd;

        nk_draw_foreach(cmd, &_nk_ctx, &_commands)
        {
            if (!cmd->elem_count)
                continue;

            _graphics->SetTexture(0, (Urho3D::Texture2D*)cmd->texture.ptr);
            _graphics->SetScissorTest(true, Urho3D::IntRect((int)cmd->clip_rect.x, (int)cmd->clip_rect.y,
                                                            (int)(cmd->clip_rect.x + cmd->clip_rect.w),
                                                            (int)(cmd->clip_rect.y + cmd->clip_rect.h)));

            _graphics->Draw(Urho3D::TRIANGLE_LIST, offset, cmd->elem_count, 0, 0, _vertex_buffer->GetVertexCount());

            offset += cmd->elem_count;
        }
        nk_clear(&_nk_ctx);
    }

    _graphics->SetScissorTest(false);
}
