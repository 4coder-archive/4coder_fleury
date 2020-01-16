
//~ NOTE(rjf): Light/Dark Mode

static void Fleury4LightMode(Application_Links *app);
static void Fleury4DarkMode(Application_Links *app);

static void
Fleury4LightMode(Application_Links *app)
{
    Color_Table *table = &active_color_table;
    Arena *arena = &global_theme_arena;
    linalloc_clear(arena);
    *table = make_color_table(app, arena);
    
    table->arrays[defcolor_bar]                   = make_colors(arena, 0xFFd9dfe2);
    table->arrays[defcolor_base]                  = make_colors(arena, 0xff806d56);
    table->arrays[defcolor_pop1]                  = make_colors(arena, 0xffde9343);
    table->arrays[defcolor_pop2]                  = make_colors(arena, 0xFFFF0000);
    table->arrays[defcolor_back]                  = make_colors(arena, 0xFFd9dfe2);
    table->arrays[defcolor_margin]                = make_colors(arena, 0xFFd9dfe2);
    table->arrays[defcolor_margin_hover]          = make_colors(arena, 0xff63523d);
    table->arrays[defcolor_margin_active]         = make_colors(arena, 0xff63523d);
    table->arrays[defcolor_list_item]             = make_colors(arena, 0xFF222425);
    table->arrays[defcolor_list_item_hover]       = make_colors(arena, 0xff63523d);
    table->arrays[defcolor_list_item_active]      = make_colors(arena, 0xff63523d);
    table->arrays[defcolor_cursor]                = make_colors(arena, 0xFF00EE00);
    table->arrays[defcolor_at_cursor]             = make_colors(arena, 0xFF0C0C0C);
    table->arrays[defcolor_highlight_cursor_line] = make_colors(arena, 0xFFd9dfe2);
    table->arrays[defcolor_highlight]             = make_colors(arena, 0xFFDDEE00);
    table->arrays[defcolor_at_highlight]          = make_colors(arena, 0xFFFF44DD);
    table->arrays[defcolor_mark]                  = make_colors(arena, 0xFF494949);
    table->arrays[defcolor_text_default]          = make_colors(arena, 0xff806d56);
    table->arrays[defcolor_comment]               = make_colors(arena, 0xff9ba290);
    table->arrays[defcolor_comment_pop]           = make_colors(arena, 0xff2ab34f, 0xFFdb2828);
    table->arrays[defcolor_keyword]               = make_colors(arena, 0xff3b3733);
    table->arrays[defcolor_str_constant]          = make_colors(arena, 0xffb67900);
    table->arrays[defcolor_char_constant]         = make_colors(arena, 0xffb67900);
    table->arrays[defcolor_int_constant]          = make_colors(arena, 0xffb67900);
    table->arrays[defcolor_float_constant]        = make_colors(arena, 0xffb67900);
    table->arrays[defcolor_bool_constant]         = make_colors(arena, 0xffb67900);
    table->arrays[defcolor_preproc]               = make_colors(arena, 0xFFdc7575);
    table->arrays[defcolor_include]               = make_colors(arena, 0xffb67900);
    table->arrays[defcolor_special_character]     = make_colors(arena, 0xFFFF0000);
    table->arrays[defcolor_ghost_character]       = make_colors(arena, 0xFF4E5E46);
    table->arrays[defcolor_highlight_junk]        = make_colors(arena, 0xFF3A0000);
    table->arrays[defcolor_highlight_white]       = make_colors(arena, 0xFF003A3A);
    table->arrays[defcolor_paste]                 = make_colors(arena, 0xFFDDEE00);
    table->arrays[defcolor_undo]                  = make_colors(arena, 0xFF00DDEE);
    table->arrays[defcolor_back_cycle]            = make_colors(arena, 0xffd9dfe2, 0xffd9dfe2, 0xffd9dfe2, 0xff9db8c5);
    table->arrays[defcolor_text_cycle]            = make_colors(arena, 0xFFA00000, 0xFF00A000, 0xFF0030B0, 0xFFA0A000);
    table->arrays[defcolor_line_numbers_back]     = make_colors(arena, 0xFF101010);
    table->arrays[defcolor_line_numbers_text]     = make_colors(arena, 0xFF404040);
}

static void
Fleury4DarkMode(Application_Links *app)
{
    Color_Table *table = &active_color_table;
    Arena *arena = &global_theme_arena;
    linalloc_clear(arena);
    *table = make_color_table(app, arena);
    
    table->arrays[defcolor_bar]                   = make_colors(arena, 0xFF222425);
    table->arrays[defcolor_base]                  = make_colors(arena, 0xffb99468);
    table->arrays[defcolor_pop1]                  = make_colors(arena, 0xffde8150);
    table->arrays[defcolor_pop2]                  = make_colors(arena, 0xFFFF0000);
    table->arrays[defcolor_back]                  = make_colors(arena, 0xFF222425);
    table->arrays[defcolor_margin]                = make_colors(arena, 0xFF222425);
    table->arrays[defcolor_margin_hover]          = make_colors(arena, 0xff63523d);
    table->arrays[defcolor_margin_active]         = make_colors(arena, 0xff63523d);
    table->arrays[defcolor_list_item]             = make_colors(arena, 0xFF222425);
    table->arrays[defcolor_list_item_hover]       = make_colors(arena, 0xff63523d);
    table->arrays[defcolor_list_item_active]      = make_colors(arena, 0xff63523d);
    table->arrays[defcolor_cursor]                = make_colors(arena, 0xFF00EE00);
    table->arrays[defcolor_at_cursor]             = make_colors(arena, 0xFF0C0C0C);
    table->arrays[defcolor_highlight_cursor_line] = make_colors(arena, 0xFF1E1E1E);
    table->arrays[defcolor_highlight]             = make_colors(arena, 0xFFDDEE00);
    table->arrays[defcolor_at_highlight]          = make_colors(arena, 0xFFFF44DD);
    table->arrays[defcolor_mark]                  = make_colors(arena, 0xFF494949);
    table->arrays[defcolor_text_default]          = make_colors(arena, 0xffb99468);
    table->arrays[defcolor_comment]               = make_colors(arena, 0xff9ba290);
    table->arrays[defcolor_comment_pop]           = make_colors(arena, 0xff2ab34f, 0xFFdb2828);
    table->arrays[defcolor_keyword]               = make_colors(arena, 0xfff0c674);
    table->arrays[defcolor_str_constant]          = make_colors(arena, 0xffffa900);
    table->arrays[defcolor_char_constant]         = make_colors(arena, 0xffffa900);
    table->arrays[defcolor_int_constant]          = make_colors(arena, 0xffffa900);
    table->arrays[defcolor_float_constant]        = make_colors(arena, 0xffffa900);
    table->arrays[defcolor_bool_constant]         = make_colors(arena, 0xffffa900);
    table->arrays[defcolor_preproc]               = make_colors(arena, 0xFFdc7575);
    table->arrays[defcolor_include]               = make_colors(arena, 0xffffa900);
    table->arrays[defcolor_special_character]     = make_colors(arena, 0xFFFF0000);
    table->arrays[defcolor_ghost_character]       = make_colors(arena, 0xFF4E5E46);
    table->arrays[defcolor_highlight_junk]        = make_colors(arena, 0xFF3A0000);
    table->arrays[defcolor_highlight_white]       = make_colors(arena, 0xFF003A3A);
    table->arrays[defcolor_paste]                 = make_colors(arena, 0xFFDDEE00);
    table->arrays[defcolor_undo]                  = make_colors(arena, 0xFF00DDEE);
    table->arrays[defcolor_back_cycle]            = make_colors(arena, 0xFF222425, 0xff1e1f20, 0xff1e1f20, 0xff13141);
    table->arrays[defcolor_text_cycle]            = make_colors(arena, 0xFFA00000, 0xFF00A000, 0xFF0030B0, 0xFFA0A000);
    table->arrays[defcolor_line_numbers_back]     = make_colors(arena, 0xFF101010);
    table->arrays[defcolor_line_numbers_text]     = make_colors(arena, 0xFF404040);
}

CUSTOM_COMMAND_SIG(fleury_toggle_colors)
CUSTOM_DOC("Toggles light/dark mode.")
{
    if(global_dark_mode)
    {
        Fleury4LightMode(app);
        global_dark_mode = 0;
    }
    else
    {
        Fleury4DarkMode(app);
        global_dark_mode = 1;
    }
}
