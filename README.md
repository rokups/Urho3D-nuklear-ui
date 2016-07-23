# Nuklear UI for Urho3D

This is [Nuklear UI](https://github.com/vurtun/nuklear) subsystem for Urho3D.

![Screenshot 01](https://github.com/rokups/Urho3D-nuklear-ui/blob/master/screenshot.png?raw=true)

Dependencies
============

* `E_SDL_RAW_INPUT_EVENT.patch` for tapping into raw SDL events.
* `nuklear_sdl_gl3.h`
* `nuklear.h`
* `Urho3D` (obviously)

Usage
=====

1. Patch `Urho3D` with `E_SDL_RAW_INPUT_EVENT.patch`
2. Get [nuklear.h](https://github.com/vurtun/nuklear/raw/master/nuklear.h)
3. Get [nuklear\_sdl\_gl3.h](https://github.com/vurtun/nuklear/raw/master/demo/sdl_opengl3/nuklear_sdl_gl3.h)
4. Place initialization code in your Application::Start()

```
    context_->RegisterSubsystem(new NuklearUI(context_));
    nk_font_atlas* atlas = 0;
    nk_sdl_font_stash_begin(&atlas);
    // Additional font initialization here. See https://github.com/vurtun/nuklear/blob/master/demo/sdl_opengl3/main.c
    nk_sdl_font_stash_end();
```
