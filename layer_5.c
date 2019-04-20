#include <stdio.h>
#include <string.h>
#define SUCCESS 0 
#define FAIL 1


/* ETAPE 1 */
// Lecture de la commande : la fonction retourne un entier correspondant à une commande:
// ls [-l] : 1
// cat [filename] : 2
// rm [filename] : 3
// create [filename] : 4
// edit [filename] : 5
// load [filename] : 6
// store [filename] : 7
// quit : 0
int read_command(char * cmd){
	if(strcmp(cmd, "ls") == 0)
		return 1;
	else if(strcmp(cmd, "cat") == 0)
		return 2;
	else if(strcmp(cmd, "rm") == 0)
		return 3;
	else if(strcmp(cmd, "create") == 0)
		return 4;
	else if(strcmp(cmd, "edit") == 0)
		return 5;
	else if(strcmp(cmd, "load") == 0)
		return 6;
	else if(strcmp(cmd, "store") == 0)
		return 7;
	else if(strcmp(cmd, "quit") == 0)
		return 0;
	//Commande invalide
	return -1;
}


/* DERNIERE ETAPE */
// Execution de la commande en fonction de: l'entier récupéré correspondant à la commande et le nom du fichier si besoin
void execute_command(int cmd_value, char * filename){
	switch(cmd_value){
		case 1: if(filename == NULL)
					//A remplacer par la fonction correspondante
					printf("exec commande ls\n");
				else if(strcmp(filename, "-l") == 0)
					//A remplacer par la fonction correspondante
					printf("exec commande ls -l\n");
				else
					//A remplacer par la fonction correspondante
					printf("exec commande ls\n");
				break;

		case 2: if(filename != NULL)
					//A remplacer par la fonction correspondante
					printf("exec commande cat %s\n",filename);
				break;

		case 3: if(filename != NULL)
					//A remplacer par la fonction correspondante
					printf("exec commande rm %s\n", filename);
				break;

		case 4: if(filename != NULL)
					//A remplacer par la fonction correspondante
					printf("exec commande create %s\n", filename);
				break;

		case 5: if(filename != NULL)
					//A remplacer par la fonction correspondante
					printf("exec commande edit %s\n", filename);
				break;

		case 6: if(filename != NULL)
					//A remplacer par la fonction correspondante
					printf("exec commande load %s\n", filename);
				break;

		case 7: if(filename != NULL)
					//A remplacer par la fonction correspondante
					printf("exec commande store %s\n",filename);
				break;

		case 0: break;

		default: printf("Commande invalide\n");
	}
}



int main(void){
	//Démarrage systeme raid5
	//Creation de la var globale disk
	//Vérification existence + validité des fichiers de stockage
	//Lecture et init du superblock
	//Lecture et init de la table d'inode

	char buffer[255];
	int lecture = 1;
	while(lecture){
		printf("Please type a commmand\n");
		fgets(buffer, 255, stdin);
		char * cmd = strtok(buffer, " \n");
		char * fname = strtok(NULL, " \n");
		lecture = read_command(cmd);
		execute_command(lecture, fname);
	}

	//Sauvegarde des edit si besoin
	//Verification existence + validité des fichiers de stockage
	//Ecriture des dernieres màj du super block et table d'inode
	//Fermeture des fichiers
	return SUCCESS;
}