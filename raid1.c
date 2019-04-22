
//              /* COUCHE 1 */
//
//

void write_block_raid1(block_t block, int pos, virtual_disk_t virtual_disk)
{
    for (int i = 0 ; i < virtual_disk.ndisk ; i++)
    {
        FILE * fichier = virtual_disk.storage[i];
        fseek(fichier, pos * BLOCK_SIZE, SEEK_SET);
        fwrite(block.data, sizeof(uchar), BLOCK_SIZE, fichier);
    }
}

void repair(block_t block, int pos, int disk_id, virtual_disk_t disk){
	block_t newBlock;
    read_block(&newBlock, pos, (disk_id + 1) % disk.ndisk, disk);
	block = newBlock;
}

//
//
//              /* COUCHE 2 */
//
//

void write_chunk_raid1(uchar buffer[],int noctet, virtual_disk_t disk, int startblock){

	// 1 - Calculer le nombre de bande (compute nstripe)
	int nblock = compute_nblock(noctet);

	// 2 - Créer un tableau pour chaque block
	block_t *tabblock  = malloc(nblock * sizeof(block_t));

	int nbdisk = disk.ndisk;
	int indicebuffer = 0;

	for (int i = 0 ; i < nblock ; i++)
	{
        int indiceblock = 0;
		while(indicebuffer < noctet && indiceblock != BLOCK_SIZE)	//Tant qu'on est pas à la fin du buffer et qu'on a pas dépasser la limite du bloc
		{
			tabblock[i].data[indiceblock] = buffer[indicebuffer];
			indicebuffer++;
			indiceblock++;
		}

	    // 3 - écrire dans le systeme avec write_block chaque bande 

	    write_block_raid1(tabblock[i], startblock, disk);
    	startblock ++;
	}
}


void read_chunk_raid1(int start_byte, uchar buffer[], int noctet, virtual_disk_t disk)
{
	int nblock = compute_nblock(noctet);
	//start_byte est la position de départ
	int pos = start_byte;
	block_t blockInter;

	int indice_buffer = 0;
	int idChar;

	//On lit tous les blocks dans le buffer
	for(int i = 0 ; i < nblock ; i++)
	{
        read_block(&blockInter, pos, 0, disk);
		idChar = 0;
		while(idChar < BLOCK_SIZE && indice_buffer < noctet)
		{
			buffer[indice_buffer] = blockInter.data[idChar];
			indice_buffer++;
			idChar++;
        }
        pos++;
    }
	//printf("indice buff = %d\n",indice_buffer);
	buffer[indice_buffer] = '\0';
	//printf("%s\n",buffer);
}

//
//
//              /* COUCHE 3 */
//
//

//Voir couche 3 de raid0.c

//
//
//              /* COUCHE 4 */
//
//

void write_file_raid1(char * fname, file_t file, virtual_disk_t * disk){
	//On regarde si le fichier n'est pas deja présent
	int inodeId = find_inode(disk -> inodes, fname);
	//printf("inodeID = %d\n",inodeId);
	//Si le fichier existe
	if(inodeId != INODE_TABLE_SIZE){
		//Si la taille est inférieure ou égale au fichier présent, on re écris ce même fichier dans le systeme
		if(inodeId != INODE_TABLE_SIZE && file.size <= disk -> inodes[inodeId].size){
			write_chunk_raid1(file.data, file.size-1, *disk, disk->inodes[inodeId].first_byte);
			disk -> inodes[inodeId].size = file.size;
		}
		//Sinon, on doit écrire le fichier à la fin du systeme et supprimer l'inode correspondante au premier fichier
		else{
			delete_inode(&(disk -> inodes), inodeId);
			int newId = get_unused_inode(disk -> inodes);
			//printf("Id new = %d\n",newId);
			inode_t newInode = init_inode(fname, file.size, disk -> super_block.first_free_byte, *disk);
			write_chunk_raid1(file.data, file.size, *disk, newInode.first_byte);
			disk->number_of_files ++;
			disk -> inodes[newId] = newInode;
		}
	}
	int newId = get_unused_inode(disk -> inodes);
			//printf("Id new = %d\n",newId);
			inode_t newInode = init_inode(fname, file.size, disk -> super_block.first_free_byte , *disk);
			write_chunk_raid1(file.data, file.size, *disk, newInode.first_byte);
			disk->number_of_files ++;
			disk -> inodes[newId] = newInode;
}

int read_file_raid1(char * fname, file_t * file, virtual_disk_t disk){
	//Recherche de l'inode dans la table
	int idInode = find_inode(disk.inodes, fname);

	//Si le fichier existe dans le système
	if(idInode != INODE_TABLE_SIZE){
		inode_t inode = disk.inodes[idInode];

		//printf("size = %d\n",inode.size);
		uchar read_buffer[inode.size];

		//On lit le fichier
		read_chunk_raid1(inode.first_byte, read_buffer, inode.size, disk);
		//On modifie l'élément de type file_t
		file -> size = inode.size;
		strcpy(file -> data, read_buffer);
		//Enfin, on retourne 1 pour montrer que tout s'est bien passé
		return 1;
	}
	//Sinon, on renvoie 0
	return 0;
}


void load_file_from_host_raid1(char *fname, virtual_disk_t *disk){
	FILE * input = fopen(fname,"r");
	uint compteur = 0;
	file_t new_file;
	uchar car;
	printf("data : %s\n",new_file.data);
	//Si le fichier sur l'host existe
	if(input != NULL){
		//On lit tous les caractères du fichier
		while(!feof(input)){
			car = (uchar)fgetc(input);
			new_file.data[compteur] = car;
			//printf("char numéro %d : %c\n",compteur, car);
			compteur++;
		}

		//la taille du fichier ne comprends pas \0, il faut donc réduire de 1
		new_file.size = compteur-1;

		printf("%s -- TAILLE : %d\n",new_file.data, new_file.size);
		//On écrit sur le systeme 
		write_file_raid1(fname, new_file, disk);
	}

	else{
		printf("Erreur, fichier non existant\n");
	}
}


void store_file_to_host_raid1(char * fname, virtual_disk_t disk){
	int inodeId = find_inode(disk.inodes, fname);
	if(inodeId == INODE_TABLE_SIZE)
		printf("Erreur: le fichier n'existe pas sur le systeme\n");
	else{
		//Lecture du fichier
		uchar buffer[disk.inodes[inodeId].size];
		read_chunk_raid1(disk.inodes[inodeId].first_byte, buffer, disk.inodes[inodeId].size, disk);
		int fd = open(fname, O_RDWR|O_CREAT|O_TRUNC, S_IRWXU);
		if(fd != -1)
			write(fd, buffer, disk.inodes[inodeId].size);
		else
			printf("Erreur lors de l'écriture\n");
	}

}