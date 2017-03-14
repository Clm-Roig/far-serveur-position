#ifndef DEF_FILS
#define DEF_FILS

// EN-TETE fils.h

// Sockets / HTTP / IP
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// Threads 
#include <pthread.h>

// Base 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// ---- CONSTANTES ---- //
const int LONGUEUR_GRILLE;
const int LARGEUR_GRILLE;

// ---- FONCTIONS ---- //

/**
    @brief génère un fils en dupliquant le processus, puis renvoie le pid
    @return int, pid du fils si tout s'est bien passé, 0 sinon
*/
int creerFils(); 

// ---- COMMUNICATION PERE ---- //
/**
    @brief gère l'envoie d'informations du fils vers un autre processus
    @param char* nomPipe, le nom du pipe
    @param char* donnees, le contenu des données envoyées
    @return int, 1 si tout s'est bien passé, 0 sinon
*/
int filsEnvoiPere(char* nomPipe, char* donnees);


/**
    @brief gère la reception d'informations du fils depuis un autre processus
    @param char* nomPipe, le nom du pipe
    @return int, 1 si tout s'est bien passé, 0 sinon
*/
char* filsReceptionPere(char* nomPipe);


/**
    @brief permet la saisie de deux entiers dans un tableau.
    @return int*, [x,y] avec x la coordonnée en longueur et y en largueur si tout s'est bien passé, [-1,-1] sinon
*/
int* saisirCoord();


// ---- COMMUNICATION FILS ---- //

/**
    @brief 
    @param char* nomPipe, le nom du pipe
    @param char* donnees, le contenu des données envoyées
    @return int, 1 si tout s'est bien passé, 0 sinon
*/
int filsEnvoiFils(char* nomPipe, char* donnees);


int filsReceptionFils();
#endif
