#ifndef SERVER_HPP
# define SERVER_HPP

# include <stdexcept>

class Server {
public:
    Server();
    ~Server();

    // Méthode pour démarrer le serveur en passant le port en paramètre
    void start(const char* portStr);

private:
    int _serverSocket; // Descripteur de la socket serveur
    int _port;         // Port d'écoute (converti en entier)
};

#endif
