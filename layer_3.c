#include "raid_defines.h"
#include "layer_3.h"

inode_table_t read_inodes_table(virtual_disk_t disk)
{
    return disk.inodes;
}