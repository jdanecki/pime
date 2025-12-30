#!/bin/bash

#eg. log
ING_NAME=$1
UPPERCASE=`echo $ING_NAME | tr 'a-z' 'A-Z'`

first_letter=$(echo "${ING_NAME:0:1}" | tr '[:lower:]' '[:upper:]')
CLASS_NAME="$first_letter${ING_NAME:1}"

cat template_ing.cpp | sed "s/HEADER/$ING_NAME/" | sed "s/CLASS_NAME/$CLASS_NAME/g" | sed "s/ING/ING_$UPPERCASE/ " > $ING_NAME.cpp


