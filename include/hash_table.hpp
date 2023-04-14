#pragma once
#include "stdio.h"
#include "config.hpp"


typedef struct Ht_elem 
{
    Ht_key_type key;
    Ht_val_type value;
} Ht_elem;


typedef struct Ht_node 
{
    Ht_elem* elem; 
    Ht_node* next;
} Ht_node;


typedef struct Hash_table 
{
    Ht_elem** elements;
    Ht_node** chains;
    
    int size;
    int count;
} Hash_table;


Hash_table* Table_ctor (int size);

Ht_node*    allocate_node     ();
Ht_node*    Insert_node       (Ht_node* list, Ht_elem* elem);
Ht_elem*    Remove_chains     (Ht_node* ht_list);
Ht_node**   Chain_ctor        (Hash_table* table);
Ht_elem*    create_elem       (Ht_key_type key, Ht_val_type value);
Ht_val_type Search            (Hash_table* table, Ht_key_type key, Hash_t (*hash_func) (Ht_key_type));

void  Free_chain      (Ht_node* list);
void  Free_chains     (Hash_table* table);
void  Free_elem       (Ht_elem* elem);
void  Free_table      (Hash_table* table);
void  Solve_collision (Hash_table* table, unsigned long index, Ht_elem* elem);
void  Insert          (Hash_table* table, Ht_key_type key, Ht_val_type value, Hash_t (*hash_func) (Ht_key_type));
//void  Delete          (Hash_table* table, Ht_key_type key, Hash_t (*hash_func) (Ht_key_type));
void  Search_show     (Hash_table* table, Ht_key_type key, Hash_t (*hash_func) (Ht_key_type));
void  Table_show      (Hash_table* table);