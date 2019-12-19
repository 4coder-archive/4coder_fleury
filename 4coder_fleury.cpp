#include "4coder_default_include.cpp"

static void Fleury4LightMode(Application_Links *app);
static void Fleury4DarkMode(Application_Links *app);
static void Fleury4DrawCTokenColors(Application_Links *app, Text_Layout_ID text_layout_id, Token_Array *array);
static void Fleury4Render(Application_Links *app, Frame_Info frame_info, View_ID view_id);
static void Fleury4RenderBuffer(Application_Links *app, View_ID view_id, Face_ID face_id, Buffer_ID buffer, Text_Layout_ID text_layout_id, Rect_f32 rect, Frame_Info frame_info);
static void Fleury4SetBindings(Mapping *mapping);
static void Fleury4OpenCodePeek(Application_Links *app, String_Const_u8 base_needle, String_Match_Flag must_have_flags, String_Match_Flag must_not_have_flags);
static void Fleury4CloseCodePeek(void);
static void Fleury4NextCodePeek(void);
static void Fleury4CodePeekGo(Application_Links *app);
BUFFER_HOOK_SIG(Fleury4BeginBuffer);

static b32 global_dark_mode = 1;
static Vec2_f32 global_smooth_cursor_position = {0};
static b32 global_code_peek_open = 0;
static int global_code_peek_match_count = 0;
String_Match global_code_peek_matches[16] = {0};
static int global_code_peek_selected_index = -1;
static f32 global_code_peek_open_transition = 0.f;
static Range_i64 global_code_peek_token_range;

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
        set_custom_hook(app, HookID_RenderCaller, Fleury4Render);
        set_custom_hook(app, HookID_BeginBuffer, Fleury4BeginBuffer);
    mapping_init(tctx, &framework_mapping);
    Fleury4SetBindings(&framework_mapping);
    }
    
    Fleury4DarkMode(app);
}

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
                
                // NOTE(rjf): Draw main cursor.
                {
                    draw_rectangle(app, rect, roundness, fcolor_resolve(cursor_color));
                }
                
                // NOTE(rjf): Draw cursor glow (because why the hell not).
                for(int i = 0; i < 8; ++i)
                {
                    f32 alpha = 0.1f - i*0.015f;
                    if(alpha > 0)
                    {
                    Rect_f32 glow_rect = rect;
                    glow_rect.x0 -= i;
                    glow_rect.y0 -= i;
                    glow_rect.x1 += i;
                    glow_rect.y1 += i;
                        draw_rectangle(app, glow_rect, roundness + i*0.3f, fcolor_resolve(fcolor_change_alpha(cursor_color, alpha)));
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

static ARGB_Color
ARGBFromID(Managed_ID id)
{
    return fcolor_resolve(fcolor_id(id));
}

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
    
    draw_set_clip(app, prev_clip);
}

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
    }
    
    SelectMap(mapid_file);
    ParentMap(mapid_global);
    BindTextInput(write_text_input);
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
    BindTextInput(write_text_and_auto_indent);
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