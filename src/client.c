// Fichier client.c

#include "client.h"

// ---- CONSTANTES ---- //
const char* SEPARATION = ",";
const int PORT_ROBOT = 2048;

// ---- VARIABLES ---- //
/* !! TODO remplacer 'testVB' par le canal dans lequel publier (ex: partie12)
    (ici msg est la "ressource" que ce canal attend */
//char* canal = "testVB";

    /* Par convention dans FAR on parle sur ressource "msg"
      sur laquelle on envoie une chaine contenant les couples clef:valeur separes par des virgules */
char* ressource = "msg";
char* ressource2 = "msg?limit=15&time-range=1hour";

    // !! TO DO : mettre ci-dessous le token du canal !!
    // canal partie1 : 1496910572272_aS7IrPZzKJEARkIm
    // canal GPpartie1 : 1496739734657_0fJejrx6NNqf6L5Q
    // canal testVB : 1494771555601_5SGQdxJaJ8O1HBj4
    
char* clefCanal = "1494793564147_KNl54g97mG89kQSZ";
char* clefCanalGP = "1496910685059_uvgRhRg7UKmOUtfU";


// ---- FONCTIONS ---- //
int sock;

void Error(char *mess) {
    fprintf(stderr,"%s\n",mess);
    if (sock) close(sock);
    exit(-1);
}

int envoiBeebotte(char *data[], char* canal) {
    char *host = "api.beebotte.com";

    char path[100] = "/v1/data/write/";
    strcat(path,canal); 
    strcat(path,"/"); 
    strcat(path,ressource);

    struct hostent *server;
    struct sockaddr_in serv_addr;
    int sockfd, bytes, sent, received, total, message_size;
    char *message, response[4096];

    // Necessaire pour envoyer des donnees sur beebotte.com (noter le token du canal a la fin) :
    char headers[300] ="Host: api.beebotte.com\r\nContent-Type: application/json\r\nX-Auth-Token: ";
    if(strcmp(canal,"GPpartie1") == 0) {
        strcat(headers,clefCanalGP);
    }   
    else {
        strcat(headers,clefCanal);
    }

    strcat(headers,"\r\n");

    char donnees[4096] = "{\"data\":\""; // "data" est impose par beebotte.com

    // Norme d'envoi projet FAR
    strcat(donnees,"type_msg=");
    strcat(donnees,data[0]);
    strcat(donnees,",");

    strcat(donnees,"type_ent=");
    strcat(donnees,data[1]);
    strcat(donnees,",");

    strcat(donnees,"num=");
    strcat(donnees,data[2]);
    strcat(donnees,",");

    strcat(donnees,"data=");
    strcat(donnees,data[3]);
    strcat(donnees,"\"}");

    /* How big is the whole HTTP message? (POST) */
    message_size=0;
    message_size+=strlen("%s %s HTTP/1.0\r\n")+strlen("POST")+strlen(path)+strlen(headers);
    message_size+=strlen("Content-Length: %d\r\n")+10+strlen("\r\n")+strlen(donnees);

    message=malloc(message_size);

    /* Construit le message POST */
    sprintf(message,"POST %s HTTP/1.0\r\n",path);
    sprintf(message+strlen(message), "%s",headers);
    sprintf(message+strlen(message),"Content-Length: %d\r\n",(int)strlen(donnees));
    strcat(message,"\r\n");              // blank line
    strcat(message,donnees);             // body

    // printf("Request:\n%s\n-------------\n",message);

    // Socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        Error("ERROR opening socket");
    }

    // Host
    server = gethostbyname(host);
    if (server == NULL) {
        Error("ERROR, no such host");
    }

    /* fill in the structure (port 80)*/
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(80);
    memcpy(&serv_addr.sin_addr.s_addr,server->h_addr,server->h_length);

    /* connect the socket */
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) {
        Error("ERROR connecting");
    }

    total = strlen(message);
    sent = 0;
    do {
        bytes = write(sockfd,message+sent,total-sent);
        if (bytes < 0)
            Error("ERROR writing message to socket");
        if (bytes == 0)
            break;
        sent+=bytes;
    } while (sent < total);

    /* receive the response */
    memset(response,0,sizeof(response));
    total = sizeof(response)-1;
    received = 0;
    do {
        bytes = read(sockfd,response+received,total-received);
        if (bytes < 0) Error("ERROR reading response from socket");
        if (bytes == 0)
            break;
        received+=bytes;
    } while (received < total);

    if (received == total) {
        Error("ERROR storing complete response from socket");
    }

    close(sockfd);

    // printf("Response:\n%s\n",response);
    
    free(message);

    return 0;
}

char* recepBeebotte(char* typedonnee, char* canal) {
    //http://api.beebotte.com/v1/public/data/read/vberry/testVB/msg?limit=2&time-range=1hour
    char *host = "api.beebotte.com";

    char path[200] = "/v1/public/data/read/vberry/";
    strcat(path,canal); strcat(path,"/"); strcat(path,ressource2);

    struct hostent *server;
    struct sockaddr_in serv_addr;
    int sockfd, bytes, sent, received, total;
    char* message = malloc(4096*sizeof(char));

    strcat(message,"GET ");
    strcat(message,path);
    strcat(message," HTTP/1.0\r\n");
    strcat(message,"Host: ");
    strcat(message,host);
    strcat(message,"\r\n\r\n");

    // Socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        Error("ERROR opening socket");
    }

    // Host
    server = gethostbyname(host);
    if (server == NULL) {
        Error("ERROR, no such host");
    }

    /* fill in the structure (port 80)*/
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(80);
    memcpy(&serv_addr.sin_addr.s_addr,server->h_addr,server->h_length);

    /* connect the socket */
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) {
        Error("ERROR connecting");
    }

    printf("\nRequete : %s", message);
    /* receive the response */
    total = strlen(message);
    sent = 0;
    do {
        bytes = write(sockfd,message+sent,total-sent);
        if (bytes < 0)
            Error("ERROR writing message to socket");
        if (bytes == 0)
            break;
        sent+=bytes;
    } while (sent < total);

    // TODO : à changer
    char response[50000];


    memset(response,0,sizeof(response));
    total = sizeof(response)-1;
    received = 0;
    do {
        bytes = read(sockfd,response+received,total-received);
        printf("\n");
        if (bytes < 0) Error("ERROR reading response from socket");
        if (bytes == 0)
            break;
        received+=bytes;
    } while (received < total);

    if (received == total) {
        Error("ERROR storing complete response from socket");
    }
    close(sockfd);

    /* Traitement du message reçu*/
    printf("%s", response);
    char typeMsg[200]="type_msg=";
    strcat(typeMsg,typedonnee);

    char* verifType = strstr(response, typeMsg);

    char* verifData;
    char* data;
    data = malloc(200*sizeof(char));
    if (verifType == NULL) {
        printf("Pas de message trouvé");
    }
    else {
        verifData = strstr(verifType, "data=");
        verifData += strlen("data=");
        data = strtok(verifData,"\"");
    }
    return data;
}

int envoiRobot(char* donnees, char* addrRobot){
    /* CE CODE FAIT TOUT PLANTER

    int res = 1;
    int sockBot = initSocket(PORT_ROBOT, addrRobot,"null");
    if(sendToSocket(sockBot,donnees) == -1) {
        res = 0;
    }
    close(sockBot);
    return res;
    */

    // On passe par un appel système plutôt
    char* commande = malloc(256*sizeof(char));
    sprintf(commande,"echo \"%s\" | nc %s %d",donnees,addrRobot,PORT_ROBOT);
    system(commande);
    return 1;
}

int checkDebutPartie() {
    return 0;
}

int checkFinPartie() {
    return 0;
}
