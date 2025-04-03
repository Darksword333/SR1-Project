/*================================================*/
// Programme simulant un protocole de routage dynamique simplifié
// Ce programme code uniquement le comportement
// de récpeteur d'une annonce de routage
// émise depuis UN SEUL routeur voisin pour UN échange initial de routes
// T. Desprats - Novembre 2022

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h> // struct sockaddr_in
#include <time.h>
#include <signal.h>
#include <stdbool.h>

#include "tabrout.h"

#define BUF_SIZE_IN 64 // we should receive less...
#define IPV4_ADR_STRLEN 16  // == INET_ADDRSTRLEN
#define LOCALHOST "127.0.0.1"
#define NO_BASE_PORT 17900  // base number for computing real port number


/* =================================================================== */
/* FONCTION PRINCIPALE : PEER PROCESSUS DE ROUTAGE ROLE RECEPTEUR ONLY */
/* =================================================================== */
int main(int argc, char **argv) {

  if (argc != 4) {
    printf("Usage: %s IDIP@ssRouter MyNumberRouter NeigborNumberRouter\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  // Usage routPrec IDIP@ssRouter  MyNumberRouter NeigborNumberRouter
  // Example routPrec 10.1.1.1 1 2

  char idInitConfigFile [20]; //Id of the configuration file of the router
  char myId [32]; // String array representing the whole id of the Router

  routing_table_t myRoutingTable; //Routing TABLE

  /* Building ID Router from command args */
  sprintf(myId,"R%s %s",argv[2],argv[1]);
  printf("ROUTEUR : %s\n",myId );
  /* Building Config File ID from command args */
  sprintf(idInitConfigFile,"R%sCfg",argv[2]);
  strcat(idInitConfigFile,".txt");
  /* Loading My Routing Table from Initial Config file */
  init_routing_table(&myRoutingTable, idInitConfigFile);
  printf("ROUTEUR : %d entrées initialement chargées \n",myRoutingTable.nb_entry);
  display_routing_table(&myRoutingTable,myId);

 /* Socket */
  int sock;
  struct sockaddr_in addr;
  socklen_t addr_len = sizeof(struct sockaddr_in);
  char buffer[BUF_SIZE_IN];
  int port = NO_BASE_PORT;
  if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    perror("socket()");
    exit(EXIT_FAILURE);
  }

  /* Bind */
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
    perror("bind()");
    exit(EXIT_FAILURE);
  }

  /* Réception du nombre d'entrées */
  if (recvfrom(sock, buffer, BUF_SIZE_IN, 0, (struct sockaddr *) &addr, &addr_len) == -1) {
    perror("recvfrom()");
    exit(EXIT_FAILURE);
  }
  /* L'erreur que je n'arrive pas à corriger intervient ici je ne reçois rien donc mon nombre d'entrée vaut 0 donc je ne modifie jamais la table
  sinon malgré cela le reste du code semble correct et sans cette erreur cela doit être juste*/
  printf("Reçu (Nombre d'entrées) : %s\n", buffer);
  int nb_entry = atoi(buffer);

  /* Réception des entrées */
  for (int i = 0; i < nb_entry; i++) {
    if (recvfrom(sock, buffer, BUF_SIZE_IN, 0, (struct sockaddr *) &addr, &addr_len) == -1) {
      perror("recvfrom()");
      exit(EXIT_FAILURE);
    }
    if (!is_present_entry_table(&myRoutingTable, buffer)) {
      add_entry_routing_table(&myRoutingTable, buffer);
    }
  }

  // Display new content of my routing table
  display_routing_table(&myRoutingTable,myId);
  exit(EXIT_SUCCESS);
}
