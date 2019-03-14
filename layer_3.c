#include "raid_defines.h"
#include "layer_3.h"
#include <string.h>

///////////////////////////////////
//FNAME/SIZE/NBLOCKS/FIRST_BYTE////
//xINODES_TABLES_SIZE//////////////
//raidtype/nb_block_used/firstbyte/
///////////////////////////////////

inode_table_t read_inodes_table(virtual_disk_t disk)
{
    inote_table_t inodes[INODES_TABLE_SIZE];
    uchar nameInter[MAX_FILE_NAME_SIZE];
    uchar intInter[4];
    inode_t inoteInter;

    for(int i = 1; i <= INODES_TABLE_SIZE; i ++){
    	/* lecture du nom */
    	read_chunk(0, nameInter, MAX_FILE_NAME_SIZE, disk);
    	strcpy(nameInter, inoteInter.filename);

    	/* lecture de size */
    	read_chunk(i * MAX_FILE_NAME_SIZE, intInter, MAX_FILE_SIZE, disk);
    	inoteInter.size = atoi(intInter);

    	/* lecture de nblocks */
    	compute_nblock(inodeInter.size);
    	inodeInter.nblocks = compute_nblock(inodeInter.size);
    	inodeInter.nblocks += inodeInter.nblocks/(disk.ndisk-1)+1;

    	/* lecture first_byte */
    	read_chunk(i * ())
    }
}


void write_inode_tables(inode_table_t inodes, virtual_disk_t disk){

}
