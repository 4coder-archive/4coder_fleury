/* date = January 29th 2021 8:01 pm */

#ifndef FCODER_FLEURY_POWER_MODE_H
#define FCODER_FLEURY_POWER_MODE_H

//~ NOTE(rjf): Power Mode API

struct Particle
{
    f32 x;
    f32 y;
    f32 velocity_x;
    f32 velocity_y;
    f32 decay_rate;
    ARGB_Color color;
    f32 alpha;
    f32 roundness;
    f32 scale;
    String_Const_u8 string;
    u8 chrs[1];
};

internal void F4_PowerMode_SetAllow(b32 allowed);
internal b32 F4_PowerMode_IsEnabled(void);
internal f32 F4_PowerMode_ScreenShake(void);
internal f32 F4_PowerMode_ActiveCharactersPerMinute(void);
internal void F4_PowerMode_CharacterPressed(void);
internal Particle *
F4_PowerMode_Particle(f32 x, f32 y, f32 velocity_x, f32 velocity_y, f32 decay_rate, ARGB_Color color,
                      f32 roundness, f32 scale, String_Const_u8 str);
internal Vec2_f32 F4_PowerMode_CameraOffsetFromView(Application_Links *app, View_ID view);
internal void F4_PowerMode_Spawn(Application_Links *app, View_ID view, u8 character);
internal void F4_PowerMode_Tick(Application_Links *app, Frame_Info frame_info);
internal void F4_PowerMode_RenderBuffer(Application_Links *app, View_ID view, Face_ID face, Frame_Info frame_info);
internal void F4_PowerMode_RenderWholeScreen(Application_Links *app, Frame_Info frame_info);

#endif // FCODER_FLEURY_POWER_MODE_H
