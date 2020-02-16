
//~ NOTE(rjf): Power Mode

static b32 global_power_mode_enabled = 0;
static struct
{
    int particle_count;
    struct
    {
        f32 x;
        f32 y;
        f32 velocity_x;
        f32 velocity_y;
        ARGB_Color color;
        f32 alpha;
        f32 roundness;
        f32 scale;
    }
    particles[4096];
    f32 screen_shake;
}
global_power_mode;

static void
Fleury4Particle(f32 x, f32 y, f32 velocity_x, f32 velocity_y, ARGB_Color color,
                f32 roundness, f32 scale)
{
    if(global_power_mode.particle_count < ArrayCount(global_power_mode.particles))
    {
        int i = global_power_mode.particle_count++;
        global_power_mode.particles[i].x = x;
        global_power_mode.particles[i].y = y;
        global_power_mode.particles[i].velocity_x = velocity_x;
        global_power_mode.particles[i].velocity_y = velocity_y;
        global_power_mode.particles[i].color = color;
        global_power_mode.particles[i].alpha = 1.f;
        global_power_mode.particles[i].roundness = roundness;
        global_power_mode.particles[i].scale = scale;
    }
}

static Vec2_f32
Fleury4GetCameraFromView(Application_Links *app, View_ID view)
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

static void
Fleury4SpawnPowerModeParticles(Application_Links *app, View_ID view)
{
    if(global_power_mode_enabled)
    {
        Vec2_f32 camera = Fleury4GetCameraFromView(app, view);
        
        for(int i = 0; i < 60; ++i)
        {
            f32 movement_angle = RandomF32(-3.1415926535897f*3.f/2.f, 3.1415926535897f*1.f/3.f);
            f32 velocity_magnitude = RandomF32(20.f, 180.f);
            f32 velocity_x = cosf(movement_angle)*velocity_magnitude;
            f32 velocity_y = sinf(movement_angle)*velocity_magnitude;
            Fleury4Particle(global_cursor_rect.x0 + 4 + camera.x,
                            global_cursor_rect.y0 + 8 + camera.y,
                            velocity_x, velocity_y,
                            0xffffffff,
                            RandomF32(1.5f, 8.f),
                            RandomF32(0.5f, 6.f));
        }
        
        global_power_mode.screen_shake += RandomF32(6.f, 16.f);
    }
}

static void
Fleury4RenderPowerMode(Application_Links *app, View_ID view, Face_ID face, Frame_Info frame_info)
{
    ProfileScope(app, "[Fleury] Power Mode");
    
    Buffer_Scroll buffer_scroll = view_get_buffer_scroll(app, view);
    Face_Metrics metrics = get_face_metrics(app, face);
    
    if(global_power_mode.particle_count > 0)
    {
        animate_in_n_milliseconds(app, 0);
    }
    
    f32 camera_x = buffer_scroll.position.pixel_shift.x;
    f32 camera_y = buffer_scroll.position.pixel_shift.y + buffer_scroll.position.line_number*metrics.line_height;
    
    for(int i = 0; i < global_power_mode.particle_count;)
    {
        // NOTE(rjf): Update particle.
        {
            global_power_mode.particles[i].x += global_power_mode.particles[i].velocity_x * frame_info.animation_dt;
            global_power_mode.particles[i].y += global_power_mode.particles[i].velocity_y * frame_info.animation_dt;
            global_power_mode.particles[i].velocity_x -= global_power_mode.particles[i].velocity_x * frame_info.animation_dt * 1.5f;
            global_power_mode.particles[i].velocity_y -= global_power_mode.particles[i].velocity_y * frame_info.animation_dt * 1.5f;
            global_power_mode.particles[i].velocity_y += 10.f * frame_info.animation_dt;
            global_power_mode.particles[i].alpha -= 0.5f * frame_info.animation_dt;
        }
        
        if(global_power_mode.particles[i].alpha <= 0.f)
        {
            global_power_mode.particles[i] = global_power_mode.particles[--global_power_mode.particle_count];
        }
        else
        {
            // NOTE(rjf): Render particle.
            {
                Rect_f32 rect =
                {
                    global_power_mode.particles[i].x - global_power_mode.particles[i].scale - camera_x,
                    global_power_mode.particles[i].y - global_power_mode.particles[i].scale - camera_y,
                    global_power_mode.particles[i].x + global_power_mode.particles[i].scale - camera_x,
                    global_power_mode.particles[i].y + global_power_mode.particles[i].scale - camera_y,
                };
                f32 roundness = global_power_mode.particles[i].roundness;
                ARGB_Color color = global_power_mode.particles[i].color;
                color &= 0x00ffffff;
                color |= ((u32)(global_power_mode.particles[i].alpha * 60.f)) << 24;
                draw_rectangle(app, rect, roundness, color);
            }
            
            ++i;
        }
        
    }
    
}

CUSTOM_COMMAND_SIG(fleury_toggle_power_mode)
CUSTOM_DOC("Toggles power mode.")
{
    if(global_power_mode_enabled)
    {
        global_power_mode_enabled = 0;
    }
    else
    {
        global_power_mode_enabled = 1;
    }
}
