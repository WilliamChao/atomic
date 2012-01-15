#ifndef __ist_i3ddx11_Buffer__
#define __ist_i3ddx11_Buffer__

#include "i3ddx11Types.h"
#include "i3ddx11DeviceResource.h"

namespace ist {
namespace i3ddx11 {

class Buffer : public DeviceResource
{
I3DDX11_DECLARE_DEVICE_RESOURCE(Buffer);
protected:
    GLuint m_size;
    GLuint m_capacity;

    Buffer(Device *dev);
    ~Buffer();

public:
    // data �� NULL �ł��悭�A���̏ꍇ�������m�ۂ������s����B
    void allocate(GLuint size, I3D_USAGE usage, void *data=NULL);

    void bind() const;
    void unbind() const;

    void* map(I3D_MAP_MODE mode);
    void unmap();

    GLuint size() const;
};



} // namespace i3ddx11
} // namespace ist
#endif // __ist_i3ddx11_Buffer__

