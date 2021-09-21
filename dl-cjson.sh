#!/usr/bin/bash


url="https://raw.githubusercontent.com/DaveGamble/cJSON"
version="v1.7.15"


[[ -d src/cjson/ ]] || \mkdir -pv "src/cjson/" || exit 1
wget -v --unlink -O  "src/cjson/cJSON.c"    "$url/$version/cJSON.c"    || exit 1
wget -v --unlink -O  "src/cjson/cJSON.h"    "$url/$version/cJSON.h"    || exit 1
wget -v --unlink -O  "src/cjson/README.md"  "$url/$version/README.md"  || exit 1
wget -v --unlink -O  "src/cjson/LICENSE"    "$url/$version/LICENSE"    || exit 1
