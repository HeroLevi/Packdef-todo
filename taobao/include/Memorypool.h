#ifndef _MEMORY_POOL_H
#define _MEMORY_POOL_H
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MP_OK 			0
#define MP_MALLOC_FAIL -1
#define MP_NOT_INIT    -2

typedef struct cdn memory_pool_node;

struct cdn
{
	unsigned int colnum;//列编号
	unsigned char* data;//内存节点头指针
	memory_pool_node* next;//下一个内存节点
};

typedef struct
{
	unsigned int total_count;//该列总内存节点个数
	unsigned int used_count;//该列已使用的内存节点个数
	unsigned int block_len;//该列节点长度
	memory_pool_node* free_header;//空闲节点头指针
	memory_pool_node* used_header;//占用节点头指针
}memory_pool_column;


int buffer_pool_init(unsigned int colume_no,unsigned int block_len[],unsigned int block_count[]);
memory_pool_node* buffer_malloc(unsigned int size);
int buffer_free(memory_pool_node* buffer);
int buffer_pool_destory(void);
int buffer_runtime_print(void);

#endif
