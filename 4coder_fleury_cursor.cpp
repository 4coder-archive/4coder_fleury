
//~ NOTE(rjf): Cursor rendering

static int global_cursor_count = 1;
static i64 global_cursor_positions[16] = {0};
static i64 global_mark_positions[16] = {0};

static void
Fleury4RenderCursor(Application_Links *app, View_ID view_id, b32 is_active_view,
                    Buffer_ID buffer, Text_Layout_ID text_layout_id,
                    f32 roundness, f32 outline_thickness, Frame_Info frame_info)
{
    b32 has_highlight_range = draw_highlight_range(app, view_id, buffer, text_layout_id, roundness);
    
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
                for(int glow = 0; glow < 20; ++glow)
                {
                        f32 alpha = 0.1f - (global_power_mode_enabled ? (glow*0.005f) : (glow*0.015f));
                    if(alpha > 0)
                    {
                        Rect_f32 glow_rect = rect;
                            glow_rect.x0 -= glow;
                            glow_rect.y0 -= glow;
                            glow_rect.x1 += glow;
                            glow_rect.y1 += glow;
                            draw_rectangle(app, glow_rect, roundness + glow*0.7f, fcolor_resolve(fcolor_change_alpha(cursor_color, alpha)));
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
}
