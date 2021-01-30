
 function void
Plot2DBegin(Plot2DInfo *plot)
{
    Scratch_Block scratch(plot->app);
    
    Rect_f32 rect = plot->screen_rect;
    Rect_f32 plot_view = plot->plot_view;
    
    if(plot->title.str)
    {
        Face_Metrics metrics = get_face_metrics(plot->app, plot->title_face_id);
        draw_string(plot->app, plot->title_face_id, plot->title, V2f32(rect.x0, rect.y0 - metrics.line_height), fcolor_resolve(fcolor_id(defcolor_comment)));
    }
    
    if(plot->x_axis.str)
    {
        draw_string(plot->app, plot->label_face_id, plot->x_axis, V2f32(rect.x0, rect.y1), fcolor_resolve(fcolor_id(defcolor_comment)));
    }
    
    if(plot->y_axis.str)
    {
        draw_string_oriented(plot->app, plot->label_face_id, fcolor_resolve(fcolor_id(defcolor_comment)), plot->y_axis,
                             V2f32(rect.x0 - 10, rect.y0 + 5), 0, V2f32(0.f, 1.f));
    }
    
    plot->last_clip = draw_set_clip(plot->app, rect);
    f32 rect_width = rect.x1 - rect.x0;
    f32 rect_height = rect.y1 - rect.y0;
    draw_rectangle(plot->app, rect, 4.f, fcolor_resolve(fcolor_id(defcolor_back)));
    
    // NOTE(rjf): Draw grid lines.
    if(plot->mode != Plot2DMode_Histogram)
    {
        Face_Metrics metrics = get_face_metrics(plot->app, plot->label_face_id);
        
        ARGB_Color grid_line_color = fcolor_resolve(fcolor_id(defcolor_comment));
        grid_line_color &= 0x00ffffff;
        grid_line_color |= 0x91000000;
        
        float tick_increment_x = (plot_view.x1 - plot_view.x0) / 10.f + 1.f;
        float tick_increment_y = (plot_view.y1 - plot_view.y0) / 10.f + 1.f;
        
        tick_increment_x = powf(10.f, floorf(log10f(tick_increment_x)));
        tick_increment_y = powf(10.f, floorf(log10f(tick_increment_y)));
        
		if(tick_increment_x <= 0)
		{
			tick_increment_x = 1;
		}
		if(tick_increment_y <= 0)
		{
			tick_increment_y = 1;
		}
        
        // NOTE(rjf): Draw vertical lines.
        {
            for(float x = plot_view.x0 - fmodf(plot_view.x0, tick_increment_x);
                x <= plot_view.x1; x += tick_increment_x)
            {
                Rect_f32 line_rect = {0};
                {
                    line_rect.x0 = rect.x0 + rect_width * (x - plot_view.x0) / (plot_view.x1 - plot_view.x0);
                    line_rect.y0 = rect.y0;
                    line_rect.x1 = line_rect.x0+1;
                    line_rect.y1 = rect.y1;
                }
                
                draw_rectangle(plot->app, line_rect, 1.f, grid_line_color);
                
                // NOTE(rjf): Draw number label.
                {
                    float nearest_y_tick = (plot_view.y1 + plot_view.y0) / 2;
                    nearest_y_tick -= fmodf(nearest_y_tick, tick_increment_y);
                    
                    String_Const_u8 str = push_stringf(scratch, "%.*f", tick_increment_y >= 1 ? 0 : 3, x);
                    draw_string(plot->app, plot->label_face_id, str,
                                V2f32(line_rect.x0,
                                      rect.y0 + rect_height -
                                      rect_height * (nearest_y_tick - plot_view.y0) / (plot_view.y1 - plot_view.y0)),
                                grid_line_color);
                }
                
            }
        }
        
        // NOTE(rjf): Draw horizontal lines.
        {
            for(float y = plot_view.y0 - fmodf(plot_view.y0, tick_increment_y);
                y <= plot_view.y1; y += tick_increment_y)
            {
                Rect_f32 line_rect = {0};
                {
                    line_rect.x0 = rect.x0;
                    line_rect.y0 = rect.y0 + rect_height - rect_height * (y - plot_view.y0) / (plot_view.y1 - plot_view.y0);
                    line_rect.x1 = rect.x1;
                    line_rect.y1 = line_rect.y0+1;
                }
                
                draw_rectangle(plot->app, line_rect, 1.f, grid_line_color);
                
                // NOTE(rjf): Draw number label.
                {
                    float nearest_x_tick = (plot_view.x1 + plot_view.x0) / 2;
                    nearest_x_tick -= fmodf(nearest_x_tick, tick_increment_x);
                    
                    String_Const_u8 str = push_stringf(scratch, "%.*f", tick_increment_y >= 1 ? 0 : 3, y);
                    draw_string(plot->app, plot->label_face_id, str,
                                V2f32(rect.x0 + rect_width * (nearest_x_tick - plot_view.x0) / (plot_view.x1 - plot_view.x0),
                                      line_rect.y0),
                                grid_line_color);
                }
            }
        }
    }
}

function void
Plot2DPoints(Plot2DInfo *plot, i32 style_flags,
             float *x_data, float *y_data, int data_count)
{
    Rect_f32 rect = plot->screen_rect;
    Rect_f32 plot_view = plot->plot_view;
    
    f32 rect_width = rect.x1 - rect.x0;
    f32 rect_height = rect.y1 - rect.y0;
    
    // NOTE(rjf): Draw function samples.
    {
        Color_Array plot_cycle = finalize_color_array(fleury_color_plot_cycle);
        ARGB_Color function_color =
            plot_cycle.vals[(plot->color_cycle_position++) % plot_cycle.count];
        
        for(int i = 0; i < data_count; ++i)
        {
            f32 point_x = rect_width * (x_data[i] - plot->plot_view.x0) / (plot->plot_view.x1 - plot->plot_view.x0);
            f32 point_y = rect_height - rect_height * (y_data[i] - plot->plot_view.y0) / (plot->plot_view.y1 - plot->plot_view.y0);
            
            if(style_flags & Plot2DStyleFlags_Lines)
            {
                Rect_f32 point_rect =
                {
                    plot->screen_rect.x0 + point_x - 1,
                    plot->screen_rect.y0 + point_y - 1,
                    plot->screen_rect.x0 + point_x + 1,
                    plot->screen_rect.y0 + point_y + 1,
                };
                
                // TODO(rjf): Real line drawing.
                draw_rectangle(plot->app, point_rect, 2.f, function_color);
            }
            
            if(style_flags & Plot2DStyleFlags_Points)
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

function void
Plot2DHistogram(Plot2DInfo *plot, float *data, int data_count)
{
    if(plot->bins && plot->num_bins > 0)
    {
        for(int i = 0; i < data_count; ++i)
        {
            float t = (data[i] - plot->bin_data_range.min) / (plot->bin_data_range.max - plot->bin_data_range.min);
            int bin_to_go_in = (int)(plot->num_bins * t);
            if(bin_to_go_in >= 0 && bin_to_go_in < plot->num_bins)
            {
                ++plot->bins[bin_to_go_in + plot->current_bin_group*plot->num_bins];
            }
        }
        ++plot->current_bin_group;
    }
}

function void
Plot2DEnd(Plot2DInfo *plot)
{
    if(plot->mode == Plot2DMode_Histogram)
    {
        f32 bin_screen_width = ((plot->screen_rect.x1-plot->screen_rect.x0) / plot->num_bins) / plot->bin_group_count;
        
        for(int bin_group = 0; bin_group < plot->bin_group_count; ++bin_group)
        {
            int total_data = 0;
            
            for(int i = 0; i < plot->num_bins; ++i)
            {
                total_data += plot->bins[i + bin_group*plot->num_bins];
            }
            
            Color_Array plot_cycle = finalize_color_array(fleury_color_plot_cycle);
            ARGB_Color color =
                plot_cycle.vals[bin_group % plot_cycle.count];
            
            for(int i = 0; i < plot->num_bins; ++i)
            {
                int bin_index = i + bin_group*plot->num_bins;
                Rect_f32 bin_rect = {0};
                bin_rect.x0 = plot->screen_rect.x0 + ((float)i/plot->num_bins)*(plot->screen_rect.x1-plot->screen_rect.x0) + bin_screen_width*bin_group;
                bin_rect.x1 = bin_rect.x0 + bin_screen_width;
                bin_rect.y0 = bin_rect.y1 = plot->screen_rect.y1;
                bin_rect.y0 -= ((float)plot->bins[bin_index] / total_data) * (plot->screen_rect.y1 - plot->screen_rect.y0);
                draw_rectangle(plot->app, bin_rect, 4.f, color);
            }
        }
    }
    
    draw_rectangle_outline(plot->app, plot->screen_rect, 4.f, 3.f, fcolor_resolve(fcolor_id(defcolor_margin_active)));
    draw_set_clip(plot->app, plot->last_clip);
}
