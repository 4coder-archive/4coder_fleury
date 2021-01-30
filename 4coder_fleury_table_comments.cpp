
enum F4_TableMarkerKind
{
    F4_TableMarkerKind_Null,
    F4_TableMarkerKind_IdentifierSlot,
    F4_TableMarkerKind_SymbolSlot,
};

struct F4_TableMarker
{
    F4_TableMarkerKind kind;
    u8 specific_symbol;
    int wanted_size;
};

static void
F4_ApplyTableMarkers(Application_Links *app, Buffer_ID buffer, View_ID view, Text_Layout_ID text_layout_id,
                     F4_TableMarker *markers, int marker_count)
{
    Scratch_Block scratch(app);
    Token_Array token_array = get_token_array_from_buffer(app, buffer);
    Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
    
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
        
    }
    
    static void
        F4_DoTableComments(Application_Links *app, Buffer_ID buffer, View_ID view,
                           Text_Layout_ID text_layout_id)
    {
        ProfileScope(app, "[Fleury] Table Comments");
        
        String_Const_u8 table_comment_signifier = string_u8_litexpr("//t");
        
        Token_Array token_array = get_token_array_from_buffer(app, buffer);
        Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
        Scratch_Block scratch(app);
        
        if(token_array.tokens != 0)
        {
            i64 first_index = token_index_from_pos(&token_array, visible_range.first);
            Token_Iterator_Array it = token_iterator_index(0, &token_array, first_index);
            
            Token *comment_token = 0;
            for(;;)
            {
                comment_token = token_it_read(&it);
                if(comment_token->pos >= visible_range.one_past_last || !comment_token || !token_it_inc_non_whitespace(&it))
                {
                    break;
                }
                
                if(comment_token->kind == TokenBaseKind_Comment)
                {
                    u8 token_buffer[256] = {0};
                    if(buffer_read_range(app, buffer, Ii64(comment_token->pos, comment_token->pos + comment_token->size), token_buffer))
                    {
                        String_Const_u8 str = { token_buffer, (u64)comment_token->size };
                        
                        //~ NOTE(rjf): Determine table markers.
                        int marker_count = 0;
                        F4_TableMarker markers[256] = {};
                        for(int i = 0; i < str.size && marker_count < ArrayCount(markers); i += 1)
                        {
                            F4_TableMarker *marker = markers + marker_count;
                            marker->kind = F4_TableMarkerKind_Null;
                            
                            switch(str.str[i])
                            {
                                case '$':
                                {
                                    marker_count += 1;
                                    marker->kind = F4_TableMarkerKind_IdentifierSlot;
                                }break;
                                
                                case '`':
                                {
                                    marker_count += 1;
                                    marker->kind = F4_TableMarkerKind_SymbolSlot;
                                    marker->specific_symbol = 0;
                                }break;
                                
                                case '(': case ')': case '[': case ']': case '{': case '}':
                                case '+': case '-': case '*': case '/': case '%': case '=':
                                case '#': case '@': case '!': case '~': case '^': case '&':
                                case '|': case '_': case '<': case '>': case '.': case ',':
                                {
                                    marker_count += 1;
                                    marker->kind = F4_TableMarkerKind_SymbolSlot;
                                    marker->specific_symbol = str.str[i];
                                }break;
                                
                                default: break;
                            }
                            
                            if(marker->kind)
                            {
                                marker->wanted_size = 0;
                                for(int j = i+1; j < str.size; j += 1)
                                {
                                    if(str.str[j] <= 32)
                                    {
                                        marker->wanted_size += 1;
                                    }
                                }
                            }
                        }
                        
                    }
                }
            }
        }
    }