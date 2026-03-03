#ifndef SERIALIZATION_H
#define SERIALIZATION_H
#include "object.h"

extern NetworkObject * get_object_by_id(NetworkObject obj);

template <typename T> class SerializablePointer
{
    T * ptr;
    NetworkObject id;
  public:
    SerializablePointer(T * p);
    SerializablePointer();
    T * get()
    {
    	return ptr;
    }
    void set(T * p)
    {
        ptr = p;
        if (p)
		{
			id.c_id = p->c_id;
			id.uid = p->uid;
		}
        else {
        	id.c_id = Class_Unknown;
        	id.uid = 0;
        }
    }
    void show() {
    	CONSOLE_LOG("ptr=%p id.uid=%x\n", ptr, id.get_uid());
    }
};

template <typename T> SerializablePointer<T>::SerializablePointer()
{
	ptr = (T*)(get_object_by_id(id));
	//CONSOLE_LOG("SerializablePointer: ptr=%p uid=%lx\n", ptr, id.get_uid());
}

template <typename T> SerializablePointer<T>::SerializablePointer(T * p)
{
	set(p);
}


class SerializableCString
{
  public:
    char str[60];
    SerializableCString(const char * s);
    SerializableCString();
};

#endif
