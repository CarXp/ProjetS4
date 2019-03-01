#include "raid_defines.h"
#include "layer_1.h"
#include "layer_2.h"
#include <string.h>

int main(void){
	printf("--Démarrage de l'application test cmd_test1 ...\n");
	virtual_disk_t disk;
	init_disk_raid5("../RAID5/",&disk);
	printf("--Démarrage terminé ! \n");

	
	printf("Initialisation buffer...\n");
	uchar buffer[257];
	for(uint i=0; i < 256 ; i++ )
		buffer[i]=i;
	printf("Initialisation buffer terminée\n");
	
	printf("%x\n",buffer[5]);

	printf("\n--Écriture du buffer\n");
	write_chunk(buffer,256,disk,0);
	printf("--Ecriture terminée\n");

	printf("\n--Test lecture\n");
	uchar bufferF[257];
	read_chunk(0,bufferF,256,disk);

	for(uint i=0; i < 256 ; i++ )
		printf("Octet %d = %x \n",i,bufferF[i]);

	printf("--Fin test lecture\n");
	
	fermeture_systeme_raid5(disk);
	return 0;
}