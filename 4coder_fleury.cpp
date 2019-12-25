#include <stdlib.h>
#include <string.h>
#include "4coder_default_include.cpp"

#pragma warning(disable : 4706)

#include "4coder_fleury_utilities.cpp"
#include "4coder_fleury_ubiquitous.cpp"
#include "4coder_fleury_power_mode.cpp"
#include "4coder_fleury_smooth_cursor.cpp"
#include "4coder_fleury_code_peek.cpp"
#include "4coder_fleury_theme.cpp"
#include "4coder_fleury_bindings.cpp"
#include "4coder_fleury_brace.cpp"
#include "4coder_fleury_divider_comments.cpp"
#include "4coder_fleury_plot.cpp"
#include "4coder_fleury_calc.cpp"

/*c

plot_title('Plotting Data from C')
plot_xaxis('x', 0, 100)
plot_yaxis('y', 0, 20)
plot(@global_data, @global_data_2)






plot_title('Histogram')
plot_bin_count(5)
plot_bin_range(-5, 8)
plot_histogram(@global_data)








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

static float global_data_2[] =
{
    8.33189567f,  -2.63085084f,  -1.70883036f, -11.45459984f,
    5.16516515f,   0.27479074f,   7.36537573f,  -8.3279556f,
    -7.20506426f,  -4.78523683f,  -8.25147516f, -21.46084777f,
    19.50750363f,  -8.19224498f,  19.64859427f,   1.85584205f,
    -9.66196526f, -14.29339641f, -12.24208799f,   9.84488434f,
    3.55224796f,  -5.70598278f,   3.52293099f, -11.82275176f,
    -10.0538094f,  -0.3517224f,  13.09577244f,  -3.85530767f,
    -3.72255288f,  -4.72034f, -12.38330269f,  -6.83119644f,
    14.76931653f,  -8.44951628f,   1.89200127f, -11.57629748f,
    -24.55712341f,  -4.04348426f,  -9.33964521f,   2.45067903f,
    10.42798898f,  -1.48445553f, -23.35501647f,   1.03586129f,
    5.57883049f,   2.88323133f,   8.08523847f,  13.43119261f,
    6.12743226f,  12.5069471f,  -5.01980105f,  -5.08733503f,
    -6.88671343f, -16.10255044f,   9.87614158f,   6.1601377f,
    -2.35138122f,  -3.32922715f, -18.41436281f,  -0.15824601f,
    -0.97863953f,  -0.7112648f,  -1.30292791f,  -8.97661803f,
    -8.9839007f,   0.30024656f,  -1.43306306f,   7.54202387f,
    -9.36800044f,   8.00122332f,  -3.54725315f,  -8.10226232f,
    -21.22822363f,   1.4205526f,  -2.90602587f,   7.47372829f,
    -1.64917945f,  -3.95035333f,   8.33805499f,   7.79639271f,
    24.38806177f,   5.65251007f,  -0.23218516f,  13.18871065f,
    -13.42918445f,   6.65685942f,   2.07580543f,   4.36966305f,
    -23.84257483f,   1.53002473f,  -5.14524116f,  -1.13887405f,
    -2.71712798f,  13.43325044f, -16.40354991f,   0.48594043f,
    0.02957235f,  17.95939959f,   2.43141756f,  14.91776534f,
};


/*c

plot_function_samples(100)

plot_title('Plotting Data')
 dat = [ [ 0 1 2 3 4 5 6 7 8 9 ] [ 0 1 2 3 4 5 6 7 8 9 ] ]
plot_xaxis('x', 0, 10)
plot_yaxis('y', 0, 10)
plot(dat)

plot_title('Plotting Functions #1')
plot_xaxis('x', -0.25, 1.25)
plot_yaxis('y', -0.25, 1.25)
plot(-2*x^3 + 3*x^2, -x^2, -x, 2*x)

plot_title('Plotting Functions #2')
plot_xaxis('x', -2, 2)
plot_yaxis('y', -3, 3)
plot(1/x, x^2, -sin(x), cos(4*x))
*/



































//~ NOTE(rjf): Hooks
static i32  Fleury4BeginBuffer(Application_Links *app, Buffer_ID buffer_id);
static void Fleury4Render(Application_Links *app, Frame_Info frame_info, View_ID view_id);
static Layout_Item_List Fleury4Layout(Application_Links *app, Arena *arena, Buffer_ID buffer, Range_i64 range, Face_ID face, f32 width);


//~ NOTE(rjf): Hook Helpers
static void Fleury4RenderBuffer(Application_Links *app, View_ID view_id, Face_ID face_id, Buffer_ID buffer, Text_Layout_ID text_layout_id, Rect_f32 rect, Frame_Info frame_info);
static void Fleury4RenderRangeHighlight(Application_Links *app, View_ID view_id, Text_Layout_ID text_layout_id, Range_i64 range);


//~ NOTE(rjf): Commands

CUSTOM_COMMAND_SIG(fleury_write_text_input)
CUSTOM_DOC("Inserts whatever text was used to trigger this command.")
{
    write_text_input(app);
    Fleury4SpawnPowerModeParticles(app, get_active_view(app, Access_ReadWriteVisible));
}

CUSTOM_COMMAND_SIG(fleury_write_text_and_auto_indent)
CUSTOM_DOC("Inserts text and auto-indents the line on which the cursor sits if any of the text contains 'layout punctuation' such as ;:{}()[]# and new lines.")
{
    write_text_and_auto_indent(app);
    Fleury4SpawnPowerModeParticles(app, get_active_view(app, Access_ReadWriteVisible));
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
