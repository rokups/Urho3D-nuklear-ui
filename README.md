# Nuklear UI for Urho3D

This is [Nuklear UI](https://github.com/vurtun/nuklear) subsystem for Urho3D.

![Screenshot 01](https://github.com/rokups/Urho3D-nuklear-ui/blob/master/screenshot.png?raw=true)

Dependencies
============

* `nuklear_sdl_gl3.h`
* `nuklear.h`
* `Urho3D` (obviously)

Usage
=====

Place initialization code in your Application::Start()

```
    auto nuklear = new NuklearUI(context_);
    context_->RegisterSubsystem(nuklear);
    // Additional font initialization here. See https://github.com/vurtun/nuklear/blob/master/demo/sdl_opengl3/main.c
    nuklear->FinalizeFonts();
```
