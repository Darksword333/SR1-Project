/*=============================================================*/
// Programme simulant un protocole de routage dynamique simplifié
// Ce programme code uniquement le comportement
// d'émetteur d'une annonce de routage
// vers UN SEUL routeur voisin pour UN échange initial de routes
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

#define BUF_SIZE_OUT 64 // we should send less...
#define IPV4_ADR_STRLEN 16  // == INET_ADDRSTRLEN
#define LOCALHOST "127.0.0.1"
#define NO_BASE_PORT 17900  // base number for computing real port number

/* =================================================================== */
/* FONCTION PRINCIPALE : PEER PROCESSUS DE ROUTAGE ROLE EMETTEUR ONLY  */
/* =================================================================== */

int main(int argc, char **argv) {

  if (argc != 4) {
    printf("Usage: %s IDIP@ssRouter MyNumberRouter NeigborNumberRouter\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  // Usage routPem IDIP@ssRouter  MyNumberRouter NeigborNumberRouter
  // Example routPem 10.1.1.1 1 2

  char idInitConfigFile [20]; // Id of the configuration file of the router
  char myId [32]; // String array representing the whole id of the Router
  routing_table_t myRoutingTable; // Routing TABLE

  /* Building ID Router from command args */
  sprintf(myId, "R%s %s", argv[2], argv[1]);
  printf("ROUTEUR : %s\n", myId);

  /* Building Config File ID from command args */
  sprintf(idInitConfigFile, "R%sCfg", argv[2]);
  strcat(idInitConfigFile, ".txt");

  /* Loading My Routing Table from Initial Config file */
  init_routing_table(&myRoutingTable, idInitConfigFile);
  printf("ROUTEUR : %d entrées initialement chargées \n", myRoutingTable.nb_entry);
  display_routing_table(&myRoutingTable, myId);

  /* Socket */
  int sock;
  struct sockaddr_in addr;
  socklen_t addr_len = sizeof(struct sockaddr_in);
  char buffer[BUF_SIZE_OUT];
  int port = NO_BASE_PORT;
  if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    perror("socket()");
    exit(EXIT_FAILURE);
  }

  /* Envoie des données */
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  if (inet_pton(AF_INET, LOCALHOST, &addr.sin_addr) != 1) {
    perror("inet_pton()");
    exit(EXIT_FAILURE);
  }

  /* Envoie du Nombre d'Entrées */
  int nb_entry = myRoutingTable.nb_entry;
  if (sendto(sock, &nb_entry, BUF_SIZE_OUT, 0, (struct sockaddr *) &addr, addr_len) == -1) {
    perror("sendto()");
    exit(EXIT_FAILURE);
  }

  /* Envoie des Entrées */
  for (int i = 0; i < nb_entry; i++) {
    sprintf(buffer, "%s %s", myRoutingTable.tab_entry[i], myId);
    if (sendto(sock, buffer, BUF_SIZE_OUT, 0, (struct sockaddr *) &addr, addr_len) == -1) {
      perror("sendto()");
      exit(EXIT_FAILURE);
    }
  }

  exit(EXIT_SUCCESS);
}
