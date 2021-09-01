if [[ -z $CC ]]; then
  if [[ -x "$(command -v clang)" ]]; then
    CC=clang
  else
    CC=gcc
  fi
fi

CFLAGS="-O2 -Wall -Wextra -I ."
LIBS="-lwebsockets -ljansson -lcurl -lpthread"
INPUT=./source/alie.c
OUTPUT=./alie

$CC $CFLAGS $LIBS $INPUT -o $OUTPUT
