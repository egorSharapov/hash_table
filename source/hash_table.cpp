#include "string.h"
#include "malloc.h"
#include "assert.h"
#include "limits.h"
#include "../include/hash_table.hpp"


#define R(msg) "\033[91m" msg "\033[0m"
#define M(msg) "\033[95m" msg "\033[0m"


Ht_node* allocate_node () 
{
    Ht_node* node = (Ht_node*) calloc (1, sizeof(Ht_node));
    if (!node)
    {
        printf ("calloc error\n");
        return NULL;
    }
    node->next = NULL;
    return node;
}


Ht_node* Insert_node (Ht_node* chain_head, Ht_elem* elem)
{
    if (chain_head == NULL) 
    {
        Ht_node* new_head = allocate_node ();
        new_head->elem = elem;
        return new_head;
    }
    Ht_node* temp_head = chain_head;

    while (temp_head->next)
        temp_head = temp_head->next;

    Ht_node* node = allocate_node ();
    node->elem = elem;
    temp_head->next = node;
    
    return chain_head;
}


Ht_elem* Remove_chains (Ht_node* ht_list) 
{
    assert (ht_list);
    assert (ht_list->next);

    Ht_node* node = ht_list->next;
    Ht_node* temp = ht_list;
    
    temp->next = NULL;
    ht_list = node;
    Ht_elem* it = NULL;
    
    memcpy(temp->elem, it, sizeof(Ht_elem));
    
    free (temp->elem);
    free (temp);

    return it;
}


void Free_chain (Ht_node* head) 
{
    Ht_node* temp = head;

    while (head != NULL) 
    {
        temp = head;
        head = head->next;
        free (temp->elem);
        free (temp);
    }
}


Ht_node** Chain_ctor (Hash_table* table) 
{
    Ht_node** chain = (Ht_node**) calloc (table->size, sizeof(Ht_node*));
    return chain;
}


void Free_chains (Hash_table* table) 
{
    Ht_node** chains = table->chains;

    for (int index = 0; index < table->size; index++)
        Free_chain (chains[index]);

    free (chains);
}


Ht_elem* create_elem (Ht_key_type key, Ht_val_type value)
{
    Ht_elem* elem = (Ht_elem*) aligned_alloc (32, sizeof (Ht_elem));
    elem->key     = key;
    elem->value   = value;
    
    return elem;
}


Hash_table* Table_ctor (int size)
{
    Hash_table* table = (Hash_table*) calloc (1, sizeof(Hash_table));
    table->size = size;
    table->count = 0;
    table->elements = (Ht_elem**) calloc (table->size, sizeof(Ht_elem*));
    
    table->chains = Chain_ctor (table);

    return table;
}


void Free_elem (Ht_elem* elem)
{
    free (elem);
}


void Free_table (Hash_table* table)
{
    for (int ind = 0; ind < table->size; ind++) 
    {
        Ht_elem* elem = table->elements[ind];
        if (elem != NULL)
            Free_elem (elem);
    }
    Free_chains (table); //TO FIX
    table->count = 0;
    free (table->elements);
}


void Solve_collision (Hash_table* table, unsigned long index, Ht_elem* elem)
{
    Ht_node* chain_head = table->chains[index];

    if (chain_head == NULL) 
    {
        chain_head = allocate_node ();
        chain_head->elem = elem;
        table->chains[index] = chain_head;
    }
    else
        table->chains[index] = Insert_node (chain_head, elem);
}


void Insert (Hash_table* table, Ht_key_type key, Ht_val_type value, Hash_t (*hash_func) (Ht_key_type)) 
{

    Ht_elem* elem = create_elem (key, value);

    Hash_t index = hash_func (key);
    index %= CAPACITY;

    if (index > CAPACITY)
    {
        printf ("hash value more than table capacity\n");
        return;
    }
    Ht_elem* current_elem = table->elements[index];
    
    if (current_elem == NULL) 
    {
        table->elements[index] = elem; 
        table->count++;
    }
    else 
        Solve_collision (table, index, elem);
}


static inline int strcmp_avx (__m256i string1, __m256i string2)
{
    return _mm256_testnzc_si256 (string1, string2);
}



Ht_val_type Search (Hash_table* table, Ht_key_type key, Hash_t (* hash_func) (Ht_key_type))
{
    assert (table);

    Hash_t index = hash_func (key);

    index %= CAPACITY;
    

    Ht_elem* elem = table->elements[index];
    Ht_node* chain_head = table->chains[index];

    while (elem != NULL) 
    {
        if (_mm256_testnzc_si256 (elem->key, key) == 0)
            return elem->value;

        if (chain_head == NULL)
            return NULL;
        
        elem       = chain_head->elem;
        chain_head = chain_head->next;
    }

    return NULL;
}


