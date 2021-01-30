#ifndef FCODER_FLEURY_LANG_H
#define FCODER_FLEURY_LANG_H

struct F4_Index_ParseCtx;

//~ NOTE(rjf): Indexes an entire file of a language and adds stuff to the code index.
#define F4_LANGUAGE_INDEXFILE(name) void name(F4_Index_ParseCtx *ctx)
typedef F4_LANGUAGE_INDEXFILE(F4_Language_IndexFile);

//~ NOTE(rjf): Initializes lexer state.
#define F4_LANGUAGE_LEXINIT(name) void name(void *state_ptr, String_Const_u8 contents)
typedef F4_LANGUAGE_LEXINIT(F4_Language_LexInit);

//~ NOTE(rjf): Lexes an entire file to produce tokens for the language (with breaks).
#define F4_LANGUAGE_LEXFULLINPUT(name) b32 name(Arena *arena, Token_List *list, void *state_ptr, u64 max)
typedef F4_LANGUAGE_LEXFULLINPUT(F4_Language_LexFullInput);

//~ NOTE(rjf): Figures out some language-specific contextual information given some
// position in a buffer. For example, what type I am accessing, what function I am
// calling which parameter am I accessing, etc.

struct F4_Index_Note;
struct F4_Language_PosContextData
{
    F4_Language_PosContextData *next;
    F4_Index_Note *relevant_note;
    Token *query_token;
    int argument_index;
};

#define F4_LANGUAGE_POSCONTEXT(name) F4_Language_PosContextData * name(Application_Links *app, Arena *arena, Buffer_ID buffer, i64 pos)
typedef F4_LANGUAGE_POSCONTEXT(F4_Language_PosContext);

//~ NOTE(rjf): Does language-specific coloring for a buffer, for special-case syntax
// highlighting and stuff. Most syntax highlighting is handled through token-base-kinds
// and index lookups, but sometimes it's useful to be able to override colors by
// language-specific token sub-kinds and flags, so that's what this hook is for.
#define F4_LANGUAGE_HIGHLIGHT(name) void name(Application_Links *app, Text_Layout_ID text_layout_id, Token_Array *array, Color_Table color_table)
typedef F4_LANGUAGE_HIGHLIGHT(F4_Language_Highlight);

//~

struct F4_Language
{
    F4_Language *next;
    u64 hash;
    String_Const_u8 name;
    u64 lex_state_size;
    F4_Language_IndexFile            *IndexFile;
    F4_Language_LexInit              *LexInit;
    F4_Language_LexFullInput         *LexFullInput;
    F4_Language_PosContext           *PosContext;
    F4_Language_Highlight            *Highlight;
};

struct F4_Language_State
{
    b32 initialized;
    Arena arena;
    F4_Language *language_table[4096];
};

#endif // FCODER_FLEURY_LANG_H
