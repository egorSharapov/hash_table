#include <chrono>
#include "stdio.h"
#include "malloc.h"
#include "string.h"
#include "assert.h"
#include "../include/hash_table.hpp"
#include "../include/hash_functions.hpp"
#include "../include/hash_test.hpp"

const char *db_file_path = "dictionary/allign_big.db";
const int max_count = 1;

using namespace std;

extern "C" Hash_t CRC32_hash_asm (Ht_key_type);


int main()
{
    Test *database = NULL;
    volatile Ht_val_type value = NULL;

    Hash_table* h_table = Table_ctor (CAPACITY);
    database = Load_db (db_file_path);

    Store_db2ht (database, h_table, CRC32_hash_asm);

    auto begin = chrono::high_resolution_clock::now();

    for (int index = 0; index < database->size; index++)
        value = Search (h_table, database->dict[index].key, CRC32_hash_asm);

    auto end = chrono::high_resolution_clock::now();
    auto elapsed = chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

    size_t D = Calc_dispersion (h_table, "stat/stat.csv");
    printf ("D: %ld\n", -D);
    printf ("result time: %10ld\n", elapsed.count());

    Free_table (h_table);
    return 0;
}
