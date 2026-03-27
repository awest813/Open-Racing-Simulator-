/* -*- Mode: C++;  -*- */
/* VER: $Id$ */
// copyright (c) 2004 by Christos Dimitrakakis <dimitrak@idiap.ch>
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <learning/List.h>
#include <learning/learn_debug.h>

LIST* List(void)
{
	LIST* list = nullptr;

	if ((list = (LIST*) malloc(sizeof(LIST)))==nullptr) {
		Serror("Failed to create list structure\n");
		return nullptr;
	}

	list->head = nullptr;
	list->tail = nullptr;
	list->curr = nullptr;
	list->n = 0;
	list->retrieve = &ListLinearSearchRetrieve;

	return list;
}

LISTITEM* ListAppend(LIST* list, void* p) {
	return ListAppend (list, p, nullptr);
}

LISTITEM* ListAppend(LIST* list, void* p, void (*free_obj) (void* obj))
{
	LISTITEM* tmp = nullptr;
	assert(list);

	if (!p) {
		Swarning("NULL pointer given for new list item data\n");
	}
  
	if (!list->head) {
		tmp = ListItem(p, free_obj);
		list->head = tmp;
		list->curr = tmp;
	} else {
		tmp = LinkNext(list->tail, p, free_obj);
	}

	list->tail = tmp;
  
	list->n++;
  
	assert(list->head);
	assert(list->curr);
	assert(list->tail);
  
	return tmp;
}


LISTITEM* NextListItem(LIST* list)
{
	LISTITEM* t;
	assert(list);
  
	if (!list->curr) 
		return nullptr;

	t = GetNextItem(list->curr);
	if (t)
		list->curr =t;
	return t;
}

LISTITEM* FirstListItem(LIST* list)
{
	LISTITEM* t;
	assert(list);
  
	t = list->head;

	if (!t) {
		//    Swarning("No First Item\n");
		return nullptr;
	}
  
	list->curr = t;
	return t;
}


LISTITEM* LastListItem(LIST* list)
{
	LISTITEM* t;
	assert(list);
  
	t = list->tail;

	if (!t) {
		//    Swarning("No Last Item\n");
		return nullptr;
	}
  
	list->curr = t;
	return t;
}


LISTITEM* GetNextItem(LISTITEM* ptr)
{
	if (ptr)
		return ptr->next;
	else {
		Serror("Null pointer given to GetNextItem()\n");
		return nullptr;
	}

}

LISTITEM* GetPrevItem(LISTITEM* ptr)
{
	if (ptr)
		return ptr->prev;
	else {
		Serror("Null pointer given to GetPrevItem()\n");
		return nullptr;
	}
}

LISTITEM* LinkNext(LISTITEM* src, void* ptr, void (*free_obj) (void* obj))
{
	LISTITEM* tmp;
	LISTITEM* dst = nullptr;

	assert(ptr);
	assert(src);
  
	if ((dst = ListItem(ptr, free_obj))==nullptr) {
		return nullptr;
	}

	if ((tmp = GetNextItem(src))) {
		tmp->prev = dst;
	}
	dst->next = tmp;
	dst->prev = src;
	src->next = dst;  

	return dst;
}

LISTITEM* LinkPrev(LISTITEM* src, void* ptr, void (*free_obj) (void* obj))
{
	Serror("Not implemented\n");
	return nullptr;
}

LISTITEM* ListItem(void* ptr, void (*free_obj) (void* obj)) 
{
	LISTITEM* item = nullptr;

	assert(ptr);

	if ((item = (LISTITEM*) malloc(sizeof(LISTITEM)))==nullptr) {
		Serror("Failed to allocate new listitem\n");
		return nullptr;
	}
  
	item->prev = nullptr;
	item->next = nullptr;
	item->obj = ptr;
	item->free_obj = free_obj;
	return item;
}

int FreeListItem(LIST* list, LISTITEM* ptr)
{
	if (ptr==nullptr) {
		Serror("Null value for LISTITEM\n");
		return -1;
	}

	if (ptr->obj) {
		if (ptr->free_obj) {
			ptr->free_obj(ptr->obj);
		} else {
			free(ptr->obj);
		}
	}

	return RemoveListItem(list, ptr);
}


int RemoveListItem(LIST* list, LISTITEM* ptr) {
	LISTITEM* prev;
	LISTITEM* next;

	assert(ptr);

	prev = GetPrevItem(ptr);
	next = GetNextItem(ptr);
  
	if (prev) {
		if (prev->next != ptr) {
			Swarning("prev->next Sanity check failed on list\n");
		}
		prev->next = next;
		if (next==nullptr) {
			assert (list->tail == ptr);
			list->tail = prev;
			if (list->curr == ptr) {
				list->curr = prev;
			}
		}
	}
  
	if (next) {
		if (next->prev != ptr) {
			Swarning("next->prev Sanity check failed on list\n");
		}
		next->prev = prev;
		if (prev==nullptr) {
			assert (list->head == ptr);
			list->head = next;
			if (list->curr == ptr) {
				list->curr = next;
			}
		}
	}

	if ((next==nullptr)&&(prev==nullptr)) {
		assert (list->tail==list->head);
		list->tail = nullptr;
		list->head = nullptr;
		list->curr = nullptr;
	}

	free(ptr);
	return 0;
 
}



int PopItem(LIST* list) {

	LISTITEM* head = list->head;

	if (list->head==nullptr) {
		Swarning("List already empty\n");
		return -1;
	}

	if (FreeListItem(list, head))
		return -1;

	list->n--;

	if (list->head==nullptr) {
		if (list->n) {
			Swarning("List seems empty (%d items remaining?)",list->n);  
		}
	} else {
		assert(list->curr);
		assert(list->tail);
		if (list->head==nullptr) {
			Serror ("List already empty\n");
		}
		/* set tail to head if only one item is remaining */
		if (list->head->next==nullptr) {
			assert(list->n==1);
			list->tail = list->head;
		}
		if (list->n<=0) {
			Serror("Counter at %d, yet least not empty?\n",list->n);
			return -1;
		}
	}

	return 0;

}

int ClearList(LIST* list)
{
	int i;
	while (list->head) {
		PopItem(list);
	}
	i = list->n;

	if (i==0) {
		if (list->head) {
			Serror("List still has a head after clearing\n");
		}
		if (list->curr) {
			Serror("List still points somewhere after clearing\n");
		}
		if (list->tail) {
			Serror("List still has a tail after clearing\n");
		}
	} else {
		Serror("List size not zero after clearing\n");
	}

	free (list);

	return i;
}

LISTITEM* FindItem (LIST* list, void* ptr)
{
	return list->retrieve (list, ptr);
}


LISTITEM* ListLinearSearchRetrieve (struct List* list, void* ptr)
{
	LISTITEM* item;

	item = FirstListItem (list);
	while (item) {
		if (item->obj == ptr) {
			return item;
		}
		item = NextListItem (list);
	}

	return nullptr;
}


/* Get the size of the list */
int ListSize(LIST* list) {
	return list->n;
}

LISTITEM* GetItem (LIST* list, int n)
{
	LISTITEM* item;

	if (n>=ListSize (list)) {
		return nullptr;
	}
	item = FirstListItem (list);
	for (int i=0; i<n; i++) {
		item = NextListItem (list);
	}

	return item;
}
