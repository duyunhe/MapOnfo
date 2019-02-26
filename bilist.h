#pragma once

typedef struct BiNode
{
	void* item;
	BiNode* forward;
	BiNode* backward;		// bi direction link
}*PBiNode;

typedef struct BiList
{
	PBiNode head;
	int size;
} *PBiList;


int _InsertTail(PBiNode tail, void *item);
void _InitLink(PBiNode *head);


void BiInit(PBiList *pList);
int BiSize(PBiList list);
int BiInsert(PBiList list, void *item);
void BiFree(PBiList* list);
