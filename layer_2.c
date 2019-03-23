#include "layer_2.h"
#include "layer_1.h"
#include "raid_defines.h"

int compute_nstripe(uint n, virtual_disk_t virtual_disk)
{
        int nbdisk = virtual_disk.ndisk;
        int res = n / nbdisk;
        
        if (n % nbdisk != 0)
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
        	//printf("%x\n",parityChar);
        }

   		return parity;
}


int parity_index(int stripe_id, virtual_disk_t disk)
{
	return (disk.ndisk - (stripe_id % disk.ndisk) - 1);
}



void write_stripe(stripe_t stripe,int pos, virtual_disk_t disk)
{
	/* stripe contient deja le block de parité dans l'ordre, voir raid_defines.h */
	/* la stripe sera crée dans  write_chunk */
	for(int blockId = 0 ; blockId < stripe.nblocks ; blockId ++){
		/* blockId donne aussi le disk_id */
		write_block(stripe.stripe[blockId], pos, blockId, disk);
	}
}


void write_chunk(uchar buffer[],int noctet, virtual_disk_t disk, int startblock)
{

	// 1 - Calculer le nombre de bande (compute nstripe)

	int nblock = compute_nblock(noctet);
	//On calcule le nombre de bloc de parité qu'il faut rajouter
	nblock += nblock / (disk.ndisk - 1) + 1;
	int nstripes = compute_nstripe(nblock, disk);

	// 2 - Créer un tableau pour chaque bande, écrire dedans puis créer le block de parité

	stripe_t *tabstripe  = malloc(nstripes * sizeof(stripe_t));

	int nbdisk = disk.ndisk;
	int indicebuffer = 0;
	block_t parity;
	int index_parity;

	for (int i = 0 ; i < nstripes ; i++)	//Pour chaque bande
	{	
		int indiceblock = 0;

		tabstripe[i].nblocks = nbdisk;
		tabstripe[i].stripe = malloc(nbdisk * sizeof(block_t));

		index_parity = parity_index(i, disk);

		for(int a = 0 ; a < BLOCK_SIZE ; a++)
			tabstripe[i].stripe[index_parity].data[a] = 0;

		while(indicebuffer < noctet && indiceblock != tabstripe[i].nblocks)
		{
			if (indiceblock != index_parity) 
			{
				tabstripe[i].stripe[indiceblock].data[indicebuffer % BLOCK_SIZE] = buffer[indicebuffer];
				indicebuffer++;
				
				if (indicebuffer % BLOCK_SIZE == 0)
					indiceblock++;		
			}

			else
				indiceblock++;
		}
		

		// 3 - Mettre dans le bon odre les éléments du tableau (--> prévoir une case vide dans le tableau fait dans 2 pour y ajouter ensuite le block de parité) + compléter les fin de bandes (0 0 0 0) (à confirmer sujet)
		parity = compute_parity(tabstripe[i].stripe,tabstripe[i].nblocks);

		tabstripe[i].stripe[index_parity] = parity;

		// 4 - écrire dans le systeme avec write_stripe chaque bande 
		write_stripe(tabstripe[i], startblock, disk);
		startblock ++;
	}
}	


int read_stripe(stripe_t * stripe, int pos, virtual_disk_t disk)
{
	block_t inter;
	stripe->nblocks = disk.ndisk;

	for(int i = 0 ; i < disk.ndisk ; i++)
	{
		read_block(&inter, pos, i, disk);
		stripe->stripe[i] = inter;
	}

	return SUCCESS;
}


void read_chunk(int start_byte, uchar buffer[], int noctet, virtual_disk_t disk)
{
	int nblock = compute_nblock(noctet);
	//On calcule le nombre de bloc de parité présents
	nblock += nblock / (disk.ndisk - 1) + 1;
	int nstripes = compute_nstripe(nblock, disk);

	//start_byte est la position de départ
	int pos = start_byte;
	stripe_t stripeInter;
	stripeInter.stripe = malloc(disk.ndisk * sizeof(block_t));

	int indice_buffer = 0;
	int indParity;
	int idChar;

	//Méthode: on détermine chaque stripe (dont le block de parité)
	//Puis on lit tous les blocks dans le buffer, sauf les block de parité
	for(int i = 0 ; i < nstripes ; i++)
	{

		read_stripe(&stripeInter, pos, disk);
		indParity = parity_index(pos, disk);

		//On traite désormais chaque bloc de la bande
		for(int j = 0 ; j < stripeInter.nblocks ; j++)
		{
			idChar = 0;
			//On ne traite pas le block de parité
			if(j != indParity)
			{
				while(idChar < BLOCK_SIZE && indice_buffer < noctet)
				{
					buffer[indice_buffer] = stripeInter.stripe[j].data[idChar];
					indice_buffer++;
					idChar++;
				}
			}
		}
		pos++;
	}
}