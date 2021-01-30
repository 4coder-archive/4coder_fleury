#ifndef FCODER_FLEURY_UBIQUITOUS_H
#define FCODER_FLEURY_UBIQUITOUS_H

enum keybinding_mode
{
	KeyBindingMode_0,
	KeyBindingMode_1,
    KeyBindingMode_2,
    KeyBindingMode_3,
    KeyBindingMode_MAX
};

static keybinding_mode GlobalKeybindingMode;
static Face_ID global_styled_title_face = 0;
static Face_ID global_styled_label_face = 0;
static Face_ID global_small_code_face = 0;
static Rect_f32 global_cursor_rect = {0};
static Rect_f32 global_last_cursor_rect = {0};
static Rect_f32 global_mark_rect = {0};
static Rect_f32 global_last_mark_rect = {0};
static b32 global_dark_mode = 1;
static b32 global_battery_saver = 0;
static View_ID global_compilation_view = 0;
static b32 global_compilation_view_expanded = 0;
global Arena permanent_arena = {};

#define MemorySet                 memset
#define MemoryCopy                memcpy
#define CalculateCStringLength    strlen
#define S8Lit(s)                  string_u8_litexpr(s)

#endif // FCODER_FLEURY_UBIQUITOUS_H
