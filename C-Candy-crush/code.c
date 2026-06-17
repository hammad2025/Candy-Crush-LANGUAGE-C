#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <windows.h>

#define GRID_SIZE 15
#define OBSTACLE 'X'
#define JOCKER 'J'

// Variables globales
char utilisateur[50] = ""; // la var qui va recevoir le nom de l'utilisateur
char grid[GRID_SIZE][GRID_SIZE]; // Grille de jeu

// La fonction qui nous permettra de changer la couleur de nos bonbons
void changerCouleur(int couleur) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, couleur);
}

// la fonction qui nous permettra d initialiser la grille avec des bonbons aléatoires
void initialiserGrille(int niveau) {
    char bonbons[] = {'C', 'F', 'R', 'S', 'T'};
    srand(time(NULL));

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            grid[i][j] = bonbons[rand() % 5];
        }
    }

    // pour l ajout d'obstacles pour le niveau 2 et supérieur
    if (niveau >= 2) {
        int obstacles = GRID_SIZE * GRID_SIZE * 0.1; // 10% d'obstacles
        for (int i = 0; i < obstacles; i++) {
            int x = rand() % GRID_SIZE;
            int y = rand() % GRID_SIZE;
            grid[x][y] = OBSTACLE;
        }
    }

    // pour l ajout de bonbons jocker pour le niveau 3
    if (niveau == 3) {
        for (int i = 0; i < 10; i++) { // Maximum 10 jockers
            int x = rand() % GRID_SIZE;
            int y = rand() % GRID_SIZE;
            if (grid[x][y] != OBSTACLE) {
                grid[x][y] = JOCKER;
            }
        }
    }
}

// la fonction pour afficher la grille
void afficherGrille() {
    printf("\n   ");
    for (char c = 'A'; c < 'A' + GRID_SIZE; c++) {
        printf("%c ", c);
    }
    printf("\n");

    for (int i = 0; i < GRID_SIZE; i++) {
        printf("%2d ", i + 1);
        for (int j = 0; j < GRID_SIZE; j++) {
            switch (grid[i][j]) {
                case 'C': changerCouleur(11); break; // Bleu
                case 'F': changerCouleur(10); break; // Vert
                case 'R': changerCouleur(12); break; // Rouge
                case 'S': changerCouleur(13); break; // Violet
                case 'T': changerCouleur(14); break; // Jaune
                case OBSTACLE: changerCouleur(8); break; // Gris
                case JOCKER: changerCouleur(9); break; // Cyan
                default: changerCouleur(15); break; // Blanc
            }
            printf("%c ", grid[i][j]);
            changerCouleur(15); // Réinitialise la couleur
        }
        printf("\n");
    }
}

// la fonction pour supprimer les bonbons et calculer les points
void supprimerBonbons(int x, int y, char type, int *points, int niveau) {
    if (x < 0 || x >= GRID_SIZE || y < 0 || y >= GRID_SIZE) return;
    if (grid[x][y] != type && !(niveau >= 3 && grid[x][y] == JOCKER)) return;

    // Points pour bonbons jocker
    if (grid[x][y] == JOCKER && niveau >= 3) {
        *points += 10;
    } else {
        *points += (type == 'C') ? 1 : (type == 'F') ? 2 : (type == 'R') ? 3 : (type == 'S') ? 4 : 5;
    }

    grid[x][y] = ' '; // Supprime le bonbon, la case sera vider du bonbon qu elle contenait

    supprimerBonbons(x - 1, y, type, points, niveau);
    supprimerBonbons(x + 1, y, type, points, niveau);
    supprimerBonbons(x, y - 1, type, points, niveau);
    supprimerBonbons(x, y + 1, type, points, niveau);
}

// ici la fonction  pour simluler l effet de cascade (niveau 4)
void cascadeBonbons() {
    for (int j = 0; j < GRID_SIZE; j++) {
        for (int i = GRID_SIZE - 1; i >= 0; i--) {
            if (grid[i][j] == ' ') {
                for (int k = i; k > 0; k--) {
                    grid[k][j] = grid[k - 1][j];
                }
                grid[0][j] = "CFRST"[rand() % 5];
            }
        }
    }
}

// la fonction pour jouer une partie
void jouerPartie(int niveau) {
    int score = 0;
    char choix;
    int ligne;
    char col;

    // Demander le nom de l'utilisateur s'il n'est pas déjà saisi
    if (strlen(utilisateur) == 0) {
        printf("\nEntrez votre nom : ");
        scanf("%s", utilisateur);

        // Demander à l'utilisateur de taper 'L' ou 'l' pour commencer // pour ue le programme ne plante pas cette partie est receptive au majuscule et minuscule
        printf("\nTapez 'L' pour lancer la partie : ");
        scanf(" %c", &choix);
        if (choix != 'L' && choix != 'l') {
            printf("\nSaisie invalide. Retour au menu principal.\n");
            return;
        }
    }

    do {
        initialiserGrille(niveau); // Initialise la grille pour une nouvelle partie
        score = 0; // Réinitialise le score

        while (1) {
            afficherGrille();
            printf("\n%s, Score actuel : %d\n", utilisateur, score);
            printf("Entrez une ligne et une colonne (ex: 3 B) ou 'A' pour abandonner : ");
            scanf(" %c", &choix);

            if (choix == 'A' || choix == 'a') {
                printf("\nVous avez abandonne la partie.\n");
                return; // Retour au menu principal
            }

            ungetc(choix, stdin); // Remet le choix dans le flux
            if (scanf("%d %c", &ligne, &col) != 2) {
                printf("Saisie invalide. Réessayez.\n");
                continue;
            }

            ligne--;
            col = toupper(col) - 'A';

            if (ligne >= 0 && ligne < GRID_SIZE && col >= 0 && col < GRID_SIZE && grid[ligne][col] != OBSTACLE) {
                int points = 0;
                supprimerBonbons(ligne, col, grid[ligne][col], &points, niveau);
                score += points;
                if (niveau == 4) cascadeBonbons();

                if (score >= 50) {
                    printf("\nPartie terminee %s, votre score est %d\n", utilisateur, score);
                    break; // La partie se termine
                }
            } else {
                printf("Ooh! Coordonnees invalides ou case bloquee. Reessayez.\n");
            }
        }

        // Demande au joueur s'il veut rejouer
        printf("\nVoulez-vous rejouer ? (O/N) : ");
        scanf(" %c", &choix);
    } while (choix == 'O' || choix == 'o');

    // Retour au menu principal si le joueur ne veut pas rejouer
    printf("\nEt c est partie pour le menu principal\n");
}

// Menus
void menuNiveau() {
    int niveau;

    while (1) {
        printf("\n Menu Niveau \n");
        printf("1. Niveau 1\n");
        printf("2. Niveau 2\n");
        printf("3. Niveau 3\n");
        printf("4. Niveau 4\n");
        printf("5. Retour au menu principal\n");
        printf("Votre choix : ");
        scanf("%d", &niveau);

        if (niveau >= 1 && niveau <= 4) {
            jouerPartie(niveau);
        } else if (niveau == 5) {
            break;
        } else {
            printf("Oooh! Choix invalide. Reessayez.\n");
        }
    }
}

void menuPrincipal() {
    int choix;

    while (1) {
        printf("\n Menu Principal \n");
        printf("1. Menu niveau\n");
        printf("2. Regles du jeu\n");
        printf("3. Credits\n");
        printf("4. Quitter\n");
        printf("Votre choix : ");
        scanf("%d", &choix);

        if (choix == 1) {
            menuNiveau();
        } else if (choix == 2) {
            printf("\n Le but du jeu est de faire ecraser des bonbons colores\n en associant des combinaisons d'au moins trois bonbons identiques,\n afin de remplir l'objectif du niveau et de marquer un maximum de points.\n");
        } else if (choix == 3) {
            printf("\nCe projet a ete realiser par Mouhammad-SISSOKO\n etudiant en bachelor 1 Groupe3.\n j'esperons fort bien que vous allez vous amusez\n et trouverez cette experience douce comme un\n bonbon...amusez vous bien! \n");
        } else if (choix == 4) {
            printf("\nMerci d'avoir joue et a la prochaine pour\nune nouvelle aventure sucrement sucre!\n");
            break;
        } else {
            printf("Choix invalide. Reessayez.\n");
        }
    }
}

// Fonction principale
int main() {
    printf(" BIENVENUE A CANDY ECE CRUSH SAGA\n");
    menuPrincipal();
    return 0;
}