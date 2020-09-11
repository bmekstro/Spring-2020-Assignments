//Author: Brian Ekstrom
//Date:  12/9/19
//Assignment: Cachelab
#include "cachelab.h"
#include <getopt.h>  // getopt
#include <stdbool.h> // bool, true, false
#include <stddef.h>  // size_t
#include <stdio.h>   // fclose, fgets, fopen, printf, sscanf, FILE
#include <stdlib.h>  // exit, free, malloc, strtol
#include <string.h>  // memset, strncpy, strdup

/* type definitions */
typedef unsigned long long address_t;

typedef struct
{
  bool valid;
  int lru;
  address_t tag;
} cacheline_t;

typedef cacheline_t* cache_set;

/* global variables */
const int MAX_LINE_LENGTH = 256;
int misses = 0;
int evictions = 0;
int hits = 0;
FILE* file;
/* TODO: add global variables for cache configuration (E, s, b) and flags (verbose) */
cache_set* cache;
int bytes;
int sets;
int lines; 
int S;
bool verbose;
int leastRecentUsed;
/* function declarations */
void
printHelp (const char* cmd);

void
initCache (int argc, char* argv[]);

/* TODO: implement this function. Update the cache, misses, evictions, and hits */
void
access (address_t addr);

int
main (int argc, char* argv[])
{
  char type;
  address_t addr;
  char line[MAX_LINE_LENGTH];
  initCache (argc, argv);

  /*
      Precondition: file is pointer to file to read
      Precondition: cache is initialized

      for line in file:
        type, addr = parse(line)
        access (addr);
        if type == 'M':
          access (addr)
     */

 //While not at end of file it scans the current line.  If L, S, or M are detected it calls the access function.
 int count;
 fgets(line,MAX_LINE_LENGTH,file);
 while(!feof (file))
 {
    sscanf(line, " %c %llx,%d", &type, &addr, &count);
    if(type == 'L' || type == 'S')
    {
        access(addr);
    }
    if(type == 'M')
    {
        access(addr);
        access(addr);
    }
    fgets(line,MAX_LINE_LENGTH,file);
 }
 fclose(file);

  printSummary (hits, misses, evictions);
  free (cache);
  return EXIT_SUCCESS;
}

void
initCache (int argc, char* argv[])
{
  const char* cmd = argv[0];
  bool error = false;
  bool help = false;
  char opt;
  char fileName[MAX_LINE_LENGTH];
  bool verbose;
  int s, E, b;
  strncpy (fileName, "", MAX_LINE_LENGTH);
  while (!(error || help) && ((opt = getopt (argc, argv, "s:E:b:t:hv")) != EOF))
    switch (opt)
    {
      case 'h':
        help = true;
        break;
      case 'v':
        verbose = true;
        break;
      case 's':
        s = strtol (optarg, NULL, 10);
        break;
      case 'E':
        E = strtol (optarg, NULL, 10);
        break;
      case 'b':
        b = strtol (optarg, NULL, 10);
        break;
      case 't':
        strncpy (fileName, optarg, MAX_LINE_LENGTH);
        break;
      default:
        error = true;
        break;
    }
  if (!error)
  {
    file = fopen (fileName, "r");
    if (!file)
      printf ("%s: Invalid file specified\n", cmd);
    if (!(s && E && b))
      printf ("%s: Invalid command line argument specified for s,E,b\n", cmd);
  }
  if (error || !file || !(s && E && b) || help)
    printHelp (cmd);
  /* TODO:
     *   - set verbose, s, E, b to globals
     *   - allocate space for cache, determine cache size
     */
    //Set local variables to the matching global variables.
    lines = E;
    sets = s;
    bytes = b;
    verbose = verbose;
    S = (1 << s);
    //Find the space for the cache.  Use malloc to allocate it.
    cache = (cache_set*) malloc (S * sizeof (cache_set));

    for(int i = 0; i < S; ++i)
    {
        cache_set Set = (cacheline_t*) malloc(E * sizeof(cacheline_t));
        for(int j = 0; j < lines; ++j)
        {
            cacheline_t cacheLine;
            cacheLine.valid = false;
            cacheLine.lru = 0;
            cacheLine.tag = 0;
            Set[j] = cacheLine;
        }
        cache[i] = Set;
    }
}

// |tag||               bytes                  ||                     sets                     |
/*Find the tag first by shifting bits and removing bytes and sets.  Then find the set by removing the tag
and bytes bits.  
*/ 
void 
access (address_t addr)
{
    address_t tag = (addr >> bytes) >> sets;;
    address_t tagTwo = (addr << (64 - (bytes + sets)));
    address_t set = (tagTwo >> (64- (bytes + sets) + bytes));

    //First condition is a hit. Look through our cache which we set and find a hit
    cache_set Set = cache[set];
    for(int i = 0; i < lines; ++i)
    {
        cacheline_t Line = Set[i];
	//Update counter to leastRecentUsed and hit if a hit is found.  
        if(Line.tag == tag && Line.valid)
        {
            ++hits;
            ++leastRecentUsed;
            Line.lru = leastRecentUsed;
            return;
        }
    }

     //Second condition is a miss.  Don't need to check because a non hit is just a miss.  Update miss counter.
    //Third Condition is if the cache is already full.
    bool full = true;
    int index = 0;
    ++misses;
    for(int i = 0; i < lines; ++i)
    {
        cacheline_t Line = Set[i];
        if(Line.valid == false)
        {
            full = false;
           break;
        }
    }

    //Fourth condition is the cache is not full place the value at the first empty slot.
    if(!full)
    {
        for(int i = 0; i < lines; ++i)
        {
            cacheline_t Line = Set[i];
            if(Line.valid == false)
            {
                index = i;
                break;
            }
        }
        ++leastRecentUsed;
        Set[index].tag = tag;
        Set[index].valid = true;
        Set[index].lru = leastRecentUsed;
    }

    //Fifth condition is if the cache is full, find the least recently used value and remove it.  Place the new
    //value at that slot.
    else
    {
        int min = Set[0].lru;
        for(int i = 0; i < lines; ++i)
        {
            cacheline_t Line = Set[i];
            if(Line.lru < min)
            {
                index = i;
                min = Line.lru;
            }
        }
        ++evictions;
        ++leastRecentUsed;
        Set[index].tag = tag;
        Set[index].lru = leastRecentUsed;
    }

}

void
printHelp (const char* cmd)
{
  printf (
    "\n\
Usage:\n\
  %s [-hv] -s <num> -E <num> -b <num> -t <file>\n   \
\n\
Options:\n\
  -h         Print this help message.\n\
  -v         Optional verbose flag.\n\
  -s <num>   Number of set index bits.\n\
  -E <num>   Number of lines per set.\n\
  -b <num>   Number of block offset bits.\n\
  -t <file>  Trace file.\n\
\n\
Examples:\n\
$ %s -s 4 -E 1 -b 4 -t traces/yi.trace\n\
$ %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n\
",
    cmd, cmd, cmd);
  exit (EXIT_SUCCESS);
}
