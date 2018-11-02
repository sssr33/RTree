#include "SmartMemoryPool.h"

void SharedMemoryPoolSmartPtrDeleter::operator()(void *notused) {
    this->pool->destroySmart(this->ptr);
}

void UniqueMemoryPoolSmartPtrDeleter::operator()(void *notused) {
    this->pool->destroySmart(this->ptr);
}