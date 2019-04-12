#include "raid_defines.h"
#include "layer_1.h"
#include "layer_2.h"
#include "layer_3.h"
#include "layer_4.h"
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>


//La fonction find_inode renvoie l'indice de l'inode dans la table d'inode qui correspond au fichier demandé
int find_inode(inode_table_t tab_i, char * filename){
	for(int i = 0; i < INODE_TABLE_SIZE ; i++){
		if(strcmp(tab_i[i].filename, filename) == 0)
			return i;
	}
	return INODE_TABLE_SIZE;
}


void write_file(char * fname, file_t file, virtual_disk_t * disk){
	//On regarde si le fichier n'est pas deja présent
	int inodeId = find_inode(disk -> inodes, fname);
	//Si le fichier existe
	if(inodeId != INODE_TABLE_SIZE){
		//Si la taille est inférieure ou égale au fichier présent, on re écris ce même fichier dans le systeme
		if(file.size <= disk -> inodes[inodeId]){
			write_chunk(file.data, file.size, *disk, disk->inodes[inodeId].first_byte);
			disk -> inodes[inodeId] -> size = file.size;
		}
		//Sinon, on doit écrire le fichier à la fin du systeme et supprimer l'inode correspondante au premier fichier
		else{
			delete_inode(disk -> inodes, inodeId);
			int newId = get_unused_inode(disk -> inodes);
			inode_t newInode = init_inode(fname, file.size, disk -> superblock -> first_free_byte +1, *disk);
			write_chunk(file.data, file.size, *disk, newInode.first_byte);
			disk->number_of_file ++;
		}
	}
}



int read_file(char * fname, file_t * file, virtual_disk_t disk){
	//Recherche de l'inode dans la table
	int idInode = find_inode(disk.inodes, fname);

	//Si le fichier existe dans le système
	if(idInode != INODE_TABLE_SIZE){
		inode_t inode = disk.inodes[idInode];
		char * read_buffer[inode.size];
		//On lit le fichier
		read_chunk(inode.first_byte, read_buffer, inode.size, disk);
		//On modifie l'élément de type file_t
		file -> size = inode.size;
		strcpy(file -> data, read_buffer);
		//Enfin, on retourne 1 pour montrer que tout s'est bien passé
		return 1;
	}
	//Sinon, on renvoie 0
	return 0;
}


int delete_file(char * fname, virtual_disk_t disk){
	//Recherche de l'inode dans la table
	int idInode = find_inode(disk.inodes, fname);

	//Si le fichier existe dans le système
	if(idInode != INODE_TABLE_SIZE){
		delete_inode(disk.inodes, idInode);
		//Tout s'est bien passé, on retourne 1
		return 1;
	}
	//Sinon, on retourne 0
	return 0;
}



void load_file_from_host(char *fname, virtual_disk_t disk){
	FILE * input = fopen(fname,"r");
	uint size = 0;
	file_t new_file;
	uchar car;

	//Si le fichier sur l'host existe
	if(input != NULL){
		//On lit tous les caractères du fichier
		while(!feof(fname)){
			fscanf(input, "%c", &car);
			new_file.data = car;
		}

		//On écrit sur le systeme 
		write_file(fname, new_file, disk);
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
		read_chunk(disk.inodes[inodeId].first_byte, buffer, disk.inodes[inodeId].size, disk);
		int fd = open(fname, O_RDWR|O_CREAT|O_TRUNC, S_IRWXU);
		if(fd != -1)
			write(fd, buffer, disk.inodes[inodeId].size);
		else
			printf("Erreur lors de l'écriture\n");
	}

}