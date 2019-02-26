#include "layer_2.h"
#include "layer_1.h"
#include "raid_defines.h"

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



void write_stripe(stripe_t stripe, int pos, virtual_disk_t disk)
{
	/* stripe contient deja le block de parité dans l'ordre, voir raid_define */
	/* la stripe sera crée dans  write_chunk */
	for(int blockId = 0 ; blockId < stripe.nblocks ; blockId ++){
		/* blockId donne aussi le disk_id */
		write_block(stripe.stripe[blockId], pos, blockId, disk);
	}
}


void write_chunk(uchar buffer[],int noctet, virtual_disk_t disk){

	// 1 - Calculer le nombre de bande (compute nstripe)

	int nblock = compute_nblock(noctet);
	int nstripes = compute_nstripe(nblock);

	// 2 - Créer un tableau pour chaque bande, écrire dedans puis créer le block de parité

	stripe_t *tabstripe  = malloc(nstripes * sizeof(stripe_t));

	int nbdisk = disk.ndisk;

	int indicebuffer = 0;

	block_t parity;

	int index_parity;

	for (int i = 0 ; i < nstripes ; i++)
	{
		tabstripe[i].nblocks = nbdisk;
		tabstripe[i].stripe = malloc(nbdisk * sizeof(block_t));

		index_parity = parity_index(i, disk);

		
		for (int j = 0 ; j < nbdisk ; j++)
		{
			if(indiceblock != index_parity)
			{

				int indiceblock = 0;

				while(indicebuffer < noctet && indiceblock != )
				{
					tabstripe[i].stripe[j].data[indiceblock] = buffer[indicebuffer];
					indicebuffer++;
					indiceblock++;
				}

			}
			
		}

		parity = compute_parity(nbdisk, tabstripe[i].stripe);

		tabstripe[i].stripe[index_parity] = parity;
		
	}

	// 3 - Mettre dans le bon odre les éléments du tableau (--> prévoir une case vide dans le tableau fait dans 2 pour y ajouter ensuite le block de parité) + compléter les fin de bandes (0 0 0 0) (à confirmer sujet)
	// 4 - écrire dans le systeme avec write_stripe chaque bande 
}

