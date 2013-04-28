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


template<class T>
static inline void ForceWrite(T &dst, const T &src)
{
    DWORD old_flag;
    ::VirtualProtect(&dst, sizeof(T), PAGE_EXECUTE_READWRITE, &old_flag);
    dst = src;
    ::VirtualProtect(&dst, sizeof(T), old_flag, &old_flag);
}

static inline bool IsValidMemory(void *p)
{
    if(p==NULL) { return false; }
    MEMORY_BASIC_INFORMATION meminfo;
    return ::VirtualQuery(p, &meminfo, sizeof(meminfo))!=0 && meminfo.State!=MEM_FREE;
}

void EnumerateDLLImports(HMODULE module, const char *dllfilter,
    const std::function<void (const char*, void *&)> &f1,
    const std::function<void (DWORD ordinal, void *&func)> &f2 )
{
    if(!IsValidMemory(module)) { return; }

    size_t ImageBase = (size_t)module;
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)ImageBase;
    if(pDosHeader->e_magic!=IMAGE_DOS_SIGNATURE) { return; }

    PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)(ImageBase + pDosHeader->e_lfanew);
    DWORD RVAImports = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    if(RVAImports==0) { return; }

    std::regex reg(dllfilter, std::regex::ECMAScript|std::regex::icase);
    IMAGE_IMPORT_DESCRIPTOR *pImportDesc = (IMAGE_IMPORT_DESCRIPTOR*)(ImageBase + RVAImports);
    while(pImportDesc->Name!=0) {
        const char *pDLLName = (const char*)(ImageBase+pImportDesc->Name);
        if(std::regex_match(pDLLName, reg)) {
            IMAGE_THUNK_DATA* pThunkOrig = (IMAGE_THUNK_DATA*)(ImageBase + pImportDesc->OriginalFirstThunk);
            IMAGE_THUNK_DATA* pThunk = (IMAGE_THUNK_DATA*)(ImageBase + pImportDesc->FirstThunk);
            while(pThunkOrig->u1.AddressOfData!=0) {
                if((pThunkOrig->u1.Ordinal & 0x80000000) > 0) {
                    DWORD Ordinal = pThunkOrig->u1.Ordinal & 0xffff;
                    f2(Ordinal, *(void**)pThunk);
                }
                else {
                    IMAGE_IMPORT_BY_NAME* pIBN = (IMAGE_IMPORT_BY_NAME*)(ImageBase + pThunkOrig->u1.AddressOfData);
                    f1((char*)pIBN->Name, *(void**)pThunk);
                }
                ++pThunkOrig;
                ++pThunk;
            }
        }
        ++pImportDesc;
    }
    return;
}

void EnumerateDLLImports(HMODULE module, const char *dllfilter,
    const std::function<void (const char*, void *&)> &f1 )
{
    EnumerateDLLImports(module, dllfilter, f1, [](DWORD ordinal, void *&func){});
}


void EnumerateDLLImportsEveryModule(const char *dllfilter,
    const std::function<void (const char*, void *&)> &f1,
    const std::function<void (DWORD ordinal, void *&func)> &f2 )
{
    std::vector<HMODULE> modules;
    DWORD num_modules;
    ::EnumProcessModules(::GetCurrentProcess(), NULL, 0, &num_modules);
    modules.resize(num_modules/sizeof(HMODULE));
    ::EnumProcessModules(::GetCurrentProcess(), &modules[0], num_modules, &num_modules);
    for(size_t i=0; i<modules.size(); ++i) {
        EnumerateDLLImports(modules[i], dllfilter, f1, f2);
    }
}
void EnumerateDLLImportsEveryModule(const char *dllfilter,
    const std::function<void (const char*, void *&)> &f1 )
{
    EnumerateDLLImportsEveryModule(dllfilter, f1, [](DWORD ordinal, void *&func){});
}


// dll �� export ���Ă���֐� (�ւ� RVA) ������������
// ����ɂ��AGetProcAddress() ���Ԃ��֐�������ւ���
// ���̊֐��ւ̃|�C���^��Ԃ�
void* OverrideDLLExport(HMODULE module, const char *funcname, void *replacement)
{
    if(!IsValidMemory(module)) { return NULL; }

    size_t ImageBase = (size_t)module;
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)ImageBase;
    if(pDosHeader->e_magic!=IMAGE_DOS_SIGNATURE) { return NULL; }

    PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)(ImageBase + pDosHeader->e_lfanew);
    DWORD RVAExports = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    if(RVAExports==0) { return NULL; }

    IMAGE_EXPORT_DIRECTORY *pExportDirectory = (IMAGE_EXPORT_DIRECTORY *)(ImageBase + RVAExports);
    DWORD *RVANames = (DWORD*)(ImageBase+pExportDirectory->AddressOfNames);
    WORD *RVANameOrdinals = (WORD*)(ImageBase+pExportDirectory->AddressOfNameOrdinals);
    DWORD *RVAFunctions = (DWORD*)(ImageBase+pExportDirectory->AddressOfFunctions);
    for(DWORD i=0; i<pExportDirectory->NumberOfFunctions; ++i) {
        char *pName = (char*)(ImageBase+RVANames[i]);
        if(strcmp(pName, funcname)==0) {
            void *before = (void*)(ImageBase+RVAFunctions[RVANameOrdinals[i]]);
            ForceWrite<DWORD>(RVAFunctions[RVANameOrdinals[i]], (DWORD)replacement - ImageBase);
            return before;
        }
    }
    return NULL;
}

} // namespace ist
#endif // ist_env_Windows
