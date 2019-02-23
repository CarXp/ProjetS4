#include "layer_1"

/* fonction init_disk_raid5 qui initialise le systeme raid5 à son ouverture */
void init_disk_raid5(char *dirname,virtual_disk_t *disk){
	//Ouverture des fichiers et initialisation du tableau STORAGE

	//Mode raid5
	disk->raidmode=RAID5;
	disk->ndisk=4;
	disk->number_of_file=5;
	//Récupération des infos sur le super_block
	/*
	super_block_t sblock;
	sblock.raid_type=RAID5;
	sblock.nb_block_used=A_DETERMINER_AVEC_FICHIER
	sblock.first_free_byte=A_DETERMINER_AVEC_FICHIER
	disk->super_block=sblock;
	*/
}


/* fonction fermeture_systeme_raid5 qui ferme le systeme: vérification pour éviter les pertes + fermeture des fichiers */
void fermeture_systeme_raid5(virtual_disk_t disk)
{
	printf("Fermeture du systeme...\n");
	for(int i=0;i<disk.ndisk;i++){
		printf("Disque %d éteint\n",i);
		fclose(disk.storage[i]);
	}
	printf("Fermeture effctuée.\n");
}

int compute_nblock(uint n){
	uint res = n/4;
	if(n%4!=0){
		return res+1;
	}
	return res;
}

/* fonction write_block qui écrit un bloc dans le disque disk_id du systeme à la position pos */
void write_block(block_t block, int pos, int disk_id, virtual_disk_t virtual_disk)
{
    FILE fichier = virtual_disk.storage[disk_id];
    fseek(fichier, pos*4, SEEK_SET);
    if(fwrite(block.data, sizeof(uchar), BLOCK_SIZE, fichier)!=BLOCk_SIZE)
    	printf("Erreur écriture dans le fichier interne\n");
}

/* fonction read_block qui lit le bloc dans le disk_id du système à la position pos */
int read_block(block_t block, int pos, int disk_id, virtual_disk_t virtual_disk)
{
	FILE fichier = virtual_disk.storage[disk_id];
    fseek(fichier, pos*4, SEEK_SET);
    int nblus = fread(block.data, sizeof(uchar), BLOCK_SIZE, fichier);
    if (nblus != BLOCK_SIZE)
    {
        printf("Erreur dans la lecture du fichier\n");
        return ERREUR;
    }

    return SUCCESS;
}

/* fonction repair qui, en cas de problème de lecture de bloc, le répare grâce au bloc de parité */
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