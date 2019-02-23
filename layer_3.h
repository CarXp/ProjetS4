/*************************************************************************************/
/* 								PROJET S4 : Système RAID5							 */
/*																					 */
/*								 HEADER couche 3: inodes						     */
/*										layer_3.h                                    */
/*																					 */
/*		  Contient tous les prototypes de fonctions implémentées pour la couche 3    */
/*																					 */
/*		  Authors: DEMOUGEOT Maxence / LEMEUNIER Axel / NGUYEN-PAGES Minh Thanh      */ 
/*************************************************************************************/

/* Pour la compilation */
#ifndef LAYER_3
#define LAYER_3

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <malloc.h>
#include "raid_defines.h"
#include "layer_1.h"
#include "layer_2.h"

/******************************************************************************************/
/* fonction read inodes table permettant de charger la table d’inodes depuis RAID5        */
/******************************************************************************************/
void read_inodes_table(inode_t inode);



/******************************************************************************************/
/* fonction write_inodes_table permettant d’´ecrire la table d’inodes sur RAID5           */
/******************************************************************************************/
void write_inodes_table(inode_table_t inodes);


/******************************************************************************************/
/* fonction delete_inode qui, à partir d’un indice dans la table d’inodes supprime 		  */
/* l’inode correspondant et compacte la table de façcon à ce que, si n fichiers sont 	  */
/* stockés sur le RAID5, les n premières entrées de la table d’inodes correspondent à	  */
/* ces fichiers																			  */
/******************************************************************************************/
void delete_inode(inode_table_t inodes,int indice);


/******************************************************************************************/
/* fonction get_unused_inode qui retourne l’indice du premier inode disponible dans la    */
/* table 																				  */
/******************************************************************************************/
int get_unused_inode(inode_table_t inodes);


/******************************************************************************************/
/* fonction init_inode qui initialise un inode à partir d’un nom de fichier, de sa taille */ 
/* et de sa position sur le système RAID5												  */
/******************************************************************************************/
void init_inode(inode_t inode,char * fname,uint taille, int pos);


/******************************************************************************************/
/* fonction write_super_block qui écrit le super bloc au tout début du RAID 			  */
/******************************************************************************************/
void write_super_block(super_block_t sblock,virtual_disk_t disk);


/******************************************************************************************/
/* fonction read_super_block qui lit le super bloc au tout début du RAID                  */
/******************************************************************************************/
void read_super_block(virtual_disk_t disk);


/******************************************************************************************/
/*	fonction set_first_free_byte qui met à jour le champs first free byte du super block  */
/******************************************************************************************/
void set_first_free_byte(super_block_t sblock,uint value);

#endif //LAYER_3