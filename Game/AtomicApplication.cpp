#include "stdafx.h"
#include "types.h"
#include "AtomicGame.h"
#include "AtomicApplication.h"
#include "Text.h"
#include "Graphics/AtomicRenderingSystem.h"
#include "Game/World.h"
#include "Sound/AtomicSound.h"
#include "Graphics/Renderer.h"

#define ATOMIC_CONFIG_FILE_PATH "atomic.conf"

namespace atomic {





AtomicConfig::AtomicConfig()
{
    window_pos              = ivec2(0, 0);
    window_size             = ivec2(1024, 768);
    fullscreen              = false;
    vsync                   = true;
    pause                   = false;
    posteffect_microscopic  = false;
    posteffect_bloom        = true;
    posteffect_antialias    = false;
    multiresolution         = false;
    show_text               = true;
    show_bloodstain         = false;
    debug_show_grid         = false;
    debug_show_distance     = false;
    debug_show_gbuffer      = 0;
    debug_show_lights       = -1;
    debug_show_resolution   = 0;
    sound_enable            = true;
    bgm_volume              = 0.5;
    se_volume               = 0.5;
    language                = LANG_JP;
}

bool AtomicConfig::readFromFile( const char* filepath )
{
    FILE *f = fopen(filepath, "r");
    if(!f) { return false; }

    char buf[256];
    ivec2 itmp;
    vec2 ftmp;
    while(fgets(buf, 256, f)) {
        if(sscanf(buf, "window_pos = %d, %d", &itmp.x, &itmp.y)==2) { window_pos.x=itmp.x; window_pos.y=itmp.y; }
        if(sscanf(buf, "window_size = %d, %d", &itmp.x, &itmp.y)==2){ window_size.x=itmp.x; window_size.y=itmp.y; }
        if(sscanf(buf, "fullscreen = %d", &itmp.x)==1)              { fullscreen=itmp.x!=0; }
        if(sscanf(buf, "vsync = %d", &itmp.x)==1)                   { vsync=itmp.x!=0; }
        if(sscanf(buf, "posteffect_microscopic = %d", &itmp.x)==1)  { posteffect_microscopic=(itmp.x!=0); }
        if(sscanf(buf, "posteffect_bloom = %d", &itmp.x)==1)        { posteffect_bloom=(itmp.x!=0); }
        if(sscanf(buf, "posteffect_antialias = %d", &itmp.x)==1)    { posteffect_antialias=(itmp.x!=0); }
        if(sscanf(buf, "multiresolution = %d", &itmp.x)==1)         { multiresolution=(itmp.x!=0); }
        if(sscanf(buf, "show_text = %d", &itmp.x)==1)               { show_text=(itmp.x!=0); }
        if(sscanf(buf, "show_bloodstain = %d", &itmp.x)==1)         { show_bloodstain=(itmp.x!=0); }
        if(sscanf(buf, "debug_show_grid = %d", &itmp.x)==1)         { debug_show_grid=(itmp.x!=0); }
        if(sscanf(buf, "debug_show_distance = %d", &itmp.x)==1)     { debug_show_distance=(itmp.x!=0); }
        if(sscanf(buf, "debug_show_resolution = %d", &itmp.x)==1)   { debug_show_resolution=(itmp.x!=0); }
        if(sscanf(buf, "sound_enable = %f", &itmp.x)==1)            { sound_enable=(itmp.x!=0); }
        if(sscanf(buf, "bgm_volume = %f", &ftmp.x)==1)              { bgm_volume=ftmp.x; }
        if(sscanf(buf, "se_volume = %f", &ftmp.x)==1)               { se_volume=ftmp.x; }
    }
    fclose(f);
    return true;
}

bool AtomicConfig::writeToFile( const char* filepath )
{
    FILE *f = fopen(filepath, "w");
    if(!f) { return false; }

    fprintf(f, "window_pos = %d, %d\n",         window_pos.x, window_pos.y);
    fprintf(f, "window_size = %d, %d\n",        window_size.x, window_size.y);
    fprintf(f, "fullscreen = %d\n",             fullscreen);
    fprintf(f, "vsync = %d\n",                  vsync);
    fprintf(f, "posteffect_microscopic = %d\n", posteffect_microscopic);
    fprintf(f, "posteffect_bloom = %d\n",       posteffect_bloom);
    fprintf(f, "posteffect_antialias = %d\n",   posteffect_antialias);
    fprintf(f, "multiresolution = %d\n",        multiresolution);
    fprintf(f, "show_text = %d\n",              show_text);
    fprintf(f, "show_bloodstain = %d\n",        show_bloodstain);
    fprintf(f, "debug_show_grid = %d\n",        debug_show_grid);
    fprintf(f, "debug_show_distance = %d\n",    debug_show_distance);
    fprintf(f, "debug_show_resolution = %d\n",  debug_show_resolution);
    fprintf(f, "sound_enable = %d\n",           sound_enable);
    fprintf(f, "bgm_volume = %f\n",             bgm_volume);
    fprintf(f, "se_volume = %f\n",              se_volume);
    fclose(f);
    return true;
}


AtomicApplication *g_appinst = NULL;

AtomicApplication* AtomicApplication::getInstance() { return g_appinst; }

AtomicApplication::AtomicApplication()
    : m_request_exit(false)
    , m_game(NULL)
{
    if(g_appinst) { istAssert("already initialized"); }
    g_appinst = this;

}

AtomicApplication::~AtomicApplication()
{
    if(g_appinst==this) { g_appinst=NULL; }
}

bool AtomicApplication::initialize(int argc, char *argv[])
{
    InitializeText();


    m_config.readFromFile(ATOMIC_CONFIG_FILE_PATH);
    if(m_config.window_pos.x >= 30000) { m_config.window_pos.x = 0; }
    if(m_config.window_pos.y >= 30000) { m_config.window_pos.y = 0; }
    if(m_config.window_size.x < 320 || m_config.window_size.x < 240) { m_config.window_size = ivec2(1024, 768); }


    // create window
    ivec2 wpos = m_config.window_pos;
    ivec2 wsize = m_config.window_size;
    if(!super::initialize(wpos, wsize, L"atomic", m_config.fullscreen))
    {
        return false;
    }
    TaskScheduler::initializeInstance();


    // initialize CUDA
    {
        cudaError_t e;
        int dev_count;
        e = cudaGetDeviceCount(&dev_count);
        if(e==cudaErrorNoDevice) {
            handleError(ATERR_CUDA_NO_DEVICE);
            return false;
        }
        else if(e==cudaErrorInsufficientDriver) {
            handleError(ATERR_CUDA_INSUFFICIENT_DRIVER);
            return false;
        }

        for(int32 i=0; i<dev_count; ++i) {
            cudaDeviceProp dev;
            cudaGetDeviceProperties(&dev, i);
            istPrint("CUDA device %d: %s\n", i, dev.name);
        }

        int device_id = cutGetMaxGflopsDeviceId();
        CUDA_SAFE_CALL( cudaSetDevice(device_id) );
        CUDA_SAFE_CALL( cudaGLSetGLDevice(device_id) );
    }

    // start rendering thread
    AtomicRenderingSystem::initializeInstance();

    // initialize sound
    AtomicSound::initializeInstance();


    // create game
    m_game = istNew(AtomicGame)();
    if(argc > 1) {
        m_game->readReplayFromFile(argv[1]);
    }

    return true;
}

void AtomicApplication::finalize()
{
    m_config.writeToFile(ATOMIC_CONFIG_FILE_PATH);

    AtomicRenderingSystem::finalizeInstance();
    AtomicSound::finalizeInstance();

    istSafeDelete(m_game);

    TaskScheduler::finalizeInstance();
    super::finalize();

    FinalizeText();
}

void AtomicApplication::mainLoop()
{
    while(!m_request_exit)
    {
        translateMessage();

        PerformanceCounter pc;
        float dt = 0.0f;
        if(m_game) {
            m_game->frameBegin();
            m_game->update(dt);
            m_game->asyncupdateBegin(dt);
            updateInput();
            m_game->asyncupdateEnd();
            m_game->draw();
            m_game->frameEnd();
            dt = pc.getElapsedMillisecond();
            pc.reset();
        }
    }
}

void AtomicApplication::updateInput()
{
    super::updateInput();

    vec2 move = vec2(0.0f);
    int buttons = getJoyState().getButtons();

    const ist::MouseState &mouse = getMouseState();
    if((mouse.getButtonState() & ist::MouseState::BU_LEFT)!=0)  { buttons = buttons |= 1<<0; }
    if((mouse.getButtonState() & ist::MouseState::BU_RIGHT)!=0) { buttons = buttons |= 1<<1; }
    if((mouse.getButtonState() & ist::MouseState::BU_MIDDLE)!=0){ buttons = buttons |= 1<<2; }

    const ist::KeyboardState &kb = getKeyboardState();
    if(kb.isKeyPressed('Z')){ buttons = buttons |= 1<<0; }
    if(kb.isKeyPressed('X')){ buttons = buttons |= 1<<1; }
    if(kb.isKeyPressed('C')){ buttons = buttons |= 1<<2; }
    if(kb.isKeyPressed('V')){ buttons = buttons |= 1<<3; }
    if(kb.isKeyPressed(ist::KEY_RIGHT)  || kb.isKeyPressed('D')){ move.x = 1.0f; }
    if(kb.isKeyPressed(ist::KEY_LEFT)   || kb.isKeyPressed('A')){ move.x =-1.0f; }
    if(kb.isKeyPressed(ist::KEY_UP)     || kb.isKeyPressed('W')){ move.y = 1.0f; }
    if(kb.isKeyPressed(ist::KEY_DOWN)   || kb.isKeyPressed('S')){ move.y =-1.0f; }
    if(kb.isKeyTriggered(ist::KEY_F1)) {
        m_config.posteffect_antialias = !m_config.posteffect_antialias;
    }
    if(getKeyboardState().isKeyTriggered(ist::KEY_F2)) {
        m_config.posteffect_bloom = !m_config.posteffect_bloom;
    }
    if(getKeyboardState().isKeyTriggered(ist::KEY_F3)) {
        m_config.debug_show_gbuffer--;
    }
    if(getKeyboardState().isKeyTriggered(ist::KEY_F4)) {
        m_config.debug_show_gbuffer++;
    }
    if(getKeyboardState().isKeyTriggered(ist::KEY_F5)) {
        m_config.debug_show_lights--;
    }
    if(getKeyboardState().isKeyTriggered(ist::KEY_F6)) {
        m_config.debug_show_lights++;
    }
    if(getKeyboardState().isKeyTriggered(ist::KEY_F7)) {
        m_config.pause = !m_config.pause;
    }
    if(getKeyboardState().isKeyTriggered(ist::KEY_F8)) {
        m_config.multiresolution = !m_config.multiresolution;
    }
    if(getKeyboardState().isKeyTriggered(ist::KEY_F9)) {
        m_config.debug_show_resolution = !m_config.debug_show_resolution;
    }
    if(getKeyboardState().isKeyPressed('8')) {
        float &p = atomicGetLights()->getMultiresolutionParams().Threshold.x;
        p = clamp(p-0.001f, 0.0f, 1.0f);
    }
    if(getKeyboardState().isKeyPressed('9')) {
        float &p = atomicGetLights()->getMultiresolutionParams().Threshold.x;
        p = clamp(p+0.001f, 0.0f, 1.0f);
    }

    {
        vec2 jpos = vec2((float)getJoyState().getX(), -(float)getJoyState().getY());
        jpos /= 32768.0f;
        if(glm::length(jpos)>0.4f) { move=jpos; }
    }
    m_inputs.copyToBack();
    m_inputs.setMove(move);
    m_inputs.setButtons(buttons);
}

int AtomicApplication::handleWindowMessage(const ist::WindowMessage& wm)
{
    switch(wm.type)
    {
    case ist::WindowMessage::MES_CLOSE:
        {
            m_request_exit = true;
        }
        return 0;

    case ist::WindowMessage::MES_KEYBOARD:
        {
            const ist::WM_Keyboard& m = static_cast<const ist::WM_Keyboard&>(wm);
            if(m.action==ist::WM_Keyboard::ACT_KEYUP && m.key==ist::KEY_ESCAPE) {
                m_request_exit = true;
            }
        }
        return 0;

    case ist::WindowMessage::MES_WINDOW_SIZE:
        {
            const ist::WM_WindowSize& m = static_cast<const ist::WM_WindowSize&>(wm);
            m_config.window_size = m.window_size;
        }
        return 0;

    case ist::WindowMessage::MES_WINDOW_MOVE:
        {
            const ist::WM_WindowMove& m = static_cast<const ist::WM_WindowMove&>(wm);
            m_config.window_pos = m.window_pos;
        }
        return 0;

    case ist::WindowMessage::MES_IME_RESULT:
        {
            const ist::WM_IME& m = static_cast<const ist::WM_IME&>(wm);
            stl::wstring str(m.text, m.text_len);
            handleCommandLine(str);
        }
        return 0;

    case ist::WindowMessage::MES_IME_BEGIN:
        {
            istPrint(L"MES_IME_BEGIN\n");
        }
        break;
    case ist::WindowMessage::MES_IME_END:
        {
            istPrint(L"MES_IME_END\n");
        }
        break;
    case ist::WindowMessage::MES_IME_CHAR:
        {
            const ist::WM_IME& m = static_cast<const ist::WM_IME&>(wm);
            stl::wstring str(m.text, m.text_len);
            istPrint(L"MES_IME_CHAR %s\n", str.c_str());
        }
        break;
    }

    return 0;
}

void AtomicApplication::handleError(ATOMIC_ERROR e)
{
    stl::wstring mes;
    switch(e) {
    case ATERR_OPENGL_330_IS_NOT_SUPPORTED:   mes=GetText(TID_OPENGL330_IS_NOT_SUPPORTED); break;
    case ATERR_CUDA_NO_DEVICE:                mes=GetText(TID_ERROR_CUDA_NO_DEVICE); break;
    case ATERR_CUDA_INSUFFICIENT_DRIVER:      mes=GetText(TID_ERROR_CUDA_INSUFFICIENT_DRIVER); break;
    }
    istShowMessageDialog(mes.c_str(), L"error", DLG_OK);
}


void AtomicApplication::drawCallback()
{
    m_game->drawCallback();
}



typedef void (*CommandLineHandler)(const stl::wstring &command);
typedef stl::pair<stl::wstring, CommandLineHandler> CommandPair;
typedef stl::vector<CommandPair> CommandLineHandlerTable;


void cmd_pause(const stl::wstring &value) {
    atomicGetConfig()->pause = !atomicGetConfig()->pause;
}

void cmd_set_posteffect_microscopic(const stl::wstring &value) {
    int v;
    if( swscanf(value.c_str(), L"%d", &v) ) {
        atomicGetConfig()->posteffect_microscopic = v!=0;
    }
}

void cmd_set_posteffect_bloom(const stl::wstring &value) {
    int v;
    if( swscanf(value.c_str(), L"%d", &v) ) {
        atomicGetConfig()->posteffect_bloom = v!=0;
    }
}

void cmd_set_posteffect_antialias(const stl::wstring &value) {
    int v;
    if( swscanf(value.c_str(), L"%d", &v) ) {
        atomicGetConfig()->posteffect_antialias = v!=0;
    }
}

void cmd_set_multiresolution(const stl::wstring &value) {
    int v;
    if( swscanf(value.c_str(), L"%d", &v) ) {
        atomicGetConfig()->multiresolution = v!=0;
    }
}

void cmd_set_show_text(const stl::wstring &value) {
    int v;
    if( swscanf(value.c_str(), L"%d", &v) ) {
        atomicGetConfig()->show_text = v!=0;
    }
}

void cmd_set_show_bloodstain(const stl::wstring &value) {
    int v;
    if( swscanf(value.c_str(), L"%d", &v) ) {
        atomicGetConfig()->show_bloodstain = v!=0;
    }
}

void cmd_set_debug_show_grid(const stl::wstring &value) {
    int v;
    if( swscanf(value.c_str(), L"%d", &v) ) {
        atomicGetConfig()->debug_show_grid = v!=0;
    }
}

struct equal_first {
    const stl::wstring command;
    equal_first(const stl::wstring &c) : command(c) {}
    bool operator()(const CommandPair &v) const {
        return command.find(v.first) != stl::wstring::npos;
    }
};

int32 AtomicApplication::handleCommandLine( const stl::wstring &command )
{
    static CommandLineHandlerTable s_table;
    if(s_table.empty()) {
        s_table.push_back(CommandPair(L"pause",                     cmd_pause));
        s_table.push_back(CommandPair(L"set posteffect_microscopic",cmd_set_posteffect_microscopic));
        s_table.push_back(CommandPair(L"set posteffect_bloom",      cmd_set_posteffect_bloom));
        s_table.push_back(CommandPair(L"set posteffect_antialias",  cmd_set_posteffect_antialias));
        s_table.push_back(CommandPair(L"set multiresolution",       cmd_set_multiresolution));
        s_table.push_back(CommandPair(L"set show_text",             cmd_set_show_text));
        s_table.push_back(CommandPair(L"set show_bloodstain",       cmd_set_show_bloodstain));
        s_table.push_back(CommandPair(L"set debug_show_grid",       cmd_set_debug_show_grid));
    }

    CommandLineHandlerTable::iterator p = stl::find_if(s_table.begin(), s_table.end(), equal_first(command));
    if(p != s_table.end()) {
        stl::wstring value(command.c_str()+(p->first.size()+1));
        p->second(value);
        return 1;
    }
    return 0;
}


} // namespace atomic