
//~ NOTE(rjf): Divider Comments

static void
Fleury4RenderDividerComments(Application_Links *app, Buffer_ID buffer, View_ID view,
                             Text_Layout_ID text_layout_id)
{
    ProfileScope(app, "[Fleury] Divider Comments");
    
    String_Const_u8 divider_comment_signifier = string_u8_litexpr("//~");
    
    Token_Array token_array = get_token_array_from_buffer(app, buffer);
    Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
    Scratch_Block scratch(app);
    
    if(token_array.tokens != 0)
    {
        i64 first_index = token_index_from_pos(&token_array, visible_range.first);
        Token_Iterator_Array it = token_iterator_index(0, &token_array, first_index);
        
        Token *token = 0;
        for(;;)
        {
            token = token_it_read(&it);
            
            if(token->pos >= visible_range.one_past_last || !token || !token_it_inc_non_whitespace(&it))
            {
                break;
            }
            
            if(token->kind == TokenBaseKind_Comment)
            {
                Rect_f32 comment_first_char_rect =
                    text_layout_character_on_screen(app, text_layout_id, token->pos);
                
                Range_i64 token_range =
                {
                    token->pos,
                    token->pos + (token->size > (i64)divider_comment_signifier.size
                                  ? (i64)divider_comment_signifier.size
                                  : token->size),
                };
                
                u8 token_buffer[256] = {0};
                buffer_read_range(app, buffer, token_range, token_buffer);
                String_Const_u8 token_string = { token_buffer, (u64)(token_range.end - token_range.start) };
                
                if(string_match(token_string, divider_comment_signifier))
                {
                    // NOTE(rjf): Render divider line.
                    Rect_f32 rect =
                    {
                        comment_first_char_rect.x0,
                        comment_first_char_rect.y0-2,
                        10000,
                        comment_first_char_rect.y0,
                    };
                    f32 roundness = 4.f;
                    draw_rectangle(app, rect, roundness, fcolor_resolve(fcolor_id(defcolor_comment)));
                }
                
            }
            
        }
        
    }
    
}
