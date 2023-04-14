#pragma once


struct Token 
{
    Ht_key_type key;
    Ht_val_type value;
};



typedef struct Test 
{
    Token* dict;
    size_t size = 0;

    char *data;
} Test;


size_t Calc_dispersion (Hash_table *table, const char *f_name);
Test   *Load_db        (const char *database);
void   Test_search     (Test *db, Hash_table *table, Hash_t (*func_array[]) (Ht_key_type), size_t size);
void   Store_db2ht     (Test *db, Hash_table *table, Hash_t (*hash_func) (Ht_key_type));
