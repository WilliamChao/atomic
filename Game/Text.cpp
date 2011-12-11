#include "stdafx.h"
#include "Text.h"

namespace atomic {

    static const wchar_t* g_jp[TID_END];
    static const wchar_t* g_en[TID_END];
    static const wchar_t** g_text = g_jp;

    bool InitializeText()
    {
        // error message
        g_jp[TID_ERROR_CUDA_NO_DEVICE]          = L"CUDA �f�o�C�X��������܂���ł����B\n�\���󂠂�܂��񂪁A���g���̃r�f�I�J�[�h�ł͋��炭���̃v���O�����͓����܂���B";
        g_en[TID_ERROR_CUDA_NO_DEVICE]          = L"CUDA device not found.\nthis machine's video card can't run this program";
        g_jp[TID_ERROR_CUDA_INSUFFICIENT_DRIVER]= L"CUDA �f�o�C�X��������܂���ł����B\n�h���C�o�̃A�b�v�f�[�g�ŉ����ł���\��������܂��B";
        g_en[TID_ERROR_CUDA_INSUFFICIENT_DRIVER]= L"CUDA device not found.\nupdating video card driver may solve this problem.";

        // system message

        return true;
    }

    void FinalizeText()
    {

    }

    void SetLanguage( LANGUAGE_ID lid )
    {
        switch(lid) {
        case LANG_JP: g_text = g_jp; break;
        case LANG_EN: g_text = g_en; break;
        }
    }

    const wchar_t* GetText(TEXT_ID tid)
    {
        return g_text[tid];
    }

} // namespace atomic
