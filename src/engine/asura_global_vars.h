#pragma once

// Editor code
extern bool ark_editor_mode;

// Physical code
extern bool physical_debug_draw;
extern float target_physics_tps;
extern float target_physics_hertz;

// Windows code
extern SDL_Window* window_handle;

extern int window_width;
extern int window_height;

extern bool window_maximized;
extern bool fullscreen_mode;

// Console code
extern bool show_console;
extern std::unique_ptr<asura::ui::console> console;

// Camera code
extern float cam_zoom;

// Render 
extern SDL_Renderer* renderer;
extern asura::graphics::theme::style window_style;
extern bool show_fps_counter;