
 global b32 global_code_peek_open = 0;

function void
F4_CodePeek_Render(Application_Links *app, View_ID view_id, Face_ID face_id,
                   Buffer_ID buffer, Frame_Info frame_info)
{
    Scratch_Block scratch(app);
    Buffer_ID peek_buf = get_buffer_by_name(app, string_u8_litexpr("*peek*"), AccessFlag_Read);
    
    if(!global_code_peek_open && buffer != peek_buf)
    {
        return;
    }
    
    Rect_f32 view_rect = view_get_screen_rect(app, view_id);
    
    struct Peek
    {
        Peek *next;
        String_Const_u8 string;
        Code_Index_Note *note;
    };
    
    Peek *first = 0;
    Peek *last = 0;
    int peek_count = 0;
    
#define PushPeek(str) \
{\
Peek *p = push_array_zero(scratch, Peek, 1);\
p->string = str;\
p->note = code_index_note_from_string(p->string);\
if(last == 0)\
{\
first = last = p;\
}\
else\
{\
last->next = p;\
last = last->next;\
}\
peek_count += 1;\
}
    
    PushPeek(push_token_or_word_under_active_cursor(app, scratch));
    
    // NOTE(rjf): Push identifiers left around in *peek*.
    {
        Token_Array token_array = get_token_array_from_buffer(app, peek_buf);
        Token_Iterator_Array it = token_iterator_pos(0, &token_array, 0);
        for(;token_it_inc_non_whitespace(&it);)
        {
            Token *token = token_it_read(&it);
            if(token == 0)
            {
                break;
            }
            if(token->kind == TokenBaseKind_Identifier)
            {
                PushPeek(push_buffer_range(app, scratch, peek_buf, Ii64(token->pos, token->pos + token->size)));
            }
        }
    }
    
#undef PushPeek
    
    f32 peek_height = (f32)((view_rect.y1 - view_rect.y0) * (0.5f + 0.4f*(clamp_top(peek_count / 4, 1)))) / peek_count;
    Rect_f32 rect = {0};
    {
        rect.x0 = view_rect.x0;
        rect.x1 = view_rect.x1;
        rect.y0 = view_rect.y1 - peek_height*peek_count;
        rect.y1 = rect.y0 + peek_height;
    }
    
    for(Peek *peek = first; peek; peek = peek->next)
    {
        Code_Index_Note *note = peek->note;
        if(note)
        {
            Code_Index_File *file = note->file;
            Buffer_ID match_buffer = file->buffer;
            
            F4_DrawTooltipRect(app, rect);
            
            // NOTE(rjf): Draw code.
            {
                Rect_f32 inner_rect = rect_inner(rect, 30);
                
                Buffer_Point buffer_point =
                {
                    get_line_number_from_pos(app, match_buffer, note->pos.min),
                    0,
                };
                Text_Layout_ID text_layout_id = text_layout_create(app, match_buffer, inner_rect, buffer_point);
                
                Rect_f32 prev_prev_clip = draw_set_clip(app, inner_rect);
                {
                    Token_Array match_token_array = get_token_array_from_buffer(app, match_buffer);
                    if(match_token_array.tokens != 0)
                    {
                        F4_SyntaxHighlight(app, text_layout_id, &match_token_array);
                    }
                    else
                    {
                        Range_i64 visible_range = Ii64(note->pos.min, note->pos.max);
                        paint_text_color_fcolor(app, text_layout_id, visible_range, fcolor_id(defcolor_text_default));
                    }
                    
                    draw_text_layout_default(app, text_layout_id);
                }
                draw_set_clip(app, prev_prev_clip);
                text_layout_free(app, text_layout_id);
            }
        }
        
        f32 height = (rect.y1 - rect.y0);
        rect.y0 += height;
        rect.y1 += height;
    }
    
}

CUSTOM_COMMAND_SIG(f4_code_peek)
CUSTOM_DOC("Toggles code peek.")
{
    global_code_peek_open ^= 1;
}

CUSTOM_COMMAND_SIG(f4_code_peek_yank)
CUSTOM_DOC("Yanks the current cursor identifier into the *peek* buffer.")
{
    Scratch_Block scratch(app);
    String_Const_u8 string = push_token_or_word_under_active_cursor(app, scratch);
    Code_Index_Note *note = code_index_note_from_string(string);
    Buffer_ID buffer = get_buffer_by_name(app, string_u8_litexpr("*peek*"), Access_Read | Access_Write);
    if(buffer != 0 && note != 0)
    {
        buffer_replace_range(app, buffer, Ii64(buffer_get_size(app, buffer)), string_u8_litexpr("\n"));
        buffer_replace_range(app, buffer, Ii64(buffer_get_size(app, buffer)), string);
    }
}

CUSTOM_COMMAND_SIG(f4_code_peek_clear)
CUSTOM_DOC("Clears the *peek* buffer.")
{
    Buffer_ID buffer = get_buffer_by_name(app, string_u8_litexpr("*peek*"), Access_Read | Access_Write);
    if(buffer)
    {
        clear_buffer(app, buffer);
    }
}
