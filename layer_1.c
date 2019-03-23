#include "raid_defines.h"
#include "layer_1.h"
#include "layer_3.h"

/* fonction init_disk_raid5 qui initialise le systeme raid5 à son ouverture */
void init_disk_raid5(char *dirname, virtual_disk_t *disk){
	//Ouverture des fichiers et initialisation du tableau STORAGE

	//Mode raid5
	disk->raidmode = CINQ;
	disk->ndisk = 4;
	disk->number_of_files = 4;
	char fname[FILENAME_MAX_SIZE];

	disk->storage = malloc(4 * sizeof(FILE*));

	for(int i = 0 ; i < 4 ; i++){
		sprintf(fname,"../RAID5/d%d", i);
		disk->storage[i]=  fopen(fname, "r+b");
		printf("%s\n", fname);
	}

	//Récupération des infos sur le super_block
	/*
	inode_table_t table_inode;

	table_inode = read_inodes_table(&disk);

	super_block_t sblock;
	
	sblock.raid_type = CINQ;	
	sblock.nb_block_used = table_inode[0].nblock;
	sblock.first_free_byte = table_inode[0].size;

	disk->super_block = sblock;
	*/
}


/* fonction fermeture_systeme_raid5 qui ferme le systeme: vérification pour éviter les pertes + fermeture des fichiers */
void fermeture_systeme_raid5(virtual_disk_t disk)
{
	printf("--Fermeture du systeme...\n");
	for(int i = 0 ; i < disk.ndisk ; i++){
		printf("Disque %d éteint\n", i);
		fclose(disk.storage[i]);
	}
	printf("--Fermeture effctuée.\n");
}
	

/* fonction compute_nblock qui détermine le nombre de block qu'il faut dans le systeme RAID5 pour écrire n octets (block de parité non inclus) */
int compute_nblock(uint n){
	uint res = n/BLOCK_SIZE;
	if(n % BLOCK_SIZE != 0){
		return res + 1;
	}
	return res;
}



/* fonction write_block qui écrit un bloc dans le disque disk_id du systeme à la position pos */
void write_block(block_t block, int pos, int disk_id, virtual_disk_t virtual_disk)
{
    FILE * fichier = virtual_disk.storage[disk_id];
    fseek(fichier, pos*4, SEEK_SET);

    fwrite(block.data, sizeof(uchar), BLOCK_SIZE, fichier);
}



/* fonction read_block qui lit le bloc dans le disk_id du système à la position pos */
int read_block(block_t * block, int pos, int disk_id, virtual_disk_t virtual_disk)
{
	FILE * fichier = virtual_disk.storage[disk_id];
    fseek(fichier, pos*4, SEEK_SET);
    int nblus = fread(block->data, sizeof(uchar), BLOCK_SIZE, fichier);

    if (nblus != BLOCK_SIZE)
    {
        printf("Erreur dans la lecture du fichier\n");
        return ERREUR;
    }

    return SUCCESS;
}

/* fonction repair qui répare un block en cas de problème: xor des autres blocs */
void repair(block_t block, int pos, int disk_id, virtual_disk_t disk){
	block_t newBlock;
	uchar charInter = 0;
	block_t inter;

	/* Pour chaque caractère */
	for(int idC = 0; idC < BLOCK_SIZE; idC ++){
		/* pour chaque autre block */
		for(int idBlock = 0; idBlock < disk.ndisk ; idBlock++){
			if(idBlock != disk_id){
				read_block(&inter, pos, idBlock, disk);
				charInter ^= inter.data[idC];
			}
		}
		newBlock.data[idC] = charInter;
	}

	block = newBlock;
}


/* fonction affichage_block qui affiche les données du block en hexadecimal */
void affichage_block(block_t block)
{
    for (int i = 0 ; i < BLOCK_SIZE ; i++)
        printf("Octet %d = 0x%x\n", i, block.data[i]);
}
