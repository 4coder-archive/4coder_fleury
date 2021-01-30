/* date = January 29th 2021 8:10 pm */

#ifndef FCODER_FLEURY_PLOT_H
#define FCODER_FLEURY_PLOT_H

enum Plot2DMode
{
    Plot2DMode_Line,
    Plot2DMode_Histogram,
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

enum Plot2DStyleFlags
{
    Plot2DStyleFlags_Lines = (1<<0),
    Plot2DStyleFlags_Points = (1<<1),
};

function void Plot2DBegin(Plot2DInfo *plot);
function void Plot2DPoints(Plot2DInfo *plot, i32 style_flags, float *x_data, float *y_data, int data_count);
function void Plot2DHistogram(Plot2DInfo *plot, float *data, int data_count);
function void Plot2DEnd(Plot2DInfo *plot);

#endif // FCODER_FLEURY_PLOT_H
