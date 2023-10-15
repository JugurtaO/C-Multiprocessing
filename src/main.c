#include "../headers/table.h"

int main(int argc, char **argv)
{
    // vérification du nombre d'arguments
    if (argc != 2)
    {
        fprintf(stderr, "Usage: ./main N\n");
        exit(1);
    }
    // Nombre de Noeuds
    int N = atoi(argv[1]);
    while (N < 2)
    {
        printf("Entrer un entier N>=2 :\n");
        scanf("%d", &N);
    }
    // tableau 2D pour stocker les pipes entre processus
    int **t = (int **)malloc(N * sizeof(int *));
    // tableau 1D des listes des processus
    PTable_entry *listes = (PTable_entry *)malloc(N * sizeof(PTable_entry));

    // création des N pipes nécessaires
    for (int i = 0; i < N; i++)
    {
        t[i] = (int *)malloc(2 * sizeof(int));

        if (pipe(t[i]) != 0) // création du pipe
        {
            perror("IèmePipe:");
            exit(1);
        }
    }

    // Initialisation de tableau de listes
    for (int i = 0; i < N; i++)
    {
        listes[i] = NULL;
    }

    // création du pipe commun entre le contrôleur et les N processus
    int pipeCtrl[2];
    if (pipe(pipeCtrl) != 0)
    {
        perror("pipeCtrl");
        exit(1);
    }

    printf("I'm the controller\n");

    // création des N processus et traîtement de la commande correspondante
    for (int i = 0; i < N; i++)
    {

        switch (fork())
        {
        case -1:
            perror("fork1 : ");
            exit(1);

        case 0:
            printf("I'm the child process n°:%d; pid=%d\n", i, getpid());
            // fermeture de tous les descripteurs de fichiers non nécessaire au fonctionnement du processus i courant.
            close(pipeCtrl[0]);
            close(pipeCtrl[1]);
            closeAllFileDiscreptors(i, N, t);
            int commande;
            int key;
            char valeur[512];
            int lenValeur;

            if (i == 0)
            {

                while (true) // mettre les processus en mode read : attente d'une donnée
                {

                    if (read(t[N - 1][0], &commande, sizeof(int)) > 0)
                    {
                        switch (commande)
                        {
                        case 0: // exit

                            // code de exit
                            close(t[N - 1][0]);
                            exit(0);

                        case 1: //  store
                            read(t[N - 1][0], &key, sizeof(int));

                            // lecture de la taille de valeur
                            read(t[N - 1][0], &lenValeur, sizeof(int));

                            // lecture de la valeur
                            read(t[N - 1][0], valeur, lenValeur * sizeof(char));

                            // si la clé est gérée par le Noeud i alors on sauvegarde la valeur valeur sous cette clé
                            if (isKeyManagedByNode(i, N, key))
                            {

                                // si la clé key n'exite pas on la suavegarde ,sinon on fait rien
                                if (!isKeyExists(listes[i], key))
                                    store(&listes[i], key, valeur);
                            }
                            else
                            { // sinon on la propage au noeud suivant
                                write(t[0][1], &commande, sizeof(int));
                                write(t[0][1], &key, sizeof(int));
                                write(t[0][1], &lenValeur, sizeof(int));
                                write(t[0][1], valeur, lenValeur * sizeof(char));
                            }
                            close(t[N - 1][0]);

                            // close(t[i][1]);
                            lenValeur = 0;
                            break;

                        case 2: // lookup

                            // lecture de la clé key
                            read(t[N - 1][0], &key, sizeof(int));

                            // si la clé key est gérée par le noeud i alors on lance la recherche
                            if (isKeyManagedByNode(i, N, key))
                            {
                                strcpy(valeur, lookup(listes[i], key));
                                if (valeur != NULL) // une valeur a été retrouvée
                                {
                                    lenValeur = strlen(valeur);

                                    // envoi de la longeur de la valeur trouvée
                                    write(pipeCtrl[1], &lenValeur, sizeof(int));
                                    // envoi de la valeur trouvée
                                    write(pipeCtrl[1], valeur, lenValeur * sizeof(char));
                                }
                                else // aucune  valeur n'a été trouvée
                                {
                                    // envoi de 0 comme référence à NULL
                                    int zero = 0;
                                    write(pipeCtrl[1], &zero, sizeof(int));
                                }
                            }
                            else // sinon on la propage dans l'anneau
                            {

                                // envoie de la commande et de la clé au processus suivant
                                write(t[i][1], &commande, sizeof(int));
                                write(t[i][1], &key, sizeof(int));
                            }

                            close(t[N - 1][0]);
                            
                            lenValeur = 0;
                            break;
                        case 3: // display

                            // code de display

                            break;

                        default:
                        }
                    }
                    else
                    {
                        perror("read");
                        exit(-1);
                    }
                }
            }
            else
            {

                while (true)
                {
                    if (read(t[i - 1][0], &commande, sizeof(int)) > 0)
                    {
                        switch (commande)
                        {
                        case 0: // exit

                            // code de exit
                            // code de exit
                            close(t[i - 1][0]);

                            exit(0);

                        case 1: // store
                            // lecture de la clé key
                            read(t[i - 1][0], &key, sizeof(int));

                            // lecture de la taille de valeur
                            read(t[i - 1][0], &lenValeur, sizeof(int));

                            // lecture de la valeur
                            read(t[i - 1][0], valeur, lenValeur * sizeof(char));

                            // si la clé est gérée par le Noeud i alors on sauvegarde la valeur valeur sous cette clé
                            if (isKeyManagedByNode(i, N, key))
                            {
                                // si la clé key n'exite pas on la suavegarde ,sinon on fait rien
                                if (!isKeyExists(listes[i], key))
                                    store(&listes[i], key, valeur);
                            }
                            else
                            { // sinon on la propage dans l'anneau en passant au noeud suivant
                                write(t[i][1], &commande, sizeof(int));
                                write(t[i][1], &key, sizeof(int));
                                write(t[i][1], &lenValeur, sizeof(int));
                                write(t[i][1], valeur, lenValeur * sizeof(char));
                            }

                            lenValeur = 0;
                            break;
                        case 2: // lookup

                            // lecture de la clé key
                            read(t[i - 1][0], &key, sizeof(int));

                            // si la clé key est gérée par le noeud i alors on lance la recherche
                            if (isKeyManagedByNode(i, N, key))
                            {

                                char *bb = lookup(listes[i], key);
                                if (bb == NULL)
                                    lenValeur = 0;
                                else
                                {
                                    strcpy(valeur, bb);
                                    printf(">>>>>>>>>>>>>>>>>>%s\n",valeur);
                                    lenValeur = strlen(valeur);
                                }
                                if (lenValeur != 0) // une valeur a été retrouvée
                                {

                                    // envoi de la longeur de la valeur trouvée
                                    write(pipeCtrl[1], &lenValeur, sizeof(int));
                                    // envoi de la valeur trouvée
                                    write(pipeCtrl[1], valeur, lenValeur * sizeof(char));
                                }
                                else // aucune  valeur n'a été trouvée
                                {
                                    // envoi de 0 comme référence à NULL
                                    int zero = 0;
                                    write(pipeCtrl[1], &zero, sizeof(int));
                                }
                            }
                            else // sinon on la propage dans l'anneau
                            {

                                // envoie de la commande et de la clé au processus suivant
                                write(t[i][1], &commande, sizeof(int));
                                write(t[i][1], &key, sizeof(int));
                            }

                            lenValeur = 0;
                            break;
                        case 3: // display

                            // code de display

                            break;

                        default:
                        }
                    }
                    else
                    {
                        perror("read");
                        exit(-1);
                    }
                }
            }
        default:
            // sleep(2);
        }
    }

    // FIN DE LA CRÉATION DES N PROCESSUS
    sleep(1);

    // Ici on ferme les descripteurs dont le contrôleur n'a pas besoin
    close(pipeCtrl[0]);
    for (int k = 0; k < N; k++)
    {
        close(t[k][0]);
    }

    // ### PROGRAMME D'INTÉRACTION AVEC L'UTILISATEUR ###
    int commande;
    int key;
    char valeur[512];
    int lenValeur;

    // un flag booléen pour voir si l'utilisateur souhaite continuer ou pas
    bool continuer = true;

    while (continuer)
    {
        // affichage du Menu utilisateur
        afficherMenu();
        printf("Option > : ");
        scanf("%d", &commande);
        printf("\n");
        // vérification de la validité de la commande
        while (commande < 0 || commande > 3)
        {
            printf("option non valide! saisissez une autre :");
            scanf("%d", &commande);
            printf("\n");
        }

        // traîtement de la commande
        switch (commande)
        {
        case 0: // EXIT

            // envoyer l'ordre de exit 0 à tous les processus et faire un exit(0)
            for (int i = 0; i < N; i++)
            {
                write(t[i][1], &commande, sizeof(int));
                close(t[i][1]);
            }

            // attendre la mort de tous les processus fils
            while (wait(NULL) != -1)
            {
                printf("mort\n");
            };
            continuer = false;
            break;
        case 1: // STORE
            
            // Récupération de la clé de la valeur
            printf("Saisir la clé :");
            scanf("%d", &key);
            printf("\n");
            while (key < 0)
            {
                printf("clé erronée ! saisissez une autre :");
                scanf("%d", &key);
                printf("\n");
            }

            // Récupération de la valeur
            printf("Saisir la valeur : ");
            scanf("%s", valeur);
            printf("\n");

            lenValeur = strlen(valeur);

            // envoie de la commande store au processus 0
            write(t[N - 1][1], &commande, sizeof(int));

            // envoie de la clé
            write(t[N - 1][1], &key, sizeof(int));
            // envoi de la taille de valeur lue
            write(t[N - 1][1], &lenValeur, sizeof(int));
            // envoi de la valeur
            write(t[N - 1][1], valeur, lenValeur * sizeof(char));

            break;

        case 2: // LOOKUP
            
            // Récupération de la clé de la valeur
            printf("Saisir la clé à rechercher:");
            scanf("%d", &key);
            printf("\n");
            while (key < 0)
            {
                printf("clé erronée ! saisissez une autre :");
                scanf("%d", &key);
                printf("\n");
            }

            // envoie de la commande store au processus 0
            write(t[N - 1][1], &commande, sizeof(int));

            // envoie de la clé
            write(t[N - 1][1], &key, sizeof(int));

            // attendre la réponse du noeud effectif puis afficher le résultat ########
            sleep(2);

            // lecture de la longueur de la valeur recherchée
            read(pipeCtrl[0], &lenValeur, sizeof(int));

            if (lenValeur != 0)
            {
                read(pipeCtrl[0], valeur, lenValeur * sizeof(char));
                printf("Valeur trouvée : %s\n", valeur);
            }
            else
            {
                printf("Aucune valeur n'a été trouvée ! \n");
            }

            lenValeur=0;

            break;
        case 3: // DUMP
            break;
        default:
            break;
        }

        close(t[N - 1][1]);
    }

    close(pipeCtrl[1]);
    close(1);
    close(0);

    return 0;
}