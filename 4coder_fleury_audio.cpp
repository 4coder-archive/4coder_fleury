
function b32
F4_AudioClipIsValid(Audio_Clip clip)
{
    return clip.samples != 0 && clip.samples != (i16 *)0x00000001;
}

function b32
F4_AudioClipIsUnloadable(Audio_Clip clip)
{
    return clip.samples == (i16 *)0x00000001;
}

function void
F4_RequireWAV(Application_Links *app, Audio_Clip *clip, char *filename)
{
    if(!F4_AudioClipIsValid(*clip) && !F4_AudioClipIsUnloadable(*clip))
    {
        FILE *file = def_search_normal_fopen(&permanent_arena, filename, "rb");
        if(file != 0)
        {
            *clip = audio_clip_from_wav_FILE(&permanent_arena, file);
            fclose(file);
        }
        else
        {
            clip->samples = (i16 *)0x00000001;
        }
    }
}