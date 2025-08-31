#ifdef USE_ENET
#include "serialization-enet.cpp"
#else
#include "serialization-rust.cpp"
#endif
