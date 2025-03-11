#include <stdio.h>
#include "hashSet.h"

#define HASH_PRIME 0x100000001b3UL;

struct SetNode
{
    u64 key;
    SetNode* next;
};

// Using FNV-1a hash
u64 hashString(const char* str)
{
    u64 hash = 0xcbf29ce484222325;  // FNV offset basis

    while (*str) {
        hash ^= (u8)(*str);         // XOR with byte
        hash *= HASH_PRIME;         // Multiply by prime
        str++;
    }
    return hash;
}

void SetInit(Set* set, usize size)
{
    set->buckets = calloc(size, sizeof(SetNode));
    set->size = size;
    set->count = 0;
}

static SetNode* SetNodeCreate(u64 key)
{
    SetNode* node = malloc(sizeof(SetNode));
    node->key = key;
    node->next = NULL;
    return node;
}

static void SetResize(Set* set)
{
    usize new_size = set->size * 2;
    SetNode** new_buckets = calloc(new_size, sizeof(SetNode*));

    for(usize i = 0; i < set->size; i++)
    {
        SetNode* node = set->buckets[i];
        while(node)
        {
            SetNode* next = node->next;
            u64 new_index = node->key % new_size;

            node->next = new_buckets[new_index];
            new_buckets[new_index] = node;
            node = next;
        }
    }

    free(set->buckets);
    set->buckets = new_buckets;
    set->size = new_size;
}

void SetInsert(Set* set, u64 key)
{
    if((set->count * 100) / set->size >= 75) SetResize(set);
    
    u64 index = key % set->size;
    SetNode* node = set->buckets[index];

    while(node)
    {
        if(node->key == key) return;
        node = node->next;
    }

    SetNode* new_node = SetNodeCreate(key);
    new_node->next = set->buckets[index];
    set->buckets[index] = new_node;
    set->count++;
}

int SetContains(Set* set, u64 key)
{
    u64 index = key % set->size;

    SetNode* node = set->buckets[index];

    while(node)
    {
        if(node->key == key) return 1;
        node = node->next;
    }

    return 0;
}

void SetClear(Set* set)
{
    for(usize i = 0; i < set->size; i++)
    {
        SetNode* node = set->buckets[i];
        while(node)
        {
            SetNode* next = node->next;
            free(node);
            node = next;
        }
        set->buckets[i] = NULL;
    }
    set->count = 0;
}

void SetCleanup(Set* set)
{
    SetClear(set);
    free(set->buckets);
}