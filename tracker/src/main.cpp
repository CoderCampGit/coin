#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

int main() {
  // Create socket for listening incoming connections
  int listening = socket(AF_INET, SOCK_STREAM, 0);
  
  if (listening == -1) {
    cerr << "Can't create a socket!";
    return -1;
  }

  // hint is a port
  sockaddr_in hint;
  // hint using IP_v4
  hint.sin_family = AF_INET;
  // hint is 5400 port
  hint.sin_port = htons(54000);
  // hint address is 0.0.0.0, it means ANY
  inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);
  
  // try to bind port to socket
  if(bind(listening, (sockaddr*) &hint, sizeof(hint)) == -1) {
    cerr << "Can't bind to IP/port";
    return -2;
  }

  // start listening for connections
  if (listen(listening, SOMAXCONN) == -1) {
    cerr << "Can't listen";
    return -3;
  }
  // client port
  sockaddr_in client;
  // size of client struct
  socklen_t clientSize = sizeof(client);
  // client IP
  char host[NI_MAXHOST];
  // client service
  char svc[NI_MAXSERV];

  // accept connection from client
  int clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

  if(clientSocket == -1) {
    cerr << "Problem with client connecting!";
    return -4;
  }

  // stop listening
  close(listening);

  // empty client IP and port buffers
  memset(host, 0, NI_MAXHOST);
  memset(svc, 0, NI_MAXSERV);

  // get info about client
  int result = getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, svc, NI_MAXSERV, 0);
  
  if(result) {
    // if service name found print service
    cout << host << " connected on " << svc << endl;
  } else {
    // else get client port and print it
    inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
    cout << host << " connected on " << ntohs(client.sin_port) << endl;
  }

  // buffer for client messages
  char buf[4096];

  // while true listen client
  while(true) {
    // empty buffer
    memset(buf, 0, 4096);

    // receive client message
    int bytesRecv = recv(clientSocket, buf, 4096, 0);

    // if message go shitty exit
    if(bytesRecv == -1) {
      cerr << "There was a connection issue" << endl;
      break;
    }

    // if disconnect message then exit
    if(bytesRecv == 0) {
      cout << "The client disconnected" << endl;
      break;
    }

    // print message in cout
    cout << "Received: " << string(buf, 0, bytesRecv) << endl;
    
    // echo it to client
    send(clientSocket, buf, bytesRecv + 1, 0);
  }

  // close client port
  close(clientSocket);

  return 0;
}
