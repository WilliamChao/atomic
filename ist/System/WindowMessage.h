﻿#ifndef ist_Application_WindowMessage_h
#define ist_Application_WindowMessage_h
namespace ist {

enum WMType
{
    WMT_Unknown,

    WMT_WindowOpen,
    WMT_WindowClose,
    WMT_WindowFocus,
    WMT_WindowDefocus,
    WMT_WindowSize,
    WMT_WindowMove,

    WMT_KeyDown,
    WMT_KeyUp,
    WMT_KeyChar,

    WMT_MouseDown,
    WMT_MouseUp,
    WMT_MouseMove,
    WMT_MouseWheelDown,
    WMT_MouseWheelUp,
    WMT_MouseWheelLeft,
    WMT_MouseWheelRight,

    WMT_IMEBegin,
    WMT_IMEEnd,
    WMT_IMECandidateOpen,
    WMT_IMECandidateClose,
    WMT_IMECandidateChange,
    WMT_IMECursorMove,
    WMT_IMENotify,
    WMT_IMEChar,
    WMT_IMEResult,
};

struct WM_Base
{
    uint32 type; // WMType or user defined WM type
};

struct WM_Window : public WM_Base
{
    ivec2 window_size;
    ivec2 window_pos;

    static const WM_Window& cast(const WM_Base &wm) { return static_cast<const WM_Window&>(wm); }
};

struct WM_Keyboard : public WM_Base
{
    uint16 key;

    WM_Keyboard() { istMemset(this, 0, sizeof(*this)); }

    static const WM_Keyboard& cast(const WM_Base &wm) { return static_cast<const WM_Keyboard&>(wm); }
};

struct WM_Mouse : public WM_Base
{
    vec2 mouse_pos;
    vec2 mouse_move;
    int16 wheel;
    struct {
        uint16 left:1;
        uint16 right:1;
        uint16 middle:1;
        uint16 ctrl:1;
        uint16 shift:1;
    } button;

    WM_Mouse() { istMemset(this, 0, sizeof(*this)); }

    static const WM_Mouse& cast(const WM_Base &wm) { return static_cast<const WM_Mouse&>(wm); }
};

// MES_IME_CHAR, MES_IME_RESULT のとき、text, text_len に入力データが入っている
struct WM_IME : public WM_Base
{
    uint32 text_len;
    uint32 num_candidates;
    uint32 cursor_pos;
    uint32 candidate_index;
    wchar_t *text;
    wchar_t *candidates;

    WM_IME()
    {
        text_len = 0;
        num_candidates = 0;
        cursor_pos = 0;
        candidate_index = 0;
        text = NULL;
        candidates = NULL;
    }

    static const WM_IME& cast(const WM_Base &wm) { return static_cast<const WM_IME&>(wm); }
};


} // namspace ist
#endif // ist_Application_WindowMessage_h
