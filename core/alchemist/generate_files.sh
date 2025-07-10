#!/bin/bash

create_inline_file()
{
  local INPUT_FILE="$1.txt"  
  awk -F';' '{ print $1"," }' "$INPUT_FILE" > generated/$1.inl
}

create_enum_file()
{
    local ENUM_FILE="generated/say_enum.h"

echo "#ifndef _NPC_SAY_ENUM_H
#define _NPC_SAY_ENUM_H

enum Npc_say {" > $ENUM_FILE

for f in generated/*.inl
do
    local n=`basename $f`
    echo "#include \"$n\"" >> $ENUM_FILE
done

echo "};
#endif
" >> $ENUM_FILE
}

do_files()
{
  local BASENAME="$1"
  local INPUT_FILE="${BASENAME}.txt"  
  local HEADER_FILE="generated/${BASENAME}.h"  
  local SOURCE_FILE="generated/${BASENAME}.cpp"  
  local FUNC_NAME="init_${BASENAME}"
  local VAR_NAME="${BASENAME}"

  echo "#ifndef _${BASENAME}_H
#define _${BASENAME}_H
#include \"say_enum.h\"
" > $HEADER_FILE

  echo "extern void init_${BASENAME}();" >> $HEADER_FILE
  echo -e "class SentencesList;\n" >> $HEADER_FILE
  echo -e "extern SentencesList * ${VAR_NAME};\n" >> $HEADER_FILE
  echo "#endif" >> $HEADER_FILE

  echo -e "#include \"${BASENAME}.h\"\n" > $SOURCE_FILE
  echo -e "SentencesList * ${VAR_NAME};\n" >> $SOURCE_FILE
  echo "void ${FUNC_NAME}() {" >> $SOURCE_FILE
  echo -e "    ${VAR_NAME} = new SentencesList(\"${VAR_NAME}\");\n" >> $SOURCE_FILE
  awk -F';' -v varname="$VAR_NAME" '{  printf "    %s->add(new Sentence(%s, %s));\n", varname, $1, $2 }' "$INPUT_FILE" >> $SOURCE_FILE
  echo "}" >> $SOURCE_FILE
}

mkdir -p generated

create_inline_file questions
create_inline_file sentences

create_enum_file

do_files questions
do_files sentences

echo "#ifndef _NPC_TALK_GENERATED_H
#define _NPC_TALK_GENERATED_H
" > generated/npc_talk_generated.hpp

for f in generated/*.h
do
    n=`basename $f`
    echo "#include \"$n\"" >> generated/npc_talk_generated.hpp
done

echo "#endif" >> generated/npc_talk_generated.hpp

