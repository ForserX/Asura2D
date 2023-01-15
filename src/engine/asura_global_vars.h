#pragma once

// Editor code
extern bool is_editor_mode;

// Physical code
extern bool physical_debug_draw;
extern float target_physics_tps;
extern float target_physics_hertz;
extern float physics_delta;
extern float physics_real_delta;
extern int target_steps_count;

// Scheduler
extern float scheduler_delta;
extern float scheduler_real_delta;

// Windows code
extern GLFWwindow* window_handle;

extern int window_width;
extern int window_height;

#define fwindow_width (float)window_width
#define fwindow_height (float)window_height

extern bool window_maximized;
extern bool fullscreen_mode;

// Console code
extern bool show_console;
extern std::unique_ptr<Asura::UI::Console> console;

// Camera code
extern float CamZoom;

// Render 
extern Asura::Graphics::theme::style window_style;
extern bool show_fps_counter;

// Holders
extern Asura::GamePlay::holder_mode holder_type;

// Audio
extern float Volume;

extern GLuint RenderVBODefault, RenderVAODefault;

