#ifndef SERIALIZATION_ENET_H
#define SERIALIZATION_ENET_H
template <typename T> class SerializablePointer
{
    T * ptr;

  public:
    SerializablePointer(T * p);
    T * get()
    {
        return ptr;
    }
    void set(T * p)
    {
        ptr = p;
    }
};

template <typename T> SerializablePointer<T>::SerializablePointer(T * p) : ptr(p)
{
}

class SerializableCString
{
  public:
    char str[60];
    SerializableCString(const char * s);
    SerializableCString() {}
};

#endif
