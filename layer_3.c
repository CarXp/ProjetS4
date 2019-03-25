#include "raid_defines.h"
#include "layer_3.h"
#include <string.h>

///////////////////////////////////
//FNAME/SIZE/NBLOCKS/FIRST_BYTE////
//xINODES_TABLES_SIZE//////////////
//raidtype/nb_block_used/firstbyte/
///////////////////////////////////
/*
inode_table_t read_inodes_table(virtual_disk_t disk)
{
    inote_table_t inodes[INODES_TABLE_SIZE];
    uchar nameInter[MAX_FILE_NAME_SIZE];
    uchar intInter[4];
    inode_t inoteInter;

    for(int i = 1; i <= INODES_TABLE_SIZE; i ++){
    	/* lecture du nom */
    	//read_chunk(0, nameInter, MAX_FILE_NAME_SIZE, disk);
    	//strcpy(nameInter, inoteInter.filename);

    	/* lecture de size */
    	//read_chunk(i * MAX_FILE_NAME_SIZE, intInter, MAX_FILE_SIZE, disk);
    	//inoteInter.size = atoi(intInter);

    	/* lecture de nblocks */
    	//compute_nblock(inodeInter.size);
    	//inodeInter.nblocks = compute_nblock(inodeInter.size);
    	//inodeInter.nblocks += inodeInter.nblocks/(disk.ndisk-1)+1;

    	/* lecture first_byte */
    	//read_chunk(i * ())
    //}
//}




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
    uchar buffer[BLOCK_SIZE * (SUPER_BLOCK_SIZE-1)];
    uchar inter[BLOCK_SIZE];
    uchar inter2[BLOCK_SIZE];

    uchar salutcv = (uchar) sblock.raid_type;

    write_chunk(&salutcv, 1, disk, 0);


}




void read_super_block(virtual_disk_t * disk){
    //Création du super_block
    super_block_t sblock;
    int param = 0;
    uchar tab_inter[BLOCK_SIZE];

    //Lecture des informations sur le systeme
    uchar tab_lecture[BLOCK_SIZE * SUPER_BLOCK_SIZE];
    read_chunk(0, tab_lecture, BLOCK_SIZE * (SUPER_BLOCK_SIZE-1), *disk);
    for(int i = 0; i < BLOCK_SIZE * (SUPER_BLOCK_SIZE-1); i++) printf("%x ", tab_lecture[i]);
    printf("\n");

    //La lecture a été effectuée, il faut maintenant séparer les informations
    for(unsigned int j = 0; j < BLOCK_SIZE * (SUPER_BLOCK_SIZE-1); j += BLOCK_SIZE){
        int fin = j + BLOCK_SIZE;
        int id_tab_inter = 0;

        switch(param){
            //Case 0: on traite le RAIDTYPE 
            case 0: for(int a = j; a < fin ; a++){
                        tab_inter[id_tab_inter] = tab_lecture[a];
                        id_tab_inter++;
                    }
                    sblock.raid_type = atoi(tab_inter);
                    break;

            //Case 1: on traite le nombre de block utilisés
            case 1: for(int a = j; a < fin ; a++){
                        tab_inter[id_tab_inter] = tab_lecture[a];
                        id_tab_inter++;
                    } 
                    sblock.nb_blocks_used = atoi(tab_inter);
                    break;

            //Case 2: on traite le premier octet disponible
            case 2: for(int a = j; a < fin ; a++) {
                        tab_inter[id_tab_inter++] = tab_lecture[a];
                        id_tab_inter ++;
                    }
                    sblock.first_free_byte = atoi(tab_inter);
                    break;
        }

        param++;
    }

    disk -> super_block = sblock;
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