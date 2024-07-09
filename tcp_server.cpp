#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>


int main()
{
    int server_fd; //Declaration de l'id de la socket serveur
    int new_socket; //Declaration de l'id de la nouvelle socket pour la connexion client

    struct sockaddr_in address; //Structure pour stocker les addresses IP et les numeros de port pour IPv4
    int opt = 1; //Option pour permettre le redemarrage rapide du serveur
    int addrlen = sizeof(address); //Taille de la structure adresse 
    char buffer[1024] = {0}; //Buffer pour stocker les donnees recues
    const char *hello = "Hello from server\n"; //Message a envoyer au client 

    //Creation de la socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0 )
    {
        perror("socket failed"); //Affiche un message d'erreur si la creation echoue
        exit(EXIT_FAILURE); //Termine le programme avec un code d'erreur 
    }

    //Configuration des options de la socket
    //Attache la socket au port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) //autorise la reutilisation de l'addresse locale et du port en cas de redemarrage du server
    {
        perror("setsockopt"); //Affiche un message d'erreur si la configuration echoue
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET; //Famille d'addresses IPv4
    address.sin_addr.s_addr = INADDR_ANY; //Adresse IP sur laquelle ecouter
    address.sin_port = htons(8080); //Numero de port sur lequel ecouter 

    //Demarrage de l'ecoute sur le port specifie 
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) //associe la socket au port 8080 sur toutes les interfaces reseau de la machine
    {
        perror("bind failed");
        exit (EXIT_FAILURE);
    }

    //Ecoute du port
    if (listen(server_fd, 3) < 0)
    {
        perror ("listen");
        exit (EXIT_FAILURE);
    }
    // Accepte une nouvelle connexion
    //bloque le programme jusqu'a ce qu'une connexion entrante soit etablie, accept renvoie un nouveau fd representant la connexion avec le client
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) 
    {
        perror("accept");
        exit (EXIT_FAILURE);
    }
    //Lecture du message du client
    while (recv(new_socket, buffer, 1024, 0) > 0)//lit les donnees envoyees par le client dans le buffer 
    {
        std::cout << "Message from client: " << buffer << std::endl;

        //Envoi d'une reponse au client
        send(new_socket, hello, strlen(hello), 0); //envoie la chaine au client connecte 
        std::cout << "Hello message sent." << std::endl;
    }
    //Fermeture de la socket
    close(new_socket);
    close(server_fd);

    return(0);
}