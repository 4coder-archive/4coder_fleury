global F4_Language_State f4_langs = {};

internal F4_Language *
F4_LanguageFromString(String_Const_u8 name)
{
    F4_Language *result = 0;
    if(f4_langs.initialized)
    {
        u64 hash = table_hash_u8(name.str, name.size);
        u64 slot = hash % ArrayCount(f4_langs.language_table);
        for(F4_Language *l = f4_langs.language_table[slot]; l; l = l->next)
        {
            if(l->hash == hash && string_match(l->name, name))
            {
                result = l;
                break;
            }
        }
    }
    return result;
}

#define F4_RegisterLanguage(name, IndexFile, LexInit, LexFullInput, PosContext, Highlight, lex_state_type) _F4_RegisterLanguage(name, IndexFile, (F4_Language_LexInit *)LexInit, (F4_Language_LexFullInput *)LexFullInput, (F4_Language_PosContext *)PosContext, (F4_Language_Highlight *)Highlight, sizeof(lex_state_type))

internal void
_F4_RegisterLanguage(String_Const_u8 name,
                     F4_Language_IndexFile          *IndexFile,
                     F4_Language_LexInit            *LexInit,
                     F4_Language_LexFullInput       *LexFullInput,
                     F4_Language_PosContext         *PosContext,
                     F4_Language_Highlight          *Highlight,
                     u64 lex_state_size)
{
    if(f4_langs.initialized == 0)
    {
        f4_langs.initialized = 1;
        f4_langs.arena = make_arena_system(KB(16));
    }
    
    F4_Language *language = 0;
    u64 hash = table_hash_u8(name.str, name.size);
    u64 slot = hash % ArrayCount(f4_langs.language_table);
    for(F4_Language *l = f4_langs.language_table[slot]; l; l = l->next)
    {
        if(l->hash == hash && string_match(l->name, name))
        {
            language = l;
            break;
        }
    }
    
    if(language == 0)
    {
        language = push_array(&f4_langs.arena, F4_Language, 1);
        language->next = f4_langs.language_table[slot];
        f4_langs.language_table[slot] = language;
        language->hash = hash;
        language->name = push_string_copy(&f4_langs.arena, name);
        language->lex_state_size     = lex_state_size;
        language->IndexFile          = IndexFile;
        language->LexInit            = LexInit;
        language->LexFullInput       = LexFullInput;
		language->PosContext         = PosContext;
        language->Highlight          = Highlight;
    }
}

internal F4_Language *
F4_LanguageFromBuffer(Application_Links *app, Buffer_ID buffer)
{
    F4_Language *language = 0;
    Scratch_Block scratch(app);
    String_Const_u8 file_name = push_buffer_file_name(app, scratch, buffer);
    String_Const_u8 extension = string_file_extension(file_name);
    language = F4_LanguageFromString(extension);
    return language;
}

internal void
F4_Language_PosContext_PushData(Arena *arena,
                                F4_Language_PosContextData **first_ptr,
                                F4_Language_PosContextData **last_ptr,
                                F4_Index_Note *note,
                                Token *query,
                                int arg_index)
{
    F4_Language_PosContextData *first = *first_ptr;
    F4_Language_PosContextData *last = *last_ptr;
    F4_Language_PosContextData *func = push_array_zero(arena, F4_Language_PosContextData, 1);
    func->relevant_note = note;
    func->query_token = query;
    func->argument_index = arg_index;
    if(last == 0)
    {
        first = last = func;
    }
    else
    {
        last->next = func;
        last = last->next;
    }
    *first_ptr = first;
    *last_ptr = last;
}

internal void
F4_Language_PosContext_PushData_Call(Arena *arena,
                                     F4_Language_PosContextData **first_ptr,
                                     F4_Language_PosContextData **last_ptr,
                                     String_Const_u8 string, int param_idx)
{
    F4_Language_PosContext_PushData(arena, first_ptr, last_ptr, F4_Index_LookupNote(string, 0), 0, param_idx);
}

internal void
F4_Language_PosContext_PushData_Dot(Arena *arena,
                                    F4_Language_PosContextData **first_ptr,
                                    F4_Language_PosContextData **last_ptr,
                                    String_Const_u8 string, Token *query)
{
    F4_Language_PosContext_PushData(arena, first_ptr, last_ptr, F4_Index_LookupNote(string, 0), query, 0);
}

internal Token_List
F4_Language_LexFullInput_NoBreaks(Application_Links *app, F4_Language *language, Arena *arena, String_Const_u8 text)
{
    Token_List list = {};
    if(language != 0)
    {
        Scratch_Block scratch(app, arena);
        void *state = push_array_zero(scratch, u8, language->lex_state_size);
		language->LexInit(state, text);
        language->LexFullInput(arena, &list, state, max_u64);
    }
    return list;
}

#include "4coder_fleury_lang_list.h"