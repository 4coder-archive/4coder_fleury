#include <stdlib.h>
#include <string.h>
#include "4coder_default_include.cpp"

#pragma warning(disable : 4706)

/*c

plot_function_samples(100)

plot_title('Plotting Data')
y = [ 1, 2, 3, 4, 5, 5, 5, 5, 4, 3, 3, 4, 4, 3, 2, 1, ]
plot_xaxis('x', -1, 16)
plot_yaxis('y', 0, 10)
plot(y)

plot_title('Plotting Functions #1')
plot_xaxis('x', -0.25, 1.25)
plot_yaxis('y', -0.25, 1.25)
plot(-2*x^3 + 3*x^2, -x^2, -x, 2*x)

plot_title('Plotting Functions #2')
plot_xaxis('x', -2, 2)
plot_yaxis('y', -2, 2)
plot(1/x, x^2, -sin(x), cos(4*x))

















*/

static float global_data[] =
{
    -1.17486284f,  4.82647457f,  2.1392075f,  3.24040988f, -3.01261177f,
    -1.80178182f,  3.0814119f,  0.45651351f, -0.54543045f,  0.87532875f,
    -0.04653197f,  2.83904198f,  3.1493846f,  1.71232683f, -0.23550316f,
    3.95970422f, -1.49984445f, -5.51201236f,  4.3974133f,  1.04582354f,
    3.50869819f,  7.12127077f, -3.44388528f,  5.52093195f,  8.82771799f,
    -0.88965206f, -2.5866942f, -0.20769781f,  5.79987926f,  1.56556676f,
    0.71842099f, -2.40362115f,  8.87100587f,  2.61483935f, -0.77880958f,
    2.72920219f,  0.83748575f,  2.52557666f, -0.85457281f, -3.01372783f,
    0.85208182f,  0.70583803f,  2.17686616f,  3.47886866f,  4.95687441f,
    1.67656828f,  2.95782891f,  5.35132421f,  1.23205987f,  1.53040894f,
    1.44874326f,  0.58153073f,  5.81477872f,  5.62262758f,  1.74386285f,
    -0.89131854f,  0.53786649f,  1.19914337f,  0.89126721f, -1.012731f,
    -0.39071098f,  4.5489671f, -2.90756732f,  0.83352492f,  2.36039797f,
    6.45089185f, -1.60607191f,  9.97839872f, -3.97497564f,  3.85368261f,
    2.63624203f,  4.70335664f,  2.73974976f,  0.75650377f,  4.24667697f,
    0.09704561f,  4.94932736f,  1.31053508f, -1.46443173f,  1.37036043f,
    2.78825453f, -1.3601765f,  6.56495498f,  2.86927298f,  2.54288572f,
    -0.72509466f,  0.09576604f, -0.27861987f, -1.04368792f,  4.04856048f,
    -3.1282726f,  5.28902556f,  7.17834039f,  1.29632207f,  4.12238558f,
    5.63038359f, 10.63183719f,  0.93271892f,  3.30360596f, -1.38693783f,
};

//~ NOTE(rjf): Hooks
static i32  Fleury4BeginBuffer(Application_Links *app, Buffer_ID buffer_id);
static void Fleury4Render(Application_Links *app, Frame_Info frame_info, View_ID view_id);
static Layout_Item_List Fleury4Layout(Application_Links *app, Arena *arena, Buffer_ID buffer, Range_i64 range, Face_ID face, f32 width);

//~ NOTE(rjf): Helpers
static void Fleury4LightMode(Application_Links *app);
static void Fleury4DarkMode(Application_Links *app);
static void Fleury4DrawCTokenColors(Application_Links *app, Text_Layout_ID text_layout_id, Token_Array *array);
static void Fleury4RenderBuffer(Application_Links *app, View_ID view_id, Face_ID face_id, Buffer_ID buffer, Text_Layout_ID text_layout_id, Rect_f32 rect, Frame_Info frame_info);
static void Fleury4SetBindings(Mapping *mapping);
static void Fleury4OpenCodePeek(Application_Links *app, String_Const_u8 base_needle, String_Match_Flag must_have_flags, String_Match_Flag must_not_have_flags);
static void Fleury4CloseCodePeek(void);
static void Fleury4NextCodePeek(void);
static void Fleury4CodePeekGo(Application_Links *app);

//~ NOTE(rjf): Globals
static b32 global_dark_mode = 1;
static Vec2_f32 global_smooth_cursor_position = {0};
static b32 global_code_peek_open = 0;
static int global_code_peek_match_count = 0;
String_Match global_code_peek_matches[16] = {0};
static int global_code_peek_selected_index = -1;
static f32 global_code_peek_open_transition = 0.f;
static Range_i64 global_code_peek_token_range;
static b32 global_power_mode_enabled = 0;
static struct
{
    int particle_count;
    struct
    {
        f32 x;
        f32 y;
        f32 velocity_x;
        f32 velocity_y;
        ARGB_Color color;
        f32 alpha;
        f32 roundness;
        f32 scale;
    }
    particles[4096];
    f32 screen_shake;
}
global_power_mode;

//~ NOTE(rjf): Utilities

typedef struct MemoryArena MemoryArena;
struct MemoryArena
{
    void *buffer;
    u32 buffer_size;
    u32 alloc_position;
    u32 bytes_left;
};

static MemoryArena
MemoryArenaInit(void *buffer, u32 buffer_size)
{
    MemoryArena arena = {0};
    arena.buffer = buffer;
    arena.buffer_size = buffer_size;
    arena.bytes_left = arena.buffer_size;
    return arena;
}

static void *
MemoryArenaAllocate(MemoryArena *arena, u32 size)
{
    void *memory = 0;
    if(arena->bytes_left >= size)
    {
        memory = (char *)arena->buffer + arena->alloc_position;
        arena->alloc_position += size;
        arena->bytes_left -= size;
        u32 bytes_to_align = arena->alloc_position % 16;
        arena->alloc_position += bytes_to_align;
        arena->bytes_left -= bytes_to_align;
    }
    return memory;
}

static void
MemoryArenaClear(MemoryArena *arena)
{
    arena->bytes_left = arena->buffer_size;
    arena->alloc_position = 0;
}

static f32
RandomF32(f32 low, f32 high)
{
    return low + (high - low) * (((int)rand() % 10000) / 10000.f);
}

static f32
MinimumF32(f32 a, f32 b)
{
    return a < b ? a : b;
}

static f32
MaximumF32(f32 a, f32 b)
{
    return a > b ? a : b;
}

static ARGB_Color
ARGBFromID(Managed_ID id)
{
    return fcolor_resolve(fcolor_id(id));
}

static b32
CharIsAlpha(int c)
{
    return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}

static b32
CharIsDigit(int c)
{
    return (c >= '0' && c <= '9');
}

static b32
CharIsSymbol(int c)
{
    return (c == '~' ||
            c == '`' ||
            c == '!' ||
            c == '#' ||
            c == '$' ||
            c == '%' ||
            c == '^' ||
            c == '&' ||
            c == '*' ||
            c == '(' ||
            c == ')' ||
            c == '-' ||
            c == '+' ||
            c == '=' ||
            c == '{' ||
            c == '}' ||
            c == '[' ||
            c == ']' ||
            c == ':' ||
            c == ';' ||
            c == '<' ||
            c == '>' ||
            c == ',' ||
            c == '.' ||
            c == '?' ||
            c == '/');
}

static double
GetFirstDoubleFromBuffer(char *buffer)
{
    char number_str[256];
    int number_write_pos = 0;
    double value = 0;
    for(int i = 0; buffer[i] && number_write_pos < sizeof(number_str); ++i)
    {
        if(CharIsDigit(buffer[i]) || buffer[i] == '.')
        {
            number_str[number_write_pos++] = buffer[i];
        }
        else
        {
            number_str[number_write_pos++] = 0;
            break;
        }
    }
    number_str[sizeof(number_str)-1] = 0;
    value = atof(number_str);
    return value;
}

static unsigned int CStringCRC32(char *string);
static unsigned int StringCRC32(char *string, int string_length);
static unsigned int CRC32(unsigned char *buf, int len);
static int
StringMatchCaseSensitive(char *a, int a_length, char *b, int b_length)
{
    int match = 0;
    if(a && b && a[0] && b[0] && a_length == b_length)
    {
        match = 1;
        for(int i = 0; i < a_length; ++i)
        {
            if(a[i] != b[i])
            {
                match = 0;
                break;
            }
        }
    }
    return match;
}

#define MemorySet                 memset
#define MemoryCopy                memcpy
#define CalculateCStringLength    strlen

//~ NOTE(rjf): Power Mode

static void
Fleury4Particle(f32 x, f32 y, f32 velocity_x, f32 velocity_y, ARGB_Color color,
                f32 roundness, f32 scale)
{
    if(global_power_mode.particle_count < ArrayCount(global_power_mode.particles))
    {
        int i = global_power_mode.particle_count++;
        global_power_mode.particles[i].x = x;
        global_power_mode.particles[i].y = y;
        global_power_mode.particles[i].velocity_x = velocity_x;
        global_power_mode.particles[i].velocity_y = velocity_y;
        global_power_mode.particles[i].color = color;
        global_power_mode.particles[i].alpha = 1.f;
        global_power_mode.particles[i].roundness = roundness;
        global_power_mode.particles[i].scale = scale;
    }
}

static Vec2_f32
Fleury4GetCameraFromView(Application_Links *app, View_ID view)
{
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
    Buffer_Scroll scroll = view_get_buffer_scroll(app, view);
    Face_ID face = get_face_id(app, buffer);
    Face_Metrics metrics = get_face_metrics(app, face);

    Vec2_f32 v =
    {
        scroll.position.pixel_shift.x,
        scroll.position.pixel_shift.y + scroll.position.line_number*metrics.line_height,
    };

    return v;
}

static void
Fleury4SpawnPowerModeParticles(Application_Links *app, View_ID view)
{
    if(global_power_mode_enabled)
    {
        Vec2_f32 camera = Fleury4GetCameraFromView(app, view);

        for(int i = 0; i < 60; ++i)
        {
            f32 movement_angle = RandomF32(-3.1415926535897f*3.f/2.f, 3.1415926535897f*1.f/3.f);
            f32 velocity_magnitude = RandomF32(20.f, 180.f);
            f32 velocity_x = cosf(movement_angle)*velocity_magnitude;
            f32 velocity_y = sinf(movement_angle)*velocity_magnitude;
            Fleury4Particle(global_smooth_cursor_position.x + 4 + camera.x,
                            global_smooth_cursor_position.y + 8 + camera.y,
                            velocity_x, velocity_y,
                            0xffffffff,
                            RandomF32(1.5f, 8.f),
                            RandomF32(0.5f, 6.f));
        }

        global_power_mode.screen_shake += RandomF32(6.f, 16.f);
    }
}

static void
Fleury4RenderPowerMode(Application_Links *app, View_ID view, Face_ID face, Frame_Info frame_info)
{
    Buffer_Scroll buffer_scroll = view_get_buffer_scroll(app, view);
    Face_Metrics metrics = get_face_metrics(app, face);

    if(global_power_mode.particle_count > 0)
    {
        animate_in_n_milliseconds(app, 0);
    }

    f32 camera_x = buffer_scroll.position.pixel_shift.x;
    f32 camera_y = buffer_scroll.position.pixel_shift.y + buffer_scroll.position.line_number*metrics.line_height;

    for(int i = 0; i < global_power_mode.particle_count;)
    {
        // NOTE(rjf): Update particle.
        {
            global_power_mode.particles[i].x += global_power_mode.particles[i].velocity_x * frame_info.animation_dt;
            global_power_mode.particles[i].y += global_power_mode.particles[i].velocity_y * frame_info.animation_dt;
            global_power_mode.particles[i].velocity_x -= global_power_mode.particles[i].velocity_x * frame_info.animation_dt * 1.5f;
            global_power_mode.particles[i].velocity_y -= global_power_mode.particles[i].velocity_y * frame_info.animation_dt * 1.5f;
            global_power_mode.particles[i].velocity_y += 10.f * frame_info.animation_dt;
            global_power_mode.particles[i].alpha -= 0.5f * frame_info.animation_dt;
        }

        if(global_power_mode.particles[i].alpha <= 0.f)
        {
            global_power_mode.particles[i] = global_power_mode.particles[--global_power_mode.particle_count];
        }
        else
        {
            // NOTE(rjf): Render particle.
            {
                Rect_f32 rect =
                {
                    global_power_mode.particles[i].x - global_power_mode.particles[i].scale - camera_x,
                    global_power_mode.particles[i].y - global_power_mode.particles[i].scale - camera_y,
                    global_power_mode.particles[i].x + global_power_mode.particles[i].scale - camera_x,
                    global_power_mode.particles[i].y + global_power_mode.particles[i].scale - camera_y,
                };
                f32 roundness = global_power_mode.particles[i].roundness;
                ARGB_Color color = global_power_mode.particles[i].color;
                color &= 0x00ffffff;
                color |= ((u32)(global_power_mode.particles[i].alpha * 60.f)) << 24;
                draw_rectangle(app, rect, roundness, color);
            }

            ++i;
        }

    }

}

//~ NOTE(rjf): Code Peek

static String_Const_u8_Array
Fleury4MakeTypeSearchList(Application_Links *app, Arena *arena, String_Const_u8 base_needle)
{
    String_Const_u8_Array result = {0};
    if(base_needle.size > 0)
    {
        result.count = 9;
        result.vals = push_array(arena, String_Const_u8, result.count);
        i32 i = 0;
        result.vals[i++] = (push_u8_stringf(arena, "struct %.*s{"  , string_expand(base_needle)));
        result.vals[i++] = (push_u8_stringf(arena, "struct %.*s\n{", string_expand(base_needle)));
        result.vals[i++] = (push_u8_stringf(arena, "struct %.*s {" , string_expand(base_needle)));
        result.vals[i++] = (push_u8_stringf(arena, "union %.*s{"   , string_expand(base_needle)));
        result.vals[i++] = (push_u8_stringf(arena, "union %.*s\n{" , string_expand(base_needle)));
        result.vals[i++] = (push_u8_stringf(arena, "union %.*s {"  , string_expand(base_needle)));
        result.vals[i++] = (push_u8_stringf(arena, "enum %.*s{"    , string_expand(base_needle)));
        result.vals[i++] = (push_u8_stringf(arena, "enum %.*s\n{"  , string_expand(base_needle)));
        result.vals[i++] = (push_u8_stringf(arena, "enum %.*s {"   , string_expand(base_needle)));
        Assert(i == result.count);
    }
    return(result);
}


static void
Fleury4OpenCodePeek(Application_Links *app, String_Const_u8 base_needle,
                    String_Match_Flag must_have_flags, String_Match_Flag must_not_have_flags)
{
    global_code_peek_match_count = 0;

    global_code_peek_open_transition = 0.f;

    Scratch_Block scratch(app);
    String_Const_u8_Array type_array = Fleury4MakeTypeSearchList(app, scratch, base_needle);
    String_Match_List matches = find_all_matches_all_buffers(app, scratch, type_array, must_have_flags, must_not_have_flags);
    string_match_list_filter_remove_buffer_predicate(app, &matches, buffer_has_name_with_star);

    for(String_Match *match = matches.first; match; match = match->next)
    {
        global_code_peek_matches[global_code_peek_match_count++] = *match;
        if(global_code_peek_match_count >= sizeof(global_code_peek_matches)/sizeof(global_code_peek_matches[0]))
        {
            break;
        }
    }

    matches = find_all_matches_all_buffers(app, scratch, base_needle, must_have_flags, must_not_have_flags);

    if(global_code_peek_match_count == 0)
    {
        for(String_Match *match = matches.first; match; match = match->next)
        {
            global_code_peek_matches[global_code_peek_match_count++] = *match;
            if(global_code_peek_match_count >= sizeof(global_code_peek_matches)/sizeof(global_code_peek_matches[0]))
            {
                break;
            }
        }
    }

    if(global_code_peek_match_count > 0)
    {
        global_code_peek_selected_index = 0;
        global_code_peek_open = 1;
    }
    else
    {
        global_code_peek_selected_index = -1;
        global_code_peek_open = 0;
    }
}

static void
Fleury4CloseCodePeek(void)
{
    global_code_peek_open = 0;
}

static void
Fleury4NextCodePeek(void)
{
    if(++global_code_peek_selected_index >= global_code_peek_match_count)
    {
        global_code_peek_selected_index = 0;
    }

    if(global_code_peek_selected_index >= global_code_peek_match_count)
    {
        global_code_peek_selected_index = -1;
        global_code_peek_open = 0;
    }
}

static void
Fleury4CodePeekGo(Application_Links *app)
{
    if(global_code_peek_selected_index >= 0 && global_code_peek_selected_index < global_code_peek_match_count &&
       global_code_peek_match_count > 0)
    {
        View_ID view = get_active_view(app, Access_Always);
        String_Match *match = &global_code_peek_matches[global_code_peek_selected_index];
        view = get_next_view_looped_primary_panels(app, view, Access_Always);
        view_set_buffer(app, view, match->buffer, 0);
        i64 line_number = get_line_number_from_pos(app, match->buffer, match->range.start);
        Buffer_Scroll scroll = view_get_buffer_scroll(app, view);
        scroll.position.line_number = scroll.target.line_number = line_number;
        view_set_buffer_scroll(app, view, scroll, SetBufferScroll_SnapCursorIntoView);
        Fleury4CloseCodePeek();
    }
}

static void
Fleury4RenderCodePeek(Application_Links *app, View_ID view_id, Face_ID face_id, Buffer_ID buffer,
                      Frame_Info frame_info)
{
    if(global_code_peek_open &&
       global_code_peek_selected_index >= 0 &&
       global_code_peek_selected_index < global_code_peek_match_count)
    {
        String_Match *match = &global_code_peek_matches[global_code_peek_selected_index];

        global_code_peek_open_transition += (1.f - global_code_peek_open_transition) * frame_info.animation_dt * 14.f;
        if(fabs(global_code_peek_open_transition - 1.f) > 0.005f)
        {
            animate_in_n_milliseconds(app, 0);
        }

        Rect_f32 rect = {0};
        rect.x0 = (float)((int)global_smooth_cursor_position.x + 16);
        rect.y0 = (float)((int)global_smooth_cursor_position.y + 16);
        rect.x1 = (float)((int)rect.x0 + 400);
        rect.y1 = (float)((int)rect.y0 + 600*global_code_peek_open_transition);

        draw_rectangle(app, rect, 4.f, fcolor_resolve(fcolor_id(defcolor_back)));
        draw_rectangle_outline(app, rect, 4.f, 3.f, fcolor_resolve(fcolor_id(defcolor_pop2)));

        if(rect.y1 - rect.y0 > 60.f)
        {
            rect.x0 += 30;
            rect.y0 += 30;
            rect.x1 -= 30;
            rect.y1 -= 30;

            Buffer_Point buffer_point =
            {
                get_line_number_from_pos(app, match->buffer, match->range.start),
                0,
            };
            Text_Layout_ID text_layout_id = text_layout_create(app, match->buffer, rect, buffer_point);

            Rect_f32 prev_clip = draw_set_clip(app, rect);
            {
                Token_Array token_array = get_token_array_from_buffer(app, match->buffer);
                if(token_array.tokens != 0)
                {
                    Fleury4DrawCTokenColors(app, text_layout_id, &token_array);
                }
                else
                {
                    Range_i64 visible_range = match->range;
                    paint_text_color_fcolor(app, text_layout_id, visible_range, fcolor_id(defcolor_text_default));
                }

                draw_text_layout_default(app, text_layout_id);
            }
            draw_set_clip(app, prev_clip);
            text_layout_free(app, text_layout_id);
        }
    }
    else
    {
        global_code_peek_open_transition = 0.f;
    }

}

//~ NOTE(rjf): Commands

CUSTOM_COMMAND_SIG(fleury_write_text_input)
CUSTOM_DOC("Inserts whatever text was used to trigger this command.")
{
    User_Input in = get_current_input(app);
    String_Const_u8 insert = to_writable(&in);
    write_text(app, insert);
    Fleury4SpawnPowerModeParticles(app, get_active_view(app, Access_ReadWriteVisible));
}

CUSTOM_COMMAND_SIG(fleury_write_text_and_auto_indent)
CUSTOM_DOC("Inserts text and auto-indents the line on which the cursor sits if any of the text contains 'layout punctuation' such as ;:{}()[]# and new lines.")
{
    User_Input in = get_current_input(app);
    String_Const_u8 insert = to_writable(&in);
    if (insert.str != 0 && insert.size > 0){
        b32 do_auto_indent = false;
        for (u64 i = 0; !do_auto_indent && i < insert.size; i += 1){
            switch (insert.str[i]){
                case ';': case ':':
                case '{': case '}':
                case '(': case ')':
                case '[': case ']':
                case '#':
                case '\n': case '\t':
                {
                    do_auto_indent = true;
                }break;
            }
        }
        if (do_auto_indent){
            View_ID view = get_active_view(app, Access_ReadWriteVisible);
            Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
            Range_i64 pos = {};
            pos.min = view_get_cursor_pos(app, view);
            write_text_input(app);
            pos.max= view_get_cursor_pos(app, view);
            auto_indent_buffer(app, buffer, pos, 0);
            move_past_lead_whitespace(app, view, buffer);
        }
        else{
            write_text_input(app);
        }
    }
    Fleury4SpawnPowerModeParticles(app, get_active_view(app, Access_ReadWriteVisible));
}

CUSTOM_COMMAND_SIG(fleury_toggle_colors)
CUSTOM_DOC("Toggles light/dark mode.")
{
    if(global_dark_mode)
    {
        Fleury4LightMode(app);
        global_dark_mode = 0;
    }
    else
    {
        Fleury4DarkMode(app);
        global_dark_mode = 1;
    }
}

CUSTOM_COMMAND_SIG(fleury_toggle_power_mode)
CUSTOM_DOC("Toggles power mode.")
{
    if(global_power_mode_enabled)
    {
        global_power_mode_enabled = 0;
    }
    else
    {
        global_power_mode_enabled = 1;
    }
}

CUSTOM_COMMAND_SIG(fleury_code_peek)
CUSTOM_DOC("Opens code peek.")
{
    View_ID view = get_active_view(app, Access_ReadWriteVisible);
    i64 pos = view_get_cursor_pos(app, view);
    if(global_code_peek_open && pos >= global_code_peek_token_range.start &&
       pos <= global_code_peek_token_range.end)
    {
        Fleury4NextCodePeek();
    }
    else
    {
        Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
        Scratch_Block scratch(app);
        Range_i64 range = enclose_pos_alpha_numeric_underscore(app, buffer, pos);
        global_code_peek_token_range = range;
        String_Const_u8 base_needle = push_token_or_word_under_active_cursor(app, scratch);
        Fleury4OpenCodePeek(app, base_needle, StringMatch_CaseSensitive, StringMatch_LeftSideSloppy | StringMatch_RightSideSloppy);
    }
}

CUSTOM_COMMAND_SIG(fleury_close_code_peek)
CUSTOM_DOC("Closes code peek.")
{
    if(global_code_peek_open)
    {
        Fleury4CloseCodePeek();
    }
    else
    {
        leave_current_input_unhandled(app);
    }
}

CUSTOM_COMMAND_SIG(fleury_code_peek_go)
CUSTOM_DOC("Goes to the active code peek.")
{
    Fleury4CodePeekGo(app);
}

CUSTOM_COMMAND_SIG(fleury_write_zero_struct)
CUSTOM_DOC("At the cursor, insert a ' = {0};'.")
{
    write_string(app, string_u8_litexpr(" = {0};"));
}

CUSTOM_COMMAND_SIG(fleury_home)
CUSTOM_DOC("Goes to the beginning of the line.")
{
    seek_pos_of_visual_line(app, Side_Min);
    View_ID view = get_active_view(app, Access_ReadWriteVisible);
    Buffer_Scroll scroll = view_get_buffer_scroll(app, view);
    scroll.target.pixel_shift.x = 0;
    view_set_buffer_scroll(app, view, scroll, SetBufferScroll_NoCursorChange);
}

//~ NOTE(rjf): Custom layer initialization

void
custom_layer_init(Application_Links *app)
{
    Thread_Context *tctx = get_thread_context(app);

    // NOTE(allen): setup for default framework
    {
        async_task_handler_init(app, &global_async_system);
        code_index_init();
        buffer_modified_set_init();
        Profile_Global_List *list = get_core_profile_list(app);
        ProfileThreadName(tctx, list, string_u8_litexpr("main"));
        initialize_managed_id_metadata(app);
        set_default_color_scheme(app);
    }

    // NOTE(allen): default hooks and command maps
    {
    set_all_default_hooks(app);
        set_custom_hook(app, HookID_RenderCaller,  Fleury4Render);
        set_custom_hook(app, HookID_BeginBuffer,   Fleury4BeginBuffer);
        set_custom_hook(app, HookID_Layout,        Fleury4Layout);
    mapping_init(tctx, &framework_mapping);
    Fleury4SetBindings(&framework_mapping);
    }

    Fleury4DarkMode(app);
}

//~ NOTE(rjf): Light/Dark Mode

static void
Fleury4LightMode(Application_Links *app)
{
    Color_Table *table = &active_color_table;
    Arena *arena = &global_theme_arena;
    linalloc_clear(arena);
    *table = make_color_table(app, arena);

    table->arrays[defcolor_bar]                   = make_colors(arena, 0xFFd9dfe2);
    table->arrays[defcolor_base]                  = make_colors(arena, 0xff806d56);
    table->arrays[defcolor_pop1]                  = make_colors(arena, 0xFF3C57DC);
    table->arrays[defcolor_pop2]                  = make_colors(arena, 0xFFFF0000);
    table->arrays[defcolor_back]                  = make_colors(arena, 0xFFd9dfe2);
    table->arrays[defcolor_margin]                = make_colors(arena, 0xFFd9dfe2);
    table->arrays[defcolor_margin_hover]          = make_colors(arena, 0xff63523d);
    table->arrays[defcolor_margin_active]         = make_colors(arena, 0xff63523d);
    table->arrays[defcolor_list_item]             = make_colors(arena, 0xFF222425);
    table->arrays[defcolor_list_item_hover]       = make_colors(arena, 0xff63523d);
    table->arrays[defcolor_list_item_active]      = make_colors(arena, 0xff63523d);
    table->arrays[defcolor_cursor]                = make_colors(arena, 0xFF00EE00);
    table->arrays[defcolor_at_cursor]             = make_colors(arena, 0xFF0C0C0C);
    table->arrays[defcolor_highlight_cursor_line] = make_colors(arena, 0xFFd9dfe2);
    table->arrays[defcolor_highlight]             = make_colors(arena, 0xFFDDEE00);
    table->arrays[defcolor_at_highlight]          = make_colors(arena, 0xFFFF44DD);
    table->arrays[defcolor_mark]                  = make_colors(arena, 0xFF494949);
    table->arrays[defcolor_text_default]          = make_colors(arena, 0xff806d56);
    table->arrays[defcolor_comment]               = make_colors(arena, 0xff9ba290);
    table->arrays[defcolor_comment_pop]           = make_colors(arena, 0xff2ab34f, 0xFFdb2828);
    table->arrays[defcolor_keyword]               = make_colors(arena, 0xff3b3733);
    table->arrays[defcolor_str_constant]          = make_colors(arena, 0xffb67900);
    table->arrays[defcolor_char_constant]         = make_colors(arena, 0xffb67900);
    table->arrays[defcolor_int_constant]          = make_colors(arena, 0xffb67900);
    table->arrays[defcolor_float_constant]        = make_colors(arena, 0xffb67900);
    table->arrays[defcolor_bool_constant]         = make_colors(arena, 0xffb67900);
    table->arrays[defcolor_preproc]               = make_colors(arena, 0xFFdc7575);
    table->arrays[defcolor_include]               = make_colors(arena, 0xffb67900);
    table->arrays[defcolor_special_character]     = make_colors(arena, 0xFFFF0000);
    table->arrays[defcolor_ghost_character]       = make_colors(arena, 0xFF4E5E46);
    table->arrays[defcolor_highlight_junk]        = make_colors(arena, 0xFF3A0000);
    table->arrays[defcolor_highlight_white]       = make_colors(arena, 0xFF003A3A);
    table->arrays[defcolor_paste]                 = make_colors(arena, 0xFFDDEE00);
    table->arrays[defcolor_undo]                  = make_colors(arena, 0xFF00DDEE);
    table->arrays[defcolor_back_cycle]            = make_colors(arena, 0xffd9dfe2, 0xffd9dfe2, 0xffd9dfe2, 0xff9db8c5);
    table->arrays[defcolor_text_cycle]            = make_colors(arena, 0xFFA00000, 0xFF00A000, 0xFF0030B0, 0xFFA0A000);
    table->arrays[defcolor_line_numbers_back]     = make_colors(arena, 0xFF101010);
    table->arrays[defcolor_line_numbers_text]     = make_colors(arena, 0xFF404040);
}

static void
Fleury4DarkMode(Application_Links *app)
{
    Color_Table *table = &active_color_table;
    Arena *arena = &global_theme_arena;
    linalloc_clear(arena);
    *table = make_color_table(app, arena);

    table->arrays[defcolor_bar]                   = make_colors(arena, 0xFF222425);
    table->arrays[defcolor_base]                  = make_colors(arena, 0xffb99468);
    table->arrays[defcolor_pop1]                  = make_colors(arena, 0xFF3C57DC);
    table->arrays[defcolor_pop2]                  = make_colors(arena, 0xFFFF0000);
    table->arrays[defcolor_back]                  = make_colors(arena, 0xFF222425);
    table->arrays[defcolor_margin]                = make_colors(arena, 0xFF222425);
    table->arrays[defcolor_margin_hover]          = make_colors(arena, 0xff63523d);
    table->arrays[defcolor_margin_active]         = make_colors(arena, 0xff63523d);
    table->arrays[defcolor_list_item]             = make_colors(arena, 0xFF222425);
    table->arrays[defcolor_list_item_hover]       = make_colors(arena, 0xff63523d);
    table->arrays[defcolor_list_item_active]      = make_colors(arena, 0xff63523d);
    table->arrays[defcolor_cursor]                = make_colors(arena, 0xFF00EE00);
    table->arrays[defcolor_at_cursor]             = make_colors(arena, 0xFF0C0C0C);
    table->arrays[defcolor_highlight_cursor_line] = make_colors(arena, 0xFF1E1E1E);
    table->arrays[defcolor_highlight]             = make_colors(arena, 0xFFDDEE00);
    table->arrays[defcolor_at_highlight]          = make_colors(arena, 0xFFFF44DD);
    table->arrays[defcolor_mark]                  = make_colors(arena, 0xFF494949);
    table->arrays[defcolor_text_default]          = make_colors(arena, 0xffb99468);
    table->arrays[defcolor_comment]               = make_colors(arena, 0xff9ba290);
    table->arrays[defcolor_comment_pop]           = make_colors(arena, 0xff2ab34f, 0xFFdb2828);
    table->arrays[defcolor_keyword]               = make_colors(arena, 0xfff0c674);
    table->arrays[defcolor_str_constant]          = make_colors(arena, 0xffffa900);
    table->arrays[defcolor_char_constant]         = make_colors(arena, 0xffffa900);
    table->arrays[defcolor_int_constant]          = make_colors(arena, 0xffffa900);
    table->arrays[defcolor_float_constant]        = make_colors(arena, 0xffffa900);
    table->arrays[defcolor_bool_constant]         = make_colors(arena, 0xffffa900);
    table->arrays[defcolor_preproc]               = make_colors(arena, 0xFFdc7575);
    table->arrays[defcolor_include]               = make_colors(arena, 0xffffa900);
    table->arrays[defcolor_special_character]     = make_colors(arena, 0xFFFF0000);
    table->arrays[defcolor_ghost_character]       = make_colors(arena, 0xFF4E5E46);
    table->arrays[defcolor_highlight_junk]        = make_colors(arena, 0xFF3A0000);
    table->arrays[defcolor_highlight_white]       = make_colors(arena, 0xFF003A3A);
    table->arrays[defcolor_paste]                 = make_colors(arena, 0xFFDDEE00);
    table->arrays[defcolor_undo]                  = make_colors(arena, 0xFF00DDEE);
    table->arrays[defcolor_back_cycle]            = make_colors(arena, 0xFF222425, 0xff1e1f20, 0xff1e1f20, 0xff13141);
    table->arrays[defcolor_text_cycle]            = make_colors(arena, 0xFFA00000, 0xFF00A000, 0xFF0030B0, 0xFFA0A000);
    table->arrays[defcolor_line_numbers_back]     = make_colors(arena, 0xFF101010);
    table->arrays[defcolor_line_numbers_text]     = make_colors(arena, 0xFF404040);
}

//~ NOTE(rjf): Cursor rendering

static void
Fleury4RenderCursor(Application_Links *app, View_ID view_id, b32 is_active_view,
                    Buffer_ID buffer, Text_Layout_ID text_layout_id,
                    f32 roundness, f32 outline_thickness, Frame_Info frame_info)
{
    b32 has_highlight_range = draw_highlight_range(app, view_id, buffer, text_layout_id, roundness);

    if(!has_highlight_range)
    {
        i64 cursor_pos = view_get_cursor_pos(app, view_id);
        i64 mark_pos = view_get_mark_pos(app, view_id);
        if (is_active_view)
        {

            // NOTE(rjf): Draw cursor.
            {
                static Rect_f32 rect = {0};
                Rect_f32 target_rect = text_layout_character_on_screen(app, text_layout_id, cursor_pos);
                Rect_f32 last_rect = rect;

                float x_change = target_rect.x0 - rect.x0;
                float y_change = target_rect.y0 - rect.y0;
                float cursor_size_x = (target_rect.x1 - target_rect.x0);
                float cursor_size_y = (target_rect.y1 - target_rect.y0) * (1 + fabsf(y_change) / 60.f);

                rect.x0 += (x_change) * frame_info.animation_dt * 14.f;
                rect.y0 += (y_change) * frame_info.animation_dt * 14.f;
                rect.x1 = rect.x0 + cursor_size_x;
                rect.y1 = rect.y0 + cursor_size_y;

                global_smooth_cursor_position.x = rect.x0;
                global_smooth_cursor_position.y = rect.y0;

                if(target_rect.y0 > last_rect.y0)
                {
                    if(rect.y0 < last_rect.y0)
                    {
                        rect.y0 = last_rect.y0;
                    }
                }
                else
                {
                    if(rect.y1 > last_rect.y1)
                    {
                        rect.y1 = last_rect.y1;
                    }
                }

                if(fabs(x_change) > 1.f || fabs(y_change) > 1.f)
                {
                    animate_in_n_milliseconds(app, 0);
                }

                FColor cursor_color = fcolor_id(defcolor_cursor);

                if(global_keyboard_macro_is_recording)
                {
                    cursor_color = fcolor_argb(0xffde40df);
                }
                else if(global_power_mode_enabled)
                {
                    cursor_color = fcolor_argb(0xffefaf2f);
                }

                // NOTE(rjf): Draw main cursor.
                {
                    draw_rectangle(app, rect, roundness, fcolor_resolve(cursor_color));
                }

                // NOTE(rjf): Draw cursor glow (because why the hell not).
                for(int i = 0; i < 20; ++i)
                {
                    f32 alpha = 0.1f - (global_power_mode_enabled ? (i*0.005f) : (i*0.015f));
                    if(alpha > 0)
                    {
                    Rect_f32 glow_rect = rect;
                    glow_rect.x0 -= i;
                    glow_rect.y0 -= i;
                    glow_rect.x1 += i;
                    glow_rect.y1 += i;
                        draw_rectangle(app, glow_rect, roundness + i*0.7f, fcolor_resolve(fcolor_change_alpha(cursor_color, alpha)));
                    }
                    else
                    {
                        break;
                    }
                }

            }

            paint_text_color_pos(app, text_layout_id, cursor_pos,
                                 fcolor_id(defcolor_at_cursor));
            draw_character_wire_frame(app, text_layout_id, mark_pos,
                                      roundness, outline_thickness,
                                      fcolor_id(defcolor_mark));
        }
        else
        {
            draw_character_wire_frame(app, text_layout_id, mark_pos,
                                      roundness, outline_thickness,
                                      fcolor_id(defcolor_mark));
            draw_character_wire_frame(app, text_layout_id, cursor_pos,
                                      roundness, outline_thickness,
                                      fcolor_id(defcolor_cursor));

        }

    }
}

//~ NOTE(rjf): Brace highlight

static void
Fleury4RenderBraceHighlight(Application_Links *app, Buffer_ID buffer, Text_Layout_ID text_layout_id,
                            i64 pos, ARGB_Color *colors, i32 color_count)
{
    Token_Array token_array = get_token_array_from_buffer(app, buffer);
    if (token_array.tokens != 0)
    {
        Token_Iterator_Array it = token_iterator_pos(0, &token_array, pos);
        Token *token = token_it_read(&it);
        if(token != 0 && token->kind == TokenBaseKind_ScopeOpen)
        {
            pos = token->pos + token->size;
        }
        else
        {

            if(token_it_dec_all(&it))
            {
                token = token_it_read(&it);

                if (token->kind == TokenBaseKind_ScopeClose &&
                    pos == token->pos + token->size)
                {
                    pos = token->pos;
                }

            }

        }

    }

    draw_enclosures(app, text_layout_id, buffer,
                    pos, FindNest_Scope,
                    RangeHighlightKind_CharacterHighlight,
                    0, 0, colors, color_count);
}

//~ NOTE(rjf): Closing-brace Annotation

static void
Fleury4RenderCloseBraceAnnotation(Application_Links *app, Buffer_ID buffer, Text_Layout_ID text_layout_id,
                                  i64 pos)
{
    Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
    Token_Array token_array = get_token_array_from_buffer(app, buffer);
    Face_ID face_id = get_face_id(app, buffer);

    if(token_array.tokens != 0)
    {
        Token_Iterator_Array it = token_iterator_pos(0, &token_array, pos);
        Token *token = token_it_read(&it);

        if(token != 0 && token->kind == TokenBaseKind_ScopeOpen)
        {
            pos = token->pos + token->size;
        }
            else if(token_it_dec_all(&it))
            {
                token = token_it_read(&it);
                if (token->kind == TokenBaseKind_ScopeClose &&
                    pos == token->pos + token->size)
                {
                    pos = token->pos;
                }
            }
        }

    Scratch_Block scratch(app);
    Range_i64_Array ranges = get_enclosure_ranges(app, scratch, buffer, pos, RangeHighlightKind_CharacterHighlight);

    for (i32 i = ranges.count - 1; i >= 0; i -= 1)
    {
        Range_i64 range = ranges.ranges[i];

        if(range.start >= visible_range.start)
        {
            continue;
        }

        Rect_f32 close_scope_rect = text_layout_character_on_screen(app, text_layout_id, range.end);
        Vec2_f32 close_scope_pos = { close_scope_rect.x0 + 12, close_scope_rect.y0 };

        // NOTE(rjf): Find token set before this scope begins.
            Token *start_token = 0;
            i64 token_count = 0;
            {
            Token_Iterator_Array it = token_iterator_pos(0, &token_array, range.start-1);
            int paren_nest = 0;

                for(;;)
            {
                    Token *token = token_it_read(&it);
                    if(!token_it_dec_non_whitespace(&it))
                    {
                        break;
                }

                if(token)
                {
                    token_count += 1;

                    if(token->kind == TokenBaseKind_ParentheticalClose)
                    {
                        ++paren_nest;
                    }
                    else if(token->kind == TokenBaseKind_ParentheticalOpen)
                    {
                        --paren_nest;
                    }
                    else if(paren_nest == 0 &&
                            (token->kind == TokenBaseKind_ScopeClose ||
                             token->kind == TokenBaseKind_StatementClose))
                    {
                        break;
                    }
                    else if((token->kind == TokenBaseKind_Identifier || token->kind == TokenBaseKind_Keyword ||
                             token->kind == TokenBaseKind_Comment) &&
                            !paren_nest)
                    {
                        start_token = token;
                        break;
                    }

                }
                else
                {
                    break;
                }
                }

            }

        // NOTE(rjf): Draw.
        if(start_token)
            {
            draw_string(app, face_id, string_u8_litexpr("← "), close_scope_pos, finalize_color(defcolor_comment, 0));
            close_scope_pos.x += 32;
            String_Const_u8 start_line = push_buffer_line(app, scratch, buffer,
                                                          get_line_number_from_pos(app, buffer, start_token->pos));

            u64 first_non_whitespace_offset = 0;
            for(u64 c = 0; c < start_line.size; ++c)
            {
                if(start_line.str[c] <= 32)
                {
                    ++first_non_whitespace_offset;
                }
                else
                {
                    break;
                }
            }
            start_line.str += first_non_whitespace_offset;
            start_line.size -= first_non_whitespace_offset;

            u32 color = finalize_color(defcolor_comment, 0);
            color &= 0x00ffffff;
            color |= 0x80000000;
            draw_string(app, face_id, start_line, close_scope_pos, color);

            }

    }

}

//~ NOTE(rjf): Brace lines

static void
Fleury4RenderBraceLines(Application_Links *app, Buffer_ID buffer, View_ID view,
                        Text_Layout_ID text_layout_id, i64 pos)
{
    Face_ID face_id = get_face_id(app, buffer);
    Token_Array token_array = get_token_array_from_buffer(app, buffer);
    Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);

    if (token_array.tokens != 0)
    {
        Token_Iterator_Array it = token_iterator_pos(0, &token_array, pos);
        Token *token = token_it_read(&it);
        if(token != 0 && token->kind == TokenBaseKind_ScopeOpen)
        {
            pos = token->pos + token->size;
        }
        else
        {

            if(token_it_dec_all(&it))
            {
                token = token_it_read(&it);

                if (token->kind == TokenBaseKind_ScopeClose &&
                    pos == token->pos + token->size)
                {
                    pos = token->pos;
                }

            }

        }

    }

    Face_Metrics metrics = get_face_metrics(app, face_id);

    Scratch_Block scratch(app);
    Range_i64_Array ranges = get_enclosure_ranges(app, scratch, buffer, pos, RangeHighlightKind_CharacterHighlight);
    float x_position = view_get_screen_rect(app, view).x0 + 4 -
        view_get_buffer_scroll(app, view).position.pixel_shift.x;

    for (i32 i = ranges.count - 1; i >= 0; i -= 1)
    {
        Range_i64 range = ranges.ranges[i];

        Rect_f32 range_start_rect = text_layout_character_on_screen(app, text_layout_id, range.start);
        Rect_f32 range_end_rect = text_layout_character_on_screen(app, text_layout_id, range.end);

        float y_start = 0;
        float y_end = 10000;

        if(range.start >= visible_range.start)
        {
            y_start = range_start_rect.y0 + metrics.line_height;
        }
        if(range.end <= visible_range.end)
        {
            y_end = range_end_rect.y0;
        }

        Rect_f32 line_rect = {0};
        line_rect.x0 = x_position;
        line_rect.x1 = x_position+1;
        line_rect.y0 = y_start;
        line_rect.y1 = y_end;
        u32 color = finalize_color(defcolor_comment, 0);
        color &= 0x00ffffff;
        color |= 0x60000000;
        draw_rectangle(app, line_rect, 0.5f, color);

        x_position += metrics.space_advance * 4;

    }

}

//~ NOTE(rjf): Range highlight

static void
Fleury4RenderRangeHighlight(Application_Links *app, View_ID view_id, Text_Layout_ID text_layout_id,
                            Range_i64 range)
{
    Rect_f32 range_start_rect = text_layout_character_on_screen(app, text_layout_id, range.start);
    Rect_f32 range_end_rect = text_layout_character_on_screen(app, text_layout_id, range.end-1);
    Rect_f32 total_range_rect = {0};
    total_range_rect.x0 = MinimumF32(range_start_rect.x0, range_end_rect.x0);
    total_range_rect.y0 = MinimumF32(range_start_rect.y0, range_end_rect.y0);
    total_range_rect.x1 = MaximumF32(range_start_rect.x1, range_end_rect.x1);
    total_range_rect.y1 = MaximumF32(range_start_rect.y1, range_end_rect.y1);

    ARGB_Color background_color = fcolor_resolve(fcolor_id(defcolor_pop2));
    float background_color_r = (float)((background_color & 0x00ff0000) >> 16) / 255.f;
    float background_color_g = (float)((background_color & 0x0000ff00) >>  8) / 255.f;
    float background_color_b = (float)((background_color & 0x000000ff) >>  0) / 255.f;
    background_color_r += (1.f - background_color_r) * 0.5f;
    background_color_g += (1.f - background_color_g) * 0.5f;
    background_color_b += (1.f - background_color_b) * 0.5f;
    ARGB_Color highlight_color = (0x75000000 |
                                  (((u32)(background_color_r * 255.f)) << 16) |
        (((u32)(background_color_g * 255.f)) <<  8) |
        (((u32)(background_color_b * 255.f)) <<  0));
    draw_rectangle(app, total_range_rect, 4.f, highlight_color);
}

//~ NOTE(rjf): Divider Comments

static void
Fleury4RenderDividerComments(Application_Links *app, Buffer_ID buffer, View_ID view,
                             Text_Layout_ID text_layout_id)
{
    String_Const_u8 divider_comment_signifier =
    {
        "//~",
        0,
    };
    int divider_comment_signifier_length = 0;
    for(; divider_comment_signifier.str[divider_comment_signifier_length];
        ++divider_comment_signifier_length);
    divider_comment_signifier.size = divider_comment_signifier_length;

    Token_Array token_array = get_token_array_from_buffer(app, buffer);
    Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
    Scratch_Block scratch(app);

    if(token_array.tokens != 0)
    {
        i64 first_index = token_index_from_pos(&token_array, visible_range.first);
        Token_Iterator_Array it = token_iterator_index(0, &token_array, first_index);

        Token *token = 0;
        for(;;)
        {
            token = token_it_read(&it);

            if(token->pos >= visible_range.one_past_last || !token || !token_it_inc_non_whitespace(&it))
            {
                break;
            }

            if(token->kind == TokenBaseKind_Comment)
            {
                Rect_f32 comment_first_char_rect =
                    text_layout_character_on_screen(app, text_layout_id, token->pos);

                Range_i64 token_range =
                {
                    token->pos,
                    token->pos + (token->size > divider_comment_signifier_length
                                  ? divider_comment_signifier_length
                                  : token->size),
                };

                u8 token_buffer[256] = {0};
                buffer_read_range(app, buffer, token_range, token_buffer);
                String_Const_u8 token_string = { token_buffer, (u64)(token_range.end - token_range.start) };

                if(string_match(token_string, divider_comment_signifier))
                {
                    // NOTE(rjf): Render divider line.
                    Rect_f32 rect =
                    {
                        comment_first_char_rect.x0,
                        comment_first_char_rect.y0-2,
                        10000,
                        comment_first_char_rect.y0,
                    };
                    f32 roundness = 4.f;
                    draw_rectangle(app, rect, roundness, fcolor_resolve(fcolor_id(defcolor_comment)));
                }

            }

        }

    }

}

//~ NOTE(rjf): Plotting Tools

typedef struct PlotData2D PlotData2D;
struct PlotData2D
{
    Application_Links *app;
    Face_ID face_id;
    String_Const_u8 title;
    String_Const_u8 x_axis;
    String_Const_u8 y_axis;
    Rect_f32 screen_rect;
    Rect_f32 plot_view;
    int color_cycle_position;

    // NOTE(rjf): Used internally.
    Rect_f32 last_clip;
};

typedef enum Plot2DStyleFlags Plot2DStyleFlags;
enum Plot2DStyleFlags
{
    PLOT2D_LINES  = (1<<0),
    PLOT2D_POINTS = (1<<1),
};

static void
Fleury4BeginPlot2D(PlotData2D *plot)
{
    Rect_f32 rect = plot->screen_rect;
    Rect_f32 plot_view = plot->plot_view;

    draw_string(plot->app, plot->face_id, plot->title, V2f32(rect.x0, rect.y0 - 20), fcolor_resolve(fcolor_id(defcolor_comment)));
    
    if(plot->x_axis.data)
    {
        draw_string(plot->app, plot->face_id, plot->x_axis, V2f32(rect.x0, rect.y1), fcolor_resolve(fcolor_id(defcolor_comment)));
    }
    
    if(plot->y_axis.data)
    {
    draw_string_oriented(plot->app, plot->face_id, fcolor_resolve(fcolor_id(defcolor_comment)), plot->y_axis,
                             V2f32(rect.x0 - 20, rect.y0 + 5), 0, V2f32(0.f, 1.f));
    }

     plot->last_clip = draw_set_clip(plot->app, rect);
    f32 rect_width = rect.x1 - rect.x0;
    f32 rect_height = rect.y1 - rect.y0;
    draw_rectangle(plot->app, rect, 4.f, fcolor_resolve(fcolor_id(defcolor_back)));
    draw_rectangle_outline(plot->app, rect, 4.f, 3.f, fcolor_resolve(fcolor_id(defcolor_pop2)));

    // NOTE(rjf): Draw grid lines.
    {
        ARGB_Color grid_line_color = fcolor_resolve(fcolor_id(defcolor_comment));
        grid_line_color &= 0x00ffffff;
        grid_line_color |= 0x91000000;

        for(int x = (int)plot_view.x0; x <= (int)plot_view.x1; ++x)
        {
            f32 point_x = rect_width * ((float)x - plot_view.x0) / (plot_view.x1 - plot_view.x0);
            Rect_f32 line_rect =
            {
                rect.x0 + point_x,
                rect.y0,
                rect.x0 + point_x + 1,
                rect.y1,
            };
            draw_rectangle(plot->app, line_rect, 1.f, grid_line_color);
        }

        for(int y = (int)plot_view.y0; y <= (int)plot_view.y1; ++y)
        {
            f32 point_y = rect_height * ((float)y - plot_view.y0) / (plot_view.y1 - plot_view.y0);
            Rect_f32 line_rect =
            {
                rect.x0,
                rect.y0 + point_y,
                rect.x1,
                rect.y0 + point_y+1,
            };
            draw_rectangle(plot->app, line_rect, 1.f, grid_line_color);
        }

    }
}

static void
Fleury4Plot2D(PlotData2D *plot, i32 style_flags,
              float *x_data, float *y_data, int data_count)
{
    Rect_f32 rect = plot->screen_rect;
    Rect_f32 plot_view = plot->plot_view;

    f32 rect_width = rect.x1 - rect.x0;
    f32 rect_height = rect.y1 - rect.y0;

    ARGB_Color function_color_cycle[] =
    {
        0xff03d3fc,
        0xff22b80b,
        0xfff0bb0c,
        0xfff0500c,
    };

    // NOTE(rjf): Draw function samples.
    {
        ARGB_Color function_color =
            function_color_cycle[(plot->color_cycle_position++) % ArrayCount(function_color_cycle)];

        for(int i = 0; i < data_count; ++i)
        {
            f32 point_x = rect_width * (x_data[i] - plot->plot_view.x0) / (plot->plot_view.x1 - plot->plot_view.x0);
            f32 point_y = rect_height - rect_height * (y_data[i] - plot->plot_view.y0) / (plot->plot_view.y1 - plot->plot_view.y0);

            if(style_flags & PLOT2D_LINES)
            {
                Rect_f32 point_rect =
                {
                    plot->screen_rect.x0 + point_x - 1,
                    plot->screen_rect.y0 + point_y - 1,
                    plot->screen_rect.x0 + point_x + 1,
                    plot->screen_rect.y0 + point_y + 1,
                };
                
                // TODO(rjf): Real line drawing.
                draw_rectangle(plot->app, point_rect, 1.f, function_color);
            }
            
            if(style_flags & PLOT2D_POINTS)
            {
                Rect_f32 point_rect =
                {
                    plot->screen_rect.x0 + point_x - 4,
                    plot->screen_rect.y0 + point_y - 4,
                    plot->screen_rect.x0 + point_x + 4,
                    plot->screen_rect.y0 + point_y + 4,
                };
                
                draw_rectangle(plot->app, point_rect, 6.f, function_color);
            }
        }
    }

}

static void
Fleury4EndPlot2D(PlotData2D *plot)
{
    draw_set_clip(plot->app, plot->last_clip);
}

//~ NOTE(rjf): Calc Comments

typedef enum CalcTokenType CalcTokenType;
enum CalcTokenType
{
    CALC_TOKEN_TYPE_invalid,
    CALC_TOKEN_TYPE_identifier,
    CALC_TOKEN_TYPE_source_code_identifier,
    CALC_TOKEN_TYPE_number,
    CALC_TOKEN_TYPE_symbol,
    CALC_TOKEN_TYPE_string_constant,
};

typedef struct CalcToken CalcToken;
struct CalcToken
{
    CalcTokenType type;
    char *string;
    int string_length;
};

static CalcToken
Fleury4GetNextCalcToken(char *buffer)
{
    CalcToken token = {0};

    if(buffer)
    {
    for(int i = 0; buffer[i]; ++i)
        {
            if(buffer[i] == '@')
            {
                token.type = CALC_TOKEN_TYPE_source_code_identifier;
                token.string = buffer+i+1;
                int j;
                for(j = i+1; buffer[j] &&
                    (CharIsDigit(buffer[j]) || buffer[j] == '_' ||
                     CharIsAlpha(buffer[j]));
                    ++j);
                token.string_length = j - i;
                break;
            }
            else if(CharIsAlpha(buffer[i]))
            {
                token.type = CALC_TOKEN_TYPE_identifier;
                token.string = buffer+i;
                int j;
                for(j = i+1; buffer[j] &&
                    (CharIsDigit(buffer[j]) || buffer[j] == '_' ||
                     CharIsAlpha(buffer[j]));
                    ++j);
                token.string_length = j - i;
                break;
            }
        else if(CharIsDigit(buffer[i]))
        {
            token.type = CALC_TOKEN_TYPE_number;
            token.string = buffer+i;
            int j;
            for(j = i+1; buffer[j] &&
                (CharIsDigit(buffer[j]) || buffer[j] == '.' ||
                 CharIsAlpha(buffer[j]));
                ++j);
            token.string_length = j - i;
            break;
        }
        else if(CharIsSymbol(buffer[i]))
        {
            token.type = CALC_TOKEN_TYPE_symbol;
            token.string = buffer+i;

            // NOTE(rjf): Assumes 1-length symbols. Might not always be true.
            int j = i+1;
            // for(j = i+1; buffer[j] && CharIsSymbol(buffer[j]); ++j);

            token.string_length = j - i;
            break;
            }
            else if(buffer[i] == '"' || buffer[i] == '\'')
            {
                int starting_char = buffer[i];
                token.type = CALC_TOKEN_TYPE_string_constant;
                token.string = buffer+i;
                int j;
                for(j = i+1; buffer[j] && buffer[j] != starting_char; ++j);
                token.string_length = j - i + 1;
                break;
            }
    }
    }

    return token;
}

static CalcToken
Fleury4NextCalcToken(char **at)
{
    CalcToken token = Fleury4GetNextCalcToken(*at);
    *at = token.string + token.string_length;
    return token;
}

static CalcToken
Fleury4PeekCalcToken(char **at)
{
    CalcToken token = Fleury4GetNextCalcToken(*at);
    return token;
}

static int
Fleury4CalcTokenMatch(CalcToken token, char *str)
{
    int match = 0;

    if(token.string && token.string_length > 0 &&
       token.type != CALC_TOKEN_TYPE_invalid)
    {
        match = 1;
    for(int i = 0; i < token.string_length; ++i)
    {
        if(token.string[i] == str[i])
        {
            if(i == token.string_length-1)
            {
                if(str[i+1] != 0)
                {
                    match = 0;
                    break;
                }
            }
        }
        else
        {
            match = 0;
            break;
        }
        }
    }
    return match;
}

static int
Fleury4RequireCalcToken(char **at, char *str)
{
    int result = 0;
    CalcToken token = Fleury4GetNextCalcToken(*at);
    if(Fleury4CalcTokenMatch(token, str))
    {
        result = 1;
        *at = token.string + token.string_length;
    }
    return result;
}

static int
Fleury4RequireCalcTokenType(char **at, CalcTokenType type, CalcToken *token_ptr)
{
    int result = 0;
    CalcToken token = Fleury4GetNextCalcToken(*at);
    if(token.type == type)
    {
        result = 1;
        *at = token.string + token.string_length;
        if(token_ptr)
        {
            *token_ptr = token;
        }
    }
    return result;
}

static int
Fleury4RequireNewline(char **at)
{
    int result = 0;

    CalcToken next_token = Fleury4PeekCalcToken(at);
    char *newline = 0;
    for(int i = 0; (*at)[i]; ++i)
    {
        if((*at)[i] == '\n')
        {
            newline = (*at)+i;
            break;
        }
    }

    if(newline)
    {
        if(next_token.string > newline)
        {
            result = 1;
        }
    }

    return result;
}

typedef enum CalcNodeType CalcNodeType;
enum CalcNodeType
{
    CALC_NODE_TYPE_invalid,
    CALC_NODE_TYPE_number,
    CALC_NODE_TYPE_string_constant,
    CALC_NODE_TYPE_array,
    CALC_NODE_TYPE_identifier,
    CALC_NODE_TYPE_source_code_identifier,
    CALC_NODE_TYPE_function_call,
    CALC_NODE_TYPE_add,
    CALC_NODE_TYPE_subtract,
    CALC_NODE_TYPE_multiply,
    CALC_NODE_TYPE_divide,
    CALC_NODE_TYPE_raise_to_power,
    CALC_NODE_TYPE_negate,
    CALC_NODE_TYPE_assignment,
};

typedef enum CalcType CalcType;
enum CalcType
{
    CALC_TYPE_error,
    CALC_TYPE_none,
    CALC_TYPE_number,
    CALC_TYPE_array,
    CALC_TYPE_string,
};

static int
Fleury4CalcOperatorPrecedence(CalcNodeType type)
{
    static int precedence_table[] =
    {
        0,
        0,
        0,
        0,
        0,
        0,
        1,
        1,
        2,
        2,
        3,
        0,
        0,
    };
    return precedence_table[type];
}

typedef struct CalcNode CalcNode;
struct CalcNode
{
    CalcNodeType type;
    double value;
    union
    {
        CalcNode *operand;
        CalcNode *left;
    };
    CalcNode *right;
    union
    {
        CalcNode *first_parameter;
        CalcNode *first_member;
    };
    CalcNode *next;
    CalcToken token;
    int num_params;
};

static CalcNode *
AllocateCalcNode(MemoryArena *arena, CalcNodeType type)
{
    CalcNode *node = (CalcNode *)MemoryArenaAllocate(arena, sizeof(*node));
    MemorySet(node, 0, sizeof(*node));
    node->type = type;
    return node;
}

static CalcNode *Fleury4ParseCalcExpression(MemoryArena *arena, char **at_ptr);

static CalcNode *
Fleury4ParseCalcUnaryExpression(MemoryArena *arena, char **at_ptr)
{
    CalcNode *expression = 0;

    CalcToken token = Fleury4PeekCalcToken(at_ptr);

    if(Fleury4CalcTokenMatch(token, "-"))
    {
        Fleury4NextCalcToken(at_ptr);
        expression = AllocateCalcNode(arena, CALC_NODE_TYPE_negate);
        expression->operand = Fleury4ParseCalcUnaryExpression(arena, at_ptr);
    }
    else if(token.type == CALC_TOKEN_TYPE_source_code_identifier)
    {
        Fleury4NextCalcToken(at_ptr);
        expression = AllocateCalcNode(arena, CALC_NODE_TYPE_source_code_identifier);
    }
    else if(token.type == CALC_TOKEN_TYPE_identifier)
    {
        Fleury4NextCalcToken(at_ptr);

        // NOTE(rjf): Function call.
        if(Fleury4RequireCalcToken(at_ptr, "("))
        {
            expression = AllocateCalcNode(arena, CALC_NODE_TYPE_function_call);
            expression->token = token;

            CalcNode **target_param = &expression->first_parameter;
            for(;;)
            {
                CalcToken next_token = Fleury4PeekCalcToken(at_ptr);
                if(next_token.type == CALC_TOKEN_TYPE_invalid ||
                   Fleury4CalcTokenMatch(next_token, ")"))
                {
                    break;
                }

                CalcNode *param = Fleury4ParseCalcExpression(arena, at_ptr);

                if(param)
                {
                *target_param = param;
                    target_param = &(*target_param)->next;
                    Fleury4RequireCalcToken(at_ptr, ",");
                }
                else
                {
                    expression = 0;
                    goto end_parse;
                }
            }

            if(!Fleury4RequireCalcToken(at_ptr, ")"))
            {
                expression = 0;
                goto end_parse;
            }
        }

        // NOTE(rjf): Constant or variable.
        else
        {
                expression = AllocateCalcNode(arena, CALC_NODE_TYPE_identifier);
                expression->token = token;
        }
    }
    else if(Fleury4CalcTokenMatch(token, "("))
    {
        Fleury4NextCalcToken(at_ptr);
        expression = Fleury4ParseCalcExpression(arena, at_ptr);
        Fleury4RequireCalcToken(at_ptr, ")");
    }
    else if(token.type == CALC_TOKEN_TYPE_number)
    {
        Fleury4NextCalcToken(at_ptr);
        expression = AllocateCalcNode(arena, CALC_NODE_TYPE_number);
        expression->value = GetFirstDoubleFromBuffer(token.string);
    }
    else if(token.type == CALC_TOKEN_TYPE_string_constant)
    {
        Fleury4NextCalcToken(at_ptr);
        expression = AllocateCalcNode(arena, CALC_NODE_TYPE_string_constant);
        expression->token = token;
    }
    else if(Fleury4CalcTokenMatch(token, "["))
    {
        Fleury4NextCalcToken(at_ptr);
        
        expression = AllocateCalcNode(arena, CALC_NODE_TYPE_array);
        CalcNode **target_member = &expression->first_member;
        
        for(;;)
        {
            token = Fleury4PeekCalcToken(at_ptr);
            if(Fleury4CalcTokenMatch(token, "]") || token.type == CALC_TOKEN_TYPE_invalid)
            {
                break;
            }
            
            *target_member = Fleury4ParseCalcExpression(arena, at_ptr);
            target_member = &(*target_member)->next;
            
            while(Fleury4RequireCalcToken(at_ptr, ","));
        }
        
        Fleury4RequireCalcToken(at_ptr, "]");
    }
    
    if(Fleury4RequireCalcToken(at_ptr, "^"))
    {
        CalcNode *old_expr = expression;
        expression = AllocateCalcNode(arena, CALC_NODE_TYPE_raise_to_power);
        expression->left = old_expr;
        expression->right = Fleury4ParseCalcUnaryExpression(arena, at_ptr);
    }

    end_parse:;
    return expression;
}

static CalcNodeType
Fleury4GetCalcBinaryOperatorTypeFromToken(CalcToken token)
{
    CalcNodeType type = CALC_NODE_TYPE_invalid;
    switch(token.type)
    {
        case CALC_TOKEN_TYPE_symbol:
        {
            if(token.string[0] == '+')
            {
                type = CALC_NODE_TYPE_add;
            }
            else if(token.string[0] == '-')
            {
                type = CALC_NODE_TYPE_subtract;
            }
            else if(token.string[0] == '*')
            {
                type = CALC_NODE_TYPE_multiply;
            }
            else if(token.string[0] == '/')
            {
                type = CALC_NODE_TYPE_divide;
            }
            break;
        }
        default: break;
    }
    return type;
}

static CalcNode *
Fleury4ParseCalcExpression_(MemoryArena *arena, char **at_ptr, int precedence_in)
{
    CalcNode *expression = Fleury4ParseCalcUnaryExpression(arena, at_ptr);

    if(!expression)
    {
        goto end_parse;
    }

    CalcToken token = Fleury4PeekCalcToken(at_ptr);
    CalcNodeType operator_type = Fleury4GetCalcBinaryOperatorTypeFromToken(token);

    if(token.string && operator_type != CALC_NODE_TYPE_invalid &&
       operator_type != CALC_NODE_TYPE_number)
    {
        for(int precedence = Fleury4CalcOperatorPrecedence(operator_type);
            precedence >= precedence_in;
            --precedence)
        {
            for(;;)
            {
                token = Fleury4PeekCalcToken(at_ptr);

                operator_type = Fleury4GetCalcBinaryOperatorTypeFromToken(token);
                int operator_precedence = Fleury4CalcOperatorPrecedence(operator_type);

                if(operator_precedence != precedence)
                {
                    break;
                }

                if(operator_type == CALC_NODE_TYPE_invalid)
                {
                    break;
                }

                Fleury4NextCalcToken(at_ptr);

                 CalcNode *right = Fleury4ParseCalcExpression_(arena, at_ptr, precedence+1);
                CalcNode *existing_expression = expression;
                expression = AllocateCalcNode(arena, operator_type);
                expression->type = operator_type;
                expression->left = existing_expression;
                expression->right = right;

                if(!right)
                {
                    goto end_parse;
                }
            }
        }
    }

    end_parse:;
    return expression;
}

static CalcNode *
Fleury4ParseCalcExpression(MemoryArena *arena, char **at_ptr)
{
    return Fleury4ParseCalcExpression_(arena, at_ptr, 1);
}

static CalcNode *
Fleury4ParseCalcCode(MemoryArena *arena, char **at_ptr)
{
    CalcNode *root = 0;
    CalcNode **target = &root;

    for(;;)
    {
        CalcToken token = Fleury4PeekCalcToken(at_ptr);

        // NOTE(rjf): Parse assignment.
        if(token.type == CALC_TOKEN_TYPE_identifier)
        {
            char *at_reset = *at_ptr;
            Fleury4NextCalcToken(at_ptr);

            // NOTE(rjf): Variable assignment
            if(Fleury4RequireCalcToken(at_ptr, "="))
            {
                CalcNode *identifier = AllocateCalcNode(arena, CALC_NODE_TYPE_identifier);
                identifier->token = token;

                CalcNode *assignment = AllocateCalcNode(arena, CALC_NODE_TYPE_assignment);
                assignment->left = identifier;
                assignment->right = Fleury4ParseCalcExpression(arena, at_ptr);

                if(assignment == 0)
                {
                    break;
                }

                *target = assignment;
                target = &(*target)->next;
                goto end_parse;
            }
            else
            {
                *at_ptr = at_reset;
            }
        }

        // NOTE(rjf): Parse expression.
        {
            CalcNode *expression = Fleury4ParseCalcExpression(arena, at_ptr);
            if(expression == 0)
            {
                break;
            }
            *target = expression;
            target = &(*target)->next;
            goto end_parse;
        }

        end_parse:;

        if(!Fleury4RequireCalcToken(at_ptr, ";") && !Fleury4RequireNewline(at_ptr))
        {
            break;
        }
    }

    return root;
}

typedef struct CalcSymbolKey CalcSymbolKey;
struct CalcSymbolKey
{
    char *string;
    int string_length;
};

typedef struct CalcSymbolValue CalcSymbolValue;
struct CalcSymbolValue
{
    CalcNode *node;
};

typedef struct CalcSymbolTable CalcSymbolTable;
struct CalcSymbolTable
{
    unsigned int size;
    CalcSymbolKey *keys;
    CalcSymbolValue *values;
};

typedef struct CalcInterpretGraph CalcInterpretGraph;
struct CalcInterpretGraph
{
    CalcNode *graph_expression;
    CalcNode *input_value;
    CalcNode *parent_call;
    char *plot_title;
    int plot_title_length;
    char *x_axis;
    int x_axis_length;
    char *y_axis;
    int y_axis_length;
    Rect_f32 plot_view;
    int num_function_samples;
    CalcInterpretGraph *next;
};

typedef struct CalcValue CalcValue;
struct CalcValue
{
    union
    {
        struct
        {
    int string_length;
            char *as_string;
        };
        
        struct
        {
    int array_count;
            CalcValue *as_array;
        };
        
        struct
        {
            char *as_error;
        };
        
        struct
        {
            double as_f64;
        };
    };
    
    CalcType type;
};

static CalcValue
CalcValueNone(void)
{
    CalcValue calc_value = {0};
    calc_value.type = CALC_TYPE_none;
    return calc_value;
}

static CalcValue
CalcValueF64(double num)
{
    CalcValue val = {0};
    val.type = CALC_TYPE_number;
    val.as_f64 = num;
        return val;
}

static CalcValue
CalcValueError(char *string)
{
    CalcValue val = {0};
    val.type = CALC_TYPE_error;
    val.as_error = string;
        return val;
}

static CalcValue
CalcValueString(char *string, int string_length)
{
    CalcValue val = {0};
    val.type = CALC_TYPE_string;
    val.as_string = string;
    val.string_length = string_length;
    return val;
}

typedef struct CalcInterpretResult CalcInterpretResult;
struct CalcInterpretResult
{
    CalcValue value;
    CalcInterpretGraph *first_graph;
};

typedef struct CalcInterpretContext CalcInterpretContext;
struct CalcInterpretContext
{
    MemoryArena *arena;
    CalcSymbolTable *symbol_table;
    char *plot_title;
    int plot_title_length;
    char *x_axis;
    int x_axis_length;
    char *y_axis;
    int y_axis_length;
    f32 x_low;
    f32 x_high;
    f32 y_low;
    f32 y_high;
    int num_function_samples;
};

// NOTE(rjf): WHY DOESN'T C++ ALLOW DECLARING THINGS IN THE ORDER I WANT THIS SUCKS SO BAD
static CalcInterpretResult
Fleury4InterpretCalcExpression(CalcInterpretContext *context, CalcNode *root);

static CalcValue
CalcValueArray(CalcInterpretContext *context, CalcNode *first_member)
{
    CalcValue val = {0};
    val.type = CALC_TYPE_array;
    
    CalcType array_type = CALC_TYPE_none;
    // NOTE(rjf): WHY DOESN'T C++ ALLOW IMPLICIT POINTER CASTING THIS SUCKS SO BAD
    CalcValue *array = (CalcValue *)MemoryArenaAllocate(context->arena, 0);
    int count = 0;
    
    for(CalcNode *member = first_member; member; member = member->next)
    {
        CalcInterpretResult result = Fleury4InterpretCalcExpression(context, member);
        if(member == first_member)
        {
            array_type = result.value.type;
            if(array_type == CALC_TYPE_error)
            {
                val = result.value;
                goto end_create;
            }
            else if(array_type == CALC_TYPE_none)
            {
                val = CalcValueError("Cannot make arrays of 'none' type.");
                goto end_create;
            }
        }
        else
        {
            if(result.value.type != array_type)
            {
                val = CalcValueError("Cannot have multiple types in an array.");
                goto end_create;
            }
        }
        
        MemoryArenaAllocate(context->arena, sizeof(*array));
        array[count] = result.value;
        
        ++count;
    }
    
    if(array && count)
    {
        val.as_array = array;
        val.array_count = count;
    }
    
    end_create:;
    return val;
}

static CalcSymbolTable
CalcSymbolTableInit(MemoryArena *arena, unsigned int size)
{
    CalcSymbolTable table = {0};
    table.size = size;
    table.keys = (CalcSymbolKey *)MemoryArenaAllocate(arena, sizeof(*table.keys)*size);
    table.values = (CalcSymbolValue *)MemoryArenaAllocate(arena, sizeof(*table.values)*size);
    MemorySet(table.keys, 0, sizeof(*table.keys)*size);
    MemorySet(table.values, 0, sizeof(*table.values)*size);
    return table;
}

static CalcSymbolValue *
CalcSymbolTableLookup_(CalcSymbolTable *table, char *string, int length)
{
    CalcSymbolValue *result = 0;

    unsigned int hash = StringCRC32(string, length) % table->size;
    unsigned int original_hash = hash;

    CalcSymbolValue *value = 0;

    for(;;)
    {
        if(table->keys[hash].string)
        {
            if(StringMatchCaseSensitive(table->keys[hash].string, table->keys[hash].string_length,
                                        string, length))
            {
                value = table->values + hash;
                break;
            }
            else
            {
                if(++hash >= table->size)
                {
                    hash = 0;
                }
                if(hash == original_hash)
                {
                    break;
                }
            }
        }
        else
        {
            break;
        }
    }

    if(value)
    {
        result = value;
    }

    return result;
}

static CalcNode *
CalcSymbolTableLookup(CalcSymbolTable *table, char *string, int string_length)
{
    CalcNode *result = 0;
    CalcSymbolValue *value = CalcSymbolTableLookup_(table, string, string_length);
        if(value)
    {
        result = value->node;
    }
    return result;
}

static void
CalcSymbolTableAdd(CalcSymbolTable *table, char *string, int string_length, CalcNode *node)
{
    unsigned int hash = StringCRC32(string, string_length) % table->size;
    unsigned int original_hash = hash;
    int found = 0;

    for(;;)
    {
        if(table->keys[hash].string)
        {
            if(StringMatchCaseSensitive(table->keys[hash].string, table->keys[hash].string_length,
                                        string, string_length))
            {
                found = 1;
                break;
            }
            else
            {
                if(++hash >= table->size)
                {
                    hash = 0;
                }
                if(hash == original_hash)
                {
                    break;
                }
            }
        }
        else
        {
            found = 1;
            break;
        }
    }

    if(found)
    {
        table->keys[hash].string = string;
        table->keys[hash].string_length = string_length;
        table->values[hash].node = node;
    }
}

static void
Fleury4GraphCalcExpression(Application_Links *app, Face_ID face_id,
                           Rect_f32 rect, CalcInterpretGraph *first_graph,
                           CalcInterpretContext *context)
{
    CalcNode *parent_call = first_graph->parent_call;
    Rect_f32 plot_view = first_graph->plot_view;
    
    PlotData2D plot_data = {0};
    {
        plot_data.app = app;
        plot_data.face_id = face_id;
        plot_data.title  = { first_graph->plot_title, (u64)first_graph->plot_title_length };
        plot_data.x_axis = { first_graph->x_axis, (u64)first_graph->x_axis_length };
        plot_data.y_axis = { first_graph->y_axis, (u64)first_graph->y_axis_length };
        plot_data.screen_rect = rect;
        plot_data.plot_view = plot_view;
    }
    Fleury4BeginPlot2D(&plot_data);
    
    for(CalcInterpretGraph *graph = first_graph; graph && graph->parent_call == parent_call;
        graph = graph->next)
    {
        CalcNode *expression = graph->graph_expression;
        CalcInterpretResult expression_result = Fleury4InterpretCalcExpression(context, expression);
        
        if(expression_result.value.type == CALC_TYPE_array)
        {
            if(expression_result.value.array_count > 0 &&
               expression_result.value.as_array[0].type == CALC_TYPE_number)
            {
            int values_to_plot = expression_result.value.array_count;
            float *x_values = (float *)MemoryArenaAllocate(context->arena, sizeof(float)*values_to_plot);
            float *y_values = (float *)MemoryArenaAllocate(context->arena, sizeof(float)*values_to_plot);
            
            for(int i = 0; i < values_to_plot; ++i)
            {
                x_values[i] = (float)i;
                    y_values[i] = (float)expression_result.value.as_array[i].as_f64;
            }
            
                Fleury4Plot2D(&plot_data, PLOT2D_POINTS, x_values, y_values, values_to_plot);
            }
        }
        else
        {
     CalcNode *value = graph->input_value;
    CalcNode value_node = {0};
    {
        value_node.type = CALC_NODE_TYPE_number;
    }
        
    if(value)
    {
            CalcSymbolTableAdd(context->symbol_table, value->token.string,
                               value->token.string_length, &value_node);
    }

    // NOTE(rjf): Find function sample points.
    int values_to_plot = graph->num_function_samples;
        float *x_values = (float *)MemoryArenaAllocate(context->arena, values_to_plot * sizeof(*x_values));
        float *y_values = (float *)MemoryArenaAllocate(context->arena, values_to_plot * sizeof(*y_values));
    {
    for(int i = 0; i < values_to_plot; ++i)
    {
                value_node.value = plot_view.x0 + (i / (float)values_to_plot) * (plot_view.x1 - plot_view.x0);
        CalcInterpretResult result = Fleury4InterpretCalcExpression(context, expression);
                    if(result.value.type != CALC_TYPE_number)
        {
            goto end_graph;
        }
        else
        {
            x_values[i] = (float)value_node.value;
            y_values[i] = (float)result.value.as_f64;
        }
    }
    }

        Fleury4Plot2D(&plot_data, PLOT2D_LINES, x_values, y_values, values_to_plot);
        }
        
    end_graph:;
    }

    Fleury4EndPlot2D(&plot_data);
}

typedef struct CalcFindInputResult CalcFindInputResult;
struct CalcFindInputResult
{
    CalcNode *unknown;
    int number_unknowns;
};

static CalcFindInputResult
Fleury4FindUnknownForGraph(CalcSymbolTable *table, CalcNode *expression)
{
    CalcFindInputResult result = {0};

    if(expression && expression->type != CALC_NODE_TYPE_invalid)
    {
        if(expression->type == CALC_NODE_TYPE_identifier)
        {
            CalcSymbolValue *symbol_value =
                CalcSymbolTableLookup_(table, expression->token.string,
                                                                   expression->token.string_length);

            if(!symbol_value)
            {
            result.unknown = expression;
                ++result.number_unknowns;
            }
        }
        else
        {
            CalcFindInputResult results[] =
            {
                Fleury4FindUnknownForGraph(table, expression->left),
                Fleury4FindUnknownForGraph(table, expression->right),
                Fleury4FindUnknownForGraph(table, expression->first_parameter),
                Fleury4FindUnknownForGraph(table, expression->next),
            };

            for(int i = 0; i < ArrayCount(results); ++i)
            {
                if(results[i].unknown)
                {
                if(!result.unknown)
                {
                    result.unknown = results[i].unknown;
                    ++result.number_unknowns;
                }
                else
                {
                    if(!StringMatchCaseSensitive(results[i].unknown->token.string, results[i].unknown->token.string_length,
                                                 result.unknown->token.string, result.unknown->token.string_length))
                    {
                        ++result.number_unknowns;
                    }
                }
            }
            }
        }
    }

    return result;
}

static CalcInterpretResult
Fleury4InterpretCalcExpression(CalcInterpretContext *context, CalcNode *root)
{
    CalcInterpretResult result = {0};

    if(root == 0)
    {
        result.value = CalcValueError("something went wrong");
    }
    else
    {
        switch(root->type)
        {
            case CALC_NODE_TYPE_number:
            {
                result.value = CalcValueF64(root->value);
                break;
            }
            
            case CALC_NODE_TYPE_array:
            {
                result.value = CalcValueArray(context, root->first_member);
                break;
            }
            
            case CALC_NODE_TYPE_string_constant:
            {
                result.value = CalcValueString(root->token.string, root->token.string_length);
                break;
            }

            case CALC_NODE_TYPE_add:
            case CALC_NODE_TYPE_subtract:
            case CALC_NODE_TYPE_multiply:
            case CALC_NODE_TYPE_divide:
            case CALC_NODE_TYPE_raise_to_power:
            {
                CalcInterpretResult left_result = Fleury4InterpretCalcExpression(context, root->left);
                CalcInterpretResult right_result = Fleury4InterpretCalcExpression(context, root->right);
                if(left_result.value.type == CALC_TYPE_error ||
                   right_result.value.type == CALC_TYPE_error)
                {
                    result.value = CalcValueError(left_result.value.type == CALC_TYPE_error ? left_result.value.as_error : right_result.value.as_error);
                    goto end_interpret;
                }

                else if(left_result.value.type != CALC_TYPE_number ||
                        right_result.value.type != CALC_TYPE_number)
                {
                    result.value = CalcValueError("Cannot use non-numbers in expressions.");
                    goto end_interpret;
                }

                switch(root->type)
                {
                    case CALC_NODE_TYPE_add:            result.value = CalcValueF64(left_result.value.as_f64 + right_result.value.as_f64); break;
                    case CALC_NODE_TYPE_subtract:       result.value = CalcValueF64(left_result.value.as_f64 - right_result.value.as_f64); break;
                    case CALC_NODE_TYPE_multiply:       result.value = CalcValueF64(left_result.value.as_f64 * right_result.value.as_f64); break;
                    case CALC_NODE_TYPE_divide:
                    {
                        if(right_result.value.as_f64 == 0)
                        {
                            result.value = CalcValueF64(NAN);
                        }
                        else
                        {
                            result.value = CalcValueF64(left_result.value.as_f64 / right_result.value.as_f64);
                        }
                        break;
                    }
                    case CALC_NODE_TYPE_raise_to_power:
                    {
                        result.value = CalcValueF64(pow(left_result.value.as_f64, right_result.value.as_f64));
                        break;
                    }
                }

                break;
            }

            case CALC_NODE_TYPE_negate:
            {
                result = Fleury4InterpretCalcExpression(context, root->operand);
                if(result.value.type == CALC_TYPE_number)
                {
                    result.value = CalcValueF64(-result.value.as_f64);
                }
                break;
            }

            case CALC_NODE_TYPE_function_call:
            {

                //~ NOTE(rjf): Built-in functions.
                
                // NOTE(rjf): Functions that take a single number and return a single number.
                
                if(Fleury4CalcTokenMatch(root->token, "sin") ||
                   Fleury4CalcTokenMatch(root->token, "cos") ||
                   Fleury4CalcTokenMatch(root->token, "tan") ||
                   Fleury4CalcTokenMatch(root->token, "abs"))
                {
                    typedef double NumToNumProc(double);
                    
                    struct
                    {
                        char *name;
                         NumToNumProc *proc;
                    }
                    functions[] =
                    {
                        { "sin", sin },
                        { "cos", cos },
                        { "tan", tan },
                        { "abs", fabs },
                    };
                    
                    for(int i = 0; i < ArrayCount(functions); ++i)
                    {
                        if(Fleury4CalcTokenMatch(root->token, functions[i].name))
                        {
                        CalcInterpretResult arg = Fleury4InterpretCalcExpression(context, root->first_parameter);
                    if(arg.value.type == CALC_TYPE_error)
                    {
                        result.value = arg.value;
                    }
                    else if(arg.value.type != CALC_TYPE_number)
                    {
                        result.value = CalcValueError("Function requires number.");
                    }
                    else
                    {
                                result.value = CalcValueF64(functions[i].proc(arg.value.as_f64));
                            }
                            break;
                        }
                    }
                }
                
                //~ NOTE(rjf): Plots.

                else if(Fleury4CalcTokenMatch(root->token, "plot_title"))
                {
                    result.value = CalcValueNone();

                    if(root->first_parameter)
                    {
                        CalcInterpretResult title = Fleury4InterpretCalcExpression(context, root->first_parameter);

                        if(title.value.type == CALC_TYPE_string)
                        {
                            context->plot_title = title.value.as_string + 1;
                            context->plot_title_length = title.value.string_length - 2;
                        }
                        else
                        {
                            result.value = CalcValueError("plot_title expects a string.");
                            break;
                        }
                    }
                    else
                    {
                        result.value = CalcValueError("plot_title expects a string.");
                        break;
                    }
                }
                
                else if(Fleury4CalcTokenMatch(root->token, "plot_function_samples"))
                {
                    result.value = CalcValueNone();
                    
                    if(root->first_parameter)
                    {
                        CalcInterpretResult num = Fleury4InterpretCalcExpression(context, root->first_parameter);
                        
                        if(num.value.type == CALC_TYPE_number)
                        {
                            context->num_function_samples = (int)num.value.as_f64;
                        }
                        else
                        {
                            result.value = CalcValueError("plot_function_samples expects a number.");
                            break;
                        }
                    }
                    else
                    {
                        result.value = CalcValueError("plot_function_samples expects a number.");
                        break;
                    }
                }
                
                else if(Fleury4CalcTokenMatch(root->token, "plot_xaxis") ||
                        Fleury4CalcTokenMatch(root->token, "plot_yaxis"))
                {
                    int is_y_axis = Fleury4CalcTokenMatch(root->token, "plot_yaxis");
                    
                    result.value = CalcValueNone();
                    
                    CalcNode *title_param = 0;
                    CalcNode *low_param = 0;
                    CalcNode *high_param = 0;
                    
                    CalcInterpretResult title_result = {0};
                    CalcInterpretResult low_result = {0};
                    CalcInterpretResult high_result = {0};
                    
                    for(CalcNode *param = root->first_parameter;
                        param; param = param->next)
                    {
                        CalcInterpretResult interpret =
                            Fleury4InterpretCalcExpression(context, param);
                        
                        if(interpret.value.type == CALC_TYPE_string)
                        {
                            if(title_param)
                            {
                                result.value = CalcValueError(is_y_axis
                                                              ? "plot_yaxis only accepts one string."
                                                              : "plot_xaxis only accepts one string.");
                                goto end_interpret;
                            }
                            else
                            {
                                title_param = param;
                                title_result = interpret;
                            }
                        }
                        else if(interpret.value.type == CALC_TYPE_number)
                        {
                            if(low_param)
                            {
                                if(high_param)
                                {
                                    result.value = CalcValueError(is_y_axis
                                                                  ? "plot_yaxis only accepts two numbers."
                                                                  : "plot_xaxis only accepts two numbers.");
                                    
                                    goto end_interpret;
                                }
                                else
                                {
                                    high_param = param;
                                    high_result = interpret;
                                }
                            }
                            else
                            {
                                low_param = param;
                                low_result = interpret;
                            }
                        }
                        else
                        {
                            result = interpret;
                            break;
                        }
                    }
                    
                    if(low_param && high_param)
                    {
                        if(is_y_axis)
                        {
                            if(title_result.value.as_string)
                            {
                                context->y_axis = title_result.value.as_string + 1;
                                context->y_axis_length = title_result.value.string_length - 2;
                            }
                            else
                            {
                                context->y_axis = 0;
                                context->y_axis_length = 0;
                            }
                            
                            context->y_low = (f32)low_result.value.as_f64;
                            context->y_high = (f32)high_result.value.as_f64;
                        }
                        else
                        {
                            if(title_result.value.as_string)
                            {
                                context->x_axis = title_result.value.as_string + 1;
                                context->x_axis_length = title_result.value.string_length - 2;
                            }
                            else
                            {
                                context->x_axis = 0;
                                context->x_axis_length = 0;
                            }
                            
                            context->x_low = (f32)low_result.value.as_f64;
                            context->x_high = (f32)high_result.value.as_f64;
                        }
                    }
                    else
                    {
                        result.value = CalcValueError(is_y_axis
                                                      ? "plot_yaxis needs two bounds (title optional)."
                                                      : "plot_xaxis needs two bounds (title optional).");
                    }
                }

                else if(Fleury4CalcTokenMatch(root->token, "plot"))
                {
                    result.value = CalcValueNone();

                    CalcInterpretGraph **target = &result.first_graph;
                    for(CalcNode *graph_expression = root->first_parameter;
                        graph_expression; graph_expression = graph_expression->next)
                    {
                        CalcFindInputResult input_find = Fleury4FindUnknownForGraph(context->symbol_table,
                                                                                    graph_expression);
                    if(input_find.number_unknowns <= 1)
                    {
                         CalcNode *input_variable = input_find.unknown;
                        CalcInterpretGraph *new_graph =
                            (CalcInterpretGraph *)MemoryArenaAllocate(context->arena, sizeof(*new_graph));
                        new_graph->graph_expression = graph_expression;
                        new_graph->input_value = input_variable;
                        new_graph->next = 0;
                            new_graph->parent_call = root;
                            new_graph->plot_title = context->plot_title;
                            new_graph->plot_title_length = context->plot_title_length;
                            new_graph->x_axis = context->x_axis;
                            new_graph->x_axis_length = context->x_axis_length;
                            new_graph->y_axis = context->y_axis;
                            new_graph->y_axis_length = context->y_axis_length;
                            new_graph->plot_view =
                                Rf32(context->x_low, context->y_low,
                                     context->x_high, context->y_high);
                            new_graph->num_function_samples =
                                context->num_function_samples;
                            *target = new_graph;
                            target = &(*target)->next;
                    }
                    else
                        {
                            result.value = CalcValueError("Too many unknowns in graphing expression.");
                            break;
                        }
                    }
                }

                // NOTE(rjf): Non-built-in functions.
                else
                {
                    result.value = CalcValueError("Unrecognized function.");
                }

                break;
            }

            case CALC_NODE_TYPE_identifier:
            {
                if(Fleury4CalcTokenMatch(root->token, "e"))
                {
                    result.value = CalcValueF64(2.71828);
                }
                else if(Fleury4CalcTokenMatch(root->token, "pi"))
                {
                    result.value = CalcValueF64(3.1415926535897);
                }
                else
                {
                    CalcNode *value = CalcSymbolTableLookup(context->symbol_table, root->token.string, root->token.string_length);
                    result = Fleury4InterpretCalcExpression(context, value);
                }

                break;
            }

            default: break;
        }
    }
    
    end_interpret:;
    return result;
}

static int
Fleury4IdentifierExistsInCalcExpression(CalcNode *root, char *string, int string_length)
{
    int result = 0;

    if(root && root->type != CALC_NODE_TYPE_invalid)
    {
        if(StringMatchCaseSensitive(root->token.string, root->token.string_length, string, string_length))
        {
            result = 1;
        }
        else
        {
        result |= Fleury4IdentifierExistsInCalcExpression(root->left, string, string_length);
            result |= Fleury4IdentifierExistsInCalcExpression(root->right, string, string_length);
            result |= Fleury4IdentifierExistsInCalcExpression(root->first_parameter, string, string_length);
            result |= Fleury4IdentifierExistsInCalcExpression(root->next, string, string_length);
    }
    }

    return result;
}

static CalcInterpretResult
Fleury4InterpretCalcCode(CalcInterpretContext *context, CalcNode *tree_root)
{
    CalcInterpretResult result = {0};
    CalcInterpretResult last_result = result;

    for(CalcNode *root = tree_root; root; root = root->next)
    {
        last_result = result;
        
        if(root->type == CALC_NODE_TYPE_assignment)
        {
            if(root->left->type == CALC_NODE_TYPE_identifier)
            {
                if(!Fleury4IdentifierExistsInCalcExpression(root->right, root->left->token.string, root->left->token.string_length))
                {
                    CalcSymbolTableAdd(context->symbol_table, root->left->token.string,
                                       root->left->token.string_length, root->right);
                    result = Fleury4InterpretCalcExpression(context, root->right);
                    result.first_graph = last_result.first_graph;
                }
                else
                {
                    result.value = CalcValueError("Recursive definition.");
                    result.first_graph = last_result.first_graph;
                }
            }
            else
            {
                result.value = CalcValueError("Assignment to non-identifier.");
                result.first_graph = last_result.first_graph;
            }
        }
        else
        {
            result = Fleury4InterpretCalcExpression(context, root);
            if(last_result.first_graph)
            {
                for(CalcInterpretGraph *graph = last_result.first_graph; graph; graph = graph->next)
                {
                    if(graph->next == 0)
                    {
                        graph->next = result.first_graph;
                        break;
                    }
                }

                result.first_graph = last_result.first_graph;
            }
            else if(result.value.type == CALC_TYPE_error)
            {
                break;
            }
        }
    }

    return result;
}

static void
Fleury4RenderCalcComments(Application_Links *app, Buffer_ID buffer, View_ID view,
                             Text_Layout_ID text_layout_id)
{
    Token_Array token_array = get_token_array_from_buffer(app, buffer);
    Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
    Scratch_Block scratch(app);

    if(token_array.tokens != 0)
    {
        static char arena_buffer[64*1024*1024];
        MemoryArena arena = MemoryArenaInit(arena_buffer, sizeof(arena_buffer));
        CalcSymbolTable symbol_table = CalcSymbolTableInit(&arena, 1024);

        CalcInterpretContext context_ = {0};
        CalcInterpretContext *context = &context_;
        context->arena = &arena;
        context->symbol_table = &symbol_table;
        context->num_function_samples = 128;
        
        i64 first_index = token_index_from_pos(&token_array, visible_range.first);
        Token_Iterator_Array it = token_iterator_index(0, &token_array, first_index);

        Token *token = 0;
        for(;;)
        {
            token = token_it_read(&it);

            if(token->pos >= visible_range.one_past_last || !token || !token_it_inc_non_whitespace(&it))
            {
                break;
            }

            if(token->kind == TokenBaseKind_Comment)
            {
                Rect_f32 comment_first_char_rect =
                    text_layout_character_on_screen(app, text_layout_id, token->pos);

                Rect_f32 comment_last_char_rect =
                    text_layout_character_on_screen(app, text_layout_id, token->pos + token->size - 1);

                Range_i64 token_range =
                {
                    token->pos,
                    token->pos + (token->size > 1024
                                  ? 1024
                                  : token->size),
                };

                u32 token_buffer_size = (u32)(token_range.end - token_range.start);
                if(token_buffer_size < 4)
                {
                    token_buffer_size = 4;
                }
                u8 *token_buffer = (u8 *)MemoryArenaAllocate(&arena, token_buffer_size+1);
                buffer_read_range(app, buffer, token_range, token_buffer);
                token_buffer[token_buffer_size] = 0;

                    if((token_buffer[0] == '/' && token_buffer[1] == '/' && token_buffer[2] == 'c' &&
                        token_buffer[3] <= 32) ||
                       (token_buffer[0] == '/' && token_buffer[1] == '*' && token_buffer[2] == 'c'))

                {
                    int is_multiline_comment = (token_buffer[1] == '*');
                    if(is_multiline_comment)
                    {
                        if(token_buffer[token_buffer_size-1] == '/' &&
                           token_buffer[token_buffer_size-2] == '*')
                        {
                            token_buffer[token_buffer_size-2] = 0;
                        }
                    }

                    char *at = (char *)token_buffer + 3;
                    CalcNode *expr = Fleury4ParseCalcCode(&arena, &at);
                     CalcInterpretResult result = Fleury4InterpretCalcCode(context, expr);

                    char result_buffer[256] = {0};
                    String_Const_u8 result_string =
                    {
                        (u8 *)result_buffer,
                    };

                    switch(result.value.type)
                    {
                        case CALC_TYPE_error:
                        {
                            if(expr == 0)
                            {
                                result_string.size = (u64)snprintf(result_buffer, sizeof(result_buffer),
                                                                   "= (syntax error: \'parse failed\')");
                            }
                            else
                            {
                            result_string.size = (u64)snprintf(result_buffer, sizeof(result_buffer),
                                                                   "= (syntax error: \'%s\')", result.value.as_error);
                            }
                            break;
                        }
                        case CALC_TYPE_number:
                        {
                            result_string.size = (u64)snprintf(result_buffer, sizeof(result_buffer),
                                                               "= %f", result.value.as_f64);
                            break;
                        }
                        case CALC_TYPE_string:
                        {
                            result_string.size = (u64)snprintf(result_buffer, sizeof(result_buffer),
                                                               "= '%.*s'", result.value.string_length, result.value.as_string);
                            break;
                        }
                        default: break;
                    }

                    Vec2_f32 point =
                    {
                        comment_last_char_rect.x1 + 20,
                        comment_first_char_rect.y0,
                    };

                    u32 color = finalize_color(defcolor_comment, 0);
                    color &= 0x00ffffff;
                    color |= 0x80000000;
                    draw_string(app, get_face_id(app, buffer), result_string, point, color);

                    Rect_f32 view_rect = view_get_screen_rect(app, view);

                    Rect_f32 graph_rect = {0};
                    {
                        graph_rect.x0 = view_rect.x1 - 30 - 300;
                        graph_rect.y0 = comment_first_char_rect.y0;
                        graph_rect.x1 = graph_rect.x0 + 300;
                        graph_rect.y1 = graph_rect.y0 + 200;
                    }

                    CalcNode *last_parent_call = 0;
                    for(CalcInterpretGraph *graph = result.first_graph; graph;
                        graph = graph->next)
                    {
                        if(last_parent_call == 0 || graph->parent_call != last_parent_call)
                        {
                            Fleury4GraphCalcExpression(app, get_face_id(app, buffer), graph_rect, graph, context);

                        // NOTE(rjf): Bump graph rect forward.
                        {
                            f32 rect_height = graph_rect.y1 - graph_rect.y0;
                            graph_rect.y0 += rect_height + 50;
                        graph_rect.y1 += rect_height + 50;
                            }

                            last_parent_call = graph->parent_call;
                        }
                    }

                }

            }

        }

    }

}

//~ NOTE(rjf): Plotting comments.

static void
Fleury4RenderPlotComments(Application_Links *app, Buffer_ID buffer, View_ID view,
                             Text_Layout_ID text_layout_id)
{
    String_Const_u8 plot_comment_signifier =
    {
        "//p",
        0,
    };
    for(; plot_comment_signifier.str[plot_comment_signifier.size];
        ++plot_comment_signifier.size);
    
    Token_Array token_array = get_token_array_from_buffer(app, buffer);
    Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
    Scratch_Block scratch(app);
    
    static char arena_buffer[32*1024*1024];
    MemoryArena arena = MemoryArenaInit(arena_buffer, sizeof(arena_buffer));
    
    if(token_array.tokens != 0)
    {
        i64 first_index = token_index_from_pos(&token_array, visible_range.first);
        Token_Iterator_Array it = token_iterator_index(0, &token_array, first_index);
        
        Token *token = 0;
        for(;;)
        {
            token = token_it_read(&it);
            
            if(token->pos >= visible_range.one_past_last || !token || !token_it_inc_non_whitespace(&it))
            {
                break;
            }
            
            if(token->kind == TokenBaseKind_Comment)
            {
                Rect_f32 comment_last_char_rect =
                    text_layout_character_on_screen(app, text_layout_id, token->pos + token->size - 1);
                
                String_Const_u8 token_string;
                {
                Range_i64 token_range =
                {
                    token->pos,
                    token->pos + (token->size > (i64)plot_comment_signifier.size
                                  ? (i64)plot_comment_signifier.size
                                  : token->size),
                };
                
                u8 token_buffer[256] = {0};
                buffer_read_range(app, buffer, token_range, token_buffer);
                token_string = { token_buffer, (u64)(token_range.end - token_range.start) };
                }
                
                if(string_match(token_string, plot_comment_signifier))
                {
                     float *data = (float *)MemoryArenaAllocate(&arena, 0);
                    int data_count = 0;
                    
                    Rect_f32 plot_view = { -1, -1, 1, 1 };
                    
                    for(;;)
                    {
                        token = token_it_read(&it);
                        if(token->kind == TokenBaseKind_ScopeOpen)
                        {
                            for(;;)
                            {
                                token = token_it_read(&it);
                                if(token->kind == TokenBaseKind_ScopeClose ||
                                        token->kind == TokenBaseKind_EOF ||
                                        token->kind == TokenBaseKind_LexError)
                                {
                                    break;
                                }
                                else if(token->kind == TokenBaseKind_LiteralFloat ||
                                        token->kind == TokenBaseKind_LiteralInteger)
                                {
                                    Range_i64 token_range =
                                    {
                                        token->pos,
                                        token->pos + (token->size > 256 ? 256 : token->size),
                                    };
                                    
                                    u8 token_buffer[256];
                                    buffer_read_range(app, buffer, token_range, token_buffer);
                                    
                                    MemoryArenaAllocate(&arena, sizeof(data[0]));
                                    data[data_count++] = (float)GetFirstDoubleFromBuffer((char *)token_buffer);
                                }
                                token_it_inc_non_whitespace(&it);
                            }
                            break;
                        }
                        else if(token->kind == TokenBaseKind_ScopeClose ||
                                token->kind == TokenBaseKind_EOF ||
                                token->kind == TokenBaseKind_LexError)
                        {
                            break;
                        }
                        token_it_inc_non_whitespace(&it);
                    }
                    
                     float *x_data = (float *)MemoryArenaAllocate(&arena, sizeof(*x_data)*data_count);
                    for(int i = 0; i < data_count; ++i)
                    {
                        x_data[i] = (float)i;
                        
                        if(data[i] < plot_view.y0)
                        {
                            plot_view.y0 = data[i];
                        }
                        else if(data[i] > plot_view.y1)
                        {
                            plot_view.y1 = data[i];
                        }
                        
                        if(x_data[i] < plot_view.x0)
                        {
                            plot_view.x0 = x_data[i];
                        }
                        else if(x_data[i] > plot_view.x1)
                        {
                            plot_view.x1 = x_data[i];
                        }
                    }
                    
                    // NOTE(rjf): Plot.
                    {
                        PlotData2D plot_data = {0};
                        {
                            float screen_width = 300;
                            float screen_height = 200;
                            
                            plot_data.app = app;
                            plot_data.face_id = get_face_id(app, view);
                            
                            plot_data.screen_rect.x1 = view_get_screen_rect(app, view).x1 - 30;
                            plot_data.screen_rect.x0 = plot_data.screen_rect.x1 - screen_width;
                            plot_data.screen_rect.y0 = comment_last_char_rect.y0;
                            plot_data.screen_rect.y1 = plot_data.screen_rect.y0 + screen_height;
                            plot_data.plot_view = plot_view;
                        }
                        
                        Fleury4BeginPlot2D(&plot_data);
                        Fleury4Plot2D(&plot_data, PLOT2D_POINTS, x_data, data, data_count);
                        Fleury4EndPlot2D(&plot_data);
                    }
                    
                }
                
            }
            
        }
        
    }
    
}


//~ NOTE(rjf): C/C++ Token Highlighting

static ARGB_Color
Fleury4GetCTokenColor(Token token)
{
     ARGB_Color color = ARGBFromID(defcolor_text_default);

    switch(token.kind)
    {
        case TokenBaseKind_Preprocessor:     color = ARGBFromID(defcolor_preproc); break;
        case TokenBaseKind_Keyword:          color = ARGBFromID(defcolor_keyword); break;
        case TokenBaseKind_Comment:          color = ARGBFromID(defcolor_comment); break;
        case TokenBaseKind_LiteralString:    color = ARGBFromID(defcolor_str_constant); break;
        case TokenBaseKind_LiteralInteger:   color = ARGBFromID(defcolor_int_constant); break;
        case TokenBaseKind_LiteralFloat:     color = ARGBFromID(defcolor_float_constant); break;
        case TokenBaseKind_Operator:         color = ARGBFromID(defcolor_preproc); break;

        case TokenBaseKind_ScopeOpen:
        case TokenBaseKind_ScopeClose:
        case TokenBaseKind_ParentheticalOpen:
        case TokenBaseKind_ParentheticalClose:
        case TokenBaseKind_StatementClose:
        {
            u32 r = (color & 0x00ff0000) >> 16;
            u32 g = (color & 0x0000ff00) >>  8;
            u32 b = (color & 0x000000ff) >>  0;

            if(global_dark_mode)
            {
                r = (r * 3) / 5;
                g = (g * 3) / 5;
                b = (b * 3) / 5;
            }
            else
            {
                r = (r * 4) / 3;
                g = (g * 4) / 3;
                b = (b * 4) / 3;
            }

            color = 0xff000000 | (r << 16) | (g << 8) | (b << 0);

            break;
        }

        default:
        {
            switch(token.sub_kind)
            {
                case TokenCppKind_LiteralTrue:
                case TokenCppKind_LiteralFalse:
                {
                    color = ARGBFromID(defcolor_bool_constant);
                    break;
                }
                case TokenCppKind_LiteralCharacter:
                case TokenCppKind_LiteralCharacterWide:
                case TokenCppKind_LiteralCharacterUTF8:
                case TokenCppKind_LiteralCharacterUTF16:
                case TokenCppKind_LiteralCharacterUTF32:
                {
                    color = ARGBFromID(defcolor_char_constant);
                    break;
                }
                case TokenCppKind_PPIncludeFile:
                {
                    color = ARGBFromID(defcolor_include);
                    break;
                }
            }
            break;
        }
    }

    return color;
}

static void
Fleury4DrawCTokenColors(Application_Links *app, Text_Layout_ID text_layout_id, Token_Array *array)
{
    Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
    i64 first_index = token_index_from_pos(array, visible_range.first);
    Token_Iterator_Array it = token_iterator_index(0, array, first_index);

    for(;;)
    {
        Token *token = token_it_read(&it);
        if(token->pos >= visible_range.one_past_last)
        {
            break;
        }
        ARGB_Color argb = Fleury4GetCTokenColor(*token);
        paint_text_color(app, text_layout_id, Ii64_size(token->pos, token->size), argb);
        if(!token_it_inc_all(&it))
        {
            break;
        }
    }
}

//~ NOTE(rjf): Buffer Render

static void
Fleury4RenderBuffer(Application_Links *app, View_ID view_id, Face_ID face_id,
                      Buffer_ID buffer, Text_Layout_ID text_layout_id,
                    Rect_f32 rect, Frame_Info frame_info)
{
    ProfileScope(app, "[Fleury] Render Buffer");

    View_ID active_view = get_active_view(app, Access_Always);
    b32 is_active_view = (active_view == view_id);
    Rect_f32 prev_clip = draw_set_clip(app, rect);
    
    // NOTE(allen): Token colorizing
    Token_Array token_array = get_token_array_from_buffer(app, buffer);
    if(token_array.tokens != 0)
    {
        Fleury4DrawCTokenColors(app, text_layout_id, &token_array);

        // NOTE(allen): Scan for TODOs and NOTEs
        if(global_config.use_comment_keyword)
        {
            char user_string_buf[256] = {0};
            String_Const_u8 user_string = {0};
            {
                user_string.data = user_string_buf;
                user_string.size = snprintf(user_string_buf, sizeof(user_string_buf), "(%.*s)",
                                            string_expand(global_config.user_name));
            }

            Comment_Highlight_Pair pairs[] =
            {
                {string_u8_litexpr("NOTE"), finalize_color(defcolor_comment_pop, 0)},
                {string_u8_litexpr("TODO"), finalize_color(defcolor_comment_pop, 1)},
                {user_string, 0xffffdd23},
            };
            draw_comment_highlights(app, buffer, text_layout_id,
                                    &token_array, pairs, ArrayCount(pairs));
        }
    }
    else
    {
        Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
        paint_text_color_fcolor(app, text_layout_id, visible_range, fcolor_id(defcolor_text_default));
    }

    i64 cursor_pos = view_correct_cursor(app, view_id);
    view_correct_mark(app, view_id);

    // NOTE(allen): Scope highlight
    if(global_config.use_scope_highlight)
    {
        Color_Array colors = finalize_color_array(defcolor_back_cycle);
        draw_scope_highlight(app, buffer, text_layout_id, cursor_pos, colors.vals, colors.count);
    }

    // NOTE(rjf): Brace highlight
    {
        ARGB_Color colors[] =
        {
            0xff8ffff2,
        };

        Fleury4RenderBraceHighlight(app, buffer, text_layout_id, cursor_pos,
                             colors, sizeof(colors)/sizeof(colors[0]));
    }

    if(global_config.use_error_highlight || global_config.use_jump_highlight)
    {
        // NOTE(allen): Error highlight
        String_Const_u8 name = string_u8_litexpr("*compilation*");
        Buffer_ID compilation_buffer = get_buffer_by_name(app, name, Access_Always);
        if(global_config.use_error_highlight)
        {
            draw_jump_highlights(app, buffer, text_layout_id, compilation_buffer,
                                 fcolor_id(defcolor_highlight_junk));
        }

        // NOTE(allen): Search highlight
        if(global_config.use_jump_highlight)
        {
            Buffer_ID jump_buffer = get_locked_jump_buffer(app);
            if(jump_buffer != compilation_buffer)
            {
                draw_jump_highlights(app, buffer, text_layout_id, jump_buffer,
                                     fcolor_id(defcolor_highlight_white));
            }
        }
    }

    // NOTE(allen): Color parens
    if(global_config.use_paren_helper)
    {
        Color_Array colors = finalize_color_array(defcolor_text_cycle);
        draw_paren_highlight(app, buffer, text_layout_id, cursor_pos, colors.vals, colors.count);
    }

    // NOTE(allen): Line highlight
    if(global_config.highlight_line_at_cursor && is_active_view)
    {
        i64 line_number = get_line_number_from_pos(app, buffer, cursor_pos);
        draw_line_highlight(app, text_layout_id, line_number,
                            fcolor_id(defcolor_highlight_cursor_line));
    }
    
    // NOTE(rjf): Divider Comments
    {
        Fleury4RenderDividerComments(app, buffer, view_id, text_layout_id);
    }
    
    // NOTE(allen): Cursor shape
    Face_Metrics metrics = get_face_metrics(app, face_id);
    f32 cursor_roundness = (metrics.normal_advance*0.5f)*0.9f;
    f32 mark_thickness = 2.f;

    // NOTE(allen): Cursor
    switch (fcoder_mode)
    {
        case FCoderMode_Original:
        {
            Fleury4RenderCursor(app, view_id, is_active_view, buffer, text_layout_id, cursor_roundness, mark_thickness, frame_info);
            break;
        }

        case FCoderMode_NotepadLike:
        {
            draw_notepad_style_cursor_highlight(app, view_id, buffer, text_layout_id, cursor_roundness);
            break;
        }
    }

    // NOTE(rjf): Brace annotations
    {
        Fleury4RenderCloseBraceAnnotation(app, buffer, text_layout_id, cursor_pos);
    }

    // NOTE(rjf): Brace lines
    {
        Fleury4RenderBraceLines(app, buffer, view_id, text_layout_id, cursor_pos);
    }

    // NOTE(allen): put the actual text on the actual screen
    draw_text_layout_default(app, text_layout_id);
    
    // NOTE(rjf): Draw code peek
    if(global_code_peek_open)
    {
        Fleury4RenderRangeHighlight(app, view_id, text_layout_id, global_code_peek_token_range);
        Fleury4RenderCodePeek(app, view_id, face_id, buffer, frame_info);
    }
    
    // NOTE(rjf): Draw calc comments.
    {
        Fleury4RenderCalcComments(app, buffer, view_id, text_layout_id);
    }
    
    // NOTE(rjf): Draw plot comments.
    {
        Fleury4RenderPlotComments(app, buffer, view_id, text_layout_id);
    }
    
    // NOTE(rjf): Draw power mode.
    {
        Fleury4RenderPowerMode(app, view_id, face_id, frame_info);
    }

    draw_set_clip(app, prev_clip);
}

//~ NOTE(rjf): Render hook

static void
Fleury4Render(Application_Links *app, Frame_Info frame_info, View_ID view_id)
{
    ProfileScope(app, "[Fleury] Render");
    View_ID active_view = get_active_view(app, Access_Always);
    b32 is_active_view = (active_view == view_id);

    Rect_f32 region = draw_background_and_margin(app, view_id, is_active_view);
    Rect_f32 prev_clip = draw_set_clip(app, region);

    Buffer_ID buffer = view_get_buffer(app, view_id, Access_Always);
    Face_ID face_id = get_face_id(app, buffer);
    Face_Metrics face_metrics = get_face_metrics(app, face_id);
    f32 line_height = face_metrics.line_height;
    f32 digit_advance = face_metrics.decimal_digit_advance;

    // NOTE(allen): file bar
    b64 showing_file_bar = false;
    if(view_get_setting(app, view_id, ViewSetting_ShowFileBar, &showing_file_bar) && showing_file_bar)
    {
        Rect_f32_Pair pair = layout_file_bar_on_top(region, line_height);
        draw_file_bar(app, view_id, buffer, face_id, pair.min);
        region = pair.max;
    }

    Buffer_Scroll scroll = view_get_buffer_scroll(app, view_id);

    Buffer_Point_Delta_Result delta = delta_apply(app, view_id, frame_info.animation_dt, scroll);

    if(!block_match_struct(&scroll.position, &delta.point))
    {
        block_copy_struct(&scroll.position, &delta.point);
        view_set_buffer_scroll(app, view_id, scroll, SetBufferScroll_NoCursorChange);
    }

    if(delta.still_animating)
    {
        animate_in_n_milliseconds(app, 0);
    }

    // NOTE(allen): query bars
    {
        Query_Bar *space[32];
        Query_Bar_Ptr_Array query_bars = {};
        query_bars.ptrs = space;
        if (get_active_query_bars(app, view_id, ArrayCount(space), &query_bars))
        {
            for (i32 i = 0; i < query_bars.count; i += 1)
            {
                Rect_f32_Pair pair = layout_query_bar_on_top(region, line_height, 1);
                draw_query_bar(app, query_bars.ptrs[i], face_id, pair.min);
                region = pair.max;
            }
        }
    }

    // NOTE(allen): FPS hud
    if(show_fps_hud)
    {
        Rect_f32_Pair pair = layout_fps_hud_on_bottom(region, line_height);
        draw_fps_hud(app, frame_info, face_id, pair.max);
        region = pair.min;
animate_in_n_milliseconds(app, 1000);
    }

    // NOTE(allen): layout line numbers
    Rect_f32 line_number_rect = {};
    if(global_config.show_line_number_margins)
    {
        Rect_f32_Pair pair = layout_line_number_margin(app, buffer, region, digit_advance);
        line_number_rect = pair.min;
        region = pair.max;
    }

    // NOTE(allen): begin buffer render
    Buffer_Point buffer_point = scroll.position;
    if(is_active_view)
    {
    buffer_point.pixel_shift.y += global_power_mode.screen_shake*1.f;
        global_power_mode.screen_shake -= global_power_mode.screen_shake * frame_info.animation_dt * 12.f;
    }
    Text_Layout_ID text_layout_id = text_layout_create(app, buffer, region, buffer_point);

    // NOTE(allen): draw line numbers
    if(global_config.show_line_number_margins)
    {
        draw_line_number_margin(app, view_id, buffer, face_id, text_layout_id, line_number_rect);
    }

    // NOTE(allen): draw the buffer
    Fleury4RenderBuffer(app, view_id, face_id, buffer, text_layout_id, region, frame_info);

    text_layout_free(app, text_layout_id);
    draw_set_clip(app, prev_clip);
}

//~ NOTE(rjf): Bindings

static void
Fleury4SetBindings(Mapping *mapping)
{
    MappingScope();
    SelectMapping(mapping);

    SelectMap(mapid_global);
    BindCore(default_startup, CoreCode_Startup);
    BindCore(default_try_exit, CoreCode_TryExit);
    Bind(keyboard_macro_start_recording , KeyCode_U, KeyCode_Control);
    Bind(keyboard_macro_finish_recording, KeyCode_U, KeyCode_Control, KeyCode_Shift);
    Bind(keyboard_macro_replay,           KeyCode_U, KeyCode_Alt);
    Bind(change_active_panel,           KeyCode_Comma, KeyCode_Control);
    Bind(change_active_panel_backwards, KeyCode_Comma, KeyCode_Control, KeyCode_Shift);
    Bind(interactive_new,               KeyCode_N, KeyCode_Control);
    Bind(interactive_open_or_new,       KeyCode_O, KeyCode_Control);
    Bind(open_in_other,                 KeyCode_O, KeyCode_Alt);
    Bind(interactive_kill_buffer,       KeyCode_K, KeyCode_Control);
    Bind(interactive_switch_buffer,     KeyCode_I, KeyCode_Control);
    Bind(project_go_to_root_directory,  KeyCode_H, KeyCode_Control);
    Bind(save_all_dirty_buffers,        KeyCode_S, KeyCode_Control, KeyCode_Shift);
    Bind(change_to_build_panel,         KeyCode_Period, KeyCode_Alt);
    Bind(close_build_panel,             KeyCode_Comma, KeyCode_Alt);
    Bind(goto_next_jump,                KeyCode_N, KeyCode_Alt);
    Bind(goto_prev_jump,                KeyCode_N, KeyCode_Alt, KeyCode_Shift);
    Bind(build_in_build_panel,          KeyCode_M, KeyCode_Alt);
    Bind(goto_first_jump,               KeyCode_M, KeyCode_Alt, KeyCode_Shift);
    Bind(toggle_filebar,                KeyCode_B, KeyCode_Alt);
    Bind(execute_any_cli,               KeyCode_Z, KeyCode_Alt);
    Bind(execute_previous_cli,          KeyCode_Z, KeyCode_Alt, KeyCode_Shift);
    Bind(command_lister,                KeyCode_X, KeyCode_Alt);
    Bind(project_command_lister,        KeyCode_X, KeyCode_Alt, KeyCode_Shift);
    Bind(list_all_functions_current_buffer, KeyCode_I, KeyCode_Control, KeyCode_Shift);
    Bind(project_fkey_command, KeyCode_F1);
    Bind(project_fkey_command, KeyCode_F2);
    Bind(project_fkey_command, KeyCode_F3);
    Bind(project_fkey_command, KeyCode_F4);
    Bind(project_fkey_command, KeyCode_F5);
    Bind(project_fkey_command, KeyCode_F6);
    Bind(project_fkey_command, KeyCode_F7);
    Bind(project_fkey_command, KeyCode_F8);
    Bind(project_fkey_command, KeyCode_F9);
    Bind(project_fkey_command, KeyCode_F10);
    Bind(project_fkey_command, KeyCode_F11);
    Bind(project_fkey_command, KeyCode_F12);
    Bind(project_fkey_command, KeyCode_F13);
    Bind(project_fkey_command, KeyCode_F14);
    Bind(project_fkey_command, KeyCode_F15);
    Bind(project_fkey_command, KeyCode_F16);
    Bind(exit_4coder,          KeyCode_F4, KeyCode_Alt);
    BindMouseWheel(mouse_wheel_scroll);
    BindMouseWheel(mouse_wheel_change_face_size, KeyCode_Control);

    // NOTE(rjf): Custom bindings.
    {
        Bind(open_panel_vsplit, KeyCode_P, KeyCode_Control);
        Bind(open_panel_hsplit, KeyCode_Minus, KeyCode_Control);
        Bind(close_panel, KeyCode_P, KeyCode_Control, KeyCode_Shift);
        Bind(fleury_toggle_colors, KeyCode_Tick, KeyCode_Control);
        Bind(fleury_toggle_power_mode, KeyCode_P, KeyCode_Alt);
    }

    SelectMap(mapid_file);
    ParentMap(mapid_global);
    BindTextInput(fleury_write_text_input);
    BindMouse(click_set_cursor_and_mark, MouseCode_Left);
    BindMouseRelease(click_set_cursor, MouseCode_Left);
    BindCore(click_set_cursor_and_mark, CoreCode_ClickActivateView);
    BindMouseMove(click_set_cursor_if_lbutton);
    Bind(delete_char,            KeyCode_Delete);
    Bind(backspace_char,         KeyCode_Backspace);
    Bind(move_up,                KeyCode_Up);
    Bind(move_down,              KeyCode_Down);
    Bind(move_left,              KeyCode_Left);
    Bind(move_right,             KeyCode_Right);
    Bind(seek_end_of_line,       KeyCode_End);
    Bind(fleury_home,            KeyCode_Home);
    Bind(page_up,                KeyCode_PageUp);
    Bind(page_down,              KeyCode_PageDown);
    Bind(goto_beginning_of_file, KeyCode_PageUp, KeyCode_Control);
    Bind(goto_end_of_file,       KeyCode_PageDown, KeyCode_Control);
    Bind(move_up_to_blank_line_end,        KeyCode_Up, KeyCode_Control);
    Bind(move_down_to_blank_line_end,      KeyCode_Down, KeyCode_Control);
    Bind(move_left_whitespace_boundary,    KeyCode_Left, KeyCode_Control);
    Bind(move_right_whitespace_boundary,   KeyCode_Right, KeyCode_Control);
    Bind(move_line_up,                     KeyCode_Up, KeyCode_Alt);
    Bind(move_line_down,                   KeyCode_Down, KeyCode_Alt);
    Bind(backspace_alpha_numeric_boundary, KeyCode_Backspace, KeyCode_Control);
    Bind(delete_alpha_numeric_boundary,    KeyCode_Delete, KeyCode_Control);
    Bind(snipe_backward_whitespace_or_token_boundary, KeyCode_Backspace, KeyCode_Alt);
    Bind(snipe_forward_whitespace_or_token_boundary,  KeyCode_Delete, KeyCode_Alt);
    Bind(set_mark,                    KeyCode_Space, KeyCode_Control);
    Bind(replace_in_range,            KeyCode_A, KeyCode_Control);
    Bind(copy,                        KeyCode_C, KeyCode_Control);
    Bind(delete_range,                KeyCode_D, KeyCode_Control);
    Bind(delete_line,                 KeyCode_D, KeyCode_Control, KeyCode_Shift);
    Bind(center_view,                 KeyCode_E, KeyCode_Control);
    Bind(left_adjust_view,            KeyCode_E, KeyCode_Control, KeyCode_Shift);
    Bind(search,                      KeyCode_F, KeyCode_Control);
    Bind(list_all_locations,          KeyCode_F, KeyCode_Control, KeyCode_Shift);
    Bind(list_all_substring_locations_case_insensitive, KeyCode_F, KeyCode_Alt);
    Bind(goto_line,                   KeyCode_G, KeyCode_Control);
    Bind(list_all_locations_of_selection,  KeyCode_G, KeyCode_Control, KeyCode_Shift);
    Bind(snippet_lister,              KeyCode_J, KeyCode_Control);
    Bind(kill_buffer,                 KeyCode_K, KeyCode_Control, KeyCode_Shift);
    Bind(duplicate_line,              KeyCode_L, KeyCode_Control);
    Bind(cursor_mark_swap,            KeyCode_M, KeyCode_Control);
    Bind(reopen,                      KeyCode_O, KeyCode_Control, KeyCode_Shift);
    Bind(query_replace,               KeyCode_Q, KeyCode_Control);
    Bind(query_replace_identifier,    KeyCode_Q, KeyCode_Control, KeyCode_Shift);
    Bind(query_replace_selection,     KeyCode_Q, KeyCode_Alt);
    Bind(reverse_search,              KeyCode_R, KeyCode_Control);
    Bind(save,                        KeyCode_S, KeyCode_Control);
    Bind(save_all_dirty_buffers,      KeyCode_S, KeyCode_Control, KeyCode_Shift);
    Bind(search_identifier,           KeyCode_T, KeyCode_Control);
    Bind(list_all_locations_of_identifier, KeyCode_T, KeyCode_Control, KeyCode_Shift);
    Bind(paste_and_indent,            KeyCode_V, KeyCode_Control);
    Bind(paste_next_and_indent,       KeyCode_V, KeyCode_Control, KeyCode_Shift);
    Bind(cut,                         KeyCode_X, KeyCode_Control);
    Bind(redo,                        KeyCode_Y, KeyCode_Control);
    Bind(undo,                        KeyCode_Z, KeyCode_Control);
    Bind(view_buffer_other_panel,     KeyCode_1, KeyCode_Control);
    Bind(swap_panels,                 KeyCode_2, KeyCode_Control);
    Bind(if_read_only_goto_position,  KeyCode_Return);
    Bind(if_read_only_goto_position_same_panel, KeyCode_Return, KeyCode_Shift);
    Bind(view_jump_list_with_lister,  KeyCode_Period, KeyCode_Control, KeyCode_Shift);

    SelectMap(mapid_code);
    ParentMap(mapid_file);
    BindTextInput(fleury_write_text_and_auto_indent);
    Bind(move_left_alpha_numeric_boundary,           KeyCode_Left, KeyCode_Control);
    Bind(move_right_alpha_numeric_boundary,          KeyCode_Right, KeyCode_Control);
    Bind(move_left_alpha_numeric_or_camel_boundary,  KeyCode_Left, KeyCode_Alt);
    Bind(move_right_alpha_numeric_or_camel_boundary, KeyCode_Right, KeyCode_Alt);
    Bind(comment_line_toggle,        KeyCode_Semicolon, KeyCode_Control);
    Bind(word_complete,              KeyCode_Tab);
    Bind(auto_indent_range,          KeyCode_Tab, KeyCode_Control);
    Bind(auto_indent_line_at_cursor, KeyCode_Tab, KeyCode_Shift);
    Bind(word_complete_drop_down,    KeyCode_Tab, KeyCode_Shift, KeyCode_Control);
    Bind(write_block,                KeyCode_R, KeyCode_Alt);
    Bind(write_todo,                 KeyCode_T, KeyCode_Alt);
    Bind(write_note,                 KeyCode_Y, KeyCode_Alt);
    Bind(list_all_locations_of_type_definition,               KeyCode_D, KeyCode_Alt);
    Bind(list_all_locations_of_type_definition_of_identifier, KeyCode_T, KeyCode_Alt, KeyCode_Shift);
    Bind(open_long_braces,           KeyCode_LeftBracket, KeyCode_Control);
    Bind(open_long_braces_semicolon, KeyCode_LeftBracket, KeyCode_Control, KeyCode_Shift);
    Bind(open_long_braces_break,     KeyCode_RightBracket, KeyCode_Control, KeyCode_Shift);
    Bind(select_surrounding_scope,   KeyCode_LeftBracket, KeyCode_Alt);
    Bind(select_surrounding_scope_maximal, KeyCode_LeftBracket, KeyCode_Alt, KeyCode_Shift);
    Bind(select_prev_scope_absolute, KeyCode_RightBracket, KeyCode_Alt);
    Bind(select_prev_top_most_scope, KeyCode_RightBracket, KeyCode_Alt, KeyCode_Shift);
    Bind(select_next_scope_absolute, KeyCode_Quote, KeyCode_Alt);
    Bind(select_next_scope_after_current, KeyCode_Quote, KeyCode_Alt, KeyCode_Shift);
    Bind(place_in_scope,             KeyCode_ForwardSlash, KeyCode_Alt);
    Bind(delete_current_scope,       KeyCode_Minus, KeyCode_Alt);
    Bind(if0_off,                    KeyCode_I, KeyCode_Alt);
    Bind(open_file_in_quotes,        KeyCode_1, KeyCode_Alt);
    Bind(open_matching_file_cpp,     KeyCode_2, KeyCode_Alt);

    // NOTE(rjf): Custom bindings.
    {
        Bind(fleury_code_peek,          KeyCode_Alt, KeyCode_Control);
        Bind(fleury_close_code_peek,    KeyCode_Escape);
        Bind(fleury_code_peek_go,       KeyCode_Return, KeyCode_Control);
        Bind(fleury_write_zero_struct,  KeyCode_0, KeyCode_Control);
    }

}

//~ NOTE(rjf): Begin buffer hook

BUFFER_HOOK_SIG(Fleury4BeginBuffer)
{
    ProfileScope(app, "[Fleury] Begin Buffer");

    Scratch_Block scratch(app);
    b32 treat_as_code = false;
    String_Const_u8 file_name = push_buffer_file_name(app, scratch, buffer_id);

    if(file_name.size > 0)
    {
        String_Const_u8_Array extensions = global_config.code_exts;
        String_Const_u8 ext = string_file_extension(file_name);

        for(i32 i = 0; i < extensions.count; ++i)
        {
            if(string_match(ext, extensions.strings[i]))
            {
                treat_as_code = true;
                break;
            }
        }
    }

    Command_Map_ID map_id = (treat_as_code) ? (mapid_code) : (mapid_file);
    Managed_Scope scope = buffer_get_managed_scope(app, buffer_id);
    Command_Map_ID *map_id_ptr = scope_attachment(app, scope, buffer_map_id, Command_Map_ID);
    *map_id_ptr = map_id;

    Line_Ending_Kind setting = guess_line_ending_kind_from_buffer(app, buffer_id);
    Line_Ending_Kind *eol_setting = scope_attachment(app, scope, buffer_eol_setting, Line_Ending_Kind);
    *eol_setting = setting;

    // NOTE(allen): Decide buffer settings
    b32 wrap_lines = true;
    b32 use_virtual_whitespace = false;
    b32 use_lexer = false;
    if(treat_as_code)
    {
        wrap_lines = global_config.enable_code_wrapping;
        use_virtual_whitespace = global_config.enable_virtual_whitespace;
        use_lexer = true;
    }

    String_Const_u8 buffer_name = push_buffer_base_name(app, scratch, buffer_id);
    if(string_match(buffer_name, string_u8_litexpr("*compilation*")))
    {
        wrap_lines = false;
    }

    if(use_lexer)
    {
        ProfileBlock(app, "begin buffer kick off lexer");
        Async_Task *lex_task_ptr = scope_attachment(app, scope, buffer_lex_task, Async_Task);
        *lex_task_ptr = async_task_no_dep(&global_async_system, do_full_lex_async, make_data_struct(&buffer_id));
    }

    {
        b32 *wrap_lines_ptr = scope_attachment(app, scope, buffer_wrap_lines, b32);
        *wrap_lines_ptr = wrap_lines;
    }

    if (use_virtual_whitespace)
    {
        if (use_lexer)
        {
            buffer_set_layout(app, buffer_id, layout_virt_indent_index_generic);
        }
        else
        {
            buffer_set_layout(app, buffer_id, layout_virt_indent_literal_generic);
        }
    }
    else
{
        buffer_set_layout(app, buffer_id, layout_generic);
    }

    // no meaning for return
    return(0);
}


//~ NOTE(rjf): Layout

 static Layout_Item_List
Fleury4Layout(Application_Links *app, Arena *arena, Buffer_ID buffer, Range_i64 range, Face_ID face, f32 width)
{
    return(layout_unwrapped__inner(app, arena, buffer, range, face, width, LayoutVirtualIndent_Off));
}


//~ NOTE(rjf): CRC32

static unsigned int
CRC32(unsigned char *buf, int len)
{
    static unsigned int init = 0xffffffff;
    static const unsigned int crc32_table[] =
    {
        0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9,
        0x130476dc, 0x17c56b6b, 0x1a864db2, 0x1e475005,
        0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61,
        0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd,
        0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9,
        0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
        0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011,
        0x791d4014, 0x7ddc5da3, 0x709f7b7a, 0x745e66cd,
        0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
        0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5,
        0xbe2b5b58, 0xbaea46ef, 0xb7a96036, 0xb3687d81,
        0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
        0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49,
        0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95,
        0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1,
        0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d,
        0x34867077, 0x30476dc0, 0x3d044b19, 0x39c556ae,
        0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
        0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16,
        0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca,
        0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde,
        0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02,
        0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1, 0x53dc6066,
        0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
        0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e,
        0xbfa1b04b, 0xbb60adfc, 0xb6238b25, 0xb2e29692,
        0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6,
        0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a,
        0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e,
        0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
        0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686,
        0xd5b88683, 0xd1799b34, 0xdc3abded, 0xd8fba05a,
        0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637,
        0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb,
        0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f,
        0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
        0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47,
        0x36194d42, 0x32d850f5, 0x3f9b762c, 0x3b5a6b9b,
        0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
        0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623,
        0xf12f560e, 0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7,
        0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
        0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f,
        0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3,
        0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7,
        0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b,
        0x9b3660c6, 0x9ff77d71, 0x92b45ba8, 0x9675461f,
        0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
        0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640,
        0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c,
        0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8,
        0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24,
        0x119b4be9, 0x155a565e, 0x18197087, 0x1cd86d30,
        0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
        0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088,
        0x2497d08d, 0x2056cd3a, 0x2d15ebe3, 0x29d4f654,
        0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0,
        0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c,
        0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18,
        0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
        0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0,
        0x9abc8bd5, 0x9e7d9662, 0x933eb0bb, 0x97ffad0c,
        0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668,
        0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4
    };

    unsigned int crc = init;
    while(len--)
    {
        crc = (crc << 8) ^ crc32_table[((crc >> 24) ^ *buf) & 255];
        buf++;
    }
    return crc;
}

static unsigned int
StringCRC32(char *string, int string_length)
{
    unsigned int hash = CRC32((unsigned char *)string, string_length);
    return hash;
}

static unsigned int
CStringCRC32(char *string)
{
    int string_length = (int)CalculateCStringLength(string);
    unsigned int hash = CRC32((unsigned char *)string, string_length);
    return hash;
}