Audio_Clip PowerWAV = {};
Audio_Clip HitWAV = {};
Audio_Control PowerWAVControl = {};

CUSTOM_COMMAND_SIG(casey_demo_audio)
CUSTOM_DOC("He used to cut my grass... he was a very nice boy!")
{
    F4_RequireWAV(app, &PowerWAV, "sounds/hit.wav");
    PowerWAV.channel_volume[0] = 0.5f;
    PowerWAV.channel_volume[1] = 0.25f;
	if(!def_audio_is_playing(&PowerWAVControl))
	{
        def_audio_play_clip(PowerWAV, &PowerWAVControl);
	}
}

CUSTOM_COMMAND_SIG(casey_demo_audio_switch_panel)
CUSTOM_DOC("The white zone is for loading and unloading only...")
{
    f32 Temp = PowerWAVControl.channel_volume[0];
	PowerWAVControl.channel_volume[0] = PowerWAVControl.channel_volume[1];
    PowerWAVControl.channel_volume[1] = Temp;
	change_active_panel(app);
}

CUSTOM_COMMAND_SIG(casey_demo_audio_one_shot)
CUSTOM_DOC("... if you gotta load, or if you gotta unload, you go to the white zone...")
{
    F4_RequireWAV(app, &HitWAV, "sounds/hit.wav");
    HitWAV.channel_volume[0] = 0.5f;
    HitWAV.channel_volume[1] = 0.5f;
    def_audio_play_clip(HitWAV, 0);
}

CUSTOM_COMMAND_SIG(casey_seek_beginning_of_line_and_tab)
CUSTOM_DOC("Goes to the beginning of the line and indents the line with default indenting.")
{
    seek_beginning_of_line(app);
    auto_indent_line_at_cursor(app);
}

CUSTOM_COMMAND_SIG(casey_clean_file_and_save)
CUSTOM_DOC("Standardizes line endings and tabs, then saves the active buffer.")
{
	View_ID view = get_active_view(app, Access_ReadWriteVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
	clean_all_lines_buffer(app, buffer, CleanAllLinesMode_RemoveBlankLines);
    rewrite_lines_to_lf(app, buffer);
    
    save(app);
}

CUSTOM_COMMAND_SIG(casey_switch_to_keybinding_0)
CUSTOM_DOC("WENSLEYDALE.")
{
	switch_to_keybinding_0(app);
	global_hide_region_boundary = true;
}

CUSTOM_COMMAND_SIG(casey_switch_to_keybinding_1)
CUSTOM_DOC("STILTON.")
{
	switch_to_keybinding_1(app);
	global_hide_region_boundary = false;
}

CUSTOM_COMMAND_SIG(casey_newline_and_indent)
CUSTOM_DOC("Inserts a newline at the cursor position and indent the next line automatically.")
{
    // NOTE(allen): The idea here is that if the current buffer is
    // read-only, it cannot be edited anyway.  So instead let the return
    // key indicate an attempt to interpret the line as a location to jump to.
    
	View_ID view = get_active_view(app, Access_Always);
    Buffer_ID buffer = view_get_buffer(app, view, Access_Always);
    
    if (buffer_get_access_flags(app, buffer) & Access_Write)
	{
		write_text(app, string_u8_litexpr("\n"));
        auto_indent_line_at_cursor(app);
    }
    else
	{
        goto_jump_at_cursor(app);
    }
}

CUSTOM_COMMAND_SIG(casey_delete_to_end_of_line)
CUSTOM_DOC("Deletes everything from the cursor to the end of the line.")
{
    View_ID view = get_active_view(app, Access_ReadWriteVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
    i64 pos = view_get_cursor_pos(app, view);
    i64 line = get_line_number_from_pos(app, buffer, pos);
    Range_i64 range = get_line_pos_range(app, buffer, line);
	if(pos == range.end)
	{
		range.end = pos + 1;
		range.start = pos;
	}
	else
	{
		range.start = pos + 1;
	}
    
    i32 size = (i32)buffer_get_size(app, buffer);
    range.end = clamp_top(range.end, size);
    if (range_size(range) == 0 ||
        buffer_get_char(app, buffer, range.end - 1) != '\n'){
        range.start -= 1;
        range.first = clamp_bot(0, range.first);
    }
    buffer_replace_range(app, buffer, range, string_u8_litexpr(""));
}

CUSTOM_COMMAND_SIG(casey_find_matching_file)
CUSTOM_DOC("If the current file is a *.cpp or *.h, attempts to open the corresponding *.h or *.cpp file in the same view.")
{
    View_ID view = get_active_view(app, Access_Always);
    Buffer_ID buffer = view_get_buffer(app, view, Access_Always);
    Buffer_ID new_buffer = 0;
    if (get_cpp_matching_file(app, buffer, &new_buffer)){
        view_set_buffer(app, view, new_buffer, 0);
    }
}

CUSTOM_COMMAND_SIG(casey_go_to_code_peek)
CUSTOM_DOC("Jumps to the most likely thing you'd want to see for the identifier you're on.")
{
    fleury_go_to_definition(app);
}

BUFFER_HOOK_SIG(casey_new_file)
{
	Scratch_Block scratch(app);
	String_Const_u8 file_name = push_buffer_file_name(app, scratch, buffer_id);
    String_Const_u8 header_text = push_u8_stringf(scratch,
                                                  "/* ========================================================================\n"
                                                  "   %cFile: %.*s $\n"
                                                  "   %cDate: $\n"
                                                  "   %cRevision: $\n"
                                                  "   %cCreator: Casey Muratori $\n"
                                                  "   %cNotice: (C) Copyright by Molly Rocket, Inc., All Rights Reserved. $\n"
                                                  "   ======================================================================== */\n\n",
                                                  '$', string_expand(file_name),
                                                  '$', '$', '$', '$');
    
    buffer_replace_range(app, buffer_id, Ii64(0, 0), header_text);
    
    return(0);
}

