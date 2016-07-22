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

using namespace Urho3D;

NuklearUI::NuklearUI(Context* ctx)
    : Urho3D::Object(ctx)
{
    _nk_ctx = nk_sdl_init(GetSubsystem<Graphics>()->GetWindow());

    SubscribeToEvent(E_ENDFRAME, URHO3D_HANDLER(NuklearUI, OnEndFrame));
    SubscribeToEvent(E_SDL_RAW_INPUT_EVENT, URHO3D_HANDLER(NuklearUI, OnRawEvent));
    SubscribeToEvent(E_POSTUPDATE, URHO3D_HANDLER(NuklearUI, OnPostUpdate));
    SubscribeToEvent(E_ENDRENDERING, URHO3D_HANDLER(NuklearUI, OnEndRendering));
}

NuklearUI::~NuklearUI()
{
    nk_sdl_shutdown();
}

void NuklearUI::OnEndFrame(StringHash, VariantMap&)
{
    // Begin capturing input for nuklear. This is done in E_ENDFRAME in order to begin capturing before E_BEGINFRAME
    // where Urho3D does input handling.
    nk_input_begin(_nk_ctx);
}

void NuklearUI::OnRawEvent(StringHash, VariantMap& args)
{
    nk_sdl_handle_event(static_cast<SDL_Event*>(args[SdlRawInputEvent::P_SDL_EVENT].Get<void*>()));
}

void NuklearUI::OnPostUpdate(StringHash, VariantMap&)
{
    nk_input_end(_nk_ctx);
}

void NuklearUI::OnEndRendering(StringHash, VariantMap&)
{
    const int MAX_VERTEX_MEMORY = 512 * 1024;
    const int MAX_ELEMENT_MEMORY = 128 * 1024;
    nk_sdl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY, MAX_ELEMENT_MEMORY);
}
