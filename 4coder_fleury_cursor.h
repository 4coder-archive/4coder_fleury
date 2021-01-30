/* date = January 29th 2021 7:57 pm */

#ifndef FCODER_FLEURY_CURSOR_H
#define FCODER_FLEURY_CURSOR_H

function void F4_Cursor_RenderEmacsStyle(Application_Links *app, View_ID view_id, b32 is_active_view,
                                         Buffer_ID buffer, Text_Layout_ID text_layout_id,
                                         f32 roundness, f32 outline_thickness, Frame_Info frame_info);

function void F4_Cursor_RenderNotepadStyle(Application_Links *app, View_ID view_id, b32 is_active_view,
                                           Buffer_ID buffer, Text_Layout_ID text_layout_id,
                                           f32 roundness, f32 outline_thickness, Frame_Info frame_info);

#endif // FCODER_FLEURY_CURSOR_H
