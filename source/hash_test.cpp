#include <sys/stat.h>
#include <chrono>
#include "malloc.h"
#include "string.h"
#include "../include/hash_table.hpp"
#include "../include/hash_test.hpp"
using namespace std;


const char *data_name = "stat/data.csv";


static size_t Get_size (const char *file_name)
{
    struct stat db_stat = {};
    stat (file_name, &db_stat);
    
    size_t size = db_stat.st_size;

    return size;
}


static size_t Count_lines (const char *data)
{
    size_t lines = 0;
    const char *next = strchr (data, '\n');

    while (next)
    {
        next  += 1;
        lines += 1;
        next  = strchr (next, '\n');
    }
    return lines;
}


static void Parse_data (Test *test)
{
    size_t lines = Count_lines (test->data);
    printf ("succes count lines: %ld\n", lines);;

    test->dict = (Token *) aligned_alloc (32, lines * sizeof (Token));
    test->size = lines;

    const char *current = strchr (test->data, '\n') + 1;
    const char *separator = NULL;
    const char *next      = NULL;

    size_t index = 0;
    size_t counter = current - test->data;

    while (current)
    {
        separator = current + 31;
        if (separator == NULL)
        {
            printf ("couldnt find separator %c %ld\n", SEP, index + 1);
            break;
        }
        counter = separator - test->data;

        test->data[counter] = '\0';
    
        _mm256_storeu_si256 (&test->dict[index].key, *((__m256i_u *) current));

        current = separator + 1;
        test->dict[index].value = (char *) current;

        next = strchr (current, '\n');
        if (!next) break;
        counter = next - test->data;

        test->data[counter] = '\0';
        current = next + 1;
        index += 1;
    }
}


void Test_hash (Hash_table *table, Hash_t (*hash_func) (Ht_key_type), const char *database)
{
    Test test = {};

    const size_t chunk = 64;
    size_t read_size = 0;

    FILE *file = fopen (database, "r");
    if (file == NULL)
    {
        printf ("couldnt open file\n");
        return;
    }
    size_t size = Get_size (database);

    test.data = (char *) calloc (sizeof (char), size);
    read_size = fread (test.data, sizeof (char), size, file);
    
    if (read_size != size)
        printf ("fread error\n");

    Parse_data (&test);

    for (int index = 0; index < test.size; index++)
        Insert (table, test.dict[index].key, test.dict[index].value, hash_func);

}


size_t Calc_dispersion (Hash_table *table, const char *f_name)
{
    FILE *data = fopen (data_name, "a");
    if (!data) 
    {
        printf ("open data file error\n");
        return -1;
    }
    fprintf (data, "%s;\n", f_name);
    
    size_t E = 0;
    size_t E_2 = 0;
    size_t chain_len = 0;

    for (size_t index = 0; index < table->size; index++)
    {
        if (table->elements[index])
        {
            chain_len += 1;
            Ht_node *current = table->chains[index];
            
            while (current)
            {
                chain_len += 1;
                current = current->next;
            }
        }
        fprintf (data, "%ld;%ld\n", index, chain_len);
        E += chain_len;
        E_2 += chain_len * chain_len;
        chain_len = 0;

    }
    fclose (data);
    return E^2 - E_2;
}



Test *Load_db (const char *database)
{
    Test *test = (Test *) calloc (1, sizeof (Test));

    size_t read_size = 0;

    FILE *file = fopen (database, "r");
    if (file == NULL)
    {
        printf ("couldnt open file\n");
        return nullptr;
    }
    size_t size = Get_size (database);

    test->data = (char *) calloc (sizeof (char), size);
    read_size = fread (test->data, sizeof (char), size, file);
    
    if (read_size != size)
        printf ("fread error\n");

    Parse_data (test);

    return test;
}


void Store_db2ht (Test *db, Hash_table *table, Hash_t (*hash_func) (Ht_key_type))
{
    if (table->size < db->size)
        printf ("warning: db larger than hash table\n");

    for (int index = 0; index < db->size; index++)
    {
        Insert (table, db->dict[index].key, db->dict[index].value, hash_func);
    }
}



void Test_search (Test *db, Hash_table *table, Hash_t (*func_array[]) (Ht_key_type), size_t size)
{
    for (int index = 0; index < size; index++)
    {
        Store_db2ht (db, table, func_array[index]);

        auto begin = chrono::high_resolution_clock::now();
        for (int key_n = 0; key_n < db->size; key_n++)
            Search (table, db->dict[key_n].key, func_array[index]);

        auto end = chrono::high_resolution_clock::now();
        auto elapsed = chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

        printf ("result time: %10ld\n", elapsed.count());
        Free_table (table);
        table = Table_ctor (CAPACITY);
    }

}


// #include <stdio.h>
// #include <sys/mman.h>
// #include <stdlib.h>
// #include <sys/stat.h>
// #include <fcntl.h>
// #include <unistd.h>

// int main (int argc, char *argv[])
// {

//     if (argc < 2) 
//     {
//         printf ("File path not mentioned\n");
//         exit (0);
//     }
   
//     const char *filepath = argv[1];
//     int fd = open (filepath, O_RDWR);
//     if(fd < 0)
//     {
//         printf ("\n\"%s \" could not open\n",
//                filepath);
//         exit (1);
//     }

//     struct stat statbuf;
//     int err = fstat (fd, &statbuf);

//     if(err < 0)
//     {
//         printf ("\n\"%s \" could not open\n", filepath);
//         exit (2);
//     }

//     char *ptr = (char *) mmap (0, statbuf.st_size,
//                      PROT_READ|PROT_WRITE,
//                      MAP_SHARED,
//                      fd, 0);

//     if (ptr == MAP_FAILED)
//     {
//         printf ("Mapping Failed\n");
//         return 1;
//     }
//     close (fd);

//     ssize_t n = write (1, ptr, statbuf.st_size);
//     if (n != statbuf.st_size)
//     {
//         printf ("Write failed\n");
//     }


//     err = munmap (ptr, statbuf.st_size);

//     if (err != 0)
//     {
//         printf ("UnMapping Failed\n");
//         return 1;
//     }
//     return 0;
// }