/*Filename	:  Queue.c
  Author	:  Brian Ekstrom
  Course	:  CSCI 380
  Assignment	:  Lab 1, C Queue
  Description	:  Build a queue like system using malloc/free to have similiar features to a queue.
*/
/*

 * Code for basic C skills diagnostic.
 * Developed for courses 15-213/18-213/15-513 by R. E. Bryant, 2017
 */

/*
 * This program implements a queue supporting both FIFO and LIFO
 * operations.
 *
 * It uses a singly-linked list to represent the set of queue elements
 */

#include <stdio.h>
#include <stdlib.h>

#include "harness.h"
#include "queue.h"

/*
  Create empty queue.
  Return NULL if could not allocate space.
*/
queue_t *
q_new ()
{
  /* What if malloc returned NULL? */
  queue_t *q = malloc (sizeof (queue_t));

  if (q == NULL)
  {
    return NULL;
  }
   q->head = NULL;
   q->tail = NULL;
   q->size = 0;
   return q;
}

/* Free all storage used by queue */
void
q_free (queue_t *q)
{
  if (q == NULL)
  {
     return;
  }

  list_ele_t *tmp = q->head;
  free(q);

  while (tmp != NULL)
  {
    list_ele_t *tmpChange = tmp;
    tmp = tmpChange->next;
    free(tmpChange);
  }
}

/*
  Attempt to insert element at head of queue.
  Return true if successful.
  Return false if q is NULL or could not allocate space.
 */
bool
q_insert_head (queue_t *q, int v)
{
  list_ele_t *newh;
  /* What should you do if the q is NULL? */
  if (q == NULL)
  {
    return false;
  }

  /* What if malloc returned NULL? */
  newh = malloc (sizeof (list_ele_t));
  if (newh == NULL)
  {
    return false;
  }

  newh->value = v;
  newh->next = q->head;
  if (q->head == NULL)
  {
    q->tail = newh;
  }

  q->head = newh;
  q->size++;

  return true;
}

/*
  Attempt to insert element at tail of queue.
  Return true if successful.
  Return false if q is NULL or could not allocate space.
 */
bool
q_insert_tail (queue_t *q, int v)
{
  list_ele_t *newt;
  /*What if q is NULL?*/
  if (q == NULL)
  {
    return false;
  }
  /*What if newt is NULL?*/
  newt = malloc(sizeof(list_ele_t));

  if (newt == NULL)
  {
    return false;
  }

  if(q->tail != NULL)
  {
    q->tail->next = newt;
  }

  q->tail = newt;

  if(q->head == NULL)
  {
    q->head = newt;
  }

  newt->value = v;
  newt->next = NULL;
  q->size++;
  /* You need to write the complete code for this function */
  /* Remember: It should operate in O(1) time */
  return true;
}

/*
  Attempt to remove element from head of queue.
  Return true if successful.
  Return false if queue is NULL or empty.
  If vp non-NULL and element removed, store removed value at *vp.
  Any unused storage should be freed
*/
bool
q_remove_head (queue_t *q, int *vp)
{
  /* You need to fix up this code. */
  list_ele_t *newh = q->head;

  if (q == NULL || q->head == NULL)
  {
    return false;
  }

  if (vp != NULL)
  {
    *vp = newh->value;
  }

  newh = q->head;
  q->head = q->head->next;
  free(newh);
  q->size--;
  return true;
}

/*
  Return number of elements in queue.
  Return 0 if q is NULL or empty
 */
int
q_size (queue_t *q)
{
  /* You need to write the code for this function */
  /* Remember: It should operate in O(1) time */
  if (q == NULL)
  {
    return 0;
  }
   return q->size;
}

/*
  Reverse elements in queue.

  Your implementation must not allocate or free any elements (e.g., by
  calling q_insert_head or q_remove_head).  Instead, it should modify
  the pointers in the existing data structure.
 */
void
q_reverse (queue_t *q)
{
  if (q == NULL || q->head == NULL)
  {
    return;
  }
  /* You need to write the code for this function */
}
