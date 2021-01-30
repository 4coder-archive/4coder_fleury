/* date = January 29th 2021 7:46 pm */

#ifndef FCODER_FLEURY_HOOKS_H
#define FCODER_FLEURY_HOOKS_H

//~ NOTE(rjf): @f4_hooks The hooks that 4coder's core will call back, that are
// implemented by 4coder_fleury.

function void F4_Tick(Application_Links *app, Frame_Info frame_info);
function i32  F4_BeginBuffer(Application_Links *app, Buffer_ID buffer_id);
function void F4_Render(Application_Links *app, Frame_Info frame_info, View_ID view_id);
function Layout_Item_List F4_Layout(Application_Links *app, Arena *arena, Buffer_ID buffer, Range_i64 range, Face_ID face, f32 width);
function void F4_WholeScreenRender(Application_Links *app, Frame_Info frame_info);
function BUFFER_EDIT_RANGE_SIG(F4_BufferEditRange);
function DELTA_RULE_SIG(F4_DeltaRule)
{
    Vec2_f32 *velocity = (Vec2_f32*)data;
    if(velocity->x == 0.f)
    {
        velocity->x = 1.f;
        velocity->y = 1.f;
    }
    Smooth_Step step_x = smooth_camera_step(pending.x, velocity->x, 80.f, 1.f/4.f);
    Smooth_Step step_y = smooth_camera_step(pending.y, velocity->y, 80.f, 1.f/4.f);
    *velocity = V2f32(step_x.v, step_y.v);
    return(V2f32(step_x.p, step_y.p));
}

//~ NOTE(rjf): @f4_hook_helpers
function void F4_RenderBuffer(Application_Links *app, View_ID view_id, Face_ID face_id, Buffer_ID buffer, Text_Layout_ID text_layout_id, Rect_f32 rect, Frame_Info frame_info);

#endif // 4CODER_FLEURY_HOOKS_H
