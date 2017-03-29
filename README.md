# Nuklear UI for Urho3D

This is [Nuklear UI](https://github.com/vurtun/nuklear) subsystem for Urho3D.

![Screenshot 01](https://github.com/rokups/Urho3D-nuklear-ui/blob/master/screenshot.png?raw=true)

Dependencies
============

* `nuklear.h`
* `Urho3D` (obviously)

Usage
=====

1. Place `Shaders` directory in any of your resource dirs.
2. Place initialization code in your Application::Start()

```
    auto nuklear = new NuklearUI(context_);
    context_->RegisterSubsystem(nuklear);
    // Initialize default font of your choice or use default one.
    nuklear->GetFontAtlas()->default_font = nk_font_atlas_add_default(nuklear->GetFontAtlas(), 13.f, 0);
    // Additional font initialization here. See https://github.com/vurtun/nuklear/blob/master/demo/sdl_opengl3/main.c
    nuklear->FinalizeFonts();
```
