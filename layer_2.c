#include "layer_2.h"
#include "layer_1.h"

int compute_nstripe(int nblock, virtual_disk_t virtual_disk)
{
        nbdisk = virtual_disk.ndisk
        res = nblock / nbdisk;
        
        if (nblock % disk.ndisk != 0)
            return res + 1;
        
        return res;
        
}

block_t compute_parity(block_t * tabBlocks,int nblocks)
{
		/* création du bloc et du caractère intermediaire */
		block_t parity;
        uchar parityChar;

        for(int charId = 0 ; charId < BLOCK_SIZE ; charId++){
        	parityChar = 0;

        	for(int blockId = 0 ; blockId < nblocks ; blockId ++){
        		parityChar ^= tabBlocks[blockId].data[charId];
        	}

        	parity.data[charId] = parityChar;
        }

   		return parity;
}


int parity_index(int stripe_id, virtual_disk_t disk)
{
	return (disk.ndisk-(stripe_id%disk.ndisk)-1);
}



void write_stripe(stripe_t stripe, block_t parity,int pos, virtual_disk_t disk)
{
	/* stripe contient deja le block de parité dans l'ordre, voir raid_define */
	/* la stripe sera crée dans  write_chunk */
	for(int blockId = 0 ; blockId < stripe.nblocks ; blockId ++){
		/* blockId donne aussi le disk_id */
		write_block(stripe.stripe[blockId], pos, blockId, disk);
	}
}


void write_chunk(uchar buffer[],int noctet, virtual_disk_t disk){
	//IL FAUDRA:
	// 1 - Calculer le nombre de bande (compute nstripe)
	// 2 - Créer un tableau pour chaque bande, écrire dedans puis créer le block de parité
	// 3 - Mettre dans le bon odre des éléments du tableau (--> prévoir une case vide dans le tableau fait dans 2 pour y ajouter ensuite le block de parité) + compléter les fin de bandes (0 0 0 0) (à confirmer sujet)
	// 4 - écrire dans le systeme avec write_stripe chaque bande 
}

