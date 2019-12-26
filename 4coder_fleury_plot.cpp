static ARGB_Color global_plot_color_cycle[] =
{
    0xff03d3fc,
    0xff22b80b,
    0xfff0bb0c,
    0xfff0500c,
};

typedef enum Plot2DMode Plot2DMode;
enum Plot2DMode
{
    PLOT2D_MODE_LINE,
    PLOT2D_MODE_HISTOGRAM,
};

typedef struct Plot2DInfo Plot2DInfo;
struct Plot2DInfo
{
    // NOTE(rjf): 4coder stuff.
    Application_Links *app;
    Face_ID title_face_id;
    Face_ID label_face_id;
    
    // NOTE(rjf): Data generic to all plots.
    Plot2DMode mode;
    String_Const_u8 title;
    String_Const_u8 x_axis;
    String_Const_u8 y_axis;
    Rect_f32 screen_rect;
    
    // NOTE(rjf): Line and point plot.
    Rect_f32 plot_view;
    
    // NOTE(rjf): Histogram stuff.
    int num_bins;
    int *bins;
    Range_f32 bin_data_range;
    int bin_group_count;
    
    // NOTE(rjf): Used internally; zero initialize.
    Rect_f32 last_clip;
    int color_cycle_position;
    int current_bin_group;
};

typedef enum Plot2DStyleFlags Plot2DStyleFlags;
enum Plot2DStyleFlags
{
    PLOT2D_LINES  = (1<<0),
    PLOT2D_POINTS = (1<<1),
};

static void
Plot2DBegin(Plot2DInfo *plot)
{
    Rect_f32 rect = plot->screen_rect;
    Rect_f32 plot_view = plot->plot_view;
    
    if(plot->title.data)
    {
        Face_Metrics metrics = get_face_metrics(plot->app, plot->title_face_id);
        draw_string(plot->app, plot->title_face_id, plot->title, V2f32(rect.x0, rect.y0 - metrics.line_height), fcolor_resolve(fcolor_id(defcolor_comment)));
    }
    
    if(plot->x_axis.data)
    {
        draw_string(plot->app, plot->label_face_id, plot->x_axis, V2f32(rect.x0, rect.y1), fcolor_resolve(fcolor_id(defcolor_comment)));
    }
    
    if(plot->y_axis.data)
    {
        draw_string_oriented(plot->app, plot->label_face_id, fcolor_resolve(fcolor_id(defcolor_comment)), plot->y_axis,
                             V2f32(rect.x0 - 10, rect.y0 + 5), 0, V2f32(0.f, 1.f));
    }
    
    plot->last_clip = draw_set_clip(plot->app, rect);
    f32 rect_width = rect.x1 - rect.x0;
    f32 rect_height = rect.y1 - rect.y0;
    draw_rectangle(plot->app, rect, 4.f, fcolor_resolve(fcolor_id(defcolor_back)));
    
    // NOTE(rjf): Draw grid lines.
    {
        ARGB_Color grid_line_color = fcolor_resolve(fcolor_id(defcolor_comment));
        grid_line_color &= 0x00ffffff;
        grid_line_color |= 0x91000000;
        
        float scale_factor_x = 1.f * ((int)((plot_view.x1 - plot_view.x0) / 10.f) + 1);
        float scale_factor_y = 1.f * ((int)((plot_view.y1 - plot_view.y0) / 10.f) + 1);
        
		if(scale_factor_x <= 0)
		{
			scale_factor_x = 1;
		}
		if(scale_factor_y <= 0)
		{
			scale_factor_y = 1;
		}
        
        for(int x = (int)(plot_view.x0 / scale_factor_x); x <= (int)(plot_view.x1 / scale_factor_x); ++x)
        {
            f32 point_x = rect_width * ((float)x - plot_view.x0*scale_factor_x) / (plot_view.x1 - plot_view.x0)*scale_factor_x;
            Rect_f32 line_rect =
            {
                rect.x0 + point_x,
                rect.y0,
                rect.x0 + point_x + 1,
                rect.y1,
            };
            draw_rectangle(plot->app, line_rect, 1.f, grid_line_color);
        }
        
        for(int y = (int)(plot_view.y0 / scale_factor_y); y <= (int)(plot_view.y1 / scale_factor_y); ++y)
        {
            f32 point_y = rect_height * ((float)y - plot_view.y0*scale_factor_y) / (plot_view.y1 - plot_view.y0)*scale_factor_y;
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
Plot2DPoints(Plot2DInfo *plot, i32 style_flags,
                    float *x_data, float *y_data, int data_count)
{
    Rect_f32 rect = plot->screen_rect;
    Rect_f32 plot_view = plot->plot_view;
    
    f32 rect_width = rect.x1 - rect.x0;
    f32 rect_height = rect.y1 - rect.y0;
    
    // NOTE(rjf): Draw function samples.
    {
        ARGB_Color function_color =
            global_plot_color_cycle[(plot->color_cycle_position++) % ArrayCount(global_plot_color_cycle)];
        
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
                draw_rectangle(plot->app, point_rect, 2.f, function_color);
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

static void
Plot2DEnd(Plot2DInfo *plot)
{
    if(plot->mode == PLOT2D_MODE_HISTOGRAM)
    {
        f32 bin_screen_width = ((plot->screen_rect.x1-plot->screen_rect.x0) / plot->num_bins) / plot->bin_group_count;
        
        for(int bin_group = 0; bin_group < plot->bin_group_count; ++bin_group)
        {
            int total_data = 0;
            
            for(int i = 0; i < plot->num_bins; ++i)
            {
                total_data += plot->bins[i + bin_group*plot->num_bins];
            }
            
            ARGB_Color color = global_plot_color_cycle[bin_group % ArrayCount(global_plot_color_cycle)];
            
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
