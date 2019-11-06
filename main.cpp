#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {

    int listening = socket(AF_INET, SOCK_STREAM, 0);

    if (listening == -1) {
        cerr << "Can't create a socket!" << endl;
        return -1;
    }

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(3000);

    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);
    if (bind(listening, (sockaddr *)&hint, sizeof(hint)) == -1) {
        cerr << "Can't bind to ip/port" << endl;
        return -2;
    }

    if (listen(listening, SOMAXCONN) == -1) {
        cerr << "Can't listen" << endl;
        return -3;
    }

    sockaddr_in client;
    socklen_t clientSize = sizeof(client);

    char host[NI_MAXHOST];
    char svc[NI_MAXSERV];

    int clientSocket = accept(
        listening,
        (sockaddr *)&client,
        &clientSize);

    if (clientSocket == -1) {
        cerr << "Problem with client connecting!" << endl;
        return -4;
    }

    close(listening);

    memset(host, 0, NI_MAXHOST);
    memset(svc, 0, NI_MAXSERV);

    int result = getnameinfo(
        (sockaddr *)&client,
        sizeof(client),
        host,
        NI_MAXHOST,
        svc,
        NI_MAXSERV,
        0);

    if (result == 0) {
        cout << host << " connected on " << svc << endl;
    } else {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        cout << host << " connected on " << ntohs(client.sin_port) << endl;
    }

    char buf[4096];
    while (true) {
        memset(buf, 0, 4096);
        int bytesRecv = recv(clientSocket, buf, 4096, 0);
        if (bytesRecv == -1) {
            cerr << "There was a connection issue" << endl;
            break;
        }
        if (bytesRecv == 0) {
            cout << "The client disconnected" << endl;
            break;
        }
        cout << "Received: " << string(buf, 0, bytesRecv) << endl;

        send(clientSocket, buf, bytesRecv + 1, 0);
    }

    close(clientSocket);
    
    return 0;
}