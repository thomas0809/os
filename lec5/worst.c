#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define SIZE 1000000

struct node{              // 链表的数据结构，用于维护内存块的信息
	int size;
	void* first;
	char dirty;
	struct node* next;
	struct node* pred;
};

struct node *start;
void *p = NULL;

void swap(struct node *n1, struct node *n2) 
{
	n1->next = n2->next;
	n2->pred = n1->pred;
	n1->pred = n2;
	n2->next = n1;
	if (n1->pred == NULL)
		start = n2;
}

void check(struct node *n)
{
	while (n->next != NULL && n->size < n->next->size) {
		swap(n, n->next);
	}
	while (n->pred != NULL && n->size > n->pred->size) {
		swap(n->pred, n);
	}
}

void remove_node(struct node *n) //删除一个节点ßßß
{
	if(n->pred == NULL) {
		start = n->next;
		start->pred = NULL;
	}
	else if(n->next == NULL) {
		n->pred->next = NULL;
	}
	else {
		n->next->pred = n->pred;
		n->pred->next = n->next;
	}
}

void *mymalloc(int size) // 申请内存
{
	if(p == NULL) {      // 初始化申请一块内存，并初始化链表
		p = sbrk(SIZE);
		start = sbrk(sizeof(struct node));
		start->size = SIZE;
		start->first = p;
		start->dirty = 0;
		start->next = NULL;
		start->pred = NULL;
	}
	struct node *temp = start;
	while(temp != NULL) {    // 遍历链表，找到第一块符合要求的内存块
		if(temp->size > size && temp->dirty == 0) {
			struct node *temp2 = sbrk(sizeof(struct node));
			temp2->size = temp->size - size;
			temp2->first = temp->first + size;
			temp2->dirty = 0;
			temp2->next = temp->next;
			temp2->pred = temp;
			if (temp2->next != NULL)
				temp2->next->pred = temp2;
			temp->size -= size;
			temp->dirty = 1;
			temp->next = temp2;
			check(temp);
			check(temp2);
			return temp->first;
		}
		else if(temp->size == size && temp->dirty == 0) {
			temp->dirty = 1;
			return temp->first;
		}
		if (temp->dirty == 0)
			break;
		temp = temp->next;
	}
	return NULL;
}

int myfree(void *pt) // 释放内存
{
	struct node *pred = NULL, *succ = NULL;
	struct node *temp = start;
	while(temp != NULL) {
		if (temp->first == pt && temp->dirty == 1) {  // 在链表中找到需要释放的内存块
			temp->dirty = 0;
			struct node *temp2 = start;
			while (temp2 != NULL) {
				if (temp2->first == pt + temp->size && temp2->dirty == 0) { // 与后面的内存块合并
					temp->size += temp2->size;
					remove_node(temp2);
				}
				if (temp2->first + temp2->size == pt && temp2->dirty == 0) {  // 与前面的内存快合并
					temp->size += temp2->size;
					temp->first = temp2->first;
					remove_node(temp2);
					//free(temp);
				}
				temp2 = temp2->next;
			}
			check(temp);
			return 1;
		}
		temp = temp->next;
	}
	return 0;
}

int main()
{
	void* a = mymalloc(100);
	printf("%p\n", a);
	void* b = mymalloc(200);
	printf("%p\n", b);
	int c = myfree(a);
	printf("%d\n", c);
	void* d = mymalloc(50);
	printf("%p\n", d);
}