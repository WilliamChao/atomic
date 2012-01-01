#ifndef __ist_Application__
#define __ist_Application__

#include "WindowMessage.h"
#include "InputState.h"


namespace ist {


class Application
{
public:
    enum DLG_TYPE {
        DLG_OK              = 0x00000000L,
        DLG_OKCANCEL        = 0x00000001L,
        DLG_ABORTIGNORE     = 0x00000002L,
        DLG_CANCELCONTINUE  = 0x00000006L,
        DLG_RETRYCANCEL     = 0x00000005L,
        DLG_YESNO           = 0x00000004L,
        DLG_YESNOCANCEL     = 0x00000003L,
    };
    enum DLG_ICON {
        ICON_EXCLAMATION    = 0x00000030L,
        ICON_WARNING        = 0x00000030L,
        ICON_INFORMATION    = 0x00000040L,
        ICON_ASTERISK       = 0x00000040L,
        ICON_QUESTION       = 0x00000020L,
        ICON_ERROR          = 0x00000010L,
        ICON_HAND           = 0x00000010L,
    };
    enum DLG_RET {
        DLGRET_OK       = 1,
        DLGRET_CANCEL   = 2,
        DLGRET_ABORT    = 3,
        DLGRET_RETRY    = 4,
        DLGRET_IGNORE   = 5,
        DLGRET_YES      = 6,
        DLGRET_NO       = 7,
        DLGRET_TRYAGAIN = 10,
        DLGRET_CONTINUE = 11,
    };

private:
    static const int MAX_JOYSTICK_NUM = 4;

    HWND        m_hwnd;
    DEVMODE     m_devmode;
    bool        m_fullscreen;

    KeyboardState   m_keyboard_state;
    MouseState      m_mouse_state;
    JoyState        m_joy_state[MAX_JOYSTICK_NUM];

    size_t m_width, m_height;

public:
    static Application* getInstance();

    Application();
    virtual ~Application();

    virtual bool initialize(ivec2 wpos, ivec2 wsize, const wchar_t *title, bool fullscreen=false);
    virtual void finalize();

    virtual void mainLoop()=0;
    virtual int handleWindowMessage(const WindowMessage& wm)=0;

    // ���͏��̍X�V�͎��Ԃ������邱�Ƃɒ��ӁB(�����炭 GPU ���\�[�X�̂悤�Ƀ��b�N������̂��Ǝv����)
    // �܂��A�����������X���b�h����łȂ��Ɛ���ɍX�V�ł��Ȃ��H�悤�ŁA�񓯊��ɍX�V����ۂ́A
    // ���[�J�[�X���b�h�Ƀ^�X�N������U�����チ�C���X���b�h����X�V�A�̂悤�Ȃ�₱�����菇���K�v�B
    virtual void updateInput();

    virtual void translateMessage();

    int showMessageDialog(const wchar_t* message, const wchar_t* caption, int dlgtype=DLG_OK);

    size_t getWindowWidth() const   { return m_width; }
    size_t getWindowHeight() const  { return m_height; }
    const KeyboardState& getKeyboardState() const   { return m_keyboard_state; }
    const MouseState& getMouseState() const         { return m_mouse_state; }
    const JoyState& getJoyState(int i=0) const      { return m_joy_state[i]; }

    DisplaySetting getCurrentDisplaySetting() const;
    void getAvalableDisplaySettings(DisplaySetting*& settings, int& num_settings) const;

#ifdef _WIN32
    HWND getWindowHandle() const { return m_hwnd; }
#endif // _WIN32
};

} // namespace ist


#define istGetAplication()  ist::Application::getInstance()
#define istShowMessageDialog(mes, cap, dtype) istGetAplication()->showMessageDialog(mes, cap, dtype)


#endif // __ist_Application__
