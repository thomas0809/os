#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define SIZE 1000000

struct node{
	int size;
	void* first;
	char dirty;
	struct node* next;
};

struct node *start;
void *p;


void *mymalloc(int size)
{
	if(p == NULL) {
		p = sbrk(SIZE);
		start = sbrk(sizeof(struct node));
		start->size = SIZE;
		start->first = p;
		start->dirty = 0;
		start->next = NULL;
	}

	struct node *temp = start;
	while(temp != NULL) {
		if(temp->size > size && temp->dirty == 0) {
			struct node *temp2 = sbrk(sizeof(struct node));
			temp2->size = temp->size - size;
			temp2->first = temp->first + size;
			temp2->dirty = 0;
			temp2->next = temp->next;
			temp->size -= size;
			temp->dirty = 1;
			temp->next = temp2;
			return temp->first;
		}
		else if(temp->size == size && temp->dirty == 0) {
			temp->dirty = 1;
			return temp->first;
		}
		temp = temp->next;
	}
	return NULL;
}

int myfree(void *pt)
{
	struct node *pred = NULL, *succ = NULL;
	struct node *temp = start;
	while(temp != NULL) {
		if (temp->first == pt && temp->dirty == 1) {
			temp->dirty = 0;
			if (temp->next != NULL && temp->next->dirty == 0) {
				temp->size += temp->next->size;
				succ = temp->next;
				temp->next = temp->next->next;
				//free(succ);
			}
			if (pred != NULL && pred->dirty == 0) {
				pred->size += temp->size;
				pred->next = temp->next;
				//free(temp);
			}
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