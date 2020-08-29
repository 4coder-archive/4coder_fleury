#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "4coder_default_include.cpp"
#include "generated/managed_id_metadata.cpp"

#pragma warning(disable : 4706)

#include "4coder_fleury_ubiquitous.cpp"
#include "4coder_fleury_power_mode.cpp"
#include "4coder_fleury_cursor.cpp"
#include "4coder_fleury_code_peek.cpp"
#include "4coder_fleury_bindings.cpp"
#include "4coder_fleury_brace.cpp"
#include "4coder_fleury_divider_comments.cpp"
#include "4coder_fleury_plot.cpp"
#include "4coder_fleury_calc.cpp"

//~ TODO(rjf)
//
// [ ] Project-wide todo list (by searching for TODO and DONE comments, lister for toggling)
// [ ] Project switcher
// [ ] Remember to enable type/macro/function highlighting when Allen adds a table lookup for
//   the code index
// [X] Fix the cursor in the right panel when we use page up/down please?
// [ ] Plan + do modal input scheme... Identifier-mode, text-mode, semantics mode, search mode...?
// [X] Fix plot layout bugs when the plot call isn't in visible range
// [ ] Fix plot clip rect bugs when the plot is not 100% in the visible range
//   (caused by 4coder laying out characters off the screen as being at 0, 0)
// [X] Nested parentheses bug in function helper
// [ ] Labels for histogram bins
// [X] Make plot grid lines + tick labels not terrible
// [X] Let function helper work without closing paren
// [X] Fix comment calc comment output, by interpreting entire scripts, and layouting
//   results correctly.
// [X] Investigate weird layout positioning issue in *calc* buffer.
// [X] Finish *calc* buffer.


//~

/*c
plot_function_samples(100)
plot_title('My Plot')
plot_xaxis('x', -4, 4)
plot_yaxis('y', -4, 4)
plot(x^2 * sin(time()), 4*cos(time())*sin(x*time()))
plot(sin(time())*cos(x+time()), x^3 * sin(time()),
sin(-time())*3*x)
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

sin2_time = sin(time()/2)^2 + 0.2

plot_title('Plotting Data')
 dat = [ [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, ], [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, ], ]
plot_xaxis('x', 0, 10*sin2_time)
plot_yaxis('y', 0, 10)
plot(dat)

plot_title('Plotting Functions #1')
plot_xaxis('x', -0.25, 1.25*sin2_time)
plot_yaxis('y', -0.25, 1.25*sin2_time)
plot(-2*x^3 + 3*x^2, -x^2, -x, 2*x)

plot_title('Plotting Functions #2')
plot_xaxis('x', -4*sin2_time, 2*sin2_time)
plot_yaxis('y', -3*sin2_time, 3*sin2_time)
plot(1/(x), x^2, -sin(x), cos(4*x))
*/
























/*c
t = 2 * abs((time()/8) % 1 - 0.5)

plot_xaxis('t', -0.25, 1.25)
plot_yaxis('v', -0.25, 1.25)

  plot_title('Linear')
transition = [ [t], [t] ]
plot(x, transition, t)

 plot_title('Cubic')
 transition = [ [t], [-2*t^3+3*t^2] ]
 plot(-2*x^3+3*x^2, transition, -2*t^3+3*t^2)

 plot_title('Exponential')
 transition = [ [t], [ 1-1 * 0.5^(10*t) ] ]
 plot(1-1 * 0.5^(10*x), transition, 1-1 * 0.5^(10*t))
*/





























//~ NOTE(rjf): Hooks
static void F4_Tick(Application_Links *app, Frame_Info frame_info);
static i32  F4_BeginBuffer(Application_Links *app, Buffer_ID buffer_id);
static void F4_Render(Application_Links *app, Frame_Info frame_info, View_ID view_id);
static Layout_Item_List F4_Layout(Application_Links *app, Arena *arena, Buffer_ID buffer, Range_i64 range, Face_ID face, f32 width);
function void F4_WholeScreenRender(Application_Links *app, Frame_Info frame_info);

//~ NOTE(rjf): Helpers
static void F4_RenderBuffer(Application_Links *app, View_ID view_id, Face_ID face_id, Buffer_ID buffer, Text_Layout_ID text_layout_id, Rect_f32 rect, Frame_Info frame_info);
static void F4_RenderRangeHighlight(Application_Links *app, View_ID view_id, Text_Layout_ID text_layout_id, Range_i64 range);
static void F4_SmartReplaceIdentifier(Application_Links *app, String_Const_u8 needle, String_Const_u8 replace);

//~ NOTE(rjf): Commands

CUSTOM_COMMAND_SIG(fleury_write_text_input)
CUSTOM_DOC("Inserts whatever text was used to trigger this command.")
{
    write_text_input(app);
    F4_SpawnPowerModeParticles(app, get_active_view(app, Access_ReadWriteVisible));
}

CUSTOM_COMMAND_SIG(fleury_write_text_and_auto_indent)
CUSTOM_DOC("Inserts text and auto-indents the line on which the cursor sits if any of the text contains 'layout punctuation' such as ;:{}()[]# and new lines.")
{
    write_text_and_auto_indent(app);
    F4_SpawnPowerModeParticles(app, get_active_view(app, Access_ReadWriteVisible));
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

CUSTOM_COMMAND_SIG(fleury_place_cursor)
CUSTOM_DOC("Places a new cursor at the current main cursor position.")
{
    //View_ID view = get_active_view(app, Access_ReadWriteVisible);
    //i64 current_cursor_pos = view_get_cursor_pos(app, view);
    //global_cursor_positions[global_cursor_count++] = current_cursor_pos;
}

CUSTOM_COMMAND_SIG(fleury_smart_replace_identifier)
CUSTOM_DOC("Does a smart-replace of the identifier under the cursor")
{
    View_ID view = get_active_view(app, Access_ReadWriteVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
    
    // NOTE(rjf): Get token from cursor.
    Token *token = 0;
    {
        Token_Array token_array = get_token_array_from_buffer(app, buffer);
        Token_Iterator_Array it = token_iterator_pos(0, &token_array, view_get_cursor_pos(app, view));
        token = token_it_read(&it);
    }
    
    // NOTE(rjf): Query user
    if(token)
    {
        Scratch_Block scratch(app);
        Query_Bar_Group group(app);
        String_Const_u8 token_string = push_buffer_range(app, scratch, buffer,
                                                         Ii64(token->pos, token->pos + token->size));
        Query_Bar *with = push_array(scratch, Query_Bar, 1);
        u8 *with_space = push_array(scratch, u8, KB(1));
        with->prompt = push_u8_stringf(scratch, "Replace \"%.*s\" with: ", string_expand(token_string));
        with->string = SCu8(with_space, (u64)0);
        with->string_capacity = KB(1);
        
        if(query_user_string(app, with))
        {
            F4_SmartReplaceIdentifier(app, token_string, with->string);
        }
    }
}

CUSTOM_COMMAND_SIG(fleury_toggle_battery_saver)
CUSTOM_DOC("Toggles battery saving mode.")
{
    global_battery_saver = !global_battery_saver;
}

//~ NOTE(rjf): Custom layer initialization

DELTA_RULE_SIG(F4_DeltaRule)
{
    Vec2_f32 *velocity = (Vec2_f32*)data;
    if (velocity->x == 0.f){
        velocity->x = 1.f;
        velocity->y = 1.f;
    }
    Smooth_Step step_x = smooth_camera_step(pending.x, velocity->x, 80.f, 1.f/4.f);
    Smooth_Step step_y = smooth_camera_step(pending.y, velocity->y, 80.f, 1.f/4.f);
    *velocity = V2f32(step_x.v, step_y.v);
    return(V2f32(step_x.p, step_y.p));
}

void
custom_layer_init(Application_Links *app)
{
    Thread_Context *tctx = get_thread_context(app);
    
    global_frame_arena = make_arena(get_base_allocator_system());
    
    default_framework_init(app);
    set_all_default_hooks(app);
    set_custom_hook(app, HookID_Tick,                    F4_Tick);
    set_custom_hook(app, HookID_RenderCaller,            F4_Render);
    set_custom_hook(app, HookID_BeginBuffer,             F4_BeginBuffer);
    set_custom_hook(app, HookID_Layout,                  F4_Layout);
    set_custom_hook(app, HookID_WholeScreenRenderCaller, F4_WholeScreenRender);
    set_custom_hook(app, HookID_DeltaRule,               F4_DeltaRule);
    set_custom_hook_memory_size(app, HookID_DeltaRule, delta_ctx_size(sizeof(Vec2_f32)));
    mapping_init(tctx, &framework_mapping);
    F4_SetBindings(&framework_mapping);
    
    // NOTE(rjf): Open calc buffer.
    {
        Buffer_ID calc_buffer = create_buffer(app, string_u8_litexpr("*calc*"),
                                              BufferCreate_NeverAttachToFile |
                                              BufferCreate_AlwaysNew);
        buffer_set_setting(app, calc_buffer, BufferSetting_Unimportant, true);
        // (void)calc_buffer;
    }
}


//~ NOTE(rjf): Startup.

// TODO(rjf): This is only being used to check if a font file exists because
// there's a bug in try_create_new_face that crashes the program if a font is
// not found. This function is only necessary until that is fixed.
static b32
IsFileReadable(String_Const_u8 path)
{
    b32 result = 0;
    FILE *file = fopen((char *)path.str, "r");
    if(file)
    {
        result = 1;
        fclose(file);
    }
    return result;
}

CUSTOM_COMMAND_SIG(fleury_startup)
CUSTOM_DOC("Fleury startup event")
{
    ProfileScope(app, "default startup");
    User_Input input = get_current_input(app);
    if(match_core_code(&input, CoreCode_Startup))
    {
        String_Const_u8_Array file_names = input.event.core.file_names;
        load_themes_default_folder(app);
        default_4coder_initialize(app, file_names);
        default_4coder_side_by_side_panels(app, file_names);
        if (global_config.automatically_load_project)
        {
            load_project(app);
        }
        
        // NOTE(rjf): Initialize stylish fonts.
        {
            Scratch_Block scratch(app);
            String_Const_u8 bin_path = system_get_path(scratch, SystemPath_Binary);
            
            // NOTE(rjf): Fallback font.
            Face_ID face_that_should_totally_be_there = get_face_id(app, 0);
            
            // NOTE(rjf): Title font.
            {
                Face_Description desc = {0};
                {
                    desc.font.file_name =  push_u8_stringf(scratch, "%.*sfonts/RobotoCondensed-Regular.ttf", string_expand(bin_path));
                    desc.parameters.pt_size = 18;
                    desc.parameters.bold = 0;
                    desc.parameters.italic = 0;
                    desc.parameters.hinting = 0;
                }
                
                if(IsFileReadable(desc.font.file_name))
                {
                    global_styled_title_face = try_create_new_face(app, &desc);
                }
                else
                {
                    global_styled_title_face = face_that_should_totally_be_there;
                }
            }
            
            // NOTE(rjf): Label font.
            {
                Face_Description desc = {0};
                {
                    desc.font.file_name =  push_u8_stringf(scratch, "%.*sfonts/RobotoCondensed-Regular.ttf", string_expand(bin_path));
                    desc.parameters.pt_size = 10;
                    desc.parameters.bold = 1;
                    desc.parameters.italic = 1;
                    desc.parameters.hinting = 0;
                }
                
                if(IsFileReadable(desc.font.file_name))
                {
                    global_styled_label_face = try_create_new_face(app, &desc);
                }
                else
                {
                    global_styled_label_face = face_that_should_totally_be_there;
                }
            }
            
            // NOTE(rjf): Small code font.
            {
                Face_Description normal_code_desc = get_face_description(app, get_face_id(app, 0));
                
                Face_Description desc = {0};
                {
                    desc.font.file_name =  push_u8_stringf(scratch, "%.*sfonts/Inconsolata-Regular.ttf", string_expand(bin_path));
                    desc.parameters.pt_size = normal_code_desc.parameters.pt_size - 1;
                    desc.parameters.bold = 1;
                    desc.parameters.italic = 1;
                    desc.parameters.hinting = 0;
                }
                
                if(IsFileReadable(desc.font.file_name))
                {
                    global_small_code_face = try_create_new_face(app, &desc);
                }
                else
                {
                    global_small_code_face = face_that_should_totally_be_there;
                }
            }
        }
    }
}


//~ NOTE(rjf): Error annotations

static void
F4_RenderErrorAnnotations(Application_Links *app, Buffer_ID buffer,
                          Text_Layout_ID text_layout_id,
                          Buffer_ID jump_buffer)
{
    ProfileScope(app, "[Fleury] Error Annotations");
    
    Heap *heap = &global_heap;
    Scratch_Block scratch(app);
    
    Locked_Jump_State jump_state = {};
    {
        ProfileScope(app, "[Fleury] Error Annotations (Get Locked Jump State)");
        jump_state = get_locked_jump_state(app, heap);
    }
    
    Face_ID face = global_small_code_face;
    Face_Metrics metrics = get_face_metrics(app, face);
    
    if(jump_buffer != 0 && jump_state.view != 0)
    {
        Managed_Scope buffer_scopes[2];
        {
            ProfileScope(app, "[Fleury] Error Annotations (Buffer Get Managed Scope)");
            buffer_scopes[0] = buffer_get_managed_scope(app, jump_buffer);
            buffer_scopes[1] = buffer_get_managed_scope(app, buffer);
        }
        
        Managed_Scope comp_scope = 0;
        {
            ProfileScope(app, "[Fleury] Error Annotations (Get Managed Scope)");
            comp_scope = get_managed_scope_with_multiple_dependencies(app, buffer_scopes, ArrayCount(buffer_scopes));
        }
        
        Managed_Object *buffer_markers_object = 0;
        {
            ProfileScope(app, "[Fleury] Error Annotations (Scope Attachment)");
            buffer_markers_object = scope_attachment(app, comp_scope, sticky_jump_marker_handle, Managed_Object);
        }
        
        // NOTE(rjf): Get buffer markers (locations where jumps point at).
        i32 buffer_marker_count = 0;
        Marker *buffer_markers = 0;
        {
            ProfileScope(app, "[Fleury] Error Annotations (Load Managed Object Data)");
            buffer_marker_count = managed_object_get_item_count(app, *buffer_markers_object);
            buffer_markers = push_array(scratch, Marker, buffer_marker_count);
            managed_object_load_data(app, *buffer_markers_object, 0, buffer_marker_count, buffer_markers);
        }
        
        i64 last_line = -1;
        
        for(i32 i = 0; i < buffer_marker_count; i += 1)
        {
            ProfileScope(app, "[Fleury] Error Annotations (Buffer Loop)");
            
            i64 jump_line_number = get_line_from_list(app, jump_state.list, i);
            i64 code_line_number = get_line_number_from_pos(app, buffer, buffer_markers[i].pos);
            
            if(code_line_number != last_line)
            {
                ProfileScope(app, "[Fleury] Error Annotations (Jump Line)");
                
                String_Const_u8 jump_line = push_buffer_line(app, scratch, jump_buffer, jump_line_number);
                
                // NOTE(rjf): Remove file part of jump line.
                {
                    u64 index = string_find_first(jump_line, string_u8_litexpr("error"), StringMatch_CaseInsensitive);
                    if(index == jump_line.size)
                    {
                        index = string_find_first(jump_line, string_u8_litexpr("warning"), StringMatch_CaseInsensitive);
                        if(index == jump_line.size)
                        {
                            index = 0;
                        }
                    }
                    jump_line.str += index;
                    jump_line.size -= index;
                }
                
                // NOTE(rjf): Render annotation.
                {
                    Range_i64 line_range = Ii64(code_line_number);
                    Range_f32 y1 = text_layout_line_on_screen(app, text_layout_id, line_range.min);
                    Range_f32 y2 = text_layout_line_on_screen(app, text_layout_id, line_range.max);
                    Range_f32 y = range_union(y1, y2);
                    Rect_f32 last_character_on_line_rect =
                        text_layout_character_on_screen(app, text_layout_id, get_line_end_pos(app, buffer, code_line_number)-1);
                    
                    if(range_size(y) > 0.f)
                    {
                        Rect_f32 region = text_layout_region(app, text_layout_id);
                        Vec2_f32 draw_position =
                        {
                            region.x1 - metrics.max_advance*jump_line.size -
                                (y.max-y.min)/2 - metrics.line_height/2,
                            y.min + (y.max-y.min)/2 - metrics.line_height/2,
                        };
                        
                        if(draw_position.x < last_character_on_line_rect.x1 + 30)
                        {
                            draw_position.x = last_character_on_line_rect.x1 + 30;
                        }
                        
                        draw_string(app, face, jump_line, draw_position, 0xffff0000);
                        
                        Mouse_State mouse_state = get_mouse_state(app);
                        if(mouse_state.x >= region.x0 && mouse_state.x <= region.x1 &&
                           mouse_state.y >= y.min && mouse_state.y <= y.max)
                        {
                            F4_PushTooltip(jump_line, 0xffff0000);
                        }
                    }
                }
            }
            
            last_line = code_line_number;
        }
    }
}


//~ NOTE(rjf): Function Helper

static Range_i64_Array
F4_GetLeftParens(Application_Links *app, Arena *arena, Buffer_ID buffer, i64 pos, u32 flags)
{
    Range_i64_Array array = {};
    i32 max = 100;
    array.ranges = push_array(arena, Range_i64, max);
    
    for(;;)
    {
        Range_i64 range = {};
        range.max = pos;
        if(find_nest_side(app, buffer, pos - 1, flags | FindNest_Balanced,
                          Scan_Backward, NestDelim_Open, &range.start))
        {
            array.ranges[array.count] = range;
            array.count += 1;
            pos = range.first;
            if (array.count >= max)
            {
                break;
            }
        }
        else
        {
            break;
        }
    }
    return(array);
}

static String_Const_u8
F4_CopyStringButOnlyAllowOneSpace(Arena *arena, String_Const_u8 string)
{
    String_Const_u8 result = {0};
    
    u64 space_to_allocate = 0;
    u64 spaces_left_this_gap = 1;
    
    for(u64 i = 0; i < string.size; ++i)
    {
        if(string.str[i] <= 32)
        {
            if(spaces_left_this_gap > 0)
            {
                --spaces_left_this_gap;
                ++space_to_allocate;
            }
        }
        else
        {
            spaces_left_this_gap = 1;
            ++space_to_allocate;
        }
    }
    
    result.data = push_array(arena, u8, space_to_allocate);
    for(u64 i = 0; i < string.size; ++i)
    {
        if(string.str[i] <= 32)
        {
            if(spaces_left_this_gap > 0)
            {
                --spaces_left_this_gap;
                result.str[result.size++] = string.str[i];
            }
        }
        else
        {
            spaces_left_this_gap = 1;
            result.str[result.size++] = string.str[i];
        }
    }
    
    return result;
}

static void
F4_RenderFunctionHelper(Application_Links *app, View_ID view, Buffer_ID buffer,
                        Text_Layout_ID text_layout_id, i64 pos)
{
    ProfileScope(app, "[Fleury] Function Helper");
    
    Token_Array token_array = get_token_array_from_buffer(app, buffer);
    Token_Iterator_Array it;
    Token *token = 0;
    
    Rect_f32 view_rect = view_get_screen_rect(app, view);
    
    Face_ID face = global_small_code_face;
    Face_Metrics metrics = get_face_metrics(app, face);
    
    if(token_array.tokens != 0)
    {
        it = token_iterator_pos(0, &token_array, pos);
        token = token_it_read(&it);
        
        if(token != 0 && token->kind == TokenBaseKind_ParentheticalOpen)
        {
            pos = token->pos + token->size;
        }
        else
        {
            if (token_it_dec_all(&it))
            {
                token = token_it_read(&it);
                if (token->kind == TokenBaseKind_ParentheticalClose &&
                    pos == token->pos + token->size)
                {
                    pos = token->pos;
                }
            }
        }
    }
    
    Scratch_Block scratch(app);
    Range_i64_Array ranges = F4_GetLeftParens(app, scratch, buffer, pos, FindNest_Paren);
    
    for(int range_index = 0; range_index < ranges.count; ++range_index)
    {
        it = token_iterator_pos(0, &token_array, ranges.ranges[range_index].min);
        token_it_dec_non_whitespace(&it);
        token = token_it_read(&it);
        if(token->kind == TokenBaseKind_Identifier)
        {
            Range_i64 function_name_range = Ii64(token->pos, token->pos+token->size);
            String_Const_u8 function_name = push_buffer_range(app, scratch, buffer, function_name_range);
            
            F4_RenderRangeHighlight(app, view, text_layout_id, function_name_range);
            
            // NOTE(rjf): Find active parameter.
            int active_parameter_index = 0;
            static int last_active_parameter = -1;
            {
                it = token_iterator_pos(0, &token_array, function_name_range.min);
                int paren_nest = 0;
                for(;token_it_inc_non_whitespace(&it);)
                {
                    token = token_it_read(&it);
                    if(token->pos + token->size > pos)
                    {
                        break;
                    }
                    
                    if(token->kind == TokenBaseKind_ParentheticalOpen)
                    {
                        ++paren_nest;
                    }
                    else if(token->kind == TokenBaseKind_StatementClose)
                    {
                        if(paren_nest == 1)
                        {
                            ++active_parameter_index;
                        }
                    }
                    else if(token->kind == TokenBaseKind_ParentheticalClose)
                    {
                        if(!--paren_nest)
                        {
                            break;
                        }
                    }
                }
            }
            b32 active_parameter_has_increased_by_one = active_parameter_index == last_active_parameter + 1;
            last_active_parameter = active_parameter_index;
            
            for(Buffer_ID buffer_it = get_buffer_next(app, 0, Access_Always);
                buffer_it != 0; buffer_it = get_buffer_next(app, buffer_it, Access_Always))
            {
                Code_Index_File *file = code_index_get_file(buffer_it);
                if(file != 0)
                {
                    for(i32 i = 0; i < file->note_array.count; i += 1)
                    {
                        Code_Index_Note *note = file->note_array.ptrs[i];
                        
                        if((note->note_kind == CodeIndexNote_Function ||
                            note->note_kind == CodeIndexNote_Macro) &&
                           string_match(note->text, function_name))
                        {
                            Range_i64 function_def_range;
                            function_def_range.min = note->pos.min;
                            function_def_range.max = note->pos.max;
                            
                            Range_i64 highlight_parameter_range = {0};
                            
                            Token_Array find_token_array = get_token_array_from_buffer(app, buffer_it);
                            it = token_iterator_pos(0, &find_token_array, function_def_range.min);
                            
                            int paren_nest = 0;
                            int param_index = 0;
                            for(;token_it_inc_non_whitespace(&it);)
                            {
                                token = token_it_read(&it);
                                if(token->kind == TokenBaseKind_ParentheticalOpen)
                                {
                                    if(++paren_nest == 1)
                                    {
                                        if(active_parameter_index == param_index)
                                        {
                                            highlight_parameter_range.min = token->pos+1;
                                        }
                                    }
                                }
                                else if(token->kind == TokenBaseKind_ParentheticalClose)
                                {
                                    if(!--paren_nest)
                                    {
                                        function_def_range.max = token->pos + token->size;
                                        if(param_index == active_parameter_index)
                                        {
                                            highlight_parameter_range.max = token->pos;
                                        }
                                        break;
                                    }
                                }
                                else if(token->kind == TokenBaseKind_StatementClose)
                                {
                                    if(param_index == active_parameter_index)
                                    {
                                        highlight_parameter_range.max = token->pos;
                                    }
                                    
                                    if(paren_nest == 1)
                                    {
                                        ++param_index;
                                    }
                                    
                                    if(param_index == active_parameter_index)
                                    {
                                        highlight_parameter_range.min = token->pos+1;
                                    }
                                }
                            }
                            
                            if(highlight_parameter_range.min > function_def_range.min &&
                               function_def_range.max > highlight_parameter_range.max)
                            {
                                
                                String_Const_u8 function_def = push_buffer_range(app, scratch, buffer_it,
                                                                                 function_def_range);
                                String_Const_u8 highlight_param_untrimmed = push_buffer_range(app, scratch, buffer_it,
                                                                                              highlight_parameter_range);
                                
                                String_Const_u8 pre_highlight_def_untrimmed =
                                {
                                    function_def.str,
                                    (u64)(highlight_parameter_range.min - function_def_range.min),
                                };
                                
                                String_Const_u8 post_highlight_def_untrimmed =
                                {
                                    function_def.str + highlight_parameter_range.max - function_def_range.min,
                                    (u64)(function_def_range.max - highlight_parameter_range.max),
                                };
                                
                                String_Const_u8 highlight_param = F4_CopyStringButOnlyAllowOneSpace(scratch, highlight_param_untrimmed);
                                String_Const_u8 pre_highlight_def = F4_CopyStringButOnlyAllowOneSpace(scratch, pre_highlight_def_untrimmed);
                                String_Const_u8 post_highlight_def = F4_CopyStringButOnlyAllowOneSpace(scratch, post_highlight_def_untrimmed);
                                
                                Rect_f32 helper_rect =
                                {
                                    global_cursor_rect.x0 + 16,
                                    global_cursor_rect.y0 + 16,
                                    global_cursor_rect.x0 + 16,
                                    global_cursor_rect.y0 + metrics.line_height + 26,
                                };
                                
                                f32 padding = (helper_rect.y1 - helper_rect.y0)/2 -
                                    metrics.line_height/2;
                                
                                // NOTE(rjf): Size helper rect by how much text to draw.
                                {
                                    helper_rect.x1 += get_string_advance(app, face, highlight_param);
                                    helper_rect.x1 += get_string_advance(app, face, pre_highlight_def);
                                    helper_rect.x1 += get_string_advance(app, face, post_highlight_def);
                                    helper_rect.x1 += 2 * padding;
                                }
                                
                                if(helper_rect.x1 > view_get_screen_rect(app, view).x1)
                                {
                                    f32 difference = helper_rect.x1 - view_get_screen_rect(app, view).x1;
                                    helper_rect.x0 -= difference;
                                    helper_rect.x1 -= difference;
                                }
                                
                                Vec2_f32 text_position =
                                {
                                    helper_rect.x0 + padding,
                                    helper_rect.y0 + padding,
                                };
                                
                                F4_DrawTooltipRect(app, helper_rect);
                                
                                text_position = draw_string(app, face, pre_highlight_def,
                                                            text_position, finalize_color(defcolor_comment, 0));
                                
                                // NOTE(rjf): Spawn power mode particles if we've changed active parameters.
                                if(active_parameter_has_increased_by_one && global_power_mode_enabled)
                                {
                                    Vec2_f32 camera = F4_GetCameraFromView(app, view);
                                    
                                    f32 text_width = get_string_advance(app, face, highlight_param);
                                    
                                    for(int particle_i = 0; particle_i < 600; ++particle_i)
                                    {
                                        f32 movement_angle = RandomF32(-3.1415926535897f*3.f/2.f, 3.1415926535897f*1.f/3.f);
                                        f32 velocity_magnitude = RandomF32(20.f, 180.f);
                                        f32 velocity_x = cosf(movement_angle)*velocity_magnitude;
                                        f32 velocity_y = sinf(movement_angle)*velocity_magnitude;
                                        F4_Particle(text_position.x + 4 + camera.x + (particle_i/500.f)*text_width,
                                                    text_position.y + 8 + camera.y,
                                                    velocity_x, velocity_y,
                                                    0xffffffff,
                                                    RandomF32(1.5f, 8.f),
                                                    RandomF32(0.5f, 6.f));
                                    }
                                    
                                    global_power_mode.screen_shake += RandomF32(20.f, 40.f);
                                }
                                
                                text_position = draw_string(app, face, highlight_param,
                                                            text_position, finalize_color(defcolor_comment_pop, 1));
                                text_position = draw_string(app, face, post_highlight_def,
                                                            text_position, finalize_color(defcolor_comment, 0));
                                
                                goto end_lookup;
                            }
                        }
                    }
                }
            }
            
            end_lookup:;
            break;
        }
    }
}


//~ NOTE(rjf): Type Helper

typedef struct Declaration Declaration;
struct Declaration
{
    Token *type_name;
    Token *declaration_name;
    int pointer_count;
    Declaration *next;
};

static Declaration
F4_FindDeclarationWithString(Application_Links *app, Buffer_ID buffer,
                             Token_Iterator_Array it, Token *needle)
{
    Declaration declaration = {0};
    
    i64 pos = needle->pos;
    
    Scratch_Block scratch(app);
    get_enclosure_ranges(app, scratch, buffer, pos, RangeHighlightKind_CharacterHighlight);
    
    String_Const_u8 needle_string =
        push_buffer_range(app, scratch, buffer, Ii64(needle->pos,
                                                     needle->pos +
                                                     needle->size));
    
    for(;;)
    {
        Token *declaration_name = token_it_read(&it);
        token_it_dec_non_whitespace(&it);
        
        if(declaration_name->kind == TokenBaseKind_Identifier)
        {
            String_Const_u8 declaration_name_string =
                push_buffer_range(app, scratch, buffer, Ii64(declaration_name->pos,
                                                             declaration_name->pos +
                                                             declaration_name->size));
            
            if(string_match(declaration_name_string, needle_string))
            {
                Token *declaration_type = 0;
                int pointer_count = 0;
                
                for(;;)
                {
                    Token *token = token_it_read(&it);
                    token_it_dec_non_whitespace(&it);
                    if(token->kind == TokenBaseKind_Identifier)
                    {
                        declaration_type = token;
                        break;
                    }
                    else if(token->kind == TokenBaseKind_Operator &&
                            token->kind == TokenCppKind_Star)
                    {
                        ++pointer_count;
                    }
                    else
                    {
                        break;
                    }
                }
                
                if(declaration_name && pointer_count >= 0)
                {
                    declaration.type_name = declaration_type;
                    declaration.declaration_name = declaration_name;
                    declaration.pointer_count = pointer_count;
                    declaration.next = 0;
                    break;
                }
            }
        }
    }
    
    return declaration;
}

static Declaration *
F4_ParseDeclarationList(Application_Links *app, Arena *arena, Buffer_ID buffer, i64 pos)
{
    Declaration *head = 0;
    Declaration **target = &head;
    
    Token_Array token_array = get_token_array_from_buffer(app, buffer);
    Token_Iterator_Array it = token_iterator_pos(0, &token_array, pos);
    
    int scope_nest = 0;
    
    for(;;)
    {
        Token *type_name = token_it_read(&it);
        token_it_inc_non_whitespace(&it);
        
        if(type_name == 0)
        {
            break;
        }
        
        if(type_name->kind == TokenBaseKind_ScopeOpen)
        {
            ++scope_nest;
        }
        else if(type_name->kind == TokenBaseKind_ScopeClose)
        {
            if(--scope_nest <= 0)
            {
                break;
            }
        }
        else if((type_name->kind == TokenBaseKind_Identifier ||
                 type_name->kind == TokenBaseKind_Keyword) &&
                scope_nest == 1)
        {
            Token *declaration_name = 0;
            int pointer_count = 0;
            
            for(;;)
            {
                Token *token = token_it_read(&it);
                token_it_inc_non_whitespace(&it);
                
                if(token == 0 || token->kind == 0)
                {
                    break;
                }
                
                if(token->kind == TokenBaseKind_Operator &&
                   token->sub_kind == TokenCppKind_Star)
                {
                    ++pointer_count;
                }
                else if(token->kind == TokenBaseKind_Identifier)
                {
                    declaration_name = token;
                    break;
                }
                else
                {
                    break;
                }
            }
            
            if(declaration_name)
            {
                Declaration *decl = push_array(arena, Declaration, 1);
                decl->type_name = type_name;
                decl->pointer_count = pointer_count;
                decl->declaration_name = declaration_name;
                decl->next = 0;
                
                *target = decl;
                target = &(*target)->next;
            }
        }
    }
    
    return head;
}

static void
F4_RenderTypeHelper(Application_Links *app, Buffer_ID buffer, i64 pos)
{
    Scratch_Block scratch(app);
    
    Token_Array token_array = get_token_array_from_buffer(app, buffer);
    Token_Iterator_Array it = token_iterator_pos(0, &token_array, pos);
    token_it_dec_non_whitespace(&it);
    Token *token = token_it_read(&it);
    
    if(token && token->kind == TokenBaseKind_Identifier)
    {
        token_it_dec_non_whitespace(&it);
        token = token_it_read(&it);
    }
    
    if(token && token->kind == TokenBaseKind_Operator &&
       (token->sub_kind == TokenCppKind_Dot ||
        token->sub_kind == TokenCppKind_Arrow))
    {
        token_it_dec_non_whitespace(&it);
        token = token_it_read(&it);
    }
    
    if(token && token->kind == TokenBaseKind_Identifier)
    {
        token_it_dec_non_whitespace(&it);
        Declaration declaration = F4_FindDeclarationWithString(app, buffer, it, token);
        
        if(declaration.type_name)
        {
            Code_Index_Note *type_note =
                F4_LookUpTokenInCodeIndex(app, buffer, *declaration.type_name);
            
            if(type_note && type_note->note_kind == CodeIndexNote_Type)
            {
                Buffer_ID note_buffer = type_note->file->buffer;
                Declaration *type_declarations =
                    F4_ParseDeclarationList(app, scratch, note_buffer, type_note->pos.start);
                
                Face_ID face = global_small_code_face;
                Face_Metrics metrics = get_face_metrics(app, face);
                
                Rect_f32 tooltip_rect =
                {
                    global_cursor_rect.x0 + 16,
                    global_cursor_rect.y0 + 16,
                    global_cursor_rect.x0 + 16 + 300,
                    global_cursor_rect.y0 + 16 + metrics.line_height + 8,
                };
                
                for(Declaration *decl = type_declarations; decl; decl = decl->next)
                {
                    String_Const_u8 name = push_buffer_range(app, scratch, note_buffer,
                                                             Ii64(decl->declaration_name->pos,
                                                                  decl->declaration_name->pos +
                                                                  decl->declaration_name->size));
                    
                    String_Const_u8 type = push_buffer_range(app, scratch, note_buffer,
                                                             Ii64(decl->type_name->pos,
                                                                  decl->type_name->pos +
                                                                  decl->type_name->size));
                    
                    F4_DrawTooltipRect(app, tooltip_rect);
                    
                    Vec2_f32 text_position =
                    {
                        tooltip_rect.x0 + (tooltip_rect.y1 - tooltip_rect.y0)/2 - metrics.line_height/2,
                        (tooltip_rect.y1 + tooltip_rect.y0)/2 - metrics.line_height/2,
                    };
                    
                    ARGB_Color main_color = fcolor_resolve(fcolor_id(defcolor_comment));
                    ARGB_Color hint_color = main_color;
                    hint_color &= 0x00ffffff;
                    hint_color |= 0xab000000;
                    
                    text_position = draw_string(app, face, name, text_position,
                                                main_color);
                    
                    text_position.x += 20;
                    
                    text_position = draw_string(app, face, type, text_position,
                                                hint_color);
                    
                    f32 tooltip_rect_height = tooltip_rect.y1 - tooltip_rect.y0;
                    tooltip_rect.y0 += tooltip_rect_height;
                    tooltip_rect.y1 += tooltip_rect_height;
                }
            }
        }
    }
}


//~ TODO(rjf): Smart(er) Replace Identifier

static void
F4_SmartReplaceIdentifier(Application_Links *app, String_Const_u8 needle, String_Const_u8 replace)
{
    Scratch_Block scratch(app);
    global_history_edit_group_begin(app);
    
    Code_Index_Note *index_note = F4_LookUpStringInCodeIndex(app, needle);
    if(index_note)
    {
        String_Const_u8_Array match_patterns =
        {
            &index_note->text,
            1,
        };
        
        String_Match_List matches = find_all_matches_all_buffers(app, scratch, match_patterns,
                                                                 StringMatch_CaseSensitive,
                                                                 StringMatch_LeftSideSloppy |
                                                                 StringMatch_RightSideSloppy);
        
        Batch_Edit *batch_edit_head = 0;
        Batch_Edit **batch_edit_target = &batch_edit_head;
        
        // NOTE(rjf): This is kind of bad because it assumes that all matches in a single buffer
        // will be contiguous in the list. This is probably true but also might not be. Not
        // really good to assume this, TODO(rjf): fix this.
        Buffer_ID last_buffer_id = 0;
        
        for(String_Match *match = matches.first; match; match = match->next)
        {
            Edit edit =
            {
                replace,
                match->range,
            };
            
            Batch_Edit *batch_edit = push_array(scratch, Batch_Edit, 1);
            batch_edit->next = 0;
            batch_edit->edit = edit;
            *batch_edit_target = batch_edit;
            batch_edit_target = &(*batch_edit_target)->next;
            
            if(last_buffer_id != match->buffer || match->next == 0)
            {
                if(batch_edit_head)
                {
                    buffer_batch_edit(app, last_buffer_id, batch_edit_head);
                    batch_edit_head = 0;
                    batch_edit_target = &batch_edit_head;
                }
            }
            
            last_buffer_id = match->buffer;
        }
    }
    else
    {
        // TODO(rjf): Non-indexed thing.
    }
    
    global_history_edit_group_end(app);
}

//~ NOTE(rjf): Buffer Render

static void
F4_RenderBuffer(Application_Links *app, View_ID view_id, Face_ID face_id,
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
        F4_DrawCTokenColors(app, text_layout_id, &token_array);
        
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
        
        F4_RenderBraceHighlight(app, buffer, text_layout_id, cursor_pos,
                                colors, sizeof(colors)/sizeof(colors[0]));
    }
    
    // NOTE(allen): Line highlight
    {
        String_Const_u8 name = string_u8_litexpr("*compilation*");
        Buffer_ID compilation_buffer = get_buffer_by_name(app, name, Access_Always);
        if(global_config.highlight_line_at_cursor && (is_active_view || buffer == compilation_buffer))
        {
            i64 line_number = get_line_number_from_pos(app, buffer, cursor_pos);
            draw_line_highlight(app, text_layout_id, line_number,
                                fcolor_id(defcolor_highlight_cursor_line));
        }
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
    
    // NOTE(rjf): Error annotations
    {
        String_Const_u8 name = string_u8_litexpr("*compilation*");
        Buffer_ID compilation_buffer = get_buffer_by_name(app, name, Access_Always);
        F4_RenderErrorAnnotations(app, buffer, text_layout_id, compilation_buffer);
    }
    
    // NOTE(rjf): Token highlight
    {
        ProfileScope(app, "[Fleury] Token Highlight");
        
        Token_Iterator_Array it = token_iterator_pos(0, &token_array, cursor_pos);
        Token *token = token_it_read(&it);
        if(token && token->kind == TokenBaseKind_Identifier)
        {
            F4_RenderRangeHighlight(app, view_id, text_layout_id,
                                    Ii64(token->pos, token->pos + token->size));
        }
    }
    
    // NOTE(allen): Color parens
    if(global_config.use_paren_helper)
    {
        Color_Array colors = finalize_color_array(defcolor_text_cycle);
        draw_paren_highlight(app, buffer, text_layout_id, cursor_pos, colors.vals, colors.count);
    }
    
    // NOTE(rjf): Divider Comments
    {
        F4_RenderDividerComments(app, buffer, view_id, text_layout_id);
    }
    
    // NOTE(rjf): Cursor Mark Range
    if(is_active_view)
    {
        F4_HighlightCursorMarkRange(app, view_id);
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
            F4_RenderCursor(app, view_id, is_active_view, buffer, text_layout_id, cursor_roundness, mark_thickness, frame_info);
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
        F4_RenderCloseBraceAnnotation(app, buffer, text_layout_id, cursor_pos);
    }
    
    // NOTE(rjf): Mark Annotation
    {
        F4_RenderMarkAnnotation(app, buffer, text_layout_id, view_id, is_active_view);
    }
    
    // NOTE(rjf): Brace lines
    {
        F4_RenderBraceLines(app, buffer, view_id, text_layout_id, cursor_pos);
    }
    
    // NOTE(allen): put the actual text on the actual screen
    draw_text_layout_default(app, text_layout_id);
    
    // NOTE(rjf): Update calc (once per frame).
    // TODO(rjf): Move this into a tick hook? WTF are you doing?
    {
        static i32 last_frame_index = -1;
        if(last_frame_index != frame_info.index)
        {
            CalcUpdateOncePerFrame(frame_info);
        }
        last_frame_index = frame_info.index;
    }
    
    // NOTE(rjf): Interpret buffer as calc code, if it's the calc buffer.
    {
        Buffer_ID calc_buffer_id = get_buffer_by_name(app, string_u8_litexpr("*calc*"), AccessFlag_Read);
        if(calc_buffer_id == buffer)
        {
            F4_RenderCalcBuffer(app, buffer, view_id, text_layout_id, frame_info);
        }
    }
    
    // NOTE(rjf): Draw calc comments.
    {
        F4_RenderCalcComments(app, buffer, view_id, text_layout_id, frame_info);
    }
    
    draw_set_clip(app, prev_clip);
    
    // NOTE(rjf): Draw tooltips and stuff.
    if(active_view == view_id)
    {
        if(global_code_peek_open)
        {
            if(buffer == global_code_peek_token_buffer)
            {
                F4_RenderRangeHighlight(app, view_id, text_layout_id, global_code_peek_token_range);
            }
            F4_RenderCodePeek(app, view_id, global_small_code_face, buffer, frame_info);
        }
        else
        {
            // NOTE(rjf): Function helper
            {
                F4_RenderFunctionHelper(app, view_id, buffer, text_layout_id, cursor_pos);
            }
            
            // NOTE(rjf): Type helper
            {
                // F4_RenderTypeHelper(app, buffer, cursor_pos);
            }
        }
        
        // NOTE(rjf): Draw tooltip list.
        {
            Mouse_State mouse = get_mouse_state(app);
            
            Rect_f32 view_rect = view_get_screen_rect(app, view_id);
            
            Face_ID tooltip_face_id = global_small_code_face;
            Face_Metrics tooltip_face_metrics = get_face_metrics(app, tooltip_face_id);
            
            Rect_f32 tooltip_rect =
            {
                (f32)mouse.x + 16,
                (f32)mouse.y + 16,
                (f32)mouse.x + 16,
                (f32)mouse.y + 16 + tooltip_face_metrics.line_height + 8,
            };
            
            for(int i = 0; i < global_tooltip_count; ++i)
            {
                String_Const_u8 string = global_tooltips[i].string;
                tooltip_rect.x1 = tooltip_rect.x0;
                tooltip_rect.x1 += get_string_advance(app, tooltip_face_id, string) + 4;
                
                if(tooltip_rect.x1 > view_rect.x1)
                {
                    f32 difference = tooltip_rect.x1 - view_rect.x1;
                    tooltip_rect.x1 = (float)(int)(tooltip_rect.x1 - difference);
                    tooltip_rect.x0 = (float)(int)(tooltip_rect.x0 - difference);
                }
                
                F4_DrawTooltipRect(app, tooltip_rect);
                
                draw_string(app, tooltip_face_id, string,
                            V2f32(tooltip_rect.x0 + 4,
                                  tooltip_rect.y0 + 4),
                            global_tooltips[i].color);
            }
        }
    }
    
    // NOTE(rjf): Draw power mode.
    {
        F4_RenderPowerMode(app, view_id, face_id, frame_info);
    }
    
}

//~ NOTE(rjf): Render hook

static void
F4_DrawFileBar(Application_Links *app, View_ID view_id, Buffer_ID buffer, Face_ID face_id, Rect_f32 bar)
{
    Scratch_Block scratch(app);
    
    draw_rectangle_fcolor(app, bar, 0.f, fcolor_id(defcolor_bar));
    
    FColor base_color = fcolor_id(defcolor_base);
    FColor pop2_color = fcolor_id(defcolor_pop2);
    
    i64 cursor_position = view_get_cursor_pos(app, view_id);
    Buffer_Cursor cursor = view_compute_cursor(app, view_id, seek_pos(cursor_position));
    
    Fancy_Line list = {};
    String_Const_u8 unique_name = push_buffer_unique_name(app, scratch, buffer);
    push_fancy_string(scratch, &list, base_color, unique_name);
    push_fancy_stringf(scratch, &list, base_color, " - Row: %3.lld Col: %3.lld -", cursor.line, cursor.col);
    
    Managed_Scope scope = buffer_get_managed_scope(app, buffer);
    Line_Ending_Kind *eol_setting = scope_attachment(app, scope, buffer_eol_setting,
                                                     Line_Ending_Kind);
    switch (*eol_setting){
        case LineEndingKind_Binary:
        {
            push_fancy_string(scratch, &list, base_color, string_u8_litexpr(" bin"));
        }break;
        
        case LineEndingKind_LF:
        {
            push_fancy_string(scratch, &list, base_color, string_u8_litexpr(" lf"));
        }break;
        
        case LineEndingKind_CRLF:
        {
            push_fancy_string(scratch, &list, base_color, string_u8_litexpr(" crlf"));
        }break;
    }
    
    u8 space[3];
    {
        Dirty_State dirty = buffer_get_dirty_state(app, buffer);
        String_u8 str = Su8(space, 0, 3);
        if (dirty != 0){
            string_append(&str, string_u8_litexpr(" "));
        }
        if (HasFlag(dirty, DirtyState_UnsavedChanges)){
            string_append(&str, string_u8_litexpr("*"));
        }
        if (HasFlag(dirty, DirtyState_UnloadedChanges)){
            string_append(&str, string_u8_litexpr("!"));
        }
        push_fancy_string(scratch, &list, pop2_color, str.string);
    }
    
    Vec2_f32 p = bar.p0 + V2f32(2.f, 2.f);
    draw_fancy_line(app, face_id, fcolor_zero(), &list, p);
    
    f32 progress = (f32)cursor.line / (f32)buffer_get_line_count(app, buffer);
    Rect_f32 progress_bar_rect =
    {
        bar.x0 + (bar.x1 - bar.x0) * progress,
        bar.y0,
        bar.x1,
        bar.y1,
    };
    ARGB_Color progress_bar_color = fcolor_resolve(fcolor_id(defcolor_pop1));
    progress_bar_color &= 0x00ffffff;
    progress_bar_color |= 0x44000000;
    draw_rectangle(app, progress_bar_rect, 0, progress_bar_color);
}

static void
F4_Render(Application_Links *app, Frame_Info frame_info, View_ID view_id)
{
    ProfileScope(app, "[Fleury] Render");
    Scratch_Block scratch(app);
    
    View_ID active_view = get_active_view(app, Access_Always);
    b32 is_active_view = (active_view == view_id);
    
    Rect_f32 view_rect = view_get_screen_rect(app, view_id);
    Rect_f32 region = rect_inner(view_rect, 1.f);
    
    Buffer_ID buffer = view_get_buffer(app, view_id, Access_Always);
    String_Const_u8 buffer_name = push_buffer_base_name(app, scratch, buffer);
    
    // NOTE(rjf): Draw background.
    {
        ARGB_Color color = fcolor_resolve(fcolor_id(defcolor_back));
        
        if(string_match(buffer_name, string_u8_litexpr("*compilation*")))
        {
            color = color_blend(color, 0.5f, 0xff000000);
        }
        
        draw_rectangle(app, region, 0.f, color);
        draw_margin(app, view_rect, region, color);
    }
    
    Rect_f32 prev_clip = draw_set_clip(app, region);
    
    Face_ID face_id = get_face_id(app, buffer);
    Face_Metrics face_metrics = get_face_metrics(app, face_id);
    f32 line_height = face_metrics.line_height;
    f32 digit_advance = face_metrics.decimal_digit_advance;
    
    // NOTE(allen): file bar
    b64 showing_file_bar = false;
    if(view_get_setting(app, view_id, ViewSetting_ShowFileBar, &showing_file_bar) && showing_file_bar)
    {
        Rect_f32_Pair pair = layout_file_bar_on_top(region, line_height);
        F4_DrawFileBar(app, view_id, buffer, face_id, pair.min);
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
        line_number_rect.x1 += 4;
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
    F4_RenderBuffer(app, view_id, face_id, buffer, text_layout_id, region, frame_info);
    
    // NOTE(rjf): Draw inactive rectangle
    if(is_active_view == 0)
    {
        draw_rectangle(app, view_rect, 0.f, 0x44000000);
    }
    
    text_layout_free(app, text_layout_id);
    draw_set_clip(app, prev_clip);
}

//~ NOTE(rjf): Begin buffer hook

BUFFER_HOOK_SIG(F4_BeginBuffer)
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
F4_LayoutInner(Application_Links *app, Arena *arena, Buffer_ID buffer, Range_i64 range, Face_ID face, f32 width, Layout_Virtual_Indent virt_indent){
    Layout_Item_List list = get_empty_item_list(range);
    
    Scratch_Block scratch(app);
    String_Const_u8 text = push_buffer_range(app, scratch, buffer, range);
    
    Face_Advance_Map advance_map = get_face_advance_map(app, face);
    Face_Metrics metrics = get_face_metrics(app, face);
    LefRig_TopBot_Layout_Vars pos_vars = get_lr_tb_layout_vars(&advance_map, &metrics, width);
    
    if (text.size == 0){
        lr_tb_write_blank(&pos_vars, face, arena, &list, range.first);
    }
    else{
        b32 skipping_leading_whitespace = (virt_indent == LayoutVirtualIndent_On);
        Newline_Layout_Vars newline_vars = get_newline_layout_vars();
        
        u8 *ptr = text.str;
        u8 *end_ptr = ptr + text.size;
        for (;ptr < end_ptr;){
            Character_Consume_Result consume = utf8_consume(ptr, (u64)(end_ptr - ptr));
            
            i64 index = layout_index_from_ptr(ptr, text.str, range.first);
            switch (consume.codepoint){
                case '\t':
                case ' ':
                {
                    newline_layout_consume_default(&newline_vars);
                    f32 advance = lr_tb_advance(&pos_vars, face, consume.codepoint);
                    if (!skipping_leading_whitespace){
                        lr_tb_write_with_advance(&pos_vars, face, advance, arena, &list, index, consume.codepoint);
                    }
                    else{
                        lr_tb_advance_x_without_item(&pos_vars, advance);
                    }
                }break;
                
                default:
                {
                    newline_layout_consume_default(&newline_vars);
                    lr_tb_write(&pos_vars, face, arena, &list, index, consume.codepoint);
                }break;
                
                case '\r':
                {
                    newline_layout_consume_CR(&newline_vars, index);
                }break;
                
                case '\n':
                {
                    i64 newline_index = newline_layout_consume_LF(&newline_vars, index);
                    lr_tb_write_blank(&pos_vars, face, arena, &list, newline_index);
                    lr_tb_next_line(&pos_vars);
                }break;
                
                case max_u32:
                {
                    newline_layout_consume_default(&newline_vars);
                    lr_tb_write_byte(&pos_vars, face, arena, &list, index, *ptr);
                }break;
            }
            
            ptr += consume.inc;
        }
        
        if (newline_layout_consume_finish(&newline_vars)){
            i64 index = layout_index_from_ptr(ptr, text.str, range.first);
            lr_tb_write_blank(&pos_vars, face, arena, &list, index);
        }
    }
    
    layout_item_list_finish(&list, -pos_vars.line_to_text_shift);
    
    return(list);
}

static Layout_Item_List
F4_Layout(Application_Links *app, Arena *arena, Buffer_ID buffer, Range_i64 range, Face_ID face, f32 width)
{
    return(F4_LayoutInner(app, arena, buffer, range, face, width, LayoutVirtualIndent_Off));
}

//~ NOTE(rjf): Tick

static void
F4_Tick(Application_Links *app, Frame_Info frame_info)
{
    // NOTE(rjf): Default tick stuff from the 4th dimension:
    default_tick(app, frame_info);
    linalloc_clear(&global_frame_arena);
    global_tooltip_count = 0;
}

//~ NOTE(rjf): Whole Screen Render Hook

function void
F4_WholeScreenRender(Application_Links *app, Frame_Info frame_info)
{
    
}