#ifndef ist_System_Hook_h
#define ist_System_Hook_h
#ifdef ist_env_Windows

namespace ist {

// target: �֐��|�C���^�B�Ώۊ֐��� hotpatch ���Č��̊֐��ւ̃|�C���^��Ԃ�
void* Hotpatch( void *target, const void *replacement );
void* UglyHotpatch( void *target, const void *replacement );

} // namespace ist
#endif // ist_env_Windows
#endif // ist_System_Hook_h
