/* date = January 29th 2021 8:06 pm */

#ifndef FCODER_FLEURY_CALC_H
#define FCODER_FLEURY_CALC_H

function void F4_CLC_Tick(Frame_Info frame_info);
function void F4_CLC_RenderCode(Application_Links *app, Buffer_ID buffer,
                                View_ID view, Text_Layout_ID text_layout_id,
                                Frame_Info frame_info, Arena *arena, char *code_buffer,
                                i64 start_char_offset);
function void F4_CLC_RenderBuffer(Application_Links *app, Buffer_ID buffer, View_ID view,
                                  Text_Layout_ID text_layout_id, Frame_Info frame_info);
function void F4_CLC_RenderComments(Application_Links *app, Buffer_ID buffer, View_ID view,
                                    Text_Layout_ID text_layout_id, Frame_Info frame_info);

#endif // FCODER_FLEURY_CALC_H
