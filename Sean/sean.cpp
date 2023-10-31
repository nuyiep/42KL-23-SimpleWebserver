#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <fstream>

#define	RESET "\033[0m"
#define GREEN "\033[32m"
#define BLUE  "\033[34m"
#define MAGENTA "\033[35m"
#define YELLOW "\033[33m"
#define CYAN  "\033[36m"
#define RED "\033[31m"

typedef struct s_serverData
{
	int			clientSocket;
	std::string	method;
	std::string	route;
	std::string	header;
	std::string body;
	std::string	response;
}	serverData;

/* Accept a connection on a socket */
/* accept function*/
/* first parameter- descriptor that identifies the socket */
/* second parameter- optional structure containing the client address info */
/* third parameter- optional length in bytes of the address structure */
/* If no error occurs, accept() returns a value of type SOCKET that is a */
/* descriptor for the new socket that is connected to the client. */
/* The original listening socket can be used to listen for more incoming calls */
void	accept_connection(int port, int &clientSocket, int server_fd) {
	std::cout << YELLOW << "(" << port << ") Waiting for connection..." << RESET << std::endl;
	clientSocket = accept(server_fd, NULL, NULL);
	if (clientSocket < 0)
	{
		perror("Accept failed");            
		exit(EXIT_FAILURE);        
	}
	printf("\033[32mConnection accepted\n\033[0m");
}

void	receive(serverData &data) {
	char buffer[1024];
	bzero(buffer, 1024);
	recv(data.clientSocket, buffer, 1024, 0);
	
	std::string sbuffer = buffer;
	data.header = sbuffer.substr(0, sbuffer.find("\r\n\r\n"));
	data.body = sbuffer.substr(sbuffer.find("\r\n\r\n") + 4);

	std::cout << CYAN << "Message received:" << std::endl;
	std::cout << MAGENTA << "Header:\n" << data.header  << "\n" << std::endl;
	std::cout << BLUE << "Body:\n" << data.body << RESET << std::endl;

	data.method = data.header.substr(0, data.header.find(" "));
	data.route = data.header.substr(data.header.find(" ") + 1, data.header.find("HTTP") - data.header.find(" ") - 2);
	std::cout << MAGENTA << "Method: " << data.method << RESET << std::endl;
	std::cout << MAGENTA << "Route: " << data.route << RESET << std::endl;
}

/**
 * @brief Post Request
 * 
 * @param data 
 * Example where file cannot be opened:
 * 			- Permission issues, file is a directory
 */
void	handlePost(serverData &data) { // If file exists, throw 400
	std::string file_path = "." + data.route;

	std::ofstream file_stream(file_path, std::ios::out | std::ios::trunc);

	if (file_stream.is_open()) {
		file_stream << data.body;
		file_stream.close();
		std::cout << "File created successfully" << std::endl;
	}
	else
	{
		std::cout << "Unable to create file" << std::endl;
		data.response = "HTTP/1.1 400 Bad Request\r\n";
	}
	data.response += "Hello from the server! POST request received.";
}

void	handleGet(serverData &data) { // If file does not exists, throw 404
	std::string file_path = "." + data.route;
	std::cout << data.route << std::endl;
	std::string line; ///Users/plau/23-SimpleWebserv/index.html

	std::ifstream file_stream(file_path);

	try
	{
		if (file_stream.is_open()) {
			while (getline(file_stream, line))
				data.response += line + "\n";
			file_stream.close();
		}
		else
		{
			std::cout << RED << "Unable to open file" << std::endl;
			data.response = "HTTP/1.1 404 Not Found\r\n";
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}

void	handlePatch(serverData &data) { // If file does not exists, throw 404
	std::string file_path = "." + data.route;

	std::ofstream file_stream(file_path);

	if (file_stream.is_open()) {
		file_stream.clear();
		file_stream << data.body;
		file_stream.close();
		std::cout << "File edited successfully" << std::endl;
	}
	else
		std::cout << "Unable to create file" << std::endl;
	data.response += "Hello from the server! PATCH request received.";
}

void	handleDelete(serverData &data) { // If file does not exists, throw 404
	std::string file_path = "." + data.route;

	if (std::remove(file_path.c_str()) != 0)
		std::cout << "Unable to delete file" << std::endl;
	else
		std::cout << "File deleted successfully" << std::endl;
	data.response += "Hello from the server! DELETE request received.";
}

void	respond(serverData &data) {
	data.response = "HTTP/1.1 200 OK\r\n";
	data.response += "\r\n";

	if (data.method == "GET")
		handleGet(data);
	else if (data.method == "POST")
		handlePost(data);
	else if (data.method == "PATCH")
		handlePatch(data);
	else if (data.method == "DELETE")
		handleDelete(data);
	send(data.clientSocket, data.response.c_str(), data.response.length(), 0);
	std::cout << GREEN << "Message replied:\n" << data.response << RESET << std::endl;
	close(data.clientSocket);
	printf("Socket is closed\n");
}

/* Create a socket that is bound to a specific TSP */
int	create_socket() {
	int server_fd = socket(AF_INET, SOCK_STREAM, 0); // connection
	if (server_fd < 0)
	{
		perror("cannot create socket"); 
		exit(EXIT_FAILURE); 
	}
	else
		printf("Socket created\n");
	return (server_fd);
}

/* Bind a local address/settings with a socket */
/* server_fd - descriptor of the unbound socket */
/* address - address to assign to the socket from the sockaddr structure */
/* length in bytes of the address structure */
void	bind_socket(int server_fd, int port) {
	/* connection's setting */
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	}
}

/* Listen if any calls from client */
void listen_socket(int server_fd) {
	if (listen(server_fd, 3) < 0) 
	{ 
		perror("listen failed"); 
		exit(EXIT_FAILURE); 
	}
}

int	main(int ac, char **av) {
	int	server_fd;
	int port;
	serverData data;

	if (ac != 2) {
		printf("Usage: %s <port>\n", av[0]);
		return 1;
	}
	port = atoi(av[1]);
	server_fd = create_socket();
	bind_socket(server_fd, port);
	listen_socket(server_fd);
	while (1) {
		accept_connection(port, data.clientSocket, server_fd);
		receive(data);
		respond(data);		
	}
}
