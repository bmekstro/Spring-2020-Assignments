/*Author:			Brian Ekstrom
  Date:				4/29/20
  Assignment:		Filesystems
  Description:		Create a dummy filesystem that you can create, delete, write, and read files from.
					
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define CMD_LENGTH 32
#define FS_NUM_BLOCKS    128
#define FS_MAX_FILE_SIZE 8
#define FS_MAX_FILENAME  16
#define FS_MAX_FILES     16
#define FS_BLOCK_SIZE    1024


struct fs_t
{
  // you determine what goes here
	int fd;
};

struct inode {
	char name[16];        //file name
	int size;             // file size (in number of blocks)
	int blockPointers[8]; // direct block pointers
	int used;             // 0 => inode is free; 1 => in use
};

// open the file with the above name
void
fs_open (struct fs_t *fs, char diskName[16])
{
	fs->fd = open(diskName, O_RDWR);
}

// close and clean up all associated things
void
fs_close (struct fs_t *fs)
{
	close(fs->fd);
}

// create a file with this name and this size
void
fs_create (struct fs_t *fs, char name[16], int size)
{
  // Step 1: check to see if we have sufficient free space on disk by
  // reading in the free block list. To do this:
  // - move the file pointer to the start of the disk file.
  // - Read the first 128 bytes (the free/in-use block information)
  // - Scan the list to make sure you have sufficient free blocks to
  //   allocate a new file of this size
	char freeBlockList[128]; 
	int freeBlock = 0; 
	lseek(fs->fd, 0, SEEK_SET);
	read(fs->fd, freeBlockList, 128); 
	for (int i = 1; i < 128; ++i) 
	{
		if (freeBlockList[i] == 0)
		{
			freeBlock++;
		}
	}

	/* Error checking.  File sizes are limited to 8kb so if greater than dont allow creation.
	   If there is not enough space left on disk don't allow creation.*/
	if (size > 8)
	{
		printf("File too big!");
		return;
	}
	if (freeBlock <= size) 
	{
		printf("Not enough space on disk.");
		return;
	}
	
  // Step 2: we look  for a free inode on disk
  // - Read in a inode
  // - check the "used" field to see if it is free
  // - If not, repeat the above two steps until you find a free inode
  // - Set the "used" field to 1
  // - Copy the filename to the "name" field
  // - Copy the file size (in units of blocks) to the "size" field
	struct inode iNode[16]; 
	lseek(fs->fd, 128, SEEK_SET); 
	read(fs->fd, iNode, sizeof(iNode)); 
	int currentNode = -1; 
	for (int i = 0; i < 16; ++i) 
	{
		if (strcmp(name, iNode[i].name) == 0 && iNode[i].used == 1) 
		{
			printf("Filename already exists.  Please enter another file name.");
			return;
		}
		if (iNode[i].used == 0) 
		{
			strcpy(iNode[i].name, name); 
			iNode[i].size = size; 
			iNode[i].used = 1; 
			currentNode = i; 
			break;
		}
	}
	if (currentNode == -1)
		{
			printf("Too many files created!");
			return;
		}
  // Step 3: Allocate data blocks to the file
  // - Scan the block list that you read in Step 1 for a free block
  // - Once you find a free block, mark it as in-use (Set it to 1)
  // - Set the blockPointer[i] field in the inode to this block number.
  // - repeat until you allocated "size" blocks
	int allocateDataBlock = 0;
	for (int i = 1; i < 128; ++i) 
	{
		if (allocateDataBlock != size) 
		{
			if (freeBlockList[i] == 0) 
			{
				freeBlockList[i] = 1; 
				iNode[currentNode].blockPointers[allocateDataBlock] = i; 
				++allocateDataBlock;
			}
		}
	}
  // Step 4: Write out the inode and free block list to disk
  // - Move the file pointer to the start of the file
  // - Write out the 128 byte free block list
  // - Move the file pointer to the position on disk where this inode was stored
  // - Write out the inode
	lseek(fs->fd, 0, SEEK_SET); 
	write(fs->fd, freeBlockList, 128); 
	lseek(fs->fd, 128 + (currentNode * 56), SEEK_SET);
	write(fs->fd, &iNode[currentNode], 56); 
}

// Delete the file with this name
void
fs_delete (struct fs_t *fs, char name[16])
{
  // Step 1: Locate the inode for this file
  //   - Move the file pointer to the 1st inode (129th byte)
  //   - Read in a inode
  //   - If the iinode is free, repeat above step.
  //   - If the iinode is in use, check if the "name" field in the
  //     inode matches the file we want to delete. IF not, read the next
  //     inode and repeat
	struct inode iNode;
	lseek(fs->fd, 128, SEEK_SET);
	read(fs->fd, &iNode, sizeof(iNode));
	int currentNode = 0;
	for (int i = 1; i < 16; ++i)
	{	
		if (strcmp(name, iNode.name) == 0 && iNode.used == 1)
		{
			break;
		}
		// If you reach the end of the disk, there are no files.
		if (i == 15)
		{
			return;
		}

		lseek(fs->fd, 128 + (i * sizeof(iNode)), SEEK_SET);
		read(fs->fd, &iNode, sizeof(iNode));
		++currentNode;
	}
  // Step 2: free blocks of the file being deleted
  // Read in the 128 byte free block list (move file pointer to start
  // of the disk and read in 128 bytes)
  // Free each block listed in the blockPointer fields
	char freeBlockList[128];
	lseek(fs->fd, 0, SEEK_SET);
	read(fs->fd, freeBlockList, sizeof(freeBlockList));
	int curr = 0;
	for (int i = 0; i < 128; ++i)
	{
		if (curr < iNode.size)
		{
			if (iNode.blockPointers[curr] == i)
			{
				freeBlockList[i] = 0;
				++curr;
			}
		}
	}

  // Step 3: mark inode as free
  // Set the "used" field to 0.
	iNode.used = 0;

  // Step 4: Write out the inode and free block list to disk
  // Move the file pointer to the start of the file
  // Write out the 128 byte free block list
  // Move the file pointer to the position on disk where this inode was stored
  // Write out the inode
	lseek(fs->fd, 0, SEEK_SET);
	write(fs->fd, freeBlockList, 128);
	lseek(fs->fd, 128 + (currentNode * 56), SEEK_SET);
	write(fs->fd, &iNode, 56);
}

// List names of all files on disk
void
fs_ls (struct fs_t *fs)
{
  // Step 1: read in each inode and print!
  // Move file pointer to the position of the 1st inode (129th byte)
  // for each inode:
  //   read it in
  //   if the inode is in-use
  //     print the "name" and "size" fields from the inode
  // end for
	struct inode iNode[16];
	lseek(fs->fd, 128, SEEK_SET);
	read(fs->fd, iNode, sizeof(iNode));
	for (int i = 0; i < 16; ++i)
	{
		if (iNode[i].used == 1)
		{
			printf("%16s %6dB\n", iNode[i].name, (iNode[i].size * 1024));
		}
	}
}

// read this block from this file
void
fs_read (struct fs_t *fs, char name[16], int blockNum, char buf[1024])
{
  // Step 1: locate the inode for this file
  //   - Move file pointer to the position of the 1st inode (129th byte)
  //   - Read in a inode
  //   - If the inode is in use, compare the "name" field with the above file
  //   - If the file names don't match, repeat

	lseek(fs->fd, 128, SEEK_SET);
	struct inode iNode;
	int current = 0;
	for (int i = 1; i < 16; ++i)
	{
		lseek(fs->fd, 128 + (current * sizeof(iNode)), SEEK_SET);
		read(fs->fd, &iNode, sizeof(iNode));
		if (strcmp(name, iNode.name) == 0 && iNode.used == 1)
		{
			break;
		}
		++current;
	}
	
  // Step 2: Read in the specified block
  // Check that blockNum < inode.size, else flag an error
  // Get the disk address of the specified block
  // move the file pointer to the block location
  // Read in the block! => Read in 1024 bytes from this location into the buffer
  // "buf"
		if (blockNum <= (iNode.size - 1))
		{
			lseek(fs->fd, (iNode.blockPointers[blockNum] * 1024), SEEK_SET);
			read(fs->fd, buf, 1024);
		}

}

// write this block to this file
void
fs_write (struct fs_t *fs, char name[16], int blockNum, char buf[1024])
{
  // Step 1: locate the inode for this file
  // Move file pointer to the position of the 1st inode (129th byte)
  // Read in a inode
  // If the inode is in use, compare the "name" field with the above file
  // If the file names don't match, repeat
	lseek(fs->fd, 128, SEEK_SET);
	struct inode iNode;
	int currentNode = 0;
	for (int i = 1; i < 16; ++i)
	{
		lseek(fs->fd, 128 + (currentNode * sizeof(iNode)), SEEK_SET);
		read(fs->fd, &iNode, sizeof(iNode));
		if (strcmp(name, iNode.name) == 0 && iNode.used == 1)
		{
			break;
		}
		++currentNode;
	}
  // Step 2: Write to the specified block
  // Check that blockNum < inode.size, else flag an error
  // Get the disk address of the specified block
  // move the file pointer to the block location
  // Write the block! => Write 1024 bytes from the buffer "buf" to this location
	if (blockNum <= iNode.size - 1)
	{
		lseek(fs->fd, (iNode.blockPointers[blockNum] * 1024), SEEK_SET);
		write(fs->fd, buf, 1024);
	}
	
}

// REPL entry point
void
fs_repl ()
{
	char command[CMD_LENGTH];
	
	struct fs_t disk0;
	while (fgets(command, sizeof(command), stdin))
	{
		strtok(command, "\n");
		int size, blockInt;
		char str[8];
		char buffer[1024];
		if (strcmp(command, "disk0") == 0)
			fs_open(&disk0, command);
		else if (sscanf(command, "C %16s %d", str, &size) == 2)
			fs_create(&disk0, str, size);
		else if (sscanf(command, "D %16s", str) == 1)
			fs_delete(&disk0, str);
		else if (strcmp(command, "L") == 0)
			fs_ls(&disk0);
		else if (sscanf(command, "R %16s %d", str, &blockInt) == 2)
			fs_read(&disk0, str, blockInt, buffer);
		else if (sscanf(command, "W %16s %d", str, &blockInt) == 2)
			fs_write(&disk0, str, blockInt, buffer);
		else
			printf("Unknown command\n");
	}

}
