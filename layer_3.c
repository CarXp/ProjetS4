#include "raid_defines.h"
#include "layer_3.h"
#include <string.h>

///////////////////////////////////
//FNAME/SIZE/NBLOCKS/FIRST_BYTE////
//xINODES_TABLES_SIZE//////////////
//raidtype/nb_block_used/firstbyte/
///////////////////////////////////


int endian(uchar *buf,int sz){
  int nb = 0;
  int dix_i = 1;
  for (int i=0;i<sz;i++){
    nb = nb+dix_i*buf[i];
    dix_i = dix_i*10;
  }
  return nb;
}


void write_inode_table(inode_table_t tab_inode, virtual_disk_t disk){
    //On détermine d'abord le nombre de bloc + nombre de bande pour déterminer l'espace de chaque inode dans le systeme
    int nb_blocks = compute_nblock(sizeof(inode_t));
    //Ajout des block de parité
    nb_blocks += nb_blocks/(disk.ndisk-1)+1;
    //Calcul du nombre de bande donc de l'espace utilisé
    int espace = compute_nstripe(nb_blocks, disk);


    uchar * buffer;
    inode_t inter;
    //On écrit donc chaque inode
    for(int i = 0; i< INODE_TABLE_SIZE ; i++){
        inter = tab_inode[i];
        //Cast de l'inode pour l'écriture
        buffer = (uchar *) &inter;
        write_chunk(buffer, sizeof(inode_t), disk, 1 + (i*espace));
    }
}


void read_inode_table(virtual_disk_t * disk){
    //On détermine d'abord le nombre de bloc + nombre de bande pour déterminer l'espace de chaque inode dans le systeme
    int nb_blocks = compute_nblock(sizeof(inode_t));
    //Ajout des block de parité
    nb_blocks += nb_blocks/(disk->ndisk-1)+1;
    //Calcul du nombre de bande donc de l'espace utilisé
    int espace = compute_nstripe(nb_blocks, *disk);

    inode_t * inter;
    uchar buffer[INODE_TABLE_SIZE * sizeof(inode_t)];
    for(int i = 0; i< INODE_TABLE_SIZE; i++){
        //Lecture de chaque élément et cast de l'information
        read_chunk(1 + (i*espace), buffer, sizeof(inode_t), *disk);
        inter = (inode_t *) buffer;
        disk -> inodes[i] = *inter;
    }

}



void delete_inode(inode_table_t  * inode, int indice){
    assert(indice >=0 && indice<INODE_TABLE_SIZE);
    
    //On "détruit" l'inode présente à l'indice indice
    inode[indice] -> first_byte = 0;

    //On compacte le reste de la table
    for(unsigned int i = indice ; i < INODE_TABLE_SIZE -1 ; i++){
        strcpy(inode[i] -> filename, inode[i+1] -> filename);
        inode[i] -> size = inode[i+1] -> size;
        inode[i] -> nblock = inode[i+1] -> nblock;
        inode[i] -> first_byte = inode[i+1] -> first_byte;
    }

    inode[INODE_TABLE_SIZE-1] -> first_byte = 0;
}


int get_unused_inode(inode_table_t tab_inode){
    int id = 0;

    while(id < INODE_TABLE_SIZE){
        //On regarde si l'inode à l'indice id du tableau est vide 
        if(tab_inode[id].first_byte == 0){
            return id;
        }
        id++;
    }
    //Si il n'y a pas de place disponible, on retourne la taille maximale de la table d'inode (10 dans notre cas)
    return INODE_TABLE_SIZE;
}
        

void write_super_block(virtual_disk_t disk){
    super_block_t sblock = disk.super_block;
    unsigned char * octets;
    octets = (unsigned char*) &sblock;
    write_chunk(octets, 12, disk, 101);
}




void read_super_block(virtual_disk_t * disk){
    //Lecture du superblock sur le systeme (position 0)
    //La taille d'un superblock est de 12 (3 entiers)
    uchar octets[12];
    read_chunk(101, octets, 12, *disk);
    //Une fois que la lecture est effectuée, on sépare les informations
    super_block_t * pSB = (super_block_t *)octets;
    disk -> super_block = *pSB;
}


void set_free_byte(super_block_t * sblock, uint value){
    sblock -> first_free_byte += value;
}


inode_t init_inode(char * fname, uint taille, int pos, virtual_disk_t disk){
    inode_t inode;

    strcpy(inode.filename, fname); //nom du fichier
    inode.size = taille; //taille du fichier
    inode.first_byte = pos; //start block number voir raid_define
    //Calcul du nombre de block: on compte aussi les block de parité
    int n = compute_nblock(inode.size);
    inode.nblock =  n + (n / (disk.ndisk - 1) + 1);

    return inode;
}