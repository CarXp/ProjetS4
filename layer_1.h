/*************************************************************************************/
/* 								PROJET S4 : Système RAID5							 */
/*																					 */
/*								 HEADER couche 1: blocs								 */
/*										layer_1.h                                    */
/*																					 */
/*		  Contient tous les prototypes de fonctions implémentées pour la couche 1    */
/*																					 */
/*		  Authors: DEMOUGEOT Maxence / LEMEUNIER Axel / NGUYEN-PAGES Minh Thanh      */ 
/*************************************************************************************/

/* Pour la compilation */
#ifndef LAYER_1
#define LAYER_1

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <malloc.h>
#include "raid_defines.h"

/******************************************************************************************/
/* fonction init_disk_raid5 qui, à partir du nom du répertoire, initialise cette variable */
/* et demarre le système raid5                                                            */
/******************************************************************************************/
void init_disk_raid5(char * dirname, virtual_disk_t * disk);

/******************************************************************************************/
/* fonction fermeture_systeme_raid5 qui ferme tous les fichiers (disques) et fait ensuite */
/* une vérification qu'il n'y ait pas eu de problème lors de l'execution du système       */
/******************************************************************************************/
void fermeture_systeme_raid5(virtual_disk_t disk);

/******************************************************************************************/
/* fonction compute_nblock qui calcule le nombre de blocs nécessaires pour stocker un    */
/* nombre n d’octets.																	  */
/******************************************************************************************/
int compute_nblock(uint n);

/******************************************************************************************/
/* fonction write_block qui écrit un bloc block, à la position pos sur le disque disk_id  */
/* du système 																			  */
/******************************************************************************************/
void write_block(block_t block,int pos,int disk_id, virtual_disk_t disk);

/******************************************************************************************/
/* fonction read_block qui lit un bloc block de données,à la position pos sur le disque   */
/* disk_id du système 																	  */
/******************************************************************************************/
int read_block(block_t * block,int pos, int disk_id,virtual_disk_t virtual_disk);

/******************************************************************************************/
/*  fonction block_repair qui, en cas d’erreur de lecture, reconstruit le bloc erroné 	  */
/*  et qui utilise le bloc de parité													  */
/******************************************************************************************/
void block_repair(block_t *block,int pos,int disk_id, virtual_disk_t disk);

/******************************************************************************************/
/* fonction affichage_block qui, depuis un bloc block, affiche en hexadecimal le contenu  */
/******************************************************************************************/
void affichage_block(block_t block);

#endif //LAYER_1