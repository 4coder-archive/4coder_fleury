//~ NOTE(rjf): Power Mode Implementation

Audio_Clip f4_powermode_music = {};
Audio_Control f4_powermode_music_ctrl = {};
Audio_Clip f4_powermode_keystroke_sounds[10] = {};

static struct
{
    b32 allowed;
    b32 enabled;
    f32 enabled_t;
    int ticks_to_enable;
    int ticks_to_disable;
    
    int particle_count;
    Particle particles[4096];
    
    int keypress_history_count;
    struct
    {
        u64 time;
    }
    keypress_history[64];
    
    f32 screen_shake;
}
power_mode;

internal void
F4_PowerMode_SetAllow(b32 allowed)
{
    power_mode.allowed = allowed;
}

CUSTOM_COMMAND_SIG(f4_powermode_allow)
CUSTOM_DOC("Allow power mode.")
{ F4_PowerMode_SetAllow(1); }

CUSTOM_COMMAND_SIG(f4_powermode_disallow)
CUSTOM_DOC("Disallow power mode.")
{ F4_PowerMode_SetAllow(0); }

internal b32
F4_PowerMode_IsEnabled(void)
{
    return power_mode.enabled;
}

internal f32
F4_PowerMode_ScreenShake(void)
{
    return power_mode.screen_shake;
}

internal f32
F4_PowerMode_ActiveCharactersPerMinute(void)
{
    f32 result = 0.f;
    if(power_mode.allowed && power_mode.keypress_history_count > 1)
    {
        u64 total_time__microseconds = power_mode.keypress_history[power_mode.keypress_history_count-1].time - power_mode.keypress_history[0].time;
        f32 total_time__seconds = (f32)(((f64)total_time__microseconds) / 1000.0 / 1000.0);
        f32 total_time__minutes = total_time__seconds / 60.f;
        result = (f32)power_mode.keypress_history_count / total_time__minutes;
    }
    return result;
}

internal void
F4_PowerMode_CharacterPressed(void)
{
    u64 now = system_now_time();
    
    if(power_mode.keypress_history_count > 0)
    {
        u64 last_keypress_time = power_mode.keypress_history[power_mode.keypress_history_count-1].time;
        u64 microseconds_since_last_keypress = now - last_keypress_time;
        f32 seconds_since_last_keypress = (f32)((f64)microseconds_since_last_keypress / 1000.0 / 1000.0);
        if(seconds_since_last_keypress > 10.f)
        {
            power_mode.keypress_history_count = 0;
        }
    }
    
    if(power_mode.keypress_history_count >= ArrayCount(power_mode.keypress_history))
    {
        memmove(power_mode.keypress_history + 0, power_mode.keypress_history + 1,
                (ArrayCount(power_mode.keypress_history) - 1) * sizeof(power_mode.keypress_history[0]));
        power_mode.keypress_history_count -= 1;
    }
    power_mode.keypress_history[power_mode.keypress_history_count].time = now;
    power_mode.keypress_history_count += 1;
    
    if(F4_PowerMode_IsEnabled())
    {
        int rando = (int)rand();
        def_audio_play_clip(f4_powermode_keystroke_sounds[rando % ArrayCount(f4_powermode_keystroke_sounds)], 0);
    }
}

internal Particle *
F4_PowerMode_Particle(f32 x, f32 y, f32 velocity_x, f32 velocity_y, f32 decay_rate, ARGB_Color color,
                      f32 roundness, f32 scale, String_Const_u8 str)
{
    Particle *result = 0;
    if(power_mode.particle_count < ArrayCount(power_mode.particles))
    {
        int i = power_mode.particle_count++;
        result = power_mode.particles + i;
        result->x = x;
        result->y = y;
        result->velocity_x = velocity_x;
        result->velocity_y = velocity_y;
        result->decay_rate = decay_rate;
        result->color = color;
        result->alpha = 1.f;
        result->roundness = roundness;
        result->scale = scale;
        result->string = str;
    }
    return result;
}

internal Vec2_f32
F4_PowerMode_CameraOffsetFromView(Application_Links *app, View_ID view)
{
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
    Buffer_Scroll scroll = view_get_buffer_scroll(app, view);
    Face_ID face = get_face_id(app, buffer);
    Face_Metrics metrics = get_face_metrics(app, face);
    
    Vec2_f32 v =
    {
        scroll.position.pixel_shift.x,
        scroll.position.pixel_shift.y + scroll.position.line_number*metrics.line_height,
    };
    
    return v;
}

internal void
F4_PowerMode_Spawn(Application_Links *app, View_ID view, u8 character)
{
    if(F4_PowerMode_IsEnabled())
    {
        Vec2_f32 camera = F4_PowerMode_CameraOffsetFromView(app, view);
        
        for(int i = 0; i < 60; ++i)
        {
            String_Const_u8 string = {};
            ARGB_Color color = 0xffffffff;
            f32 decay_rate = 1.f;
            f32 scale = RandomF32(0.5f, 6.f);
            if(RandomF32(0, 1) < 0.01f)
            {
                f32 random = RandomF32(0, 1);
                if(random < 0.03f)
                {
                    string = string_u8_litexpr("CRITICAL HIT!!!");
                    color = 0xffff6060;
                    decay_rate = 0.05f;
                    scale = 0;
                }
                else if(random < 0.33f)
                {
                    string = string_u8_litexpr("EPIC COMBO!!!");
                    scale = 0;
                }
                else if(random < 0.66f)
                {
                    string = string_u8_litexpr("SLICK TYPING DUDE!!!!!!");
                    scale = 0;
                }
                else
                {
                    string = string_u8_litexpr("WHOOOOOAAAAAAAA!!!!!!!");
                    scale = 0;
                }
            }
            
            f32 movement_angle = RandomF32(-3.1415926535897f*3.f/2.f, 3.1415926535897f*1.f/3.f);
            f32 velocity_magnitude = RandomF32(20.f, 180.f);
            f32 velocity_x = cosf(movement_angle)*velocity_magnitude;
            f32 velocity_y = sinf(movement_angle)*velocity_magnitude;
            Particle *p = F4_PowerMode_Particle(global_cursor_rect.x0 + 4 + camera.x,
                                                global_cursor_rect.y0 + 8 + camera.y,
                                                velocity_x, velocity_y, decay_rate,
                                                color,
                                                RandomF32(1.5f, 8.f),
                                                scale,
                                                string);
            if(i < 30 && character)
            {
                p->chrs[0] = character;
                p->string = {p->chrs, 1};
            }
        }
        
        power_mode.screen_shake += RandomF32(6.f, 16.f);
    }
}

internal void
F4_PowerMode_Tick(Application_Links *app, Frame_Info frame_info)
{
    // NOTE(rjf): Load keystroke sounds.
    if(power_mode.enabled && power_mode.allowed)
    {
        for(int i = 0; i < ArrayCount(f4_powermode_keystroke_sounds); i += 1)
        {
            char path[256];
            snprintf(path, sizeof(path), "sounds/PowerKey-%03d.wav", i+1);
            F4_RequireWAV(app, &f4_powermode_keystroke_sounds[i], path);
            f4_powermode_keystroke_sounds[i].channel_volume[0] = 0.25f;
            f4_powermode_keystroke_sounds[i].channel_volume[1] = 0.25f;
        }
    }
    
    power_mode.screen_shake -= power_mode.screen_shake * frame_info.animation_dt * 12.f;
    if(F4_PowerMode_ActiveCharactersPerMinute() > 200.f)
    {
        if(power_mode.ticks_to_enable > 0)
        {
            power_mode.ticks_to_enable -= 1;
            animate_in_n_milliseconds(app, 0);
        }
        if(power_mode.ticks_to_enable <= 0)
        {
            if(power_mode.enabled == 0)
            {
                power_mode.enabled = 1;
                F4_RequireWAV(app, &f4_powermode_music, "sounds/chtulthu.wav");
            }
            
            power_mode.ticks_to_disable = 120;
            
            f32 right_volume = 0.4f;
            f32 left_volume = 0.4f;
            View_ID active_view = get_active_view(app, Access_Always);
            Rect_f32 active_view_rect = view_get_screen_rect(app, active_view);
            Rect_f32 screen_rect = global_get_screen_rectangle(app);
            if((active_view_rect.x0 + active_view_rect.x1) / 2 > (screen_rect.x0 + screen_rect.y1) / 2)
            {
                left_volume *= 0.2f;
            }
            else
            {
                right_volume *= 0.2f;
            }
            
            f4_powermode_music_ctrl.channel_volume[0] += (left_volume - f4_powermode_music_ctrl.channel_volume[0]) * frame_info.animation_dt;
            f4_powermode_music_ctrl.channel_volume[1] += (right_volume - f4_powermode_music_ctrl.channel_volume[1]) * frame_info.animation_dt;
            if(!def_audio_is_playing(&f4_powermode_music_ctrl))
            {
                def_audio_play_clip(f4_powermode_music, &f4_powermode_music_ctrl);
            }
        }
    }
    else
    {
        if(power_mode.ticks_to_disable > 0)
        {
            power_mode.ticks_to_disable -= 1;
            animate_in_n_milliseconds(app, 0);
        }
        
        if(power_mode.ticks_to_disable <= 0)
        {
            power_mode.enabled = 0;
            power_mode.ticks_to_enable = 440;
            f4_powermode_music_ctrl.channel_volume[0] -= f4_powermode_music_ctrl.channel_volume[0] * frame_info.animation_dt;
            f4_powermode_music_ctrl.channel_volume[1] -= f4_powermode_music_ctrl.channel_volume[1] * frame_info.animation_dt;
            if(f4_powermode_music_ctrl.channel_volume[0] > 0.05f ||
               f4_powermode_music_ctrl.channel_volume[1] > 0.05f)
            {
                animate_in_n_milliseconds(app, 0);
            }
        }
    }
    
    power_mode.enabled_t += ((f32)(!!power_mode.enabled) - power_mode.enabled_t) * frame_info.animation_dt;
}

internal void
F4_PowerMode_RenderBuffer(Application_Links *app, View_ID view, Face_ID face, Frame_Info frame_info)
{
    ProfileScope(app, "[Fleury] Power Mode");
    
    Buffer_Scroll buffer_scroll = view_get_buffer_scroll(app, view);
    Face_Metrics metrics = get_face_metrics(app, face);
    
    if(power_mode.particle_count > 0)
    {
        animate_in_n_milliseconds(app, 0);
    }
    
    f32 camera_x = buffer_scroll.position.pixel_shift.x;
    f32 camera_y = buffer_scroll.position.pixel_shift.y + buffer_scroll.position.line_number*metrics.line_height;
    
    for(int i = 0; i < power_mode.particle_count;)
    {
        // NOTE(rjf): Update particle.
        {
            power_mode.particles[i].x += power_mode.particles[i].velocity_x * frame_info.animation_dt;
            power_mode.particles[i].y += power_mode.particles[i].velocity_y * frame_info.animation_dt;
            power_mode.particles[i].velocity_x -= power_mode.particles[i].velocity_x * frame_info.animation_dt * 1.5f;
            power_mode.particles[i].velocity_y -= power_mode.particles[i].velocity_y * frame_info.animation_dt * 1.5f;
            power_mode.particles[i].velocity_y += 10.f * frame_info.animation_dt;
            power_mode.particles[i].alpha -= power_mode.particles[i].decay_rate * 0.3f * frame_info.animation_dt;
        }
        
        if(power_mode.particles[i].alpha <= 0.f)
        {
            power_mode.particles[i] = power_mode.particles[--power_mode.particle_count];
        }
        else
        {
            // NOTE(rjf): Render particle.
            {
                Rect_f32 rect =
                {
                    power_mode.particles[i].x - power_mode.particles[i].scale - camera_x,
                    power_mode.particles[i].y - power_mode.particles[i].scale - camera_y,
                    power_mode.particles[i].x + power_mode.particles[i].scale - camera_x,
                    power_mode.particles[i].y + power_mode.particles[i].scale - camera_y,
                };
                f32 roundness = power_mode.particles[i].roundness;
                ARGB_Color color = power_mode.particles[i].color;
                color &= 0x00ffffff;
                color |= ((u32)(power_mode.particles[i].alpha * 60.f)) << 24;
                draw_rectangle(app, rect, roundness, color);
                if(power_mode.particles[i].string.size > 0)
                {
                    draw_string(app, face, power_mode.particles[i].string, rect.p0, color);
                }
            }
            
            ++i;
        }
    }
}

internal void
F4_PowerMode_RenderWholeScreen(Application_Links *app, Frame_Info frame_info)
{
    Scratch_Block scratch(app);
    Rect_f32 rect = global_get_screen_rectangle(app);
    
    // NOTE(rjf): Power mode border
    if(power_mode.enabled_t > 0.1f)
    {
        ARGB_Color color = fcolor_resolve(fcolor_change_alpha(fcolor_id(fleury_color_cursor_power_mode), 0.1f*power_mode.enabled_t));
        Rect_f32 glow_rect = rect;
        for(int i = 0; i < 15; i += 1)
        {
            draw_rectangle_outline(app, glow_rect, 0.f, 15.f - (f32)i, color);
        }
    }
    
    if(power_mode.allowed)
    {
        Face_ID face_id = get_face_id(app, 0);
        String_Const_u8 string = push_stringf(scratch, "CPM: %.2f", F4_PowerMode_ActiveCharactersPerMinute());
        f32 advance = get_string_advance(app, face_id, string);
        draw_string(app, face_id, string, V2f32(rect.x1 - advance, rect.y0), 0xffffffff);
    }
}