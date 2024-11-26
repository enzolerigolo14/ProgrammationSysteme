#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <arpa/inet.h>


typedef struct {
    char nom[50];
    char prenom[50];
    float moyennes[5];
} Etudiant;

typedef struct {
    char nom[50];
    char prenom[50];
    int indice_matiere;
} Requete;


Etudiant etudiants[50] = {
		{"Berot", "Mathis", {14.5, 12.4, 16.3, 10.9, 16.2}},
		{"Caillet", "Enzo", {11.2, 14.7, 13.5, 15.4, 16.1}}
};
int nombre_etudiants = 2;



int compare_chaines(const char* str1, const char* str2) {
    while (*str1 && *str2) {
        if (tolower(*str1 )!= tolower(*str2)) {
            return 0; // Les chaînes sont différentes
        }
        str1++;
        str2++;
    }
    return (*str1 == '\0' && *str2 == '\0');
}

float rechercher_moyenne(const char* nom, const char* prenom, int indice_matiere) {
    for (int i = 0; i < nombre_etudiants; i++) {
        if (compare_chaines(etudiants[i].nom, nom) && compare_chaines(etudiants[i].prenom, prenom)) {
            if (indice_matiere >= 0 && indice_matiere < 5) {
                return etudiants[i].moyennes[indice_matiere];
            } else {
                fprintf(stderr, "Indice de matière invalide\n");
                return -1; 
            }
        }
    }
    fprintf(stderr, "Étudiant introuvable\n");
    return -1;
}
int main() {
	
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Erreur lors de la création de la socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Écoute sur toutes les interfaces
    server_addr.sin_port = htons(12345);      // Port d'écoute

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Erreur lors du bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) == -1) {
        perror("Erreur lors du listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Serveur en attente de connexion...\n");

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd == -1) {
            perror("Erreur lors de l'acceptation");
            continue;
        }
        printf("Client connecté\n");

        // Réception de la requête
        Requete requete;
        if (read(client_fd, &requete, sizeof(requete)) <= 0) {
            perror("Erreur lors de la réception de la requête");
            close(client_fd);
            continue;
        }

        printf("Requête reçue : %s %s matière %d\n", requete.nom, requete.prenom, requete.indice_matiere);

        float moyenne = rechercher_moyenne(requete.nom, requete.prenom, requete.indice_matiere);

        if (write(client_fd, &moyenne, sizeof(moyenne)) <= 0) {
            perror("Erreur lors de l'envoi de la réponse");
        } else {
            printf("Réponse envoyée : %.2f\n", moyenne);
        }

        close(client_fd);
        printf("Client déconnecté\n");
    }

    close(server_fd);
    return 0;
}
