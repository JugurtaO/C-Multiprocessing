#ifndef TABLE_H
#define TABLE_H
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <string.h>

typedef struct _Table_entry
{
    int key;                   // cle
    char *val;                 // valeur
    struct _Table_entry *next; // suivant
} Table_entry;

typedef Table_entry *PTable_entry;

// recherche la valeur  dans la liste table associée à  la clé k
char *lookup(PTable_entry table, int k);

// enregistrer la valeur v dans la liste table sous la clé k
void store(PTable_entry *table, int k, char v[]);

// fermeture de tous  les descripteurs de fichiers non nécessaires  pour le processus i.
//@author: JUGURTA.O
void closeAllFileDiscreptors(int i, int N, int **t);

// return true si le noeud i gère la clé key , false sinon
// N ici représente le nombre de noeuds et sert comme donnée de calcule
//@author: JUGURTA.O
bool isKeyManagedByNode(int i, int N,int key);

//renvoie true si la clé k existe déjà dans la liste chaînée  liste,false sinon
//@author: JUGURTA.O
bool isKeyExists(PTable_entry liste,int k);





//Construit la chaîne de caractères correspondant à toutes les valeurs de la liste chaînée liste en paramètre
//@author: JUGURTA.O
// char *buildString(PTable_entry liste);

// affiche le menu de commandes pour l'utilisateur
//@author: JUGURTA.O
void afficherMenu();

// affiche le contenu sauvegardé par les processus
//@author: JUGURTA.O
void display(PTable_entry table);

#endif
