/* date = January 29th 2021 7:51 pm */

#ifndef FCODER_FLEURY_BRACE_H
#define FCODER_FLEURY_BRACE_H

function void F4_Brace_RenderHighlight(Application_Links *app, Buffer_ID buffer, Text_Layout_ID text_layout_id, i64 pos, ARGB_Color *colors, i32 color_count);
function void F4_Brace_RenderCloseBraceAnnotation(Application_Links *app, Buffer_ID buffer, Text_Layout_ID text_layout_id, i64 pos);
function void F4_Brace_RenderLines(Application_Links *app, Buffer_ID buffer, View_ID view, Text_Layout_ID text_layout_id, i64 pos);

#endif //4CODER_FLEURY_BRACE_H
