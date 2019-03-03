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
	
	inode_table_t table_inode;

	table_inode = read_inodes_table(&disk);

	super_block_t sblock;
	
	sblock.raid_type = CINQ;	
	sblock.nb_block_used = table_inode[0].nblock;
	sblock.first_free_byte = table_inode[0].size;

	disk->super_block = sblock;
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
	


int compute_nblock(uint n){
	uint res = n/4;
	if(n % 4 != 0){
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



/* fonction repair qui, en cas de problème de lecture de bloc, le répare grâce au bloc de parité */
/* V1 */
/*
void repair(block_t block, int pos, int disk_id, virtual_disk_t virtual_disk)
{
    uchar xor[BLOCK_SIZE];
    block_t block_tmp; 

    for (int i = 0 ; i < virtual_disk.ndisk ; i++)
    {
        if (i != disk_id)
        {
            read(block_tmp, pos, i, virtual_disk);
            xor ^= block_tmp.data;
        }
    }

    block.data = xor;

}
*/
/* FIN V1 */


/* V2.1 */
/*
void repair(block_t block, stripe_t bande, int pos, int disk_id, virtual_disk_t disk)
{
	//2 méthodes possibles: utilisation de la bande OU lecture directement sur le fichier de chaque block
	//Ici méthode 1: utilisation de la bande
	//Fonction auxiliaire: calcul_id_parity qui détermine à quelle position se trouve le bloc de parité en fonction de la pos du block à réparer dans le disque disk_id

	char charRep;
	int idParity = calcul_id_parity(disk_id, disk);

	//On regarde pour chaque caractère 
	for(int idC = 0 ;idC < BLOCK_SIZE; idC++){
		charRep = 0 ;

		//Pour chaque bloc de la bande sauf celui à réparer
		for(int idB = 0 ;idB < bande.nblocks; idB++){
			if(idB != )
			//On fait le xor de chaque caractère
			charRep ^= bande.stripe[idB].[idC];
		}

		//On remplace le caractère dans le bloc à réparer
		block.data[idC] = charRep;
	}

	//Enfin, on re écrit dans le fichier le nouveau block
	write_block(block, pos, disk_id, disk);
}
*/
/* FIN V2.1 */


void affichage_block(block_t block)
{
    for (int i = 0 ; i < BLOCK_SIZE ; i++)
        printf("Octet %d = %x\n", i, block.data[i]);
}