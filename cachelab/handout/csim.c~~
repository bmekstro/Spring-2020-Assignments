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
cache_set* cache;

/* global variables */
const int MAX_LINE_LENGTH = 256;
int misses = 0;
int evictions = 0;
int hits = 0;
FILE* file;
/* TODO: add global variables for cache configuration (E, s, b) and flags (verbose) */
int E_lines; 
int s_sets;
int b_bytes;
int S;
bool verbose_flags;
int LRUcounter = 0;

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

 int size = 0;
 fgets(line,MAX_LINE_LENGTH,file);
 while(!feof (file))
 {
    sscanf(line, " %c %llx,%d", &type, &addr, &size);
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
    //cache size determined by B * S * E
    //B = 2^b S = 2^s
    //set globals
    verbose_flags = verbose;
    E_lines = E;
    s_sets = s;
    b_bytes = b;
    S = (1 << s);
    //allocate space for cache
    cache = (cache_set*) malloc (S * sizeof (cache_set));

    for(int i = 0; i < S; i++)
    {
        cache_set cacheSet = (cacheline_t*) malloc (E * sizeof(cacheline_t));
        for(int c = 0; c < E_lines; c++)
        {
            cacheline_t cacheLine;
            cacheLine.valid = false;
            cacheLine.lru = 0;
            cacheLine.tag = 0;
            cacheSet[c] = cacheLine;
        }
        cache[i] = cacheSet;
    }
}

void 
access (address_t addr)
{
    //Calculate tag by removing the b and s bytes
    address_t tag = (addr >> b_bytes) >> s_sets;;


    //Calculate set by removing the tag and b bytes
    int tagSize = (64 - (b_bytes + s_sets));
    address_t no_tag = (addr << tagSize);

    int bShift = tagSize + b_bytes;
    address_t set = (no_tag >> bShift);

    //Look through our sets to see if we have a hit
    cache_set currSet = cache[set];
    for(int i = 0; i < E_lines; i++)
    {
        cacheline_t currLine = currSet[i];
        //if we find a hit, update hits and lru then return
        if(currLine.tag == tag && currLine.valid)
        {
            hits++;
            LRUcounter++;
            currLine.lru = LRUcounter;
            return;
        }
    }

    //If we don't find a hit, we have a miss
    bool full = true;
    misses++;
    //check to see if the cache is full
    for(int i = 0; i < E_lines; i++)
    {
        cacheline_t currLine = currSet[i];
        if(currLine.valid == false)
        {
            full = false;
            break;
        }
    }

    int index = 0;

    //If the cache is not full, find the empty slot and place the elem there
    if(!full)
    {
        for(int i = 0; i < E_lines; i++)
        {
            cacheline_t currLine = currSet[i];
            if(currLine.valid == false)
            {
                index = i;
                break;
            }
        }

        currSet[index].tag = tag;
        currSet[index].valid = true;
        LRUcounter++;
        currSet[index].lru = LRUcounter;
    }
    //If the cache is full, evict the oldest elem in cache and replace it
    else
    {
        int min = currSet[0].lru;
        for(int i = 0; i < E_lines; i++)
        {
            cacheline_t currLine = currSet[i];
            if(currLine.lru < min)
            {
                index = i;
                min = currLine.lru;
            }
        }
        evictions++;
        currSet[index].tag = tag;
        LRUcounter++;
        currSet[index].lru = LRUcounter;
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
