/* =================================================================== */
// Progrmame Client à destination d'un joueur qui doit deviner la case
// du trésor. Après chaque coup le résultat retourné par le serveur est
// affiché. Le coup consite en une abcsisse et une ordonnée (x, y).
/* =================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define N 10
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define MAGENTA "\033[35m"

/* ====================================================================== */
/*                  Affichage du jeu en mode texte brut                   */
/* ====================================================================== */
void afficher_jeu(int jeu[N][N], int res, int points, int coups) {

    printf("\n************ TROUVEZ LE TRESOR ! ************\n");
    printf("    ");
    for (int i=0; i<10; i++)
        printf("  %d ", i+1);
    printf("\n    -----------------------------------------\n");
    for (int i=0; i<10; i++){
        printf("%2d  ", i+1);
        for (int j=0; j<10; j++) {
            printf("|");
            switch (jeu[i][j]) {
                case -1:
                    printf(" 0 ");
                    break;
                case 0:
                    printf(GREEN " T " RESET);
                    break;
                case 1:
                    printf(YELLOW " %d " RESET, jeu[i][j]);
                    break;
                case 2:
                    printf(RED " %d " RESET, jeu[i][j]);
                    break;
                case 3:
                    printf(MAGENTA " %d " RESET, jeu[i][j]);
                    break;
            }
        }
        printf("|\n");
    }
    printf("    -----------------------------------------\n");
    printf("Pts dernier coup %d | Pts total %d | Nb coups %d\n", res, points, coups);
}


/* ====================================================================== */
/*                    Fonction principale                                 */
/* ====================================================================== */
int main(int argc, char **argv) {

    int jeu[N][N];
    int lig, col;
    int res = -1, points = 0, coups = 0;
    char request[4];

    /* Init args @IP et numéro de port en paramètres du programme */
    if (argc != 3) {
        printf("Usage: %s <IP address> <port number>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    char *IP_ADDRESS = argv[1];
    int PORT_NUMBER = atoi(argv[2]);

    /* Init jeu */
    for (int i=0; i<N; i++)
        for (int j=0; j<N; j++)
            jeu[i][j] = -1;

    /* Creation socket TCP */
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    /* Init caracteristiques serveur distant (struct sockaddr_in) */
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT_NUMBER);
    if (inet_pton(AF_INET, IP_ADDRESS, &server_address.sin_addr) == -1) {
        perror("inet_pton()");
        exit(EXIT_FAILURE);
    }

    /* Etablissement connexion TCP avec process serveur distant */
    if (connect(sock, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
        perror("connect()");
        exit(EXIT_FAILURE);
    }

    /* Tentatives du joueur : stoppe quand tresor trouvé */
    do {
        afficher_jeu(jeu, res, points, coups);
        printf("\nEntrer le numéro de ligne : ");
        scanf("%d", &lig);
        printf("Entrer le numéro de colonne : ");
        scanf("%d", &col);

        /* Construction requête (serialisation en chaines de caractères) */
        sprintf(request, "%d %d", lig, col);

        /* Envoi de la requête au serveur (send) */
        send(sock, request, strlen(request), 0);

        /* Réception du resultat du coup (recv) */
        char buffer[1024];
        recv(sock, buffer, 1024, 0);

        /* Deserialisation du résultat en un entier */
        res = 0;
        for (int i=0; i<strlen(buffer); i++)
            res = res*10 + (buffer[i]-'0');
        /* Mise à jour */
        if (lig>=1 && lig<=N && col>=1 && col<=N)
            jeu[lig-1][col-1] = res;
        points += res;
        coups++;

    } while (res);

    /* Fermeture connexion TCP */
    if (close(sock) == -1) {
        perror("close()");
        exit(EXIT_FAILURE);
    }

    /* Terminaison du jeu : le joueur a trouvé le tresor */
    afficher_jeu(jeu, res, points, coups);
    printf("\nBRAVO : trésor trouvé en %d essai(s) avec %d point(s)"
            " au total !\n\n", coups, points);
    return 0;
}
