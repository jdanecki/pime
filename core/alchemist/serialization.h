#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#ifdef USE_ENET
#include "serialization-enet.h"
#else
#include "serialization-rust.h"
#endif

#endif
