/* Filename   : mm.c
 * Author     : Xavier A.Corniel & Brian Ekstrom
 * Course     : CSCI 380-01
 * Assignment : Malloc
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include "memlib.h"
#include "mm.h"

 // Useful suggestions
 /*Some strong recommendations:
 Have ONE and ONLY ONE global variable:
 address g_heapBase;
 Write a function �extendHeap� that takes a number of words to increase the heap size by.
 In mm_init set g_heapBase to one double-word beyond the start of the heap (just like we do in the visual guide).
 The address calculation inline functions are one-liners.
 Functions like makeBlock need to write a header and footer and require two lines.
 */
 /****************************************************************/
 // Useful type aliases
typedef uint64_t word;
typedef uint32_t tag;
typedef uint8_t  byte;
typedef byte*    address; 
static address g_heapBase;

const uint8_t WORD_SIZE = sizeof (word);
const uint8_t DWORD_SIZE = WORD_SIZE * 2;
const uint8_t ALIGNMENT = DWORD_SIZE;
const uint8_t MIN_BLOCK_SIZE = 2;
const uint32_t CHUNKSIZE = 1<<14;

static inline uint32_t
MAX(uint32_t x, uint32_t y) 
{ 
  return ((x) > (y) ? (x) : (y)); 
}

/* Pack a size and allocated bit into a word */
static inline uint32_t
PACK(uint32_t size, uint32_t alloc)
{
	return ((size) | (alloc));
}

/* Read and write a word at address p */
static inline size_t
GET(address p)
{
	return (*(size_t *)(p));
}

static inline size_t 
PUT(address p, size_t val)
{
	return (*(size_t *)(p) = (val));
}

/* Read the size and allocated fields from address p */
static inline uint32_t
sizeOf(address p)
{
	return (GET(p) & (tag)-2);
}

static inline bool 
isAllocated(address p)
{
	return (GET(p) & (tag)1);
}

/* Given block ptr bp, compute address of its header and footer */
static inline void
*header(address p)
{
	return  ((char *)(p)-WORD_SIZE);
}

static inline void
*footer(address p)
{
	return ((char *)(p)+sizeOf(header(p)) - DWORD_SIZE);
}

/* Given block ptr bp, compute address of next and previous blocks */
static inline void 
*nextBlock(address p)
{
	return ((char *)(p)+sizeOf((p)-WORD_SIZE));
}
static inline void 
*prevBlock(address p)
{
	 return ((char *)(p)-sizeOf((p)-DWORD_SIZE));
}

/****************************************************************/
   // Non-inline functions

static inline void
*coalesce(void *bp)
{
	uint32_t prev_alloc = isAllocated(footer(prevBlock(bp)));
	uint32_t next_alloc = isAllocated(header(nextBlock(bp)));
	uint32_t size = sizeOf(header(bp));

  /*  If the previous and next blocks were allocated then return the address of 
      the block passed to coalesce.*/
	if (prev_alloc && next_alloc) 
  {
		return bp;
	}

  /* If the the previous block is allocated and the next block is
     not allocated, increment size by the size of the header of the next concurrent block.
     Allocate a block of memory with the header and footer.*/
	else if (prev_alloc && !next_alloc) 
  { 
		size += sizeOf(header(nextBlock(bp)));
		PUT(header(bp), PACK(size, 0));
		PUT(footer(bp), PACK(size, 0));
	}
  /* If the previous block is not allocated  and the next block is allocated
    increment the size by the size of the header of the previous block.  Allocate
    a block of memory with the header and footer where the prev block was allocated.
    Change the base pointer to the updated base pointer of the new block.
  */
	else if (!prev_alloc && next_alloc) 
  {
		size += sizeOf(header(prevBlock(bp)));
		PUT(footer(bp), PACK(size, 0));
		PUT(header(prevBlock(bp)), PACK(size, 0));
		bp = prevBlock(bp);

	}

  /*  Base case that if no block is allocated, increment the size of by the header
      of the prev block + the size of the footer of the next block.  Allocate a block
      of memory with the header where the prevblock ended and a footer before the next
      block begins.  Update the base pointer to where the previous blocks base pointer
      was.
  */
	else 
  { 
		size += sizeOf(header(prevBlock(bp))) +
			sizeOf(footer(nextBlock(bp)));
		PUT(header(prevBlock(bp)), PACK(size, 0));
		PUT(footer(nextBlock(bp)), PACK(size, 0));
		bp = prevBlock(bp);

	}
	return bp;
}

static void
*extend_heap(uint32_t words)
{
	address bp;
	uint32_t size;

	/* Allocate an even number of words to maintain alignment */
	size = (words % 2) ? (words + 1) * WORD_SIZE : words * WORD_SIZE;
	if ((long) (bp = mem_sbrk ((int)size)) == -1)
		return NULL;

	/* Initialize free block header/footer and the epilogue header */
	PUT(header(bp), PACK(size, 0)); /* Free block header */
	PUT(footer(bp), PACK(size, 0)); /* Free block footer */
	PUT(header(nextBlock(bp)), PACK(0, 1)); /* New epilogue header */

   /* Coalesce if the previous block was free */
	return coalesce(bp);
}

/*  Place is a helper function that places the requested block at the beginning of the
    free block.
*/
static void
place(void *bp, uint32_t asize)
{
	uint32_t csize = sizeOf(header(bp));

	if ((csize - asize) >= (2 * DWORD_SIZE)) {
		PUT(header(bp), PACK(asize, 1));
		PUT(footer(bp), PACK(asize, 1));
		bp = nextBlock(bp);
		PUT(header(bp), PACK(csize - asize, 0));
		PUT(footer(bp), PACK(csize - asize, 0));

	}
	else {
		PUT(header(bp), PACK(csize, 1));
		PUT(footer(bp), PACK(csize, 1));

	}
}

static void *find_fit(uint32_t asize)
{
	/* First-fit search */
	void *bp;

	for (bp =  g_heapBase; sizeOf(header(bp)) > 0; bp = nextBlock(bp)) {
		if (!isAllocated(header(bp)) && (asize <= sizeOf(header(bp)))) {
			return bp;

		}

	}
	return NULL; /* No fit */
}

int
mm_init(void)
{
	g_heapBase = g_heapBase + DWORD_SIZE;

	if (( g_heapBase = mem_sbrk(4 * WORD_SIZE)) == (void *)-1)
		return -1;
  
	PUT( g_heapBase + (1 * WORD_SIZE), PACK(DWORD_SIZE, 1));
	PUT( g_heapBase + (2 * WORD_SIZE), PACK(DWORD_SIZE, 1));
	g_heapBase += (2 * WORD_SIZE);

	/* Extend the empty heap with a free block of CHUNKSIZE bytes */
	if (extend_heap(CHUNKSIZE / WORD_SIZE) == NULL)
		return -1;
	return 0;
}

void
mm_free(void *ptr)
{
	uint32_t size = sizeOf(header(ptr));

	PUT(header(ptr), PACK(size, 0));
	PUT(footer(ptr), PACK(size, 0));

  fprintf(stderr, "realloc block at %p to %u\n", ptr, size);
	coalesce(ptr);
}

void*
mm_malloc(uint32_t size)
{
	uint32_t asize; /* Adjusted block size */
	uint32_t extenDWORD_SIZE; /* Amount to extend heap if no fit */
	char *bp;

	/* Ignore spurious requests */
	if (size == 0)
		return NULL;

	/* Adjust block size to include overhead and alignment reqs. */
	if (size <= DWORD_SIZE)
		asize = 2 * DWORD_SIZE;
	else
		asize = DWORD_SIZE * ((size + (DWORD_SIZE)+((uint32_t) DWORD_SIZE - 1)) / DWORD_SIZE);

	/* Search the free list for a fit */
	if ((bp = find_fit(asize)) != NULL) {
		place(bp, asize);
		return bp;

	}

	/* No fit found. Get more memory and place the block */
	extenDWORD_SIZE = MAX(asize, CHUNKSIZE);
	if ((bp = extend_heap(extenDWORD_SIZE / WORD_SIZE)) == NULL)
		return NULL;

  fprintf(stderr, "allocate block of size %u\n", size);
	place(bp, asize);
	return bp;
}

void *mm_realloc(void *ptr, uint32_t size)
{
    uint32_t oldsize;
    void *newptr;

    /* If size == 0 then this is just free, and we return NULL. */
    if(size == 0) 
    {
	    mm_free(ptr);
	    return 0;
    }

    /* If oldptr is NULL, then this is just malloc. */
    if(ptr == NULL)
    {
	    return mm_malloc(size);
    }

    newptr = mm_malloc(size);

    /* If realloc() fails the original block is left untouched  */
    if(!newptr) 
    {
	    return 0;
    }

    /* Copy the old data. */
    oldsize = (uint32_t) sizeOf(header(ptr));
    if(size < oldsize) oldsize = size;
    memcpy(newptr, ptr, oldsize);

    /* Free the old block. */
    mm_free(ptr);

    fprintf(stderr, "realloc block at %p to %u\n", ptr, size);
    return newptr;
}