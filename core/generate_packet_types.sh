#!/bin/bash

echo "#ifndef PACKET_TYPE_H
#define PACKET_TYPE_H

using PacketType = unsigned char;" > packet_types.h

echo "#include \"packet_types.h\"
#include <cstdio>
#include <cassert>

const char * packet_type_names[]={ " > packet_types.cpp

val=0

for p in `cat packet_types.txt`
do
    echo "constexpr static const PacketType $p = $val;" >> packet_types.h
    val=$(($val + 1))
    echo "\"$p\"," >> packet_types.cpp
done

echo -e "\nextern const char * packet_type_names[];
void show_packet_type_name(char what, unsigned char i);
extern bool trace_network;
#endif" >> packet_types.h

echo "};

bool trace_network;

void show_packet_type_name(char what, unsigned char i)
{
    assert(i < PACKET_COUNT);
    if (trace_network)
        printf(\"%c %d -> %s\\n\", what, i, packet_type_names[i]);
}
" >> packet_types.cpp






