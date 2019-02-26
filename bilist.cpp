#include "stdafx.h"
#include "bilist.h"
#include <stdlib.h>


#define ISHEAD(x) (x->forward == x)


int _InsertTail(PBiNode tail, void *item)
{
	PBiNode node = (PBiNode)malloc(sizeof(BiNode));
	node->item = item;
	if (ISHEAD(tail))
	{
		tail->forward = node;
		node->forward = tail;
		node->backward = tail;
		tail->backward = node;
		return 0;
	}
	else
	{
		PBiNode cur = tail->backward;
		cur->forward = node;
		node->backward = cur;
		node->forward = tail;
		tail->backward = node;
		return 1;
	}
}

int BiInsert(PBiList list, void *item)
{
	_InsertTail(list->head, item);
	list->size++;
	return 0;
}

void _InitLink(PBiNode *head)
{
	(*head) = (PBiNode)malloc(sizeof(BiNode));
	(*head)->forward = *head;
	(*head)->backward = *head;
	(*head)->item = NULL;
}

void BiInit(PBiList *list)
{
	*list = (PBiList)malloc(sizeof(BiList));
	_InitLink(&(*list)->head);
	(*list)->size = 0;
}


int BiSize(PBiList list)
{
	return list->size;
}

void BiFree(PBiList* list)
{
	PBiNode cur = (*list)->head;
	if (cur)
	{
		cur = cur->forward;
		while (cur != (*list)->head)
		{
			PBiNode pivot = cur, prev = pivot->backward;
			cur = cur->forward;
			// prev<->pivot<->cur
			// now delete pivot
			// to prev<->cur			
			prev->forward = cur;
			cur->backward = prev;
			free(pivot);
			pivot = NULL;
		}
		free(cur);
		cur = NULL;
	}
	free(*list);
	*list = NULL;
}

