/*************************************************************************************/
/* 								PROJET S4 : Système RAID5							 */
/*																					 */
/*								 HEADER couche 2: bandes						     */
/*										layer_2.h                                    */
/*																					 */
/*		  Contient tous les prototypes de fonctions implémentées pour la couche 2    */
/*																					 */
/*		  Authors: DEMOUGEOT Maxence / LEMEUNIER Axel / NGUYEN-PAGES Minh Thanh      */ 
/*************************************************************************************/

/* Pour la compilation */
#ifndef LAYER_2
#define LAYER_2

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <malloc.h>
#include "raid_defines.h"
#include "layer_1.h"

/******************************************************************************************/
/* fonction compute_nstripe qui calcule le nombre de bandes nécessaires pour stocker un	  */
/* nombre n de blocs.																	  */
/******************************************************************************************/
int compute_nstripe(uint n);

/******************************************************************************************/
/* fonction compute_parity qui, à partir d’un ensemble de nblocks blocs d’une bande       */
/* calcule le bloc de parité															  */
/******************************************************************************************/
block_t compute_parity(int nBlocks, block_t tabBlock[]);

/******************************************************************************************/
/* fonction compute_parity_index qui à partir d’un numéro de bande calcule le numéro du   */
/* disque sur lequel sera stocké le bloc de parité										  */
/******************************************************************************************/
int compute_parity_index(int numBande);

/******************************************************************************************/
/* fonction write_stripe qui écrit une bande de blocs et un bloc de parité, à la position */
/* pos sur le système 																	  */
/******************************************************************************************/
void write_stripe(stripe_t stripe,virtual_disk_t disk,int pos);

/******************************************************************************************/
/* fonction write_chunk qui, à partir d’un tableau de n octets buffer, l’ecrit sur le 	  */
/* systeme RAID5 a partir de la position startbyte indiquant la position dans la listes   */
/* complete des blocs de donnees sur le systeme. 										  */
/******************************************************************************************/
void write_chunk(uint startbyte, virtual_disk_t disk, uint buffer[],int nbOctets);

/******************************************************************************************/
/* fonction read_stripe qui lit une bande de blocs et un bloc de parité, à la position    */
/* pos sur le systeme et elle renvoie un code d'erreur: A PRECISER						  */
/******************************************************************************************/
int read_stripe(int pos,virtual_disk_t disk);

/******************************************************************************************/
/*  fonction read_chunk qui lit, à partir de la position start_byte, un tableau buffer de */
/*  n octets, en reconstruisant les blocks qui ne peuvent etre lus						  */				
/******************************************************************************************/
void read_chunk(int start_byte,virtual_disk_t disk);

#endif //LAYER_2