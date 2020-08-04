
typedef struct _MemoryArena _MemoryArena;
struct _MemoryArena
{
    void *buffer;
    u32 buffer_size;
    u32 alloc_position;
    u32 bytes_left;
};

static Face_ID global_styled_title_face = 0;
static Face_ID global_styled_label_face = 0;
static Face_ID global_small_code_face = 0;
static Rect_f32 global_cursor_rect = {0};
static Rect_f32 global_last_cursor_rect = {0};
static Rect_f32 global_mark_rect = {0};
static Rect_f32 global_last_mark_rect = {0};
static b32 global_dark_mode = 1;
static b32 global_battery_saver = 0;

static struct
{
    String_Const_u8 string;
    ARGB_Color color;
}
global_tooltips[32] = {0};
static int global_tooltip_count = 0;
static _MemoryArena _global_frame_arena = {0};
static Arena global_frame_arena;

#if 0
static _MemoryArena
_MemoryArenaInit(void *buffer, u32 buffer_size)
{
    _MemoryArena arena = {0};
    arena.buffer = buffer;
    arena.buffer_size = buffer_size;
    arena.bytes_left = arena.buffer_size;
    return arena;
}

static void *
_MemoryArenaAllocate(MemoryArena *arena, u32 size)
{
    void *memory = 0;
    if(arena->bytes_left >= size)
    {
        memory = (char *)arena->buffer + arena->alloc_position;
        arena->alloc_position += size;
        arena->bytes_left -= size;
        u32 bytes_to_align = arena->alloc_position % 16;
        arena->alloc_position += bytes_to_align;
        arena->bytes_left -= bytes_to_align;
    }
    return memory;
}

static char *
MakeCStringOnMemoryArena(MemoryArena *arena, char *format, ...)
{
    char *result = 0;
    va_list args;
    va_start(args, format);
    int required_bytes = vsnprintf(0, 0, format, args)+1;
    va_end(args);
    result = (char *)MemoryArenaAllocate(arena, required_bytes);
    va_start(args, format);
    vsnprintf(result, required_bytes, format, args);
    va_end(args);
    result[required_bytes-1] = 0;
    return result;
}

static void
MemoryArenaClear(MemoryArena *arena)
{
    arena->bytes_left = arena->buffer_size;
    arena->alloc_position = 0;
}
#endif

static String_Const_u8
StringStripBorderCharacters(String_Const_u8 string)
{
    string.str += 1;
    string.size -= 2;
    return string;
}

static f32
RandomF32(f32 low, f32 high)
{
    return low + (high - low) * (((int)rand() % 10000) / 10000.f);
}

static f32
MinimumF32(f32 a, f32 b)
{
    return a < b ? a : b;
}

static f32
MaximumF32(f32 a, f32 b)
{
    return a > b ? a : b;
}

static ARGB_Color
ARGBFromID(Managed_ID id)
{
    return fcolor_resolve(fcolor_id(id));
}

static b32
CharIsAlpha(int c)
{
    return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}

static b32
CharIsDigit(int c)
{
    return (c >= '0' && c <= '9');
}

static b32
CharIsSymbol(int c)
{
    return (c == '~' ||
            c == '`' ||
            c == '!' ||
            c == '#' ||
            c == '$' ||
            c == '%' ||
            c == '^' ||
            c == '&' ||
            c == '*' ||
            c == '(' ||
            c == ')' ||
            c == '-' ||
            c == '+' ||
            c == '=' ||
            c == '{' ||
            c == '}' ||
            c == '[' ||
            c == ']' ||
            c == ':' ||
            c == ';' ||
            c == '<' ||
            c == '>' ||
            c == ',' ||
            c == '.' ||
            c == '?' ||
            c == '/');
}

static double
GetFirstDoubleFromBuffer(char *buffer)
{
    char number_str[256];
    int number_write_pos = 0;
    double value = 0;
    for(int i = 0; buffer[i] && number_write_pos < sizeof(number_str); ++i)
    {
        if(CharIsDigit(buffer[i]) || buffer[i] == '.' || buffer[i] == '-')
        {
            number_str[number_write_pos++] = buffer[i];
        }
        else
        {
            number_str[number_write_pos++] = 0;
            break;
        }
    }
    number_str[sizeof(number_str)-1] = 0;
    value = atof(number_str);
    return value;
}

static unsigned int CStringCRC32(char *string);
static unsigned int StringCRC32(char *string, int string_length);
static unsigned int CRC32(unsigned char *buf, int len);
static int
StringMatchCaseSensitive(char *a, int a_length, char *b, int b_length)
{
    int match = 0;
    if(a && b && a[0] && b[0] && a_length == b_length)
    {
        match = 1;
        for(int i = 0; i < a_length; ++i)
        {
            if(a[i] != b[i])
            {
                match = 0;
                break;
            }
        }
    }
    return match;
}

#define MemorySet                 memset
#define MemoryCopy                memcpy
#define CalculateCStringLength    strlen

static unsigned int
CRC32(unsigned char *buf, int len)
{
    static unsigned int init = 0xffffffff;
    static const unsigned int crc32_table[] =
    {
        0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9,
        0x130476dc, 0x17c56b6b, 0x1a864db2, 0x1e475005,
        0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61,
        0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd,
        0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9,
        0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
        0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011,
        0x791d4014, 0x7ddc5da3, 0x709f7b7a, 0x745e66cd,
        0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
        0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5,
        0xbe2b5b58, 0xbaea46ef, 0xb7a96036, 0xb3687d81,
        0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
        0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49,
        0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95,
        0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1,
        0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d,
        0x34867077, 0x30476dc0, 0x3d044b19, 0x39c556ae,
        0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
        0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16,
        0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca,
        0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde,
        0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02,
        0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1, 0x53dc6066,
        0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
        0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e,
        0xbfa1b04b, 0xbb60adfc, 0xb6238b25, 0xb2e29692,
        0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6,
        0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a,
        0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e,
        0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
        0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686,
        0xd5b88683, 0xd1799b34, 0xdc3abded, 0xd8fba05a,
        0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637,
        0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb,
        0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f,
        0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
        0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47,
        0x36194d42, 0x32d850f5, 0x3f9b762c, 0x3b5a6b9b,
        0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
        0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623,
        0xf12f560e, 0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7,
        0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
        0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f,
        0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3,
        0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7,
        0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b,
        0x9b3660c6, 0x9ff77d71, 0x92b45ba8, 0x9675461f,
        0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
        0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640,
        0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c,
        0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8,
        0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24,
        0x119b4be9, 0x155a565e, 0x18197087, 0x1cd86d30,
        0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
        0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088,
        0x2497d08d, 0x2056cd3a, 0x2d15ebe3, 0x29d4f654,
        0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0,
        0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c,
        0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18,
        0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
        0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0,
        0x9abc8bd5, 0x9e7d9662, 0x933eb0bb, 0x97ffad0c,
        0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668,
        0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4
    };
    
    unsigned int crc = init;
    while(len--)
    {
        crc = (crc << 8) ^ crc32_table[((crc >> 24) ^ *buf) & 255];
        buf++;
    }
    return crc;
}

static unsigned int
StringCRC32(char *string, int string_length)
{
    unsigned int hash = CRC32((unsigned char *)string, string_length);
    return hash;
}

static unsigned int
CStringCRC32(char *string)
{
    int string_length = (int)CalculateCStringLength(string);
    unsigned int hash = CRC32((unsigned char *)string, string_length);
    return hash;
}

static Code_Index_Note *
F4_LookUpStringInCodeIndex(Application_Links *app, String_Const_u8 string)
{
    Code_Index_Note *note = 0;
    
    if (string.str)
    {
        for (Buffer_ID buffer_it = get_buffer_next(app, 0, Access_Always);
             buffer_it != 0; buffer_it = get_buffer_next(app, buffer_it, Access_Always))
        {
            Code_Index_File* file = code_index_get_file(buffer_it);
            if (file != 0)
            {
                for (i32 i = 0; i < file->note_array.count; i += 1)
                {
                    Code_Index_Note* this_note = file->note_array.ptrs[i];
                    
                    if (string_match(this_note->text, string))
                    {
                        note = this_note;
                        break;
                    }
                }
            }
        }
    }
    return note;
}

static Code_Index_Note *
F4_LookUpTokenInCodeIndex(Application_Links *app, Buffer_ID buffer, Token token)
{
    Code_Index_Note *note = 0;
    Scratch_Block scratch(app);
    String_Const_u8 string = push_buffer_range(app, scratch, buffer, Ii64(token.pos, token.pos + token.size));
    note = F4_LookUpStringInCodeIndex(app, string);
    return note;
}

static ARGB_Color
F4_GetCTokenColor(Token token)
{
    ARGB_Color color = ARGBFromID(defcolor_text_default);
    
    switch(token.kind)
    {
        case TokenBaseKind_Preprocessor:     color = ARGBFromID(defcolor_preproc); break;
        case TokenBaseKind_Keyword:          color = ARGBFromID(defcolor_keyword); break;
        case TokenBaseKind_Comment:          color = ARGBFromID(defcolor_comment); break;
        case TokenBaseKind_LiteralString:    color = ARGBFromID(defcolor_str_constant); break;
        case TokenBaseKind_LiteralInteger:   color = ARGBFromID(defcolor_int_constant); break;
        case TokenBaseKind_LiteralFloat:     color = ARGBFromID(defcolor_float_constant); break;
        case TokenBaseKind_Operator:         color = ARGBFromID(defcolor_preproc); break;
        
        case TokenBaseKind_ScopeOpen:
        case TokenBaseKind_ScopeClose:
        case TokenBaseKind_ParentheticalOpen:
        case TokenBaseKind_ParentheticalClose:
        case TokenBaseKind_StatementClose:
        {
            u32 r = (color & 0x00ff0000) >> 16;
            u32 g = (color & 0x0000ff00) >>  8;
            u32 b = (color & 0x000000ff) >>  0;
            
            if(global_dark_mode)
            {
                r = (r * 3) / 5;
                g = (g * 3) / 5;
                b = (b * 3) / 5;
            }
            else
            {
                r = (r * 4) / 3;
                g = (g * 4) / 3;
                b = (b * 4) / 3;
            }
            
            color = 0xff000000 | (r << 16) | (g << 8) | (b << 0);
            
            break;
        }
        
        default:
        {
            switch(token.sub_kind)
            {
                case TokenCppKind_LiteralTrue:
                case TokenCppKind_LiteralFalse:
                {
                    color = ARGBFromID(defcolor_bool_constant);
                    break;
                }
                case TokenCppKind_LiteralCharacter:
                case TokenCppKind_LiteralCharacterWide:
                case TokenCppKind_LiteralCharacterUTF8:
                case TokenCppKind_LiteralCharacterUTF16:
                case TokenCppKind_LiteralCharacterUTF32:
                {
                    color = ARGBFromID(defcolor_char_constant);
                    break;
                }
                case TokenCppKind_PPIncludeFile:
                {
                    color = ARGBFromID(defcolor_include);
                    break;
                }
            }
            break;
        }
    }
    
    return color;
}

static void
F4_DrawCTokenColors(Application_Links *app, Text_Layout_ID text_layout_id, Token_Array *array)
{
    Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
    i64 first_index = token_index_from_pos(array, visible_range.first);
    Token_Iterator_Array it = token_iterator_index(0, array, first_index);
    
    for(;;)
    {
        Token *token = token_it_read(&it);
        if(!token || token->pos >= visible_range.one_past_last)
        {
            break;
        }
        ARGB_Color argb = F4_GetCTokenColor(*token);
        
        if(token->kind == TokenBaseKind_Identifier && token_it_inc_all(&it))
        {
            Token *second_token = token_it_read(&it);
            token_it_dec_all(&it);
            
            // NOTE(rjf): Function or macro?
            if(second_token && second_token->kind == TokenBaseKind_ParentheticalOpen &&
               second_token->sub_kind == TokenCppKind_ParenOp)
            {
                argb = fcolor_resolve(fcolor_id(defcolor_pop1));
            }
            
            // NOTE(rjf): Is this a type?
            else
            {
                // TODO(rjf): When we can look up into the code index by table,
                // let's totally do that here. Otherwise this is way too slow.
#if 0
                Buffer_ID buffer = text_layout_get_buffer(app, text_layout_id);
                if(buffer)
                {
                    Code_Index_Note *note = 0;
                    
                    // NOTE(rjf): Look up token.
                    {
                        ProfileScope(app, "[Fleury] Code Index Token Look-Up");
                        Code_Index_Note *note = F4_LookUpTokenInCodeIndex(app, buffer, *token);
                    }
                    
                    if(note && note->note_kind == CodeIndexNote_Type)
                    {
                        argb = fcolor_resolve(fcolor_id(defcolor_pop2));
                    }
                }
#endif
            }
        }
        
        paint_text_color(app, text_layout_id, Ii64_size(token->pos, token->size), argb);
        if(!token_it_inc_all(&it))
        {
            break;
        }
    }
}

static void
F4_DrawTooltipRect(Application_Links *app, Rect_f32 rect)
{
    ARGB_Color background_color = fcolor_resolve(fcolor_id(defcolor_back));
    ARGB_Color border_color = fcolor_resolve(fcolor_id(defcolor_margin_active));
    
    background_color &= 0x00ffffff;
    background_color |= 0xd0000000;
    
    border_color &= 0x00ffffff;
    border_color |= 0xd0000000;
    
    draw_rectangle(app, rect, 4.f, background_color);
    draw_rectangle_outline(app, rect, 4.f, 3.f, border_color);
}

static void
F4_RenderRangeHighlight(Application_Links *app, View_ID view_id, Text_Layout_ID text_layout_id,
                        Range_i64 range)
{
    Rect_f32 range_start_rect = text_layout_character_on_screen(app, text_layout_id, range.start);
    Rect_f32 range_end_rect = text_layout_character_on_screen(app, text_layout_id, range.end-1);
    Rect_f32 total_range_rect = {0};
    total_range_rect.x0 = MinimumF32(range_start_rect.x0, range_end_rect.x0);
    total_range_rect.y0 = MinimumF32(range_start_rect.y0, range_end_rect.y0);
    total_range_rect.x1 = MaximumF32(range_start_rect.x1, range_end_rect.x1);
    total_range_rect.y1 = MaximumF32(range_start_rect.y1, range_end_rect.y1);
    
    ARGB_Color background_color = fcolor_resolve(fcolor_id(defcolor_pop2));
    float background_color_r = (float)((background_color & 0x00ff0000) >> 16) / 255.f;
    float background_color_g = (float)((background_color & 0x0000ff00) >>  8) / 255.f;
    float background_color_b = (float)((background_color & 0x000000ff) >>  0) / 255.f;
    background_color_r += (1.f - background_color_r) * 0.5f;
    background_color_g += (1.f - background_color_g) * 0.5f;
    background_color_b += (1.f - background_color_b) * 0.5f;
    ARGB_Color highlight_color = (0x55000000 |
                                  (((u32)(background_color_r * 255.f)) << 16) |
                                  (((u32)(background_color_g * 255.f)) <<  8) |
                                  (((u32)(background_color_b * 255.f)) <<  0));
    draw_rectangle(app, total_range_rect, 4.f, highlight_color);
}

static void
F4_PushTooltip(String_Const_u8 string, ARGB_Color color)
{
    if(global_tooltip_count < ArrayCount(global_tooltips))
    {
        String_Const_u8 string_copy = push_string_copy(&global_frame_arena, string);
        global_tooltips[global_tooltip_count].color = color;
        global_tooltips[global_tooltip_count].string = string_copy;
        global_tooltip_count += 1;
    }
}