         
//              /* COUCHE 2*/
//
//

void write_chunk_raid0(uchar buffer[],int noctet, virtual_disk_t disk, int startblock){

	// 1 - Calculer le nombre de bande (compute nstripe)

	int nblock = compute_nblock(noctet);
	//printf("nbblock = %d\n",nblock);
	int nstripes = compute_nstripe(nblock,disk);
	// printf("Il faut %d blocks : %d stripes pour %d octets à écrire\n",nblock,nstripes,noctet);

	// 2 - Créer un tableau pour chaque bande, écrire dedans puis créer le block de parité

	stripe_t *tabstripe  = malloc(nstripes * sizeof(stripe_t));

	int nbdisk = disk.ndisk;
	int indicebuffer = 0;

	for (int i = 0 ; i < nstripes ; i++)
	{	
		tabstripe[i].nblocks = nbdisk;
		tabstripe[i].stripe = malloc(nbdisk * sizeof(block_t));


		for (int j = 0 ; j < nbdisk ; j++)	//Pour chaque bloc d'une bande
		{
				int indiceblock=0;
				while(indicebuffer < noctet && indiceblock != tabstripe[i].nblocks)	//Tant qu'on est pas à la fin du buffer et qu'on a pas dépasser la limite du bloc
				{
					tabstripe[i].stripe[j].data[indiceblock] = buffer[indicebuffer];
					indicebuffer++;
					indiceblock++;
				}

		}
		// for(int k =0; k < 4; k++)
		// 	printf("%s\n",tabstripe[i].stripe[k].data);

		// 3 - écrire dans le systeme avec write_stripe chaque bande 

		write_stripe_raid0(tabstripe[i], startblock, disk);
		startblock ++;

	}
}


void read_chunk_raid0(int start_byte, uchar buffer[], int noctet, virtual_disk_t disk)
{
	int nblock = compute_nblock(noctet);

	//printf("Entrée dans ");
	int nstripes = compute_nstripe(nblock, disk);
	//start_byte est la position de départ
	int pos = start_byte;
	stripe_t stripeInter;
	stripeInter.stripe = malloc(disk.ndisk * sizeof(block_t));

	int indice_buffer = 0;
	int idChar;

	//Méthode: on détermine chaque stripe
	//Puis on lit tous les blocks dans le buffer
	for(int i = 0 ; i < nstripes ; i++)
	{
		read_stripe(&stripeInter, pos, disk);
		//On traite désormais chaque bloc de la bande
		for(int j = 0 ; j < stripeInter.nblocks ; j++)
		{
			idChar = 0;
			while(idChar < BLOCK_SIZE && indice_buffer < noctet)
			{
				//printf("idbuf = %d - char = %c\n",indice_buffer, stripeInter.stripe[j].data[idChar]);
				buffer[indice_buffer] = stripeInter.stripe[j].data[idChar];
				indice_buffer++;
				idChar++;
			}
		}
		pos++;
	}

	//printf("indice buff = %d\n",indice_buffer);
	buffer[indice_buffer] = '\0';
	//printf("%s\n",buffer);
}

//
//
//          /* COUCHE 3 */
//
//

void write_inode_table_raid0(inode_table_t tab_inode, virtual_disk_t disk){
    //On détermine d'abord le nombre de bloc + nombre de bande pour déterminer l'espace de chaque inode dans le systeme
    int nb_blocks = compute_nblock(sizeof(inode_t));

    //Calcul du nombre de bande donc de l'espace utilisé
    int espace = compute_nstripe(nb_blocks, disk);
    uchar * buffer;
    inode_t inter;
    //On écrit donc chaque inode
    for(int i = 0; i< INODE_TABLE_SIZE ; i++){
        inter = tab_inode[i];
        //Cast de l'inode pour l'écriture
        buffer = (uchar *) &inter;
        write_chunk_raid0(buffer, sizeof(inode_t), disk, 1 + (i*espace));
    }
}


void read_inode_table(virtual_disk_t * disk){
    //On détermine d'abord le nombre de bloc + nombre de bande pour déterminer l'espace de chaque inode dans le systeme
    int nb_blocks = compute_nblock(sizeof(inode_t));

    //Calcul du nombre de bande donc de l'espace utilisé
    int espace = compute_nstripe(nb_blocks, *disk);

    inode_t * inter;
    uchar buffer[INODE_TABLE_SIZE * sizeof(inode_t)];

    for(int i = 0; i< INODE_TABLE_SIZE; i++){
        //Lecture de chaque élément et cast de l'information
        read_chunk_raid0(1 + (i*espace), buffer, sizeof(inode_t), *disk);
        inter = (inode_t *) buffer;
        disk -> inodes[i] = *inter;
    }
}

void write_super_block(virtual_disk_t disk){
    super_block_t sblock = disk.super_block;
    uchar * octets;
    octets = (uchar*) &sblock;
    write_chunk_raid0(octets, 12, disk, 0);
}




void read_super_block(virtual_disk_t * disk){
    //Lecture du superblock sur le systeme (position 0)
    //La taille d'un superblock est de 12 (3 entiers)
    uchar octets[12];
    read_chunk_raid0(0, octets, 12, *disk);
    //Une fois que la lecture est effectuée, on sépare les informations
    super_block_t * pSB = (super_block_t *)octets;
    disk -> super_block = *pSB;
}


inode_t init_inode_raid0(char * fname, uint taille, int pos, virtual_disk_t disk){
    inode_t inode;

    strcpy(inode.filename, fname); //nom du fichier
    inode.size = taille; //taille du fichier
    inode.first_byte = pos; //start block number voir raid_define
    inode.nblock = compute_nblock(inode.size);
    return inode;
}

//
//
//          /* COUCHE 4 */
//
//

void write_file_raid0(char * fname, file_t file, virtual_disk_t * disk){
	//On regarde si le fichier n'est pas deja présent
	int inodeId = find_inode(disk -> inodes, fname);
	//printf("inodeID = %d\n",inodeId);
	//Si le fichier existe
	if(inodeId != INODE_TABLE_SIZE){
		//Si la taille est inférieure ou égale au fichier présent, on re écris ce même fichier dans le systeme
		if(inodeId != INODE_TABLE_SIZE && file.size <= disk -> inodes[inodeId].size){
			write_chunk_raid0(file.data, file.size-1, *disk, disk->inodes[inodeId].first_byte);
			disk -> inodes[inodeId].size = file.size;
		}
		//Sinon, on doit écrire le fichier à la fin du systeme et supprimer l'inode correspondante au premier fichier
		else{
			delete_inode(&(disk -> inodes), inodeId);
			int newId = get_unused_inode(disk -> inodes);
			//printf("Id new = %d\n",newId);
			inode_t newInode = init_inode_raid0(fname, file.size, disk -> super_block.first_free_byte, *disk);
			write_chunk_raid0(file.data, file.size, *disk, newInode.first_byte);
			disk->number_of_files ++;
			disk -> inodes[newId] = newInode;
		}
	}
	int newId = get_unused_inode(disk -> inodes);
			//printf("Id new = %d\n",newId);
			inode_t newInode = init_inode_raid0(fname, file.size, disk -> super_block.first_free_byte , *disk);
			write_chunk_raid0(file.data, file.size, *disk, newInode.first_byte);
			disk->number_of_files ++;
			disk -> inodes[newId] = newInode;
}


int read_file_raid0(char * fname, file_t * file, virtual_disk_t disk){
	//Recherche de l'inode dans la table
	int idInode = find_inode(disk.inodes, fname);

	//Si le fichier existe dans le système
	if(idInode != INODE_TABLE_SIZE){
		inode_t inode = disk.inodes[idInode];

		//printf("size = %d\n",inode.size);
		uchar read_buffer[inode.size];

		//On lit le fichier
		read_chunk_raid0(inode.first_byte, read_buffer, inode.size, disk);
		//On modifie l'élément de type file_t
		file -> size = inode.size;
		strcpy(file -> data, read_buffer);
		//Enfin, on retourne 1 pour montrer que tout s'est bien passé
		return 1;
	}
	//Sinon, on renvoie 0
	return 0;
}


void load_file_from_host_raid0(char *fname, virtual_disk_t *disk){
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
		write_file_raid0(fname, new_file, disk);
	}

	else{
		printf("Erreur, fichier non existant\n");
	}
}


void store_file_to_host(char * fname, virtual_disk_t disk){
	int inodeId = find_inode(disk.inodes, fname);
	if(inodeId == INODE_TABLE_SIZE)
		printf("Erreur: le fichier n'existe pas sur le systeme\n");
	else{
		//Lecture du fichier
		uchar buffer[disk.inodes[inodeId].size];
		read_chunk_raid0(disk.inodes[inodeId].first_byte, buffer, disk.inodes[inodeId].size, disk);
		int fd = open(fname, O_RDWR|O_CREAT|O_TRUNC, S_IRWXU);
		if(fd != -1)
			write(fd, buffer, disk.inodes[inodeId].size);
		else
			printf("Erreur lors de l'écriture\n");
	}

}