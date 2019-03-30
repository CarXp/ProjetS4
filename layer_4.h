/*************************************************************************************/
/* 								PROJET S4 : Système RAID5							 */
/*																					 */
/*								 HEADER couche 4: fichiers						     */
/*										layer_4.h                                    */
/*																					 */
/*		  Contient tous les prototypes de fonctions implémentées pour la couche 4    */
/*																					 */
/*		  Authors: DEMOUGEOT Maxence / LEMEUNIER Axel / NGUYEN-PAGES Minh Thanh      */ 
/*************************************************************************************/

/* Pour la compilation */
#ifndef LAYER_4
#define LAYER_4 

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <malloc.h>
#include "raid_defines.h"
#include "layer_1.h"
#include "layer_2.h"
#include "layer_3.h"

/******************************************************************************************/
/* fonction write_file prenant comme parametres un nom de fichier (chaıne de caracteres)  */
/* et une variable de type file_t contenant le fichier à ecrire sur le systeme RAID5      */
/* Si le fichier n'est pas présent dans la table des inodes, un inode est crée pour ce    */
/* fichier. Le fichier est écris à la suite des autres fichiers du systeme RAID5          */
/******************************************************************************************/
void write_file(char * fname, file_t file, virtual_disk_t disk);

/******************************************************************************************/
/* fonction read_file prenant en parametres un nom de fichier (chaine de caracteres) et   */
/* une variable de type file_t qui contiendra le fichier lu. Retourne 0 si fichier non    */
/* et 1 si fichier disponible															  */
/******************************************************************************************/
int read_file(char * fname,file_t * file);


/******************************************************************************************/
/* fonction delete_file prenant en parametre un nom de fichier et qui supprime l’inode    */
/* correspondant à ce fichier. Cette fonction retourne 1 en cas de suppression et 0 si le */
/* chier n’est pas présent sur le système RAID5.										  */
/******************************************************************************************/
int delete_file(char *fname, virtual_disk_t disk);


/******************************************************************************************/
/* fonction load_file from host qui prend en parametre le nom d’un fichier de             */ 
/* l’ordinateur (nommé host) et l’´ecrit sur le systeme RAID 							  */
/******************************************************************************************/
void load_file_from_host(char *fname, virtual_disk_t disk);

/******************************************************************************************/
/* fonction store_file to host qui prend en parametre le nom d’un fichier du syteme       */
/* RAID et l’ecrit sur l’ordinateur hote												  */
/******************************************************************************************/
void store_file_to_host(char * fname, virtual_disk_t disk);

#endif //LAYER_4
