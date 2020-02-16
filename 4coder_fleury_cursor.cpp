
//~ NOTE(rjf): Cursor rendering

static int global_cursor_count = 1;
static i64 global_cursor_positions[16] = {0};
static i64 global_mark_positions[16] = {0};

static void
Fleury4RenderCursorSymbolThingy(Application_Links *app, Rect_f32 rect,
                                f32 roundness, f32 thickness,
                                ARGB_Color color, b32 open)
{
    Rect_f32 top_bar_rect_thing;
    Rect_f32 side_bar_rect_thing;
    
    if(open)
    {
        top_bar_rect_thing =
        {
            rect.x0,
            rect.y0,
            rect.x1,
            rect.y0 + thickness,
        };
        
        side_bar_rect_thing =
        {
            rect.x0,
            rect.y0,
            rect.x0 + thickness,
            rect.y1,
        };
    }
    else
    {
        top_bar_rect_thing =
        {
            rect.x0,
            rect.y1 - thickness,
            rect.x1,
            rect.y1,
        };
        
        side_bar_rect_thing =
        {
            rect.x1 - thickness,
            rect.y0,
            rect.x1,
            rect.y1,
        };
    }
    
    draw_rectangle(app, top_bar_rect_thing, roundness, color);
    draw_rectangle(app, side_bar_rect_thing, roundness, color);
}

static void
DoTheCursorInterpolation(Application_Links *app, Frame_Info frame_info,
                         Rect_f32 *rect, Rect_f32 *last_rect, Rect_f32 target)
{
    *last_rect = *rect;
    
    float x_change = target.x0 - rect->x0;
    float y_change = target.y0 - rect->y0;
    
    float cursor_size_x = (target.x1 - target.x0);
    float cursor_size_y = (target.y1 - target.y0) * (1 + fabsf(y_change) / 60.f);
    
    b32 should_animate_cursor = !global_battery_saver;
    if(should_animate_cursor)
    {
        if(fabs(x_change) > 1.f || fabs(y_change) > 1.f)
        {
            animate_in_n_milliseconds(app, 0);
        }
    }
    else
    {
        *rect = *last_rect = target;
        cursor_size_y = target.y1 - target.y0;
    }
    
    if(should_animate_cursor)
    {
        rect->x0 += (x_change) * frame_info.animation_dt * 30.f;
        rect->y0 += (y_change) * frame_info.animation_dt * 30.f;
        rect->x1 = rect->x0 + cursor_size_x;
        rect->y1 = rect->y0 + cursor_size_y;
    }
    
    if(target.y0 > last_rect->y0)
    {
        if(rect->y0 < last_rect->y0)
        {
            rect->y0 = last_rect->y0;
        }
    }
    else
    {
        if(rect->y1 > last_rect->y1)
        {
            rect->y1 = last_rect->y1;
        }
    }
    
}

static void
Fleury4RenderCursor(Application_Links *app, View_ID view_id, b32 is_active_view,
                    Buffer_ID buffer, Text_Layout_ID text_layout_id,
                    f32 roundness, f32 outline_thickness, Frame_Info frame_info)
{
    Rect_f32 view_rect = view_get_screen_rect(app, view_id);
    Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
    
    b32 has_highlight_range = draw_highlight_range(app, view_id, buffer, text_layout_id, roundness);
    
    FColor cursor_color = fcolor_id(defcolor_cursor);
    
    if(global_keyboard_macro_is_recording)
    {
        cursor_color = fcolor_argb(0xffde40df);
    }
    else if(global_power_mode_enabled)
    {
        cursor_color = fcolor_argb(0xffefaf2f);
    }
    
    // TODO(rjf): REMOVE THIS
    {
        i64 cursor_pos = view_get_cursor_pos(app, view_id);
        i64 mark_pos = view_get_mark_pos(app, view_id);
        global_cursor_positions[0] = cursor_pos;
        global_mark_positions[0] = mark_pos;
    }
    
    if(!has_highlight_range)
    {
        
        for(int i = 0; i < 1/*global_cursor_count*/; ++i)
        {
            i64 cursor_pos = global_cursor_positions[0];
            i64 mark_pos = global_mark_positions[0];
            
            b32 cursor_open = cursor_pos <= mark_pos;
            
            // NOTE(rjf): Draw cursor.
            {
                if(is_active_view)
                {
                    Rect_f32 target_cursor = text_layout_character_on_screen(app, text_layout_id, cursor_pos);
                    DoTheCursorInterpolation(app, frame_info, &global_cursor_rect,
                                             &global_last_cursor_rect, target_cursor);
                    
                    Rect_f32 target_mark = text_layout_character_on_screen(app, text_layout_id, mark_pos);
                    
                    if(mark_pos > visible_range.end)
                    {
                        target_mark.x0 = 0;
                        target_mark.y0 = view_rect.y1;
                        target_mark.y1 = view_rect.y1;
                    }
                    
                    DoTheCursorInterpolation(app, frame_info, &global_mark_rect, &global_last_mark_rect,
                                             target_mark);
                }
                
                // NOTE(rjf): Draw main cursor.
                {
                    Fleury4RenderCursorSymbolThingy(app, global_cursor_rect, roundness, 4.f, fcolor_resolve(cursor_color), cursor_open);
                }
                
                // NOTE(rjf): Draw cursor glow (because why the hell not).
                for(int glow = 0; glow < 20; ++glow)
                {
                    f32 alpha = 0.1f - (global_power_mode_enabled ? (glow*0.005f) : (glow*0.015f));
                    if(alpha > 0)
                    {
                        Rect_f32 glow_rect = global_cursor_rect;
                        glow_rect.x0 -= glow;
                        glow_rect.y0 -= glow;
                        glow_rect.x1 += glow;
                        glow_rect.y1 += glow;
                        Fleury4RenderCursorSymbolThingy(app, glow_rect, roundness + glow*0.7f, 2.f,
                                                        fcolor_resolve(fcolor_change_alpha(cursor_color, alpha)), cursor_open);
                    }
                    else
                    {
                        break;
                    }
                }
                
            }
            
            // paint_text_color_pos(app, text_layout_id, cursor_pos,
            // fcolor_id(defcolor_at_cursor));
            Fleury4RenderCursorSymbolThingy(app, global_mark_rect, roundness, 2.f,
                                            fcolor_resolve(fcolor_change_alpha(cursor_color, 0.5f)), !cursor_open);
        }
    }
}

static void
Fleury4HighlightCursorMarkRange(Application_Links *app, View_ID view_id)
{
    Rect_f32 view_rect = view_get_screen_rect(app, view_id);
    Rect_f32 clip = draw_set_clip(app, view_rect);
    
    f32 lower_bound_y;
    f32 upper_bound_y;
    
    if(global_last_cursor_rect.y0 < global_last_mark_rect.y0)
    {
        lower_bound_y = global_last_cursor_rect.y0;
        upper_bound_y = global_last_mark_rect.y1;
    }
    else
    {
        lower_bound_y = global_last_mark_rect.y0;
        upper_bound_y = global_last_cursor_rect.y1;
    }
    
    draw_rectangle(app, Rf32(view_rect.x0, lower_bound_y, view_rect.x0 + 4, upper_bound_y), 3.f,
                   fcolor_resolve(fcolor_change_alpha(fcolor_id(defcolor_comment), 0.5f)));
    draw_set_clip(app, clip);
}


//~ NOTE(rjf): Mark Annotation

static void
Fleury4RenderMarkAnnotation(Application_Links *app, Buffer_ID buffer, Text_Layout_ID text_layout_id,
                            View_ID view_id, b32 is_active_view)
{
    i64 pos = view_get_mark_pos(app, view_id);
    
    if(view_get_cursor_pos(app, view_id) > pos && is_active_view)
    {
        Scratch_Block scratch(app);
        ProfileScope(app, "[Fleury] Mark Annotation");
        
        Token_Array token_array = get_token_array_from_buffer(app, buffer);
        Face_ID face_id = global_small_code_face;
        Rect_f32 view_rect = view_get_screen_rect(app, view_id);
        
        Token *start_token = 0;
        
        if(token_array.tokens != 0)
        {
            Token_Iterator_Array it = token_iterator_pos(0, &token_array, pos);
            
            int max = 5;
            int count = 0;
            
            for(Token *token = 0; (token = token_it_read(&it)) != 0 && count < max;
                token_it_inc_non_whitespace(&it), ++count)
            {
                if(token->kind == TokenBaseKind_Comment ||
                   token->kind == TokenBaseKind_Identifier ||
                   token->kind == TokenBaseKind_Keyword)
                {
                    start_token = token;
                    break;
                }
            }
        }
        
        // NOTE(rjf): Draw.
        if(start_token)
        {
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
            
            // NOTE(rjf): Special case to handle CRLF-newline files.
            if(start_line.str[start_line.size - 1] == 13)
            {
                start_line.size -= 1;
            }
            
            Vec2_f32 draw_pos =
            {
                view_rect.x0 + 30,
                global_cursor_rect.y0,
            };
            
            if(draw_pos.y < view_rect.y0)
            {
                draw_pos.y = view_rect.y0;
            }
            
            u32 color = finalize_color(defcolor_comment, 0);
            color &= 0x00ffffff;
            color |= 0x80000000;
            draw_string_oriented(app, face_id, color, start_line, draw_pos, GlyphFlag_Rotate90, V2f32(0.f, 1.f));
        }
    }
}