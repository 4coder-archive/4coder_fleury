CUSTOM_ID(attachment, f4_recentfiles_viewstate);

struct F4_RecentFiles_ViewState
{
    int recent_buffer_count;
    Buffer_ID recent_buffers[16];
};

function void
F4_RecentFiles_RefreshView(Application_Links *app, View_ID view)
{
    Buffer_ID buffer = view_get_buffer(app, view, AccessFlag_Read);
    if(buffer)
    {
        Managed_Scope scope = view_get_managed_scope(app, view);
        F4_RecentFiles_ViewState *state = scope_attachment(app, scope, f4_recentfiles_viewstate, F4_RecentFiles_ViewState);
        if(state != 0)
        {
            b32 need_push = 1;
            if(state->recent_buffer_count > 0 && state->recent_buffers[0] == buffer)
            {
                need_push = 0;
            }
            if(need_push)
            {
                if(state->recent_buffer_count >= 16)
                {
                    state->recent_buffer_count -= 1;
                }
                memmove(state->recent_buffers + 1, state->recent_buffers + 0,
                        sizeof(state->recent_buffers[0])*state->recent_buffer_count);
                state->recent_buffers[0] = buffer;
                state->recent_buffer_count += 1;
                
                // NOTE(rjf): Remove any old instances of this buffer in the list.
                {
                    for(int i = 1; i < state->recent_buffer_count; i += 1)
                    {
                        if(state->recent_buffers[i] == buffer)
                        {
                            memmove(state->recent_buffers + i, state->recent_buffers + i + 1,
                                    sizeof(state->recent_buffers[0])*(state->recent_buffer_count-i-1));
                            state->recent_buffer_count -= 1;
                        }
                    }
                }
            }
        }
    }
}

internal void
F4_RecentFiles_Render(Application_Links *app, View_ID view, Face_ID face)
{
#if 1
    Scratch_Block scratch(app);
    Rect_f32 view_rect = view_get_screen_rect(app, view);
    Face_Metrics metrics = get_face_metrics(app, face);
    
    Managed_Scope scope = view_get_managed_scope(app, view);
    F4_RecentFiles_ViewState *state = scope_attachment(app, scope, f4_recentfiles_viewstate, F4_RecentFiles_ViewState);
    
    if(state != 0)
    {
        Vec2_f32 p = view_rect.p0;
        for(int i = 0; i < state->recent_buffer_count; i += 1)
        {
            Buffer_ID buffer = state->recent_buffers[i];
            String_Const_u8 string = push_buffer_unique_name(app, scratch, buffer);
            draw_string(app, face, string, p, 0xffffffff);
            p.y += metrics.line_height;
        }
    }
#endif
}

CUSTOM_UI_COMMAND_SIG(f4_recent_files_menu)
CUSTOM_DOC("Lists the recent files used in the current panel.")
{
    View_ID view = get_active_view(app, Access_Read);
    Managed_Scope scope = view_get_managed_scope(app, view);
    F4_RecentFiles_ViewState *state = scope_attachment(app, scope, f4_recentfiles_viewstate, F4_RecentFiles_ViewState);
    
    if(state != 0)
    {
        Scratch_Block scratch(app);
        Lister_Block lister(app, scratch);
        lister_set_query(lister, "Recent Buffers:");
        lister_set_default_handlers(lister);
        
        for(int i = 1; i < state->recent_buffer_count; i += 1)
        {
            Buffer_ID buffer = state->recent_buffers[i];
            String_Const_u8 buffer_name = push_buffer_unique_name(app, scratch, buffer);
            Buffer_ID *buf = push_array(scratch, Buffer_ID, 1);
            *buf = buffer;
            lister_add_item(lister, buffer_name, S8Lit(""), buf, 0);
        }
        
        Lister_Result l_result = run_lister(app, lister);
        if(!l_result.canceled && l_result.user_data)
        {
            Buffer_ID buffer = *(Buffer_ID *)l_result.user_data;
            if(buffer != 0)
            {
                view_set_buffer(app, view, buffer, 0);
            }
        }
    }
}