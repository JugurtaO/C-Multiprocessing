

#include "../headers/table.h"
// Stocke dans table la valeur v sous la cle k
void store(PTable_entry *table, int k, char v[])
{
    PTable_entry te = (PTable_entry)malloc(sizeof(Table_entry));
    te->key = k;
    te->val = (char *)malloc(strlen(v) + 1);
    strcpy(te->val, v);
    te->next = *table;
    *table = te;
}
// Retourne la valeur se trouvant dans table sous la cle k, ou NULL si la
// cle n'existe pas
char *lookup(PTable_entry table, int k)
{
    PTable_entry pe = table;
    while (pe != NULL && pe->key != k)
    {
        pe = pe->next;
    }
    if (pe != NULL)
        return pe->val;
    else
        return NULL;
}

void closeAllFileDiscreptors(int i, int N, int **t)
{

    close(0);
    // close(1);
    switch (i)
    {
    case 0: // effet de bord
        for (int k = 0; k < N; k++)
        {
            if (k != 0)
            {
                close(t[k][1]); // je ferme tout les pipes write sauf le numéro 0
            }
            if (k != N - 1)
            {
                close(t[k][0]); // je ferme tout les pipes read sauf le dernier
            }
        }
        break;
    default: // cas général
        for (int k = 0; k < N; k++)
        {
            if (k != i)
            {
                close(t[k][1]); // je ferme tout les pipes write sauf le numéro i
            }
            if (k != i - 1)
            {
                close(t[k][0]); // je ferme tout les pipes read sauf le i-1
            }
        }
        break;
    }
}

void afficherMenu()
{
    printf("Choisissez une option \n");
    printf("1. set\n");
    printf("2. lookup\n");
    printf("3. dump\n");
    printf("0. exit\n");
}


bool isKeyManagedByNode(int i, int N, int key)
{
    return (key % N) == i;
}


bool isKeyExists(PTable_entry liste,int k){
    Table_entry * current = liste;
    bool exists=false;
    while (current!= NULL && !exists)
    {   
        if(current->key==k)
            exists=true;
        current = current->next;
    }
   
   return exists;

}
// Affiche le contenu de la table
void display(PTable_entry table)
{
    PTable_entry pe = table;
    while (pe != NULL)
    {
        printf("<> %d : %s <> \n", pe->key, pe->val);
        pe = pe->next;
    }
}
