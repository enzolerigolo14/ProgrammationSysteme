#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

typedef struct {
    char nom[50];
    char prenom[50];
    int indice_matiere; 
} Requete;

int main() {
    int client_fd;
    struct sockaddr_in server_addr;
    Requete requete;
    float moyenne;

    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("Erreur lors de la création de la socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(12345);
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Adresse IP invalide");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Erreur lors de la connexion au serveur");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    printf("Connecté au serveur\n");

    printf("Entrez le nom de l'étudiant : ");
    scanf("%19s", requete.nom); // Lit jusqu'à 19 caractères, evite débordement

    printf("Entrez le prénom de l'étudiant : ");
    scanf("%19s", requete.prenom); // Lit jusqu'à 19 caractères, evite débordement

    printf("Entrez l'indice de la matière (0-4) : ");
    scanf("%d", &requete.indice_matiere);

    if (write(client_fd, &requete, sizeof(requete)) == -1) {
        perror("Erreur lors de l'envoi de la requête");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    printf("Requête envoyée au serveur\n");

    if (read(client_fd, &moyenne, sizeof(moyenne)) == -1) {
        perror("Erreur lors de la réception de la réponse");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    if (moyenne == -1) {
        printf("Erreur : Étudiant ou matière introuvable.\n");
    } else {
        printf("La moyenne de %s %s pour la matière %d est : %.2f\n", requete.nom, requete.prenom, requete.indice_matiere, moyenne);
    }

    close(client_fd);
    printf("Connexion fermée.\n");

    return 0;
}
