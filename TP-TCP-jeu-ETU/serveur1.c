#include <stdio.h>
#include "tresor.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

int main(int argc, char **argv) {
    // Initialisation du jeu :
    int N = 10; // taille du plateau
    int XT = 5; // abscisse du trésor
    int YT = 5; // ordonnée du trésor
    //Appel avec "rand" pour avoir des coordonnées aléatoires
    if (argc > 1 && strcmp(argv[1], "rand") == 0){
        srand(time(NULL));
        XT = rand() % N;
        YT = rand() % N;
        printf("Trésor en %d %d\n", XT, YT);
    }
    int sock;
    struct sockaddr_in adresse;
    socklen_t taille = sizeof(struct sockaddr_in);
    int sock_client;
    struct sockaddr_in adresse_client;
    socklen_t taille_client = sizeof(struct sockaddr_in);
    int port = 5555;
    int lig, col, res = -1;
    char request[1024];
    char reschar = '0';

    // Création de la socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket");
        exit(-1);
    }

    // Permet la réutilisation du port
    int used = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &used, sizeof(int)) == -1) {
        perror("setsockopt");
        exit(-2);
    }

    // Préparation de l'adresse d'attachement locale
    adresse.sin_family = AF_INET;
    adresse.sin_port = htons(port);
    adresse.sin_addr.s_addr = htonl(INADDR_ANY);

    // Attachement de la socket à l'adresse locale
    if (bind(sock, (struct sockaddr *) &adresse, taille) == -1) {
        perror("bind");
        exit(-3);
    }

    // Mise en écoute de la socket
    if (listen(sock, 1) == -1) {
        perror("listen");
        exit(-4);
    }

    while (1) {
        printf("Attente de connexion\n");
        // Attente de connexion
        sock_client = accept(sock, (struct sockaddr *) &adresse_client, &taille_client);
        if (sock_client == -1) {
            perror("accept");
            exit(-5);
        }
        printf("Connexion acceptée\n");
        while (res != 0){
            // Réception de la requête
            if (recv(sock_client, request, sizeof(request), 0) == -1) {
                perror("recv");
                exit(-6);
            }
            printf("Requête reçue : %s\n", request);
            // Analyse de la requête
            sscanf(request, "%d %d", &lig, &col);

            // Emplacement du trésor et calcul du resultat
            res = recherche_tresor(N, XT, YT, lig, col);

            // Envoi du résultat
            reschar = res + '0';
            if (send(sock_client, &reschar, sizeof(reschar), 0) == -1) {
                perror("send");
                exit(-7);
            }
            printf("Résultat envoyé : %c\n", reschar);
        }
        res = -1;
        printf("Connexion fermée\n--------------\n");
        // Fermeture de la socket de service
        close(sock_client);
    }

    // Fermeture de la socket d'écoute (inutile)
    close(sock);
    return 0;
}
