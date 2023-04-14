#include "string.h"
#include "hash_functions.hpp"



static Hash_t ROL (Hash_t hash)
{
    Hash_t temp1, temp2;

    temp1 = hash << 1;
    temp2 = hash >> (32 - 1);

    return temp1 | temp2;
}


static Hash_t ROR (Hash_t hash)
{
    Hash_t temp1, temp2;

    temp1 = hash >> 1;
    temp2 = hash << (32 - 1);

    return temp1 | temp2;
}


Hash_t hash_function (Ht_key_type key)
{
    Hash_t hash = 0;
    for (int j = 0; key[j]; j++)
        hash += key[j];
    
    return hash % CAPACITY;
}


Hash_t Const_hash (Ht_key_type key)
{
    return 0xBAD;
}


Hash_t Firstletter_hash (Ht_key_type key)
{
    return key[0];
}


Hash_t Strlen_hash (Ht_key_type key)
{
    return strlen (key) % CAPACITY;
}


Hash_t Average_hash (Ht_key_type key)
{
    Hash_t hash = 0;
    Hash_t length = strlen (key);

    for (Hash_t index = 0; index < length; index++)
        hash += key[index];

    return hash/length % CAPACITY;
}

Hash_t Sum_hash (Ht_key_type key)
{
    Hash_t hash = 0;
    Hash_t length = strlen (key);

    for (Hash_t index = 0; index < length; index++)
        hash += key[index];

    return hash % CAPACITY;
}


Hash_t Rol_hash (Ht_key_type key)
{
    Hash_t hash = 0;
    Hash_t length = strlen (key);

    for (Hash_t index = 0; index < length; index++)
        hash =  ROL (hash) ^ key[index];
    
    return hash % CAPACITY;
}


Hash_t Ror_hash (Ht_key_type key)
{
    Hash_t hash = 0;

    for (Hash_t index = 0; index < strlen (key); index++)
        hash = ROR(hash) ^ key[index];
    
    return hash % CAPACITY;
}


Hash_t Gnu_hash (Ht_key_type key)
{
        Hash_t hash = 5381;

        size_t length = strlen (key);
        for (size_t ind = 0; ind < length ; ind++)
                hash = hash * 33 + key[ind];

        return hash % CAPACITY;
}


Hash_t CRC32_hash (Ht_key_type key)
{
    unsigned int byte = 0, crc = 0, mask = 0;
    int index = 0;
    crc = 0xFFFFFFFF;
    while (key[index] != 0) 
    {
        byte = (unsigned) key[index];
        crc = crc ^ byte;
        
        for (int j = 7; j >= 0; j--) 
        {
            mask = -(crc & 1);
            crc = (crc >> 1) ^ (0xEDB88320 & mask);
        }
        index += 1;
    }
    return (~crc ) % CAPACITY;
}
