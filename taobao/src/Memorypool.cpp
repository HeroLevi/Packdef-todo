#include "Memorypool.h"

memory_pool_column* b_p = NULL;
int buffer_pool_no = 0;

int buffer_pool_init(unsigned int col_no,unsigned int block_len[],unsigned int block_count[])
{
	//申请空间col_no列
	b_p = (memory_pool_column*)malloc(sizeof(memory_pool_column)*col_no);
	if(b_p == NULL)
		return MP_MALLOC_FAIL;
	memset(b_p,0,sizeof(memory_pool_column)*col_no);
	buffer_pool_no = col_no;

	memory_pool_node* cur_node = NULL;
	memory_pool_node* new_node = NULL;

	for(int i=0;i<col_no;++i)
	{
		b_p[i].block_len = block_len[i];
		b_p[i].total_count = block_count[i];
		for(int j=0;j<block_count[i];++j)
		{
			new_node = (memory_pool_node*)malloc(sizeof(memory_pool_node));
			new_node->colnum = i;
			new_node->data = (unsigned char*)malloc(block_len[i]);
			if(!new_node || !(new_node->data))
				return MP_MALLOC_FAIL;
			new_node->next = NULL;
			
			//如果之前没有节点新来的就是头
			if(!j)
			{
				b_p[i].free_header = new_node;
				cur_node = b_p[i].free_header;
			}
			else
			{
				cur_node->next = new_node;
				cur_node = cur_node->next;
			}
		}
	}
	return MP_OK;
}

memory_pool_node* buffer_malloc(unsigned int size)
{
	memory_pool_node* node = NULL;
	if(size > b_p[buffer_pool_no - 1].block_len)
	{
		printf("malloc size[%d] so big,need new from stack...\n",size);
a:
		node = (memory_pool_node*)malloc(sizeof(memory_pool_node));
		node->colnum = 9999;
		node->data = (unsigned char*)malloc(size);
		memset(node->data,0,size);
		if(!node || !(node->data))
			return NULL;
		node->next = NULL;
		return node;
	}
	for(int i=0;i<buffer_pool_no;++i)
	{
		if(size > b_p[i].block_len)
			continue;
		if(b_p[i].total_count - b_p[i].used_count == 0)
		{
			printf("warning!!! size[%d]pool use up!!!\n",b_p[i].block_len);
			continue;
		}

		node = b_p[i].free_header;
		b_p[i].free_header = b_p[i].free_header->next;
		b_p[i].used_count++;
		node->next = b_p[i].used_header;
		b_p[i].used_header = node;
		return node;
	}
	printf("warning!!! all of pool use up!!!\n");
	goto a;
}

int buffer_free(memory_pool_node* buffer)
{
	memory_pool_node* node_cur = b_p[buffer->colnum].used_header;
	memory_pool_node* node_pre = NULL;
	if(buffer->colnum == 9999)
	{
		free(buffer->data);
		free(buffer);
		buffer = NULL;
		return MP_OK;
	}

	while(node_cur)
	{
		if(node_cur != buffer)
		{
			node_pre = node_cur;
			node_cur = node_cur->next;
			continue;
		}
		if(!node_pre)
		{
			b_p[buffer->colnum].used_header = b_p[buffer->colnum].used_header->next;
		}
		else
		{
			node_pre->next = node_cur->next;
		}

		b_p[buffer->colnum].used_count--;
		node_cur->next = b_p[buffer->colnum].free_header;
		b_p[buffer->colnum].free_header = node_cur;
		break;
	}
	return MP_OK;
}

int buffer_pool_destory(void)
{
	memory_pool_node* node_cur = NULL;
	memory_pool_node* node_del = NULL;
	if(!b_p)
		return MP_NOT_INIT;
	for(int i=0;i<buffer_pool_no;++i)
	{
		node_cur = b_p[i].used_header;
		while(node_cur)
		{
			node_del = node_cur;
			node_cur = node_cur->next;
			free(node_del->data);
			free(node_del);
		}

		node_cur = b_p[i].free_header;
		while(node_cur)
		{
			node_del = node_cur;
			node_cur = node_cur->next;
			free(node_del->data);
			free(node_del);
		}
	}
	free(b_p);
	b_p = NULL;
	buffer_pool_no = 0;
	return MP_OK;
}

int buffer_runtime_print(void)
{
	if(!b_p)
	{
		printf("buffer pool not init yet!!!\n");
		return MP_NOT_INIT;
	}
	for(int i=0;i<buffer_pool_no;++i)
	{
		printf("pool no[%d] blocksize[%d] blockTotalCount[%d] usedBlock[%d] used percentage[%d%%]\n",i,b_p[i].block_len,b_p[i].total_count,b_p[i].used_count,b_p[i].used_count*100/b_p[i].total_count);
	}
	return MP_OK;
}
