#pragma once
#include <x86intrin.h>
#include "stdint.h"

const int CAPACITY = 81101;
typedef char*    Ht_val_type;
typedef __m256i  Ht_key_type;
typedef uint32_t Hash_t;
const char SEP = '#';
