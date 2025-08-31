#ifndef SERIALIZATION_RUST_H
#define SERIALIZATION_RUST_H

template <typename T> class SerializablePointer
{
    T * ptr;

  public:
    SerializablePointer(T * p);
    T * get()
    {
        return ptr;
    }
};
/// <div rustbindgen nodebug></div>
class SerializableCString
{
  public:
    const char * str;
    SerializableCString(const char * s);
};

#endif
