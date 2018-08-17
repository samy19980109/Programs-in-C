#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXNAME 80  /* maximum permitted name size, not including \0 */
#define NPITS 6  /* number of pits on a side, not including the end pit */
#define NPEBBLES 4 /* initial number of pebbles per pit */
#define MAXMESSAGE (MAXNAME + 50) /* initial number of pebbles per pit */

int port = 3000;
int listenfd;

struct player {
    int fd;
    char name[MAXNAME+1];
    int pits[NPITS+1];  // pits[0..NPITS-1] are the regular pits
                        // pits[NPITS] is the end pit
    //other stuff undoubtedly needed here
    struct player *next;
    int has_turn;
};
struct player *playerlist = NULL;
char Welcome_msg[] = "Welcome to Mancala. What is your name?\r\n";

int if_has_turn(int fd){
  for (struct player *p = playerlist; p; p = p->next){
    if (fd == p->fd){
      if (p->has_turn == 1){
        return 1;
      }
      return 0;
    }
  }
  return 0;
}

int accept_connection(int listenfd) {
    struct sockaddr_in peer;
    unsigned int peer_len = sizeof(peer);
    peer.sin_family = PF_INET;

    fprintf(stderr, "Waiting for a new connection...\n");
    int client_socket = accept(listenfd, (struct sockaddr *)&peer, &peer_len);
    if (client_socket < 0) {
        perror("accept");
        return -1;
    } else {
        fprintf(stderr,
            "New connection accepted from %s:%d\n",
            inet_ntoa(peer.sin_addr),
            ntohs(peer.sin_port));
            write(client_socket, Welcome_msg, strlen(Welcome_msg) + 1);
        return client_socket;
    }
}

extern void parseargs(int argc, char **argv);
extern void makelistener();
extern int compute_average_pebbles();
extern int game_is_over();  /* boolean */
extern void broadcast(char *s);  /* you need to write this one */

void make_player(int fd, char *s){
  struct player *newplayer = malloc(sizeof(struct player));
  newplayer->fd = fd;
  strcpy(newplayer->name, s);
  for (int i = 0; i<NPITS; i++){
    newplayer->pits[i] = compute_average_pebbles();
  }
  newplayer->has_turn = 0;
  newplayer->pits[NPITS] = 0;
  if (fd == 4){
    newplayer->has_turn = 1;
  }
  newplayer->next = playerlist;
  playerlist = newplayer;
  printf("made player with username %s\n\n", newplayer->name);

}



void broadcastToOne(int fd, char *s){
  for (int i = 0; i < strlen(s); i++){
    if (s[i] == '\n' || (s[i] == '\r' && s[i+1] == '\n')){
      s[i] = '\n';
      s[i+1] = '\0';
    }
  }
  write(fd, s, MAXMESSAGE + 1);
}

void broadcastAllButOne(int fd, char *s){
  for (int i = 0; i < strlen(s); i++){
    if (s[i] == '\n' || (s[i] == '\r' && s[i+1] == '\n')){
      s[i] = '\n';
      s[i+1] = '\0';
    }
  }
  strcat(s, "\0");
  for (struct player *p = playerlist; p; p = p->next) {
    if (p->fd != fd){
      broadcastToOne(p->fd, s);
    }
  }
}

void broadcastBoard(){
  for (struct player *p = playerlist; p; p = p->next){
    char situation[MAXMESSAGE+1] = "";
    situation[MAXMESSAGE] = '\0';
    char pits[7];
    pits[6] = '\0';
    char lastpit[18];
    lastpit[17] = '\0';

    snprintf(situation, MAXNAME+3,"%s:  ", p->name);
    for (int i = 0; i < NPITS;i++){
      sprintf(pits, " [%d]%d", i, p->pits[i]);
      strncat(situation, pits, 7);
    }

    snprintf(lastpit, 18, " [end pit]%d\r\n", p->pits[NPITS]);
    strncat(situation, lastpit, 18);
    strncat(situation, "\0", 1);

    broadcast(situation);

  }
}

int make_move(int fd, int move){
  struct player *cur;
  struct player *prev;
  int flag = 0;

  for (cur = playerlist; cur; cur = cur->next){
    if (fd == cur->fd){
      break;
    }
  }
  char send[MAXMESSAGE +1];
  send[MAXMESSAGE] = '\n';
  printf("\n%s made the move %d\n", cur->name, move);
  sprintf(send, "%s made the move %d", cur->name, move);
  strcat(send, "\r\n");
  broadcast(send);
  // printf("%s\n", send);
  int i = move;
  int number = cur->pits[move];
  // printf("%d is the number of pebbles in that pit\n", number);
  cur->pits[move] = 0;
  while (number != 0){
    if (fd == cur->fd){ // if current fd
      while (i+1< 7 && number != 0){
        cur->pits[i+1] = cur->pits[i+1] + 1;
        i += 1;
        number -= 1;
        if (number == 0 && i == 6){
          flag = 1;
        }
      }
    }// if current fd

    // find previous player
    for (prev = playerlist; prev; prev = prev->next) {
      if (prev->next == cur){
        break;
      }
    }
    if (playerlist == cur){ // if current is last player
      for (prev = playerlist; prev->next!=NULL; prev = prev->next) {
        printf("");
      }
      //printf("prev  = %s\n", prev->name);
    }

    // append to previos player PITS
    int j = 0;
    while(number != 0 && j < 6){
      prev->pits[j] = prev->pits[j] + 1;
      j += 1;
      number -= 1;
    }

    // make current player previos player
    if (playerlist == cur){ // if current is last player
      for (prev = playerlist; prev->next!=NULL; prev = prev->next) {
        printf("");
      }
      cur = prev;
    }
    else{ // current is not last player
      cur = prev;
    }

  }// while
  broadcastBoard();
  // printf("returned %d\n", flag);
  return flag;
}

int main(int argc, char **argv) {
    char msg[MAXMESSAGE];
    char username[MAXNAME+1];

    parseargs(argc, argv);
    makelistener();


    int max_fd;
    max_fd = listenfd;
    fd_set all_fds;
    FD_ZERO(&all_fds);
    FD_SET(listenfd, &all_fds);

    while (!game_is_over()) {
      // select updates the fd_set it receives, so we always use a copy and retain the original.
      fd_set listen_fds = all_fds;
      int nready = select(max_fd + 1, &listen_fds, NULL, NULL, NULL);
      if (nready == -1) {
          perror("server: select");
          exit(1);
      }

      // Is it the original socket? Create a new connection ...
      if (FD_ISSET(listenfd, &listen_fds)){
        int player_fd = accept_connection(listenfd);

          // imp
          if (player_fd > max_fd){
            max_fd = player_fd;
          }
          FD_SET(player_fd, &all_fds);
          printf("Accepted connection\n");
        }// if FD_ISSET condition

        // printf("starting for\n");
        for (int i = 4; i <= max_fd; i++){
          int flag = 0;
          //printf(" Next fd is %d\n", i);

          if (FD_ISSET(i, &listen_fds)){
            // printf("gonna read username and i = %d\n", i);
            for (struct player *p = playerlist; p; p = p->next){
              if (i == p->fd){
                flag = 1;
                // printf("outside if on 254 and flag = %d and fd = %d\n", flag, p->fd);
              }

            }
            if (flag == 0){ // is a ready to talk and not in playerlist. Therefore is a new player

              read(i, username, MAXNAME+1);
              for (int i = 0; i < MAXNAME + 1; i++){
                if (username[i] == '\n' || (username[i] == '\r' && username[i+1] == '\n')){
                  username[i] = '\0';
                }
              }
              for (struct player *p = playerlist; p; p = p->next){
                if (strcmp(p->name, username) == 0){
                  strcpy(username, "\n");
                }
              }
              if (strcmp(username, "\n") == 0){
                write(i, "Please enter a valid username\n", 31);
                printf("not valid username\n");
              }
              else{
                printf("valid username\n");
                make_player(i, username);
                // strcat and then broadcast to all but one about the new player
                char messagetosend[100 + MAXNAME] = "New player joined with username = ";
                strcat(messagetosend, username);
                strcat(messagetosend, "\n");
                broadcastAllButOne(i, messagetosend);

                // broadcast the board repr
                broadcastBoard();
                if (i == 4){
                  write(4, "Your move?\n", 11);
                }
                FD_CLR(i ,&listen_fds);
              }
            }
            else{ // flag != 0. Therefore, the fd is ready to talk and is already in the list.
              // Therefore this is a move
              char move[MAXMESSAGE+1];
              struct player *p;
              int status = read(i, &move, MAXMESSAGE+1);

              if (status!=0){
              // making the move usable
              for (int i = 0; i < MAXMESSAGE + 1; i++){
                if (move[i] == '\n' || (move[i] == '\r' && move[i+1] == '\n')){
                  move[i] = '\0';
                }
              }

              // finding which player it is
              for (p = playerlist; p; p = p->next){
                if (i == p->fd){
                  break;
                }
              }

              // converting to int
              int real_move = strtol(move, NULL, 10);
              if (real_move < 0 || real_move > 5 || p->pits[real_move] == 0){
                write(i, "Please enter a valid move\n", 26);
              }
              else{
                if (make_move(i, real_move) == 1){
                  write(i, "YOUR MOVE AGAIN\n", 16);
                  char msg[MAXMESSAGE+1];
                  snprintf(msg, MAXMESSAGE, "It's %s's move again", p->name);
                  strcat(msg, "\n");
                  broadcastAllButOne(i, msg);
                }
                else{
                  if (playerlist->fd == i){
                    struct player *b;
                    for (b = playerlist; b->next!=NULL; b = b->next) {
                      printf("");
                    }
                    write(b->fd, "Your move?\n", 11);
                    char msg[MAXMESSAGE+1];
                    snprintf(msg, MAXMESSAGE, "It's %s's move", b->name);
                    strcat(msg, "\n");
                    broadcastAllButOne(b->fd, msg);
                  }
                  else{ // not the last guy, then another guy at the last
                    write(i+1, "YOUR MOVE\n", 10);

                    struct player *b;
                    for (b = playerlist; b->next!=NULL; b = b->next) {
                      if (b->fd == i+1){
                        break;
                      }
                    }
                    char msg[MAXMESSAGE+1];
                    snprintf(msg, MAXMESSAGE, "It's %s's move", b->name);
                    strcat(msg, "\n");
                    broadcastAllButOne(b->fd, msg);
                  }
                }
              }
            } // read didnt return 0
            else{
              struct player *b;
              struct player *prev;
              for (b = playerlist; b->next!=NULL; b = b->next) {
                if (b->fd == i){
                  break;
                }
              }

              // finding prev
              if(playerlist == b){ // last player
                // for (prev = playerlist; prev->next!=NULL; prev = prev->next) {
                //   printf("");
                // }
                printf("last player exiting\n");
                playerlist = b->next;

              }
              else{ // not last player
                for (prev = playerlist; prev->next!=NULL; prev = prev->next) {
                  if (prev->next == b){
                    break;
                  }
                }
              }

              FD_CLR(i, &listen_fds);
              char msg[MAXMESSAGE+1];
              snprintf(msg, MAXMESSAGE, "%s Exited", b->name);
              strcat(msg, "\n");
              broadcast(msg);
            }

            } //if flag = 0
          }// if fd is ready to talk
        } // for i = 4
      }// while loop

    broadcast("Game over!\r\n");
    printf("Game over!\n");
    for (struct player *p = playerlist; p; p = p->next) {
        int points = 0;
        for (int i = 0; i <= NPITS; i++) {
            points += p->pits[i];
        }
        printf("%s has %d points\r\n", p->name, points);
        snprintf(msg, MAXMESSAGE, "%s has %d points\r\n", p->name, points);
        broadcast(msg);
    }

    if (game_is_over()){
      int number_of_players = 0;
      for (struct player *p = playerlist; p->next!=NULL; p = p->next){
        number_of_players += 1;
      }

      while(number_of_players-1 != 0){
        for (struct player *last = playerlist; last; last = last->next){
          printf("freed %s\n", last->name);
          free(last);
        }
        number_of_players -= 1;
      }
    }
    return 0;
}


void parseargs(int argc, char **argv) {
    int c, status = 0;
    while ((c = getopt(argc, argv, "p:")) != EOF) {
        switch (c) {
        case 'p':
            port = strtol(optarg, NULL, 0);
            break;
        default:
            status++;
        }
    }
    if (status || optind != argc) {
        fprintf(stderr, "usage: %s [-p port]\n", argv[0]);
        exit(1);
    }
}


void makelistener() {
    struct sockaddr_in r;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    int on = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
               (const char *) &on, sizeof(on)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    memset(&r, '\0', sizeof(r));
    r.sin_family = AF_INET;
    r.sin_addr.s_addr = INADDR_ANY;
    r.sin_port = htons(port);
    if (bind(listenfd, (struct sockaddr *)&r, sizeof(r))) {
        perror("bind");
        exit(1);
    }

    if (listen(listenfd, 5)) {
        perror("listen");
        exit(1);
    }
}



/* call this BEFORE linking the new player in to the list */
int compute_average_pebbles() {
    struct player *p;
    int i;

    if (playerlist == NULL) {
        return NPEBBLES;
    }

    int nplayers = 0, npebbles = 0;
    for (p = playerlist; p; p = p->next) {
        nplayers++;
        for (i = 0; i < NPITS; i++) {
            npebbles += p->pits[i];
        }
    }
    return ((npebbles - 1) / nplayers / NPITS + 1);  /* round up */
}


int game_is_over() { /* boolean */
    int i;

    if (!playerlist) {
       return 0;  /* we haven't even started yet! */
    }

    for (struct player *p = playerlist; p; p = p->next) {
        int is_all_empty = 1;
        for (i = 0; i < NPITS; i++) {
            if (p->pits[i]) {
                is_all_empty = 0;
            }
        }
        if (is_all_empty) {
            return 1;
        }
    }
    return 0;
}

void broadcast(char *s){
  for (struct player *p = playerlist; p; p = p->next) {
    write(p->fd, s, strlen(s));
  }
}
