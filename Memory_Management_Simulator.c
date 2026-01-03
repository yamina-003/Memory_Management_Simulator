#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include<time.h>

//BENTAIFOUR Yamina BENABDELLAH Hadjer

// Structure d'un processus
struct processus {
    char id[30];
    int taille;
    time_t temps_arrive;
    int temps_execution;
};

enum etat_partition {
    LIBRE,
    OCCUPE
};
// Structure d'une partition
struct partition {
    char id[30];
    struct partition* adresse_debut;  //un pointeur vers la partition elle-même
    int taille;
    enum etat_partition etat;
    time_t temps_arrive; // l'Instant où le processus a été chargé dans la partition
    int temps_execution;

};

// Definition du type ListeC : Une liste de partition
typedef struct memoire {
    struct partition donnees;
    struct memoire* suivant;
};

typedef struct memoire* ListeC;

struct element {
    struct processus Processus;
    struct element* suivant;
};

// Structure pour representer la file de processus en attente
struct FileProcessus {
    struct element* avant; // Pointeur vers le premier element
    struct element* arriere; // Pointeur vers le dernier element
};



// Création de l'etat initial de la mémoire 
void CreerEtatInitialMemoire(ListeC* T) {
    ListeC Q, S;
    int user_choice;
    

    do {
        Q = malloc(sizeof(struct memoire));
        printf("Entrez la taille de votre partition (0 pour terminer) : \n");
        scanf("%d", &Q->donnees.taille);

        if (Q->donnees.taille > 0) {
            // L'adresse de début est l'adresse de la partition elle-même
            Q->donnees.adresse_debut = &(Q->donnees);

            printf("Entrez l'etat de votre partition (0 pour LIBRE, 1 pour OCCUPE) : \n");
            scanf("%d", &user_choice);

            if (user_choice == 0) {
                Q->donnees.etat = LIBRE;
                strcpy(Q->donnees.id , "VIDE");
                Q->donnees.temps_arrive = 0;  // Partition libre, temps d'arrivée 0 
                Q->donnees.temps_execution = 0;

            } else if (user_choice == 1) {
                Q->donnees.etat = OCCUPE;
                printf("Donner le ID de votre programme: ");
                scanf("%s",Q->donnees.id);
                Q->donnees.temps_arrive = time(NULL);
                printf("Donner le temps d'execution de votre programme (en seconds): ");
                scanf("%d", &Q->donnees.temps_execution);

            } else {
                printf("Erreur : votre choix est invalide. \n");
                free(Q);  // On libère l'espace réservé à une partition invalide
                break;
            }

            // Réalisation du chainage
            Q->suivant = NULL;

            if (*T == NULL) {
                *T = Q;
            } else {
                S->suivant = Q;
            }
            S = Q;
        } else {
            // Libérer la mémoire réservée à la partition invalide
            free(Q);
        }
    } while (Q->donnees.taille > 0);

    if (*T == NULL) {
        printf("Aucune partition n'a ete creee.\n");
    }
}

// Fonction de création d'une partition à partir d'un residu
void ResiduMemoire (ListeC* T, int residu){
    ListeC R = malloc(sizeof(struct memoire));
    R->donnees.etat = LIBRE;
    R->donnees.taille = residu;
    R->donnees.temps_execution = 0;  // Partition libre, temps d'arrivée 0
    R->suivant = NULL;
    strcpy(R->donnees.id,"VIDE");
    // L'adresse de début est l'adresse de la partition elle-même
    R->donnees.adresse_debut = &(R->donnees);
    ListeC temp = *T;
    while(temp->suivant != NULL){
        temp = temp->suivant;
    }
    temp->suivant = R;
}

// Fonction d'affichage (avec tableau)
void AffichageMemoire(ListeC T) {
    printf("------------------------------------------------------------------------------------------------------------------------------\n");
    printf("| %-15s | %-20s | %-12s | %-20s | %-20s | %-20s |\n", "ID", "Adresse de debut", "La taille", "L'Etat", "Temps", "Temps d'execution");
    printf("------------------------------------------------------------------------------------------------------------------------------\n");

    ListeC temp = T;

    while (temp != NULL) {
        const char* etat_str = (temp->donnees.etat == LIBRE) ? "LIBRE" : "OCCUPE";

        char tempsStr[30];  // pour stocker le temps formaté (lisibilite au utilisatur)
        if (temp->donnees.etat == LIBRE){
            strcpy(tempsStr, "");
        } else {
            strftime(tempsStr, sizeof(tempsStr), "%H:%M:%S", localtime(&temp->donnees.temps_arrive));
        }
        printf("| %-15s | %-20p | %-12d | %-20s | %-20s | %-20d |\n",
               temp->donnees.id, temp->donnees.adresse_debut, temp->donnees.taille, etat_str, tempsStr, temp->donnees.temps_execution);

        temp = temp->suivant;
    }
    printf("------------------------------------------------------------------------------------------------------------------------------\n");
}

 // Fonction pour supprime un processus qui a terminer son execution
void EndExecution(ListeC* T, time_t tempsActuel) {
    ListeC Q = *T;
    
    // Parcourir la liste des partitions
    while (Q != NULL) {
         // Vérifier si la partition est occupée
        if (Q->donnees.etat == OCCUPE) {
            // Calculer le temps écoulé depuis le chargement du processus
            time_t tempsEcoule = tempsActuel - Q->donnees.temps_arrive;
            // Vérifier si le temps d'exécution est écoulé
            if (tempsEcoule >= Q->donnees.temps_execution) {
                // Marquer la partition comme libre seulement si elle est réellement occupée
                if (strcmp(Q->donnees.id, "VIDE") != 0) {
                    Q->donnees.etat = LIBRE;
                    Q->donnees.temps_arrive = 0;
                    Q->donnees.temps_execution = 0;
                    // Afficher que le processus a terminé son exécution
                    printf("Le processus %s a termine son execution.\n", Q->donnees.id);
                    strcpy(Q->donnees.id, "VIDE");
                   
                }
            }
        }
        // Passer à la partition suivante dans la liste
        Q = Q->suivant;
    }
}

// Fonction de relogement: on cree deux listes occupées et libres ensuite on les relies
void Reloger(ListeC* T) {
    ListeC Q = *T;
    ListeC L1 = NULL; // La liste des partitions libres
    ListeC L2 = NULL; // La liste des partitions occupées
    ListeC Occupe = NULL; // Pointeur pour parcourir la liste occupée
    ListeC Libre = NULL; // Pointeur pour parcourir la liste libre
    
    // Séparation des partitions en listes libres (L1) et occupées (L2)
    while(Q != NULL){
        while(Q != NULL && Q->donnees.etat == LIBRE){
            if(L1 == NULL){
                L1 = Q;
                Libre = L1;
            } else {
                Libre->suivant = Q;
                Libre = Q;
            }
            Q = Q->suivant;
        }
        while(Q != NULL && Q->donnees.etat == OCCUPE){
             if(L2 == NULL){
                L2 = Q;
                Occupe = L2;
            } else {
                Occupe->suivant = Q;
                Occupe = Q;
            }
            Q = Q->suivant;
            
        }
    }

     // Relier les listes (L2 est placée au début, suivie de L1, L1 se termine par NULL)
    if(L2 != NULL){
       *T =L2;
       Occupe->suivant = L1;
       if(Libre != NULL){
          Libre->suivant = NULL;
        }
    }
    
}

// Fusionner les partitions libres consécutives
void Fusionner(ListeC* T) {
    ListeC P = *T;
    ListeC S = NULL;
    int Nouvtaille = 0;

    // Parcours de la liste pour fusionner les partitions libres consécutives
    while (P != NULL && P->suivant != NULL) {
        if (P->donnees.etat == LIBRE && P->suivant->donnees.etat == LIBRE) {
            
            // Calcul de la nouvelle taille après fusion
            Nouvtaille = P->donnees.taille + P->suivant->donnees.taille;
            S = P->suivant;

            // Suppression de la partition à fusionner
            P->suivant = S->suivant;
            free(S);

            // Mise à jour de la taille de la partition courante
            P->donnees.taille = Nouvtaille;
        } else {
            P = P->suivant;
        }
    }
}



// Fonction pour ajouter un processus à la file d'attente
void AjouterProcessus(struct FileProcessus* file, struct processus Processus) {
        // Création d'un nouveau nœud de file
        struct element* nouv = (struct element*)malloc(sizeof(struct element));
        nouv->Processus = Processus;
        nouv->suivant = NULL;

        // Cas où la file est vide
        if (file->avant == NULL) {
            file->avant = nouv;
            file->arriere = nouv;
        } else {
            // Ajout à la fin de la file
            file->arriere->suivant = nouv;
            file->arriere = nouv;
        }
}

// fonction de defilement
struct processus Defiler(struct FileProcessus* file){
    struct element* temp = file->avant;
    struct processus P = temp->Processus;
    if(file->avant == file->arriere){
        // La file est vide on libere le seul element
        file->avant = NULL;
        file->arriere = NULL;
    } else {
        //On défile en déplaçant le pointeur avant vers le suivant
        file->avant = file->avant->suivant;
    } 
    free(temp);
    return(P);
}

// Fonction qui vérifie si une file de processus est vide 
int FileVide(struct FileProcessus* file) {
    if(file->avant == NULL){
        return 0;
    }
    return 1;
}

// Fonction pour afficher l'état de la file de processus
void AffichageFileProcessus(struct FileProcessus* file){
    struct processus P;
    struct FileProcessus* temp = malloc(sizeof(struct FileProcessus));  // Allouer de la mémoire pour temp
    temp->avant = NULL;
    temp->arriere = NULL;
     // Vérifier si la file est vide
    if(FileVide(file) == 0){
       printf("Tous les processus ont ete charges.\n");
       return;
    }
    
    // Affichage de l'en-tête de la table
    printf("-----------------------------------------------------------------------------------------\n");
    printf("| Taille\t\t | Id\t\t| Temps d'arrive\t | Temps d'execution\t|\n");
    printf("-----------------------------------------------------------------------------------------\n");
    char tempsStr[30];  
    // Parcours de la file
    while (FileVide(file) != 0) {
      // Initialiser P avec le sommet de la file
       P = file->avant->Processus;
       // Formatage du temps d'arrivée pour une meilleure lisibilité
       strftime(tempsStr, sizeof(tempsStr), "%H:%M:%S", localtime(&P.temps_arrive));
       // Défiler un élément de la file principale et l'ajouter à la file temporaire
       printf("| %-15d\t | %-12s\t| %-15s\t | %-20d\t|\n", P.taille, P.id, tempsStr, P.temps_execution);
       AjouterProcessus(temp, Defiler(file));
    }

    printf("-----------------------------------------------------------------------------------------\n");
   
    // Reconstitution de la file d'attente à partir de la file temporaire
    while(FileVide(temp) != 0){
        AjouterProcessus(file, Defiler(temp));
    }

    // Libérer la mémoire allouée pour temp
    free(temp);  // Libérer la mémoire allouée pour temp
}



// Fonction de politique d'allocation First fit
void FirstFit(ListeC* T, struct FileProcessus* file ){
    ListeC Q = *T;
    struct processus P = file->avant->Processus;

     // Parcours des partitions
    while(1){

        // Vérification si la partition convient
        if(Q->donnees.taille>= P.taille && Q->donnees.etat == LIBRE ){

            // Gestion de l'espace résiduel
            if(Q->donnees.taille - P.taille != 0){
                ResiduMemoire(T,Q->donnees.taille - P.taille );
            } 

            // Mise à jour des informations de la partition
            Q->donnees.taille = P.taille;
            Q->donnees.etat = OCCUPE;
            Q->donnees.temps_arrive = time(NULL);  // Enregistrez le moment où le processus a été chargé
            Q->donnees.temps_execution = P.temps_execution;
            strcpy(Q->donnees.id, P.id);
            // Défilement du processus de la file
            Defiler(file);
            break;
        } else { 
            Q = Q->suivant;
        }
    
        if(Q == NULL){
            // Si aucune partition n'est trouvée, déplace le processus à la fin de la file
            printf("Aucune partition ne convient. Deplacement du processus vers la queue de la file.\n");
            AjouterProcessus(file, Defiler(file));
            break;
        }
    }
}

// Fonction de politique d'allocation Best fit
void BestFit(ListeC* T, struct FileProcessus* file){
    ListeC Q = *T;
    ListeC Bfit = NULL;
    struct processus P = file->avant->Processus;
    int min;
    // Initialisation de min à la première rencontre d'une partition convenable 
    while (Q != NULL){
        if(Q->donnees.taille>= P.taille && Q->donnees.etat == LIBRE){
            min = Q->donnees.taille - P.taille;
            Bfit = Q;
            Q = Q->suivant;
            // Recherche de la meilleure partition
            while(Q != NULL && Q->donnees.etat == LIBRE){
                if( min > Q->donnees.taille - P.taille){
                    min = Q->donnees.taille - P.taille ;
                    Bfit = Q;
                } else {
                    Q = Q->suivant;
                }
            }

            // Si une partition est trouvée, allocation
            if (Bfit != NULL) {
                if(min != 0){
                    ResiduMemoire(T, min);
                }
                // Mise à jour des informations de la partition
                Bfit->donnees.taille = P.taille;
                Bfit->donnees.etat = OCCUPE;
                Bfit->donnees.temps_arrive = time(NULL);  // Enregistrez le moment où le processus a été chargé
                Bfit->donnees.temps_execution = P.temps_execution;
                strcpy(Bfit->donnees.id, P.id);
                // Défilement du processus de la file
                Defiler(file);
                break;
            }
        }
        else {
            Q = Q->suivant;
        }
    }

    // Si aucune partition n'est trouvée, déplace le processus à la fin de la file
    if(Bfit == NULL){
        // Le cas où aucune partition ne convient à un processus, on utilise Defiler et Enfiler(AjouterProcessus)
        printf("Aucune partition ne convient. Deplacement du processus vers la queue de la file.\n");
        AjouterProcessus(file, Defiler(file));
        
    }
}

// Fonction de politique d'allocation Worst fit
void WorstFit(ListeC* T, struct FileProcessus* file) {
    ListeC Q = *T;
    ListeC Wfit = NULL;
    struct processus P = file->avant->Processus;
    int max;

    // Initialisation de max à la première rencontre d'une partition convenable
    while (Q != NULL) {
        if (Q->donnees.taille >= P.taille && Q->donnees.etat == LIBRE) {
            max = Q->donnees.taille - P.taille;
            Wfit = Q;
            Q = Q->suivant;
            // Recherche de la pire partition
            while (Q != NULL && Q->donnees.etat == LIBRE) {
                if (max < Q->donnees.taille - P.taille) {
                    max = Q->donnees.taille - P.taille;
                    Wfit = Q;
                } else {
                    Q = Q->suivant;
                }
            }

            // Si une partition est trouvée, allocation
            if (Wfit != NULL) {
                if (max != 0) {
                    ResiduMemoire(T, max);
                }

                // Mise à jour des informations de la partition
                Wfit->donnees.taille = P.taille;
                Wfit->donnees.etat = OCCUPE;
                Wfit->donnees.temps_arrive = time(NULL);  // Enregistrez le moment où le processus a été chargé
                Wfit->donnees.temps_execution = P.temps_execution;
                strcpy(Wfit->donnees.id, P.id);
                // Défilement du processus de la file
                Defiler(file);
                break;
            }
        } else {
            Q = Q->suivant;
        }
    }
    if(Wfit == NULL){
        // Si aucune partition n'est trouvée, déplace le processus à la fin de la file
        printf("Aucune partition ne convient. Deplacement du processus vers la queue de la file.\n");
        // Le cas où aucune partition ne convient à un processus, on utilise Defiler et Enfiler(AjouterProcessus)
        AjouterProcessus(file, Defiler(file));
        
    }

}


int main(){
    ListeC T = NULL;
    ListeC Q = NULL; 
    int user_choice;

    printf("Bienvenue dans le programme de simulation de gestion d'allocation de memoire.\n");
    printf("Veuillez suivre les instructions affichees a l'ecran et lire attentivement les messages.\n");

    printf("Creation de votre memoire :\n");
    // Initialisation de l'état initial de la mémoire
    CreerEtatInitialMemoire(&T);
    printf("L'etat initial de la memoire: \n");
    AffichageMemoire(T);

    // Création de la file de processus
    struct processus Processus;
    struct FileProcessus* file = malloc(sizeof(struct FileProcessus));
    file->avant = NULL;
    file->arriere = NULL;


    printf("Creation de la file de processus (sans priorite):\n");
    
    
    // Ajout de processus à la file
    do {
        printf("Veuillez donner la taille de votre processus (0 pour terminer) : ");
        scanf("%d", &Processus.taille);

        if (Processus.taille > 0) {
            printf("Veuillez donner l'ID de votre programme : ");
            scanf("%s", Processus.id);
            Processus.temps_arrive = time(NULL);
            // on a assigner un type int au temps d'execution pour simplifier la solution
            printf("Veuillez donner le temps d'execution de votre programme (en secondes) : ");
            scanf("%d", &Processus.temps_execution);

            AjouterProcessus(file, Processus);
        }
    } while (Processus.taille > 0);


    // Affichage de la file de processus
    AffichageFileProcessus(file);

    // Boucle principale pour les allocations mémoire
    while (FileVide(file) != 0) {
        printf("\nMenu d'allocation memoire:\n");
        printf("1. Politique First Fit\n");
        printf("2. Politique Best Fit\n");
        printf("3. Politique Worst Fit\n");
        printf("0. Quitter\n");
        printf("Veuillez choisir une option : ");
        scanf("%d", &user_choice);

        switch (user_choice) {
            case 1:
                printf("\nVous avez choisi la politique First Fit.\n");
                FirstFit(&T, file);
                break;

            case 2:
                printf("\nVous avez choisi la politique Best Fit.\n");
                BestFit(&T, file);
                break;

            case 3:
                printf("\nVous avez choisi la politique Worst Fit.\n");
                WorstFit(&T, file);
                break;

            case 0:
                printf("Programme termine.\n");
                return 0;

            default:
                printf("Option non valide. Veuillez reessayer.\n");
        }

        // Affichage de la mémoire après chaque allocation
        printf("La memoire apres allocation: \n");
        AffichageMemoire(T);

        // Appel de la fonction EndExecution pour gérer la fin de l'exécution d'un processus
        EndExecution(&T, time(NULL));
        AffichageMemoire(T);

        // Appel de la fonction Reloger pour gérer les positions des partitions avant le prochain téléchargement d'un processus
        Reloger(&T);

        // Affichage de la mémoire après relogement
        printf("La memoire apres relogemment: \n");
        AffichageMemoire(T);

        // Appel de la fonction Fusionner pour fusionner les partitions libres consécutives
        Fusionner(&T);

        // Affichage de la mémoire après fusionnement
        printf("La memoire apres fusionnement: \n");
        AffichageMemoire(T);

        // Affichage de la file de processus
        AffichageFileProcessus(file);
    }




    return 0;
}