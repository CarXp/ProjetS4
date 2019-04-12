#include "raid_defines.h"
#include "layer_1.h"
#include "layer_2.h"
#include "layer_3.h"
#include <string.h>

int main(void){
	printf("--Démarrage de l'application test cmd_test1 ...\n");
	virtual_disk_t disk;
	init_disk_raid5("../RAID5/",&disk);
	printf("--Démarrage terminé ! \n\n\n");

	
	printf("-- Test écriture superblock\n");
	printf("\nÉcriture en cours du super_block ...\n");
	disk.super_block.raid_type = CINQ;
	disk.super_block.nb_blocks_used = 52;
	disk.super_block.first_free_byte = 89;
	write_super_block(disk);	

	printf("Ecriture effectuée\n");
	printf("\n-- Test lecture superblock\n");
	read_super_block(&disk);
	printf("\n");
	printf("disk raid_type %d\ndisk nb_blocks_used %d\ndisk first_free_byte %d\n\n", disk.super_block.raid_type, disk.super_block.nb_blocks_used, disk.super_block.first_free_byte);
	
	printf("\n");
	printf("--Test écriture table d'inode\n\n");
	//Création du test de l'écriture de la table d'inode
	inode_table_t tabI;
	for(int i = 0; i < 10; i++){
		strcpy(tabI[i] . filename, "test");
		tabI[i].size = 2;
		tabI[i].nblock = 3;
		tabI[i].first_byte = 4;
		if(i == 4)
			strcpy(tabI[i].filename, "weshnegro");
	}

	write_inode_table(tabI, disk);

	printf("Ecriture effectuée\n\n");
	printf("-- Test lecture table d'inode\n\n");


	//for(int i =0 ; i < 10; i++){
	//	printf("tabInode indice %d: %s - %d - %d - %d\n", i, tab[i] -> filename, tab[i] -> size, tab[i] -> nblock, tab[i] -> first_byte);
	//}

	read_inode_table(&disk);
	for(int i = 0; i< INODE_TABLE_SIZE; i++){
		printf("tabInode indice %d: %s - %d - %d - %d\n", i, disk.inodes[i].filename, disk.inodes[i].size, disk.inodes[i].nblock, disk.inodes[i].first_byte);
	}
	



	fermeture_systeme_raid5(disk);
	return 0;
}