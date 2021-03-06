﻿#include "istPCH.h"
#include "ist/Base/New.h"
#include "ist/Base/Allocator.h"

namespace ist {

void BadAllocHandlerGeneric(const void* allocator_ptr)
{
    DebugBreak();
}

IAllocator* GetDefaultAllocator()
{
    static HeapAllocator s_alloc;
    return &s_alloc;
}



void* HeapAllocator::allocate(size_t size, size_t align)
{
    return istAlignedMalloc(size, align);
}

void HeapAllocator::deallocate(void* p)
{
    istAlignedFree(p);
}



StackAllocator::StackAllocator()
    : m_memory(NULL)
    , m_block_size(0)
    , m_position(0)
    , m_parent(NULL)
{}

StackAllocator::StackAllocator(size_t block_size, size_t alignment, IAllocator *parent)
{
    initialize(block_size, alignment, parent);
}

StackAllocator::~StackAllocator()
{
    if(m_parent) {
        m_parent->deallocate(m_memory);
    }
}

void StackAllocator::initialize(size_t block_size, size_t alignment, IAllocator *parent)
{
    m_block_size    = block_size;
    m_position      = 0;
    m_parent        = parent;
    m_memory        = parent->allocate(block_size, alignment);
}

void* StackAllocator::allocate(size_t size, size_t alignment)
{
    alignment &= ~7;
    size_t alignment_mask = alignment-1;
    size_t position = m_position;
    size_t begin_pos = (position + alignment_mask) & alignment_mask;
    size_t gap = begin_pos - position;
    size_t real_size = size+gap;

    if(position+size > m_block_size) {
        BadAllocHandler(this);
    }
    char* p = (char*)m_memory+position+gap;
    m_position += real_size;
    return p;
}

void StackAllocator::deallocate(void* p)
{
    // do nothing
}

void StackAllocator::clear()
{
    m_position = 0;
}



template<class T>
TFixedAllocator<T>::TFixedAllocator( size_t element_size, size_t max_elements, size_t alignment, IAllocator *parent )
{
    istAssert(element_size>=sizeof(void*)); // element_size must be at least sizeof(void*)
    m_memory = NULL;
    m_used = NULL;
    m_element_size = element_size;
    m_max_elements = max_elements;
    m_alignment = alignment;
    m_parent = parent;

    char* mem = (char*)parent->allocate(element_size*max_elements, alignment);
    m_unused = mem;
    for(size_t i=0; i<max_elements-1; ++i) {
        reinterpret_cast<Node*>(mem + element_size*i)->pNext = mem + element_size*(i+1);
    }
    reinterpret_cast<Node*>(mem + element_size*(max_elements-1))->pNext = NULL;
    m_memory = mem;
}

template<class T>
TFixedAllocator<T>::~TFixedAllocator()
{
    if(m_parent) {
        m_parent->deallocate(m_memory);
    }
}

template<class T>
void* TFixedAllocator<T>::allocate()
{
    int32 i = m_used++;
    if(i >= (int32)m_max_elements) {
        --m_used;
        return NULL;
    }
    void *ret = m_unused;
    m_unused = reinterpret_cast<Node*>(m_unused)->pNext;
    return ret;
}

template<class T>
bool TFixedAllocator<T>::canDeallocate( void *p ) const
{
    return p>=m_memory && p<((char*)m_memory+(m_element_size*m_max_elements));
}

template<class T>
void TFixedAllocator<T>::defrag()
{
    stl::sort(m_unused+m_used, m_unused+m_max_elements);
}

template<class T>
void* TFixedAllocator<T>::allocate(size_t size, size_t align)
{
    istAssert(size <= getElementSize());
    return allocate();
}

template<class T>
void TFixedAllocator<T>::deallocate(void* p)
{
    Node *node = reinterpret_cast<Node*>(p);
    node->pNext = m_unused;
    m_unused = (char*)node;
}




template<class T>
TChainedFixedAllocator<T>::TChainedFixedAllocator(size_t element_size, size_t max_elements, size_t alignment, IAllocator *parent)
    : m_block(NULL)
    , m_next(NULL)
{
    m_block = istNew(BlockT)(element_size, max_elements, alignment, parent);
}

template<class T>
TChainedFixedAllocator<T>::~TChainedFixedAllocator()
{
    istSafeDeleteA(m_next, m_block->getParent());
    istSafeDeleteA(m_block, m_block->getParent());
}

template<class T>
void* TChainedFixedAllocator<T>::allocate()
{
    void *r = m_block->allocate();
    // allocate に失敗した場合、次の block からの確保を試みる。
    // この時、block が未構築であれば lock して構築する必要がある。
    if(r==NULL) {
        if(m_next==NULL) {
            MutexT::ScopedLock l(m_mutex);
            if(m_next==NULL) { // 別のスレッドが既に構築した可能性があるので、再チェックが必要
                m_next = istNewA(TChainedFixedAllocator, m_block->getParent())(
                    m_block->getElementSize(), m_block->getMaxElements(), m_block->getAlignment(), m_block->getParent() );
            }
        }
        r = m_next->allocate();
    }
    return r;
}

template<class T>
bool TChainedFixedAllocator<T>::canDelete(void *p) const
{
    if(!m_block->canDeallocate(p)) {
        if(m_next!=NULL) {
            return m_next->canDelete(p);
        }
    }
    return false;
}

template<class T>
void* TChainedFixedAllocator<T>::allocate(size_t size, size_t align)
{
    istAssert(size <= m_block->getElementSize());
    return allocate();
}

template<class T>
void TChainedFixedAllocator<T>::deallocate(void* p)
{
    if(m_block->canDeallocate(p)) {
        m_block->deallocate(p);
    }
    else if(m_next!=NULL) {
        m_next->deallocate(p);
    }
    else {
        istAssert(false);
    }
}

// explicit instanciation
template TFixedAllocator<Allocator_SingleThreadPolicy>;
template TFixedAllocator<Allocator_MultiThreadPolicy>;
template TChainedFixedAllocator<Allocator_SingleThreadPolicy>;
template TChainedFixedAllocator<Allocator_MultiThreadPolicy>;


} // namespace ist
