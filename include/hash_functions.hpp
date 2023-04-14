#pragma once

#include "config.hpp"
#include "stdio.h"

Hash_t hash_function    (Ht_key_type key);
Hash_t Const_hash       (Ht_key_type key);
Hash_t Firstletter_hash (Ht_key_type key);
Hash_t Strlen_hash      (Ht_key_type key);
Hash_t Average_hash     (Ht_key_type key);
Hash_t Sum_hash         (Ht_key_type key);
Hash_t Rol_hash         (Ht_key_type key);
Hash_t Ror_hash         (Ht_key_type key);
Hash_t Gnu_hash         (Ht_key_type key);
Hash_t CRC32_hash       (Ht_key_type key);


