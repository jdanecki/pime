#ifndef SERIALIZATION_RUST_H
#define SERIALIZATION_RUST_H

#include "object.h"

class InventoryElement;
extern "C" {
/// <div rustbindgen hide></div>
InventoryElement* get_object_by_id(size_t uid);
}

template <typename T> class SerializablePointer
{
    T * ptr;
    NetworkObject id;

  public:
    SerializablePointer(T * p);
    T * get()
    {
      if (ptr)
        return ptr;
      return (T*)(get_object_by_id(id.uid));
    }
};

template <typename T> SerializablePointer<T>::SerializablePointer(T * p) : ptr(p), id(Class_Unknown, 0)
{
  if (p) {
    id.c_id = p->c_id;
    id.uid = p->uid;
  }
}

/// <div rustbindgen nodebug></div>
class SerializableCString
{
  public:
    const char * str;
    SerializableCString(const char * s);
};

#endif
