#include <stdlib.h>
#include <string.h>
#include "4coder_default_include.cpp"

#pragma warning(disable : 4706)

#include "4coder_fleury_utilities.cpp"
#include "4coder_fleury_ubiquitous.cpp"
#include "4coder_fleury_power_mode.cpp"
#include "4coder_fleury_smooth_cursor.cpp"
#include "4coder_fleury_code_peek.cpp"
#include "4coder_fleury_theme.cpp"
#include "4coder_fleury_bindings.cpp"
#include "4coder_fleury_brace.cpp"
#include "4coder_fleury_divider_comments.cpp"
#include "4coder_fleury_plot.cpp"
#include "4coder_fleury_calc.cpp"

/*c
plot_function_samples(100)
plot_title('My Plot')
plot_xaxis('x', -4, 4)
plot_yaxis('y', -4, 4)
plot(x^2 * sin(time()), 4*cos(time())*sin(x*time()))
plot(sin(time())*cos(x+time()), x^3 * sin(time()),
sin(time())*x)
*/
























/*c
plot_title('Histogram')
plot_bin_count(10+5*sin(time()))
plot_bin_range(-40, 40)
plot_histogram(@global_data, @global_data_2)
*/





static float global_data[] =
{
    -9.108416130192959f, 14.98944384489895f, -10.534840051209034f, -3.614096568530723f,
    15.591337621094421f, -7.739171721170452f, -0.9026170782031733f, -13.149768977154512f,
    -17.219965300059062f, -2.6118783851474427f, 10.222099876380499f, 5.6231067088333635f,
    -5.387046221931645f, -9.347501587691713f, -16.628577856291958f, -3.549878630159417f,
    7.275919813653472f, 13.259019015296275f, -13.719547507312075f, 4.9481447540162184f,
    -0.8746865542576021f, -12.408190318285113f, -9.738643065846126f, -3.8368401053307197f,
    8.324992096614633f, 5.827221620008752f, 2.44537318282724f, 1.7618686011139677f,
    3.2475669947638597f, -1.3107566185213457f, 1.4139056845989626f, 22.335212406970864f,
    -16.628373002086192f, -2.3044147435667854f, -4.98398789678496f, 16.81181581297103f,
    13.820929426683648f, 8.273166954381164f, 4.205658111516642f, 1.3144068522342678f,
    -14.02976653200713f, -10.189462520322799f, -14.996118226598492f, -7.696043181997645f,
    -7.965758359068635f, -25.730179878849828f, 5.989522977751358f, 7.679759952052088f,
    7.137384031660718f, -9.896952786350589f, 2.6808646075560523f, 4.063113446418918f,
    -0.9051192417953401f, 6.667923468460404f, -1.186111757007614f, -1.4881950451626904f,
    4.233309258793588f, 9.784512402515142f, -1.5996194143052291f, -5.95208204626368f,
    6.527238517048863f, -5.96880713775255f, -9.180913523391514f, -3.0983475622391317f,
    -6.380172457884448f, -15.28864581444195f, 3.015771954358115f, -1.1151943462001357f,
    6.076108007434029f, -0.34535881326984036f, -9.886973961522399f, -4.737186858070488f,
    -9.5679105878576f, -12.228251499425696f, 7.250294016267537f, 4.148240878691285f,
    3.874521719638633f, 7.867739374649146f, -1.5275932972524895f, -6.608302297993162f,
    -8.803926488219307f, -1.9995038661551334f, -14.434894611647584f, -0.41132182908862497f,
    -4.349963461481015f, 3.086773296793125f, -7.220092288505216f, 9.370058826931889f,
    -2.995274172084772f, 7.996953684827249f, 8.809179230083636f, -5.218569709558633f,
    -4.590175669522937f, -5.172826845017181f, 0.49860784347011355f, 11.541334216468629f,
    -13.590303633214846f, -7.487766450284603f, -8.242079436281818f, 14.218536525979879f,
};

static float global_data_2[] =
{
    7.577748010344408f, 6.437282062034829f, -11.994330647892166f, 6.343100963328565f,
    -11.035257049206562f, -12.325201833152803f, 10.487078553467546f, -0.302281251571577f,
    6.37167161768581f, -3.029016443166927f, -2.449309813897796f, -1.710311611294908f,
    -8.849582001069985f, -5.096562336080957f, 4.08023422336442f, 13.09503213022921f,
    -4.42215698653707f, 1.4903196541032562f, 18.513375338661536f, 19.836444595990685f,
    -15.657274605334965f, 8.744172984302082f, -0.5128163692963429f, 8.652981818810666f,
    -4.501272431981506f, -5.338077342377595f, -4.516280672130289f, -5.26906827022084f,
    10.368107801542497f, 1.715483332057541f, 0.17745412681264855f, -3.907651876104541f,
    1.1152830110861718f, 8.533965328202594f, -9.569073355235334f, 11.335492351759154f,
    -8.834196801339107f, -7.388475519610194f, -0.9564301924816496f, -2.6200647013161853f,
    -3.4369245883887727f, 7.402555474674301f, 6.488608631449056f, 10.643192111382538f,
    -3.4095312717695365f, 4.244522664423113f, 3.1929321370647608f, 6.7003494689359915f,
    -1.6827069864056388f, 15.962243506800185f, 14.141247706537053f, 2.68132756874319f,
    -2.664309213424854f, 4.574984149324338f, -3.552829528232453f, -7.33980377699842f,
    4.599313341798405f, 3.9387184579833914f, 1.7303574550519154f, -9.013739849292799f,
    -10.096670009002409f, -5.376865969447776f, -8.035341457672533f, 2.827889497039652f,
    -0.9361242397823712f, -0.29467042436082236f, 1.5835137362599832f, -12.921338212205779f,
    -6.017681994641382f, 5.031487952492922f, 7.3365545974315625f, -2.8019359909922477f,
    -4.9170810446626065f, 3.5395114545808015f, 7.894893515517801f, 5.2162370199036365f,
    17.384622111747106f, -5.175019028227791f, 11.10907947411686f, 2.1177241750918325f,
    6.196644993021573f, 3.8114717688791018f, -4.052618724556429f, -2.8131388236006516f,
    3.4225121808116827f, 1.4468017854626773f, 4.936674843542018f, 12.347391841659707f,
    1.481019365327373f, -1.614588595075823f, -16.127418290574937f, 5.8754491693888f,
    5.414944259375581f, -3.9278525762988985f, 1.7713920365071134f, -5.566835799537236f,
    -2.9777849406374193f, 10.340166105177929f, -20.055841888872923f, -8.668248836552227f,
};

static float global_data_3[] =
{
    -8.8146732061949f, -11.859974461710019f, 0.6988238301382752f, 3.8493106728264226f,
    0.9975055447929071f, 2.1263183257518694f, -7.162590910370086f, -4.411404459087052f,
    12.506292741382445f, 6.7696321575984655f, 28.188196774552814f, 9.242340838166736f,
    6.690844009035476f, -12.692668952411303f, 0.1585751887962988f, 16.798063604260854f,
    -15.835398910698641f, 11.929906256712698f, -13.791788282920873f, -7.399384431165447f,
    -1.7316895840446684f, -5.765536364805584f, 0.3037440122496132f, 12.353298081681245f,
    3.381805408397766f, -10.559683278529675f, -8.421745533109155f, -5.799831766680602f,
    -5.4686307234067595f, 0.7753453426415153f, 4.779163543238979f, -7.687415465484914f,
    7.033912303434668f, -5.928271055351296f, -0.19482006115374734f, -16.09737642085842f,
    -8.02191675136603f, 5.408089412341942f, -4.408067132939348f, -5.067414164776771f,
    -4.639986755573686f, -1.3527927911711746f, 7.7525157199153565f, 1.9825121115749949f,
    -0.310969415743857f, -8.79990490996496f, 10.395427187174208f, -7.56478866186992f,
    -3.4315095975926546f, 5.5792985421522046f, -1.6018537884804516f, -18.15718558376568f,
    -9.11806103804825f, 10.945594674460144f, -0.15518187340689968f, 7.215336736037743f,
    9.10913684664322f, -1.2954014201805186f, -3.7655879556053677f, 25.513108679233362f,
    -8.899732575136678f, -8.837793636709254f, 1.4760795169766001f, -4.421912732322799f,
    9.716152080712433f, 2.281876709224348f, -0.29002192867192444f, -12.349466727748691f,
    2.7105026427207326f, -25.042540664548593f, -11.547268733662365f, -8.488266338126234f,
    -5.41217003524133f, 5.053633488587236f, 0.5216637588634313f, -10.918568429065441f,
    -12.718972500410013f, -15.709205357245652f, 6.031754862712157f, 25.62559025419464f,
    4.499488953458418f, 10.114260835136509f, -11.90982864944095f, 8.444006664292425f,
    -2.3658492626073433f, -4.194373837981419f, 14.649583361598292f, -14.041758521108942f,
    -2.2244037251609514f, 10.175103631316116f, -15.747415462670524f, -9.735420666763277f,
    -4.152295098019796f, 11.758217862826264f, -3.8244156666336853f, -1.9656147222257787f,
    13.70739704540151f, 5.396334912138413f, 4.232652499321735f, -19.10558208768004f,
};


/*c
plot_function_samples(100)

plot_title('Plotting Data')
 dat = [ [ 0 1 2 3 4 5 6 7 8 9 ] [ 0 1 2 3 4 5 6 7 8 9 ] ]
plot_xaxis('x', 0, 10)
plot_yaxis('y', 0, 10)
plot(dat)

plot_title('Plotting Functions #1')
plot_xaxis('x', -0.25, 1.25)
plot_yaxis('y', -0.25, 1.25)
plot(-2*x^3 + 3*x^2, -x^2, -x, 2*x)

plot_title('Plotting Functions #2')
plot_xaxis('x', -2, 2)
plot_yaxis('y', -3, 3)
plot(1/(x), x^2, -sin(x), cos(4*x))
*/




































/*c
t = (time()/3) % 1

plot_xaxis('t', -0.25, 1.25)
plot_yaxis('v', -0.25, 1.25)

plot_title('Linear')
transition = [ [t] [t] ]
plot(x, transition, t)

plot_title('Cubic')
transition = [ [t] [-2*t^3+3*t^2] ]
plot(-2*x^3+3*x^2, transition, -2*t^3+3*t^2)

plot_title('Exponential')
exp_factor = 1
transition = [ [t] [ 1-1 * 0.5^(10*t) ] ]
plot(1-1 * 0.5^(10*x), transition, 1-1 * 0.5^(10*t))
*/























//~ NOTE(rjf): Hooks
static i32  Fleury4BeginBuffer(Application_Links *app, Buffer_ID buffer_id);
static void Fleury4Render(Application_Links *app, Frame_Info frame_info, View_ID view_id);
static Layout_Item_List Fleury4Layout(Application_Links *app, Arena *arena, Buffer_ID buffer, Range_i64 range, Face_ID face, f32 width);


//~ NOTE(rjf): Hook Helpers
static void Fleury4RenderBuffer(Application_Links *app, View_ID view_id, Face_ID face_id, Buffer_ID buffer, Text_Layout_ID text_layout_id, Rect_f32 rect, Frame_Info frame_info);
static void Fleury4RenderRangeHighlight(Application_Links *app, View_ID view_id, Text_Layout_ID text_layout_id, Range_i64 range);


//~ NOTE(rjf): Commands

CUSTOM_COMMAND_SIG(fleury_write_text_input)
CUSTOM_DOC("Inserts whatever text was used to trigger this command.")
{
    write_text_input(app);
    Fleury4SpawnPowerModeParticles(app, get_active_view(app, Access_ReadWriteVisible));
}

CUSTOM_COMMAND_SIG(fleury_write_text_and_auto_indent)
CUSTOM_DOC("Inserts text and auto-indents the line on which the cursor sits if any of the text contains 'layout punctuation' such as ;:{}()[]# and new lines.")
{
    write_text_and_auto_indent(app);
    Fleury4SpawnPowerModeParticles(app, get_active_view(app, Access_ReadWriteVisible));
}

CUSTOM_COMMAND_SIG(fleury_write_zero_struct)
CUSTOM_DOC("At the cursor, insert a ' = {0};'.")
{
    write_string(app, string_u8_litexpr(" = {0};"));
}

CUSTOM_COMMAND_SIG(fleury_home)
CUSTOM_DOC("Goes to the beginning of the line.")
{
    seek_pos_of_visual_line(app, Side_Min);
    View_ID view = get_active_view(app, Access_ReadWriteVisible);
    Buffer_Scroll scroll = view_get_buffer_scroll(app, view);
    scroll.target.pixel_shift.x = 0;
    view_set_buffer_scroll(app, view, scroll, SetBufferScroll_NoCursorChange);
}

//~ NOTE(rjf): Custom layer initialization

void
custom_layer_init(Application_Links *app)
{
    Thread_Context *tctx = get_thread_context(app);

    // NOTE(allen): setup for default framework
    {
        async_task_handler_init(app, &global_async_system);
        code_index_init();
        buffer_modified_set_init();
        Profile_Global_List *list = get_core_profile_list(app);
        ProfileThreadName(tctx, list, string_u8_litexpr("main"));
        initialize_managed_id_metadata(app);
        set_default_color_scheme(app);
    }

    // NOTE(allen): default hooks and command maps
    {
    set_all_default_hooks(app);
        set_custom_hook(app, HookID_RenderCaller,  Fleury4Render);
        set_custom_hook(app, HookID_BeginBuffer,   Fleury4BeginBuffer);
        set_custom_hook(app, HookID_Layout,        Fleury4Layout);
    mapping_init(tctx, &framework_mapping);
    Fleury4SetBindings(&framework_mapping);
    }

    Fleury4DarkMode(app);
}

//~ NOTE(rjf): Range highlight

static void
Fleury4RenderRangeHighlight(Application_Links *app, View_ID view_id, Text_Layout_ID text_layout_id,
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
    ARGB_Color highlight_color = (0x75000000 |
                                  (((u32)(background_color_r * 255.f)) << 16) |
        (((u32)(background_color_g * 255.f)) <<  8) |
        (((u32)(background_color_b * 255.f)) <<  0));
    draw_rectangle(app, total_range_rect, 4.f, highlight_color);
}

//~ NOTE(rjf): Buffer Render

static void
Fleury4RenderBuffer(Application_Links *app, View_ID view_id, Face_ID face_id,
                      Buffer_ID buffer, Text_Layout_ID text_layout_id,
                    Rect_f32 rect, Frame_Info frame_info)
{
    ProfileScope(app, "[Fleury] Render Buffer");

    View_ID active_view = get_active_view(app, Access_Always);
    b32 is_active_view = (active_view == view_id);
    Rect_f32 prev_clip = draw_set_clip(app, rect);
    
    // NOTE(allen): Token colorizing
    Token_Array token_array = get_token_array_from_buffer(app, buffer);
    if(token_array.tokens != 0)
    {
        Fleury4DrawCTokenColors(app, text_layout_id, &token_array);

        // NOTE(allen): Scan for TODOs and NOTEs
        if(global_config.use_comment_keyword)
        {
            char user_string_buf[256] = {0};
            String_Const_u8 user_string = {0};
            {
                user_string.data = user_string_buf;
                user_string.size = snprintf(user_string_buf, sizeof(user_string_buf), "(%.*s)",
                                            string_expand(global_config.user_name));
            }

            Comment_Highlight_Pair pairs[] =
            {
                {string_u8_litexpr("NOTE"), finalize_color(defcolor_comment_pop, 0)},
                {string_u8_litexpr("TODO"), finalize_color(defcolor_comment_pop, 1)},
                {user_string, 0xffffdd23},
            };
            draw_comment_highlights(app, buffer, text_layout_id,
                                    &token_array, pairs, ArrayCount(pairs));
        }
    }
    else
    {
        Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
        paint_text_color_fcolor(app, text_layout_id, visible_range, fcolor_id(defcolor_text_default));
    }

    i64 cursor_pos = view_correct_cursor(app, view_id);
    view_correct_mark(app, view_id);

    // NOTE(allen): Scope highlight
    if(global_config.use_scope_highlight)
    {
        Color_Array colors = finalize_color_array(defcolor_back_cycle);
        draw_scope_highlight(app, buffer, text_layout_id, cursor_pos, colors.vals, colors.count);
    }

    // NOTE(rjf): Brace highlight
    {
        ARGB_Color colors[] =
        {
            0xff8ffff2,
        };

        Fleury4RenderBraceHighlight(app, buffer, text_layout_id, cursor_pos,
                             colors, sizeof(colors)/sizeof(colors[0]));
    }

    if(global_config.use_error_highlight || global_config.use_jump_highlight)
    {
        // NOTE(allen): Error highlight
        String_Const_u8 name = string_u8_litexpr("*compilation*");
        Buffer_ID compilation_buffer = get_buffer_by_name(app, name, Access_Always);
        if(global_config.use_error_highlight)
        {
            draw_jump_highlights(app, buffer, text_layout_id, compilation_buffer,
                                 fcolor_id(defcolor_highlight_junk));
        }

        // NOTE(allen): Search highlight
        if(global_config.use_jump_highlight)
        {
            Buffer_ID jump_buffer = get_locked_jump_buffer(app);
            if(jump_buffer != compilation_buffer)
            {
                draw_jump_highlights(app, buffer, text_layout_id, jump_buffer,
                                     fcolor_id(defcolor_highlight_white));
            }
        }
    }

    // NOTE(allen): Color parens
    if(global_config.use_paren_helper)
    {
        Color_Array colors = finalize_color_array(defcolor_text_cycle);
        draw_paren_highlight(app, buffer, text_layout_id, cursor_pos, colors.vals, colors.count);
    }

    // NOTE(allen): Line highlight
    if(global_config.highlight_line_at_cursor && is_active_view)
    {
        i64 line_number = get_line_number_from_pos(app, buffer, cursor_pos);
        draw_line_highlight(app, text_layout_id, line_number,
                            fcolor_id(defcolor_highlight_cursor_line));
    }
    
    // NOTE(rjf): Divider Comments
    {
        Fleury4RenderDividerComments(app, buffer, view_id, text_layout_id);
    }
    
    // NOTE(allen): Cursor shape
    Face_Metrics metrics = get_face_metrics(app, face_id);
    f32 cursor_roundness = (metrics.normal_advance*0.5f)*0.9f;
    f32 mark_thickness = 2.f;

    // NOTE(allen): Cursor
    switch (fcoder_mode)
    {
        case FCoderMode_Original:
        {
            Fleury4RenderCursor(app, view_id, is_active_view, buffer, text_layout_id, cursor_roundness, mark_thickness, frame_info);
            break;
        }

        case FCoderMode_NotepadLike:
        {
            draw_notepad_style_cursor_highlight(app, view_id, buffer, text_layout_id, cursor_roundness);
            break;
        }
    }

    // NOTE(rjf): Brace annotations
    {
        Fleury4RenderCloseBraceAnnotation(app, buffer, text_layout_id, cursor_pos);
    }

    // NOTE(rjf): Brace lines
    {
        Fleury4RenderBraceLines(app, buffer, view_id, text_layout_id, cursor_pos);
    }

    // NOTE(allen): put the actual text on the actual screen
    draw_text_layout_default(app, text_layout_id);
    
    // NOTE(rjf): Draw code peek
    if(global_code_peek_open)
    {
        Fleury4RenderRangeHighlight(app, view_id, text_layout_id, global_code_peek_token_range);
        Fleury4RenderCodePeek(app, view_id, face_id, buffer, frame_info);
    }
    
    // NOTE(rjf): Draw calc comments.
    {
        Fleury4RenderCalcComments(app, buffer, view_id, text_layout_id, frame_info);
    }
    
    // NOTE(rjf): Draw power mode.
    {
        Fleury4RenderPowerMode(app, view_id, face_id, frame_info);
    }

    draw_set_clip(app, prev_clip);
}

//~ NOTE(rjf): Render hook

static void
Fleury4Render(Application_Links *app, Frame_Info frame_info, View_ID view_id)
{
    ProfileScope(app, "[Fleury] Render");
    View_ID active_view = get_active_view(app, Access_Always);
    b32 is_active_view = (active_view == view_id);

    Rect_f32 region = draw_background_and_margin(app, view_id, is_active_view);
    Rect_f32 prev_clip = draw_set_clip(app, region);

    Buffer_ID buffer = view_get_buffer(app, view_id, Access_Always);
    Face_ID face_id = get_face_id(app, buffer);
    Face_Metrics face_metrics = get_face_metrics(app, face_id);
    f32 line_height = face_metrics.line_height;
    f32 digit_advance = face_metrics.decimal_digit_advance;

    // NOTE(allen): file bar
    b64 showing_file_bar = false;
    if(view_get_setting(app, view_id, ViewSetting_ShowFileBar, &showing_file_bar) && showing_file_bar)
    {
        Rect_f32_Pair pair = layout_file_bar_on_top(region, line_height);
        draw_file_bar(app, view_id, buffer, face_id, pair.min);
        region = pair.max;
    }

    Buffer_Scroll scroll = view_get_buffer_scroll(app, view_id);

    Buffer_Point_Delta_Result delta = delta_apply(app, view_id, frame_info.animation_dt, scroll);

    if(!block_match_struct(&scroll.position, &delta.point))
    {
        block_copy_struct(&scroll.position, &delta.point);
        view_set_buffer_scroll(app, view_id, scroll, SetBufferScroll_NoCursorChange);
    }

    if(delta.still_animating)
    {
        animate_in_n_milliseconds(app, 0);
    }

    // NOTE(allen): query bars
    {
        Query_Bar *space[32];
        Query_Bar_Ptr_Array query_bars = {};
        query_bars.ptrs = space;
        if (get_active_query_bars(app, view_id, ArrayCount(space), &query_bars))
        {
            for (i32 i = 0; i < query_bars.count; i += 1)
            {
                Rect_f32_Pair pair = layout_query_bar_on_top(region, line_height, 1);
                draw_query_bar(app, query_bars.ptrs[i], face_id, pair.min);
                region = pair.max;
            }
        }
    }

    // NOTE(allen): FPS hud
    if(show_fps_hud)
    {
        Rect_f32_Pair pair = layout_fps_hud_on_bottom(region, line_height);
        draw_fps_hud(app, frame_info, face_id, pair.max);
        region = pair.min;
animate_in_n_milliseconds(app, 1000);
    }

    // NOTE(allen): layout line numbers
    Rect_f32 line_number_rect = {};
    if(global_config.show_line_number_margins)
    {
        Rect_f32_Pair pair = layout_line_number_margin(app, buffer, region, digit_advance);
        line_number_rect = pair.min;
        region = pair.max;
    }

    // NOTE(allen): begin buffer render
    Buffer_Point buffer_point = scroll.position;
    if(is_active_view)
    {
    buffer_point.pixel_shift.y += global_power_mode.screen_shake*1.f;
        global_power_mode.screen_shake -= global_power_mode.screen_shake * frame_info.animation_dt * 12.f;
    }
    Text_Layout_ID text_layout_id = text_layout_create(app, buffer, region, buffer_point);

    // NOTE(allen): draw line numbers
    if(global_config.show_line_number_margins)
    {
        draw_line_number_margin(app, view_id, buffer, face_id, text_layout_id, line_number_rect);
    }

    // NOTE(allen): draw the buffer
    Fleury4RenderBuffer(app, view_id, face_id, buffer, text_layout_id, region, frame_info);

    text_layout_free(app, text_layout_id);
    draw_set_clip(app, prev_clip);
}

//~ NOTE(rjf): Begin buffer hook

BUFFER_HOOK_SIG(Fleury4BeginBuffer)
{
    ProfileScope(app, "[Fleury] Begin Buffer");

    Scratch_Block scratch(app);
    b32 treat_as_code = false;
    String_Const_u8 file_name = push_buffer_file_name(app, scratch, buffer_id);

    if(file_name.size > 0)
    {
        String_Const_u8_Array extensions = global_config.code_exts;
        String_Const_u8 ext = string_file_extension(file_name);

        for(i32 i = 0; i < extensions.count; ++i)
        {
            if(string_match(ext, extensions.strings[i]))
            {
                treat_as_code = true;
                break;
            }
        }
    }

    Command_Map_ID map_id = (treat_as_code) ? (mapid_code) : (mapid_file);
    Managed_Scope scope = buffer_get_managed_scope(app, buffer_id);
    Command_Map_ID *map_id_ptr = scope_attachment(app, scope, buffer_map_id, Command_Map_ID);
    *map_id_ptr = map_id;

    Line_Ending_Kind setting = guess_line_ending_kind_from_buffer(app, buffer_id);
    Line_Ending_Kind *eol_setting = scope_attachment(app, scope, buffer_eol_setting, Line_Ending_Kind);
    *eol_setting = setting;

    // NOTE(allen): Decide buffer settings
    b32 wrap_lines = true;
    b32 use_virtual_whitespace = false;
    b32 use_lexer = false;
    if(treat_as_code)
    {
        wrap_lines = global_config.enable_code_wrapping;
        use_virtual_whitespace = global_config.enable_virtual_whitespace;
        use_lexer = true;
    }

    String_Const_u8 buffer_name = push_buffer_base_name(app, scratch, buffer_id);
    if(string_match(buffer_name, string_u8_litexpr("*compilation*")))
    {
        wrap_lines = false;
    }

    if(use_lexer)
    {
        ProfileBlock(app, "begin buffer kick off lexer");
        Async_Task *lex_task_ptr = scope_attachment(app, scope, buffer_lex_task, Async_Task);
        *lex_task_ptr = async_task_no_dep(&global_async_system, do_full_lex_async, make_data_struct(&buffer_id));
    }

    {
        b32 *wrap_lines_ptr = scope_attachment(app, scope, buffer_wrap_lines, b32);
        *wrap_lines_ptr = wrap_lines;
    }

    if (use_virtual_whitespace)
    {
        if (use_lexer)
        {
            buffer_set_layout(app, buffer_id, layout_virt_indent_index_generic);
        }
        else
        {
            buffer_set_layout(app, buffer_id, layout_virt_indent_literal_generic);
        }
    }
    else
{
        buffer_set_layout(app, buffer_id, layout_generic);
    }

    // no meaning for return
    return(0);
}


//~ NOTE(rjf): Layout

 static Layout_Item_List
Fleury4Layout(Application_Links *app, Arena *arena, Buffer_ID buffer, Range_i64 range, Face_ID face, f32 width)
{
    return(layout_unwrapped__inner(app, arena, buffer, range, face, width, LayoutVirtualIndent_Off));
}
