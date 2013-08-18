#include "istPCH.h"
#include "Hook.h"

#ifdef ist_env_Windows
#pragma comment(lib, "psapi.lib")
#include <windows.h>
#include <psapi.h>

namespace ist {


// target: �֐��|�C���^�B�Ώۊ֐��� hotpatch ���Č��̊֐��ւ̃|�C���^��Ԃ�
void* Hotpatch( void *target, const void *replacement )
{
    DWORD old;
    BYTE *f = (BYTE*)target;
    void *orig_func = f+2;
    ::VirtualProtect(f-5, 7, PAGE_EXECUTE_READWRITE, &old);
    f[-5]=0xE9; // jmp
    *((ptrdiff_t*)(f-4)) = (ptrdiff_t)replacement-(ptrdiff_t)f;
    f[0]=0xEB; f[1]=0xF9; // short jmp -7
    ::VirtualProtect(f-5, 7, old, &old);
    return orig_func;
}



// �֐��|�C���^ addr ���� required byte ���܂� instruction �̃T�C�Y�����߂�
size_t GuessInstructionSize(void *addr, size_t required)
{
    // �s���S�ɂ��A���Ή��� instruction ������ΓK�X�ǉ����ׂ�
    // �֐��̓� 5 byte �ȓ��Ŏ��s�������̂͑����� mov,sub,push ������Ȃ̂ł��ꂾ���ł������ɑΉ��͂ł���n�Y
    size_t ret = 0;
    BYTE *data = (BYTE*)addr;
    for(; ret<required; ) {
        switch(data[ret]) {
            // push
        case 0x55: ret+=1; break;
        case 0x68:
            ret+=1;
            switch(data[ret]) {
            case 0x6C: ret+=4; break;
            default:   ret+=1; break;
            }
            break;
        case 0x6A: ret+=5; break;
        case 0xFF: ret+=3; break;

            // mov
        case 0x8B:
            ret+=1;
            switch(data[ret]) {
            case 0x44: ret+=3; break;
            case 0x45: ret+=2; break;
            default:   ret+=1; break;
            }
            break;
        case 0xB8: ret+=5; break;

            // sub
        case 0x81: 
            ret+=1;
            switch(data[ret]) {
            case 0xEC: ret+=5; break;
            default:   ret+=1; break;
            }
            break;
        case 0x83:
            ret+=1;
            switch(data[ret]) {
            case 0xEC: ret+=2; break;
            default:   ret+=1; break;
            }
            break;
        default: ret+=1; break;
        }
    }
    return ret;
}

// target: �֐��|�C���^�B�Ώۊ֐����㏑�������̃R�[�h�͑ޔ����āA���̊֐��ւ̃|�C���^��Ԃ�
void* UglyHotpatch( void *target, const void *replacement )
{
    // ���R�[�h�̑ޔ��
    BYTE *before = (BYTE*)::VirtualAlloc(NULL, 64, MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE);

    BYTE *f = (BYTE*)target;
    DWORD old;
    ::VirtualProtect(f, 32, PAGE_EXECUTE_READWRITE, &old);

    // ���̃R�[�h���R�s�[ & �Ō�ɃR�s�[�{�� jmp ����R�[�h��t�� (==����� call ����Ώ㏑���O�̓��������n�Y)
    size_t slice = GuessInstructionSize(f, 5);
    memcpy(before, f, slice);
    before[slice]=0xE9; // jmp
    *(DWORD*)(before+slice+1) = (ptrdiff_t)(f+slice)-(ptrdiff_t)(before+slice) - 5;

    // �֐��̐擪�� hook �֐��ւ� jmp �ɏ���������
    f[0]=0xE9; // jmp
    *(DWORD*)(f+1) = (ptrdiff_t)replacement-(ptrdiff_t)f - 5;
    ::VirtualProtect(f, 32, old, &old);

    return before;
}

} // namespace ist
#endif // ist_env_Windows
