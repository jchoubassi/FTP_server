
//=======================================================================================================================
// Course: 159.342
// Description: Cross-platform, Active mode FTP SERVER, Start-up Code for Assignment 1 
//
// This code gives parts of the answers away but this implementation is only IPv4-compliant. 
// Remember that the assignment requires a fully IPv6-compliant cross-platform FTP server that can communicate with a 
// built-in FTP client either in Windows 11, Ubuntu Linux or MacOS.
// 
// This program is cross-platform but your assignment will be marked only in Windows 11.
//
// You must change parts of this program to make it IPv6-compliant (replace all data structures and functions that work only with IPv4).
//
// Hint: The sample TCP server codes show the appropriate data structures and functions that work in both IPv4 and IPv6. 
// We also covered IPv6-compliant data structures and functions in our lectures.   
//
// Author: n.h.reyes@massey.ac.nz
//=======================================================================================================================

#define USE_IPV6 true  //Prev set to false

#if defined __unix__ || defined __APPLE__
  #include <unistd.h>
  #include <errno.h>
  #include <stdlib.h>
  #include <stdio.h>
  #include <string.h>
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <arpa/inet.h>
  #include <netdb.h> //used by getnameinfo()
  #include <iostream>

#elif defined __WIN32__
  #include <ws2tcpip.h>
  #include <winsock2.h>
  #include <ws2tcpip.h> //required by getaddrinfo() and special constants
  #include <stdlib.h>
  #include <stdio.h>
  #include <iostream>
  #include <direct.h> 
  
  #define WSVERS MAKEWORD(2,2) /* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
//The high-order byte specifies the minor version number; 
//the low-order byte specifies the major version number.
  WSADATA wsadata; //Create a WSADATA object called wsadata. 
#endif

#define BUFFER_SIZE 256
enum class FileType{BINARY, TEXT, UNKNOWN};

FileType file_type;  

//MAIN
int main(int argc, char *argv[])  {
// INITIALIZATION
file_type = FileType::UNKNOWN;

#if defined __unix__ || defined __APPLE__
//nothing to do here

#elif defined _WIN32

	   int err = WSAStartup(WSVERS, &wsadata);

		 if (err != 0) {
		      WSACleanup();
			  // Tell the user that we could not find a usable WinsockDLL
		      printf("WSAStartup failed with error: %d\n", err);
		      exit(1);
		 }
#endif		 

		 struct sockaddr_in6 localaddr,remoteaddr; // replaced ipv4 with ipv6
		 struct sockaddr_in6 local_data_addr_act; // replaced ipv4 with ipv6		

#if defined __unix__ || defined __APPLE__

		 int s,ns;                //socket declaration
		 int ns_data, s_data_act; //socket declaration
#elif defined _WIN32			 
		 SOCKET s,ns;                //socket declaration
		 SOCKET ns_data, s_data_act; //socket declaration
#endif

		 char send_buffer[BUFFER_SIZE],receive_buffer[BUFFER_SIZE];
		
         // ns_data=INVALID_SOCKET;

#if defined __unix__ || defined __APPLE__
		  ns_data = -1;
#elif defined _WIN32
		  ns_data= INVALID_SOCKET;
#endif
char ip_decimal[INET6_ADDRSTRLEN];
		 int active=0;
		 int n,bytes,addrlen;
		 
		 printf("\n============================================\n");
		 printf("   << 159.342 Cross-platform FTP Server >>");
		 printf("\n============================================\n");
		 printf("   submitted by: Jordan Huang 23016360, Neishun Lopati 21012082, ... ");
		 printf("\n           date:     ");
		 printf("\n============================================\n");
	
		 
		 memset(&localaddr,0,sizeof(localaddr));//clean up the structure
		 memset(&remoteaddr,0,sizeof(remoteaddr));//clean up the structure
		 
//SOCKET
		 s = socket(AF_INET6, SOCK_STREAM, 0); // ipv6 compliant, will test


#if defined __unix__ || defined __APPLE__
		 if (s <0) {
			 printf("socket failed\n");
		 }
#elif defined _WIN32
		 if (s == INVALID_SOCKET) {
	 		printf("socket failed\n");
	 	 }
#endif

// set our default port if they don't set
#define DEFAULT_PORT "21"

// cleans up the structure (double check)
memset(&localaddr, 0, sizeof(localaddr));
// sets socket to use IPv6
localaddr.sin6_family = AF_INET6;

// getaddrinfo (where we are storing our address information),*result (think linklist for multiple connections but I think this has to do with ipv4 and ipv6)
struct addrinfo *result = NULL, hints; // provide hints for the addrinfo structure
int iResult; // not sure why they didn't use boolean but just checks 1 or 0 if the getaddrinfo was successful or not

// memclear again for hints
memset(&hints, 0, sizeof(struct addrinfo));
hints.ai_family = AF_INET6;  
hints.ai_socktype = SOCK_STREAM;
hints.ai_protocol = IPPROTO_TCP;
hints.ai_flags = AI_PASSIVE; 

// commandline, checks if user gave a port or not
const char *port = (argc == 2) ? argv[1] : DEFAULT_PORT;  

// make sure that all the values are there, if one of the values returns wrong, prints getaddrinfo....
iResult = getaddrinfo(NULL, port, &hints, &result);
if (iResult != 0) {
    printf("getaddrinfo failed: %d\n", iResult);
    // Clean up and exit
    WSACleanup();
    exit(1);
}
// mem clean
freeaddrinfo(result);

		 // old code
		//CONTROL CONNECTION:  port number = content of argv[1]
		// if (argc == 2) {
		//	 localaddr.sin6_port = htons((u_short)atoi(argv[1]));			 
		// }
		// else {
		//	 localaddr.sin6_port = htons(1234);
		// }
		// localaddr.sin6_addr = IN6ADDR_ANY;

//BIND
		 if (bind(s, result->ai_addr, (int)result->ai_addrlen) != 0) {
			 printf("Bind failed!\n");
			 exit(0);
		 }
//LISTEN
		 listen(s,5);
		
//INFINITE LOOP
		 int count=0;
		 //====================================================================================
		 while (1) {//Start of MAIN LOOP
		 //====================================================================================
			    addrlen = sizeof(remoteaddr);
//NEW SOCKET newsocket = accept  //CONTROL 
				struct sockaddr_in6 actual_addr;
				socklen_t len = sizeof(actual_addr);
				getsockname(s, (struct sockaddr*)&actual_addr, &len);
			 printf("\n------------------------------------------------------------------------\n");
			 printf("SERVER is waiting for an incoming connection request at port:%d", ntohs(actual_addr.sin6_port));
			 printf("\n------------------------------------------------------------------------\n");

#if defined __unix__ || defined __APPLE__ 
			 ns = accept(s,(struct sockaddr *)(&remoteaddr), (socklen_t*)&addrlen); 
#elif defined _WIN32 
			 ns = accept(s,(struct sockaddr *)(&remoteaddr), &addrlen); 
#endif

#if defined __unix__ || defined __APPLE__ 
			 if (ns < 0 ) break;
#elif defined _WIN32 
			 if (ns == INVALID_SOCKET) break;
#endif

			char client_ip[INET6_ADDRSTRLEN];
			inet_ntop(AF_INET6, &remoteaddr.sin6_addr, client_ip, sizeof(client_ip));
			 printf("\n============================================================================\n");
	 		 printf("connected to [CLIENT's IP %s , port %d] through SERVER's port %d", client_ip,
			         ntohs(remoteaddr.sin6_port),ntohs(localaddr.sin6_port));
			 printf("\n============================================================================\n");
			 printf("detected CLIENT's port number: %d\n", ntohs(remoteaddr.sin6_port));

			 //printf("connected to CLIENT's IP %s at port %d of SERVER\n",
			 //inet_ntoa(remoteaddr.sin_addr),ntohs(localaddr.sin_port));
			 
			 //printf("detected CLIENT's port number: %d\n", ntohs(remoteaddr.sin_port));
			//Respond with welcome message
			
			count=snprintf(send_buffer,BUFFER_SIZE,"220 FTP Server ready. \r\n");
			 if(count >=0 && count < BUFFER_SIZE){
			 	 bytes = send(ns, send_buffer, strlen(send_buffer), 0);

			 }
			//COMMUNICATION LOOP per CLIENT
			 while (1) {
				 
				 n = 0;
				 while (1) {
//RECEIVE MESSAGE AND THEN FILTER IT
				   bytes = recv(ns, &receive_buffer[n], 1, 0);//receive byte by byte...

					 if ((bytes < 0) || (bytes == 0)) break;
					 if (receive_buffer[n] == '\n') { /*end on a LF*/
						 receive_buffer[n] = '\0';
						 break;
					 }
					 if (receive_buffer[n] != '\r') n++; /*Trim CRs*/
				 
				 } 
				 
                 if(bytes == 0) printf("\nclient has gracefully exited.\n"); //2022

				 if ((bytes < 0) || (bytes == 0)) break;

				 printf("[DEBUG INFO] command received:  '%s\\r\\n' \n", receive_buffer);
//PROCESS COMMANDS/REQUEST FROM USER*				 

				 if (strncmp(receive_buffer,"USER",4)==0)  {
					 printf("USER command received.\n");
					 if (strncmp(receive_buffer, "USER napoleon", 13) == 0) {
						 count = snprintf(send_buffer, BUFFER_SIZE, "331 Password required\r\n");
					 }
					 else {
						 count = snprintf(send_buffer, BUFFER_SIZE, "530 Invalid User\r\n");
					 }
					 if (count >= 0 && count < BUFFER_SIZE) {
						 bytes = send(ns, send_buffer, strlen(send_buffer), 0);
					 }
					 printf("[DEBUG INFO] <-- %s\n", send_buffer);
					 if (bytes < 0) break;
				 }
				 //---
				 if (strncmp(receive_buffer, "PASS", 4) == 0) {
					 printf("PASS command received.\n");
					 if (strncmp(receive_buffer, "PASS 342", 8) == 0) {
						 count = snprintf(send_buffer, BUFFER_SIZE, "230 User logged in okay\r\n");
					 }
					 else {
						 count = snprintf(send_buffer, BUFFER_SIZE, "530 Password incorrect\r\n");
					 }
					 if (count >= 0 && count < BUFFER_SIZE) {
						 bytes = send(ns, send_buffer, strlen(send_buffer), 0);
					 }
					 printf("[DEBUG INFO] <-- %s\n", send_buffer);
					 if (bytes < 0) break;
				 }
				 //---
				 if (strncmp(receive_buffer,"SYST",4)==0)  {
					 printf("<--SYST command received.\n\n");
					 printf("Information about the system \n");
					 count=snprintf(send_buffer,BUFFER_SIZE,"215 Windows 64-bit\r\n");					 
					 if(count >=0 && count < BUFFER_SIZE){
					    bytes = send(ns, send_buffer, strlen(send_buffer), 0);
					 }
					 printf("[DEBUG INFO] <-- %s\n", send_buffer);
					 if (bytes < 0) break;
				 }
				 //---
				 if (strncmp(receive_buffer,"TYPE",4)==0)  {
					 printf("<--TYPE command received.\n\n");
				 	   bytes=0;
				       char objType;
				       int scannedItems = sscanf(receive_buffer, "TYPE %c", &objType);
				       if(scannedItems < 1) {
				       				count=snprintf(send_buffer,BUFFER_SIZE,"501 Syntax error in arguments\r\n");
				       				if(count >=0 && count < BUFFER_SIZE){
				                 bytes = send(ns, send_buffer, strlen(send_buffer), 0);  
				              }
				              printf("[DEBUG INFO] <-- %s\n", send_buffer);           
				              if (bytes < 0) break;             
				       }

				       switch(toupper(objType)){
                      case 'I':  file_type = FileType::BINARY;
                      	         printf("using binary mode to transfer files.\n");
																 count=snprintf(send_buffer,BUFFER_SIZE,"200 command OK.\r\n");																 
																 if(count >=0 && count < BUFFER_SIZE){
																    bytes = send(ns, send_buffer, strlen(send_buffer), 0);
																 }
																 printf("[DEBUG INFO] <-- %s\n", send_buffer);
																 if (bytes < 0) break;

																 break;
											case 'A':  file_type = FileType::TEXT;
                      	         printf("using ASCII mode to transfer files.\n");
																 count=snprintf(send_buffer,BUFFER_SIZE,"200 command OK.\r\n");																 
																 if(count >=0 && count < BUFFER_SIZE){
																   bytes = send(ns, send_buffer, strlen(send_buffer), 0);
																 }
																 printf("[DEBUG INFO] <-- %s\n", send_buffer);
																 
																 if (bytes < 0) break;

																 break;		
											default:
																count=snprintf(send_buffer,BUFFER_SIZE,"501 Syntax error in arguments\r\n");
																if(count >=0 && count < BUFFER_SIZE){
				              					   bytes = send(ns, send_buffer, strlen(send_buffer), 0);  
				              					}
				              					printf("[DEBUG INFO] <-- %s\n", send_buffer);           
				              				  if (bytes < 0) break;  
				              				  break;
				         } 					 			 
					 
				 }
				 //---
				 if (strncmp(receive_buffer,"STOR",4)==0)  {
					 printf("unrecognised command \n");
					 count=snprintf(send_buffer,BUFFER_SIZE,"502 command not implemented\r\n");					 
					 if(count >=0 && count < BUFFER_SIZE){
					    bytes = send(ns, send_buffer, strlen(send_buffer), 0);
					 }
					 printf("[DEBUG INFO] <-- %s\n", send_buffer);
					 if (bytes < 0) break;
				 }
				 //---
				 if (strncmp(receive_buffer,"RETR",4)==0)  {
					 printf("RETR command received.\n");
					 //store filename 
					 char filename[BUFFER_SIZE];
					 int parsed_filename = sscanf(receive_buffer, "RETR %s", filename);
					 //file pointer
					 FILE* file = NULL;

					 //if we cant find a filename, show error
					 if (parsed_filename < 1) {
						 count = snprintf(send_buffer, BUFFER_SIZE, "501 Missing filename.\r\n");


					 }
					 //error handling for file type
					 else {
						 //determine binary or text mode
						 if (file_type == FileType::BINARY) {
							 printf("Opening file in binary mode.\n");
							 //Binary
							 file = fopen(filename, "rb");
						 } else if (file_type == FileType::TEXT) {
							 printf("Opening file in text mode.\n");
							 //Text
							 file = fopen(filename, "r");
						 } else {
							 // If file type unknown, don't open anything
							 printf("Unrecognized file type.\n");
							 file = NULL;
						 }
					 }

					 //ERROR handling brackets in case theres no file
					 if (file != NULL) { //if file isnt null and can open 
						 //start transsfer
						 count = snprintf(send_buffer, BUFFER_SIZE, "150 Opening data connection...\r\n");
						 char buffer[BUFFER_SIZE];
						 int read_bytes;
						 //read bits of file and transfer chunks over
						 while (!feof(file)) {
							 read_bytes = fread(buffer, 1, BUFFER_SIZE, file);

							 if (read_bytes > 0) {
								 //using active to decide between modes and which socket to send data on
								 if (active == 0) {
									 send(ns_data, buffer, read_bytes, 0); //passive
								 }
								 else {
									 send(s_data_act, buffer, read_bytes, 0); //active
								 }
							 }
						 }

						 fclose(file); //to close after transferring
						 //all done!
						 count = snprintf(send_buffer, BUFFER_SIZE, "226 Transfer successful, closing data connection\r\n");
						 active = 0; //set to passive again after we finish transfer
					 }
					 //file couldnt be opened
					 else if (file == NULL) {
						 count = snprintf(send_buffer, BUFFER_SIZE, "550 File not found or cannot be opened.\r\n");
					 }

					 if(count >=0 && count < BUFFER_SIZE){
					    bytes = send(ns, send_buffer, strlen(send_buffer), 0);
					 }
					 printf("[DEBUG INFO] <-- %s\n", send_buffer);
					 if (bytes < 0) break;
				 }
				 //---
				 if (strncmp(receive_buffer,"OPTS",4)==0)  {
					 printf("unrecognised command \n");
					 count=snprintf(send_buffer,BUFFER_SIZE,"502 command not implemented\r\n");					 
					 if(count >=0 && count < BUFFER_SIZE){
					    bytes = send(ns, send_buffer, strlen(send_buffer), 0);
					 }
					 printf("[DEBUG INFO] <-- %s\n", send_buffer);
					 if (bytes < 0) break;
				 }
				 //---
				 if (strncmp(receive_buffer,"EPRT",4)==0)  {  //more work needs to be done here
					 printf("EPRT command received.\n");
					 //store ip
					 char ip_string[INET6_ADDRSTRLEN];
					 int port;
					 //parse what we got from the user
					 int parsed_string = sscanf(receive_buffer, "EPRT |2|%[^|]|%d|", ip_string, &port);
					 if (parsed_string < 2) {
						 count = snprintf(send_buffer, BUFFER_SIZE, "501 error in ERPT command.\r\n");
					 }
					 else {

						 //creating the actual data socket here
						 s_data_act = socket(AF_INET6, SOCK_STREAM, 0);
						 //clean slate
						 memset(&local_data_addr_act, 0, sizeof(local_data_addr_act));
						 //set to ipv6
						 local_data_addr_act.sin6_family = AF_INET6;
						 //ip to binary and stored
						 inet_pton(AF_INET6, ip_string, &local_data_addr_act.sin6_addr);
						 //sets port
						 local_data_addr_act.sin6_port = htons(port);

						 //formattimg ip address from text to binary and stored
						 inet_pton(AF_INET6, ip_string, &local_data_addr_act.sin6_addr);

#if defined __unix__ || defined __APPLE__
						 if (s_data_act < 0) {
							 count = snprintf(send_buffer, BUFFER_SIZE, "425 can't get connection.\r\n");
						}
#elif defined _WIN32
						 if (s_data_act == INVALID_SOCKET) {
							 count = snprintf(send_buffer, BUFFER_SIZE, "425 can't get connection.\r\n");
						}
#endif
						 else {
							 int connect_result = connect(s_data_act, (struct sockaddr*)&local_data_addr_act, sizeof(local_data_addr_act));
							 if (connect_result == 0) {
								 count = snprintf(send_buffer, BUFFER_SIZE, "200 EPRT Command okay\r\n");
								 active = 1; //SETTING to active mode to maintain right state and choose right socket
							 }
							 else {
								 count = snprintf(send_buffer, BUFFER_SIZE, "425 Can't establish connection.\r\n");
							 }
						 }
					 }

					 if(count >=0 && count < BUFFER_SIZE){
					    bytes = send(ns, send_buffer, strlen(send_buffer), 0);
					 }
					 printf("[DEBUG INFO] <-- %s\n", send_buffer);
					 if (bytes < 0) break;
				 }
				 //---
				 if (strncmp(receive_buffer,"CWD",3)==0)  {
					 printf("CWD command received.\n");
					 char dirname[BUFFER_SIZE];

					 //take the directory name from after the CWD command
					 int detecteddir = sscanf(receive_buffer, "CWD %s", dirname);

					 if (detecteddir < 1) {
						 count = snprintf(send_buffer, BUFFER_SIZE, "501 Cannot find directory name in command.\r\n");
					 }
					 else {
						 int directory = chdir(dirname);
							 if (directory != 0) {
								 count = snprintf(send_buffer, BUFFER_SIZE, "550 Failed to change directory.\r\n");
							 }
							 else {
								 count = snprintf(send_buffer, BUFFER_SIZE, "250 Directory successfully changed.\r\n");
							 }
					 }

					 if(count >=0 && count < BUFFER_SIZE){
					   bytes = send(ns, send_buffer, strlen(send_buffer), 0);
					 }
					 printf("[DEBUG INFO] <-- %s\n", send_buffer);
					 if (bytes < 0) break;
				 }
				 //---
				 if (strncmp(receive_buffer,"QUIT",4)==0)  {
					 printf("QUIT command received.\n");
					 printf("Quit \n");
					 count=snprintf(send_buffer,BUFFER_SIZE,"221 Connection close by client\r\n");					 
					 if(count >=0 && count < BUFFER_SIZE){
					    bytes = send(ns, send_buffer, strlen(send_buffer), 0);
					 }
					 printf("[DEBUG INFO] <-- %s\n", send_buffer);
					 if (bytes < 0) break;
					 
				 }
				 //---
				 if(strncmp(receive_buffer,"PORT",4)==0) {
					 printf("PORT command received.\n");
					 s_data_act = socket(AF_INET, SOCK_STREAM, 0);
					 //local variables
					 //unsigned char act_port[2];
					 int act_port[2];
					 int act_ip[4], port_dec;
					 char ip_decimal[NI_MAXHOST];
					 printf("===================================================\n");
					 printf("\tActive FTP mode, the client is listening... \n");
					 active=1;//flag for active connection
					 
					 int scannedItems = sscanf(receive_buffer, "PORT %d,%d,%d,%d,%d,%d",
							&act_ip[0],&act_ip[1],&act_ip[2],&act_ip[3],
					      &act_port[0],&act_port[1]);
					 
					 if(scannedItems < 6) {
		       	count=snprintf(send_buffer,BUFFER_SIZE,"501 Syntax error in arguments \r\n");						
		       	if(count >=0 && count < BUFFER_SIZE){
						   bytes = send(ns, send_buffer, strlen(send_buffer), 0);
					  }
						printf("[DEBUG INFO] <-- %s\n", send_buffer);
					  if (bytes < 0) break;
			      
		       }
					 
					 local_data_addr_act.sin6_family=AF_INET6;
					 count=snprintf(ip_decimal,NI_MAXHOST, "%d.%d.%d.%d", act_ip[0], act_ip[1], act_ip[2],act_ip[3]);

					 if(!(count >=0 && count < BUFFER_SIZE)) break;
					 
					 printf("\tCLIENT's IP is %s\n",ip_decimal);  //IPv4 format
					 inet_pton(AF_INET6, ip_decimal, &local_data_addr_act.sin6_addr);
					 port_dec=act_port[0];
					 port_dec=port_dec << 8;
					 port_dec=port_dec+act_port[1];
					 printf("\tCLIENT's Port is %d\n",port_dec);
					 printf("===================================================\n");
					 
					 local_data_addr_act.sin6_port =htons(port_dec); 

        	//Note: the following connect() function is not correctly placed.  It works, but technically, as defined by the protocol, connect() should occur in another place.  Hint: carefully inspect the lecture on FTP, active operations to find the answer. 
					 if (connect(s_data_act, (struct sockaddr *)&local_data_addr_act, (int) sizeof(struct sockaddr)) != 0){
						 printf("trying connection in %s %d\n",ip_decimal, ntohs(local_data_addr_act.sin6_port));
						 inet_ntop(AF_INET6, &local_data_addr_act.sin6_addr, ip_decimal, sizeof(ip_decimal));
						 count=snprintf(send_buffer,BUFFER_SIZE, "425 Something is wrong, can't start active connection... \r\n");
						 if(count >=0 && count < BUFFER_SIZE){
						   bytes = send(ns, send_buffer, strlen(send_buffer), 0);

						   printf("[DEBUG INFO] <-- %s\n", send_buffer);
						 }

#if defined __unix__ || defined __APPLE__ 
						 close(s_data_act);
#elif defined _WIN32	
						 closesocket(s_data_act);
#endif	 

					 }
					 else {
						 count=snprintf(send_buffer,BUFFER_SIZE, "200 PORT Command successful\r\n");
						 if(count >=0 && count < BUFFER_SIZE){
						   bytes = send(ns, send_buffer, strlen(send_buffer), 0);
						   printf("[DEBUG INFO] <-- %s\n", send_buffer);
						   printf("Connected to client\n");
						 }
					 }

				 }
				 //---				 

				 //technically, LIST is different than NLST,but we make them the same here
				 if ( (strncmp(receive_buffer,"LIST",4)==0) || (strncmp(receive_buffer,"NLST",4)==0))   {
					 printf("LIST command received.\n");
#if defined __unix__ || defined __APPLE__ 

					 int i=system("ls -la > tmp.txt");//change that to 'dir', so windows can understand
			
#elif defined _WIN32	

					 int i=system("dir > tmp.txt");
#endif					 
					 printf ("The value returned by system() was: %d.\n",i);
					 FILE* fin = fopen("tmp.txt", "r");

					 //snprintf(send_buffer,BUFFER_SIZE,"125 Transfering... \r\n");
					 //snprintf(send_buffer,BUFFER_SIZE,"150 Opening ASCII mode data connection... \r\n");
					 if (fin) {
						 snprintf(send_buffer, BUFFER_SIZE, "150 Opening data connection...\r\n");
						 send(ns, send_buffer, strlen(send_buffer), 0);
						 char line[BUFFER_SIZE];
						 while (fgets(line, sizeof(line), fin)) {
							 if (active == 1) {
								 send(s_data_act, line, strlen(line), 0);
							 }
							 else if (active == 0) {
								 send(ns_data, line, strlen(line), 0);
							 }
						 }						 
							 fclose(fin);

					 count=snprintf(send_buffer,BUFFER_SIZE,"226 File transfer complete. \r\n");

					 if(count >=0 && count < BUFFER_SIZE){					  
					   bytes = send(ns, send_buffer, strlen(send_buffer), 0);
					   printf("[DEBUG INFO] <-- %s\n", send_buffer);
					 }

#if defined __unix__ || defined __APPLE__ 
					 if (active==0 )close(ns_data);
					 else close(s_data_act);

#elif defined _WIN32					 
					 if (active==0 )
					 		closesocket(ns_data);
					 else 
					 		closesocket(s_data_act);

					 //delete the temporary file
					 system("del tmp.txt");
#endif	 
					 }
					 else {
						 snprintf(send_buffer, BUFFER_SIZE, "450 Requested file action not taken.\r\n");
						 send(ns, send_buffer, strlen(send_buffer), 0);
					 }
					 
					 
				 }
                 //---		
			 //=================================================================================	 
			 }//End of COMMUNICATION LOOP per CLIENT
			 //=================================================================================
			 
//CLOSE SOCKET
			 
#if defined __unix__ || defined __APPLE__ 
			close(ns);
#elif defined _WIN32	

			closesocket(ns);
#endif
			printf("DISCONNECTED from %s\n",inet_ntop(AF_INET6, &remoteaddr.sin6_addr, ip_decimal, sizeof(ip_decimal)));

		 //====================================================================================
		 } //End of MAIN LOOP
		 //====================================================================================
		 
		  printf("\nSERVER SHUTTING DOWN...\n");

#if defined __unix__ || defined __APPLE__ 
		 close(s);

#elif defined _WIN32		 
		 closesocket(s);
		 WSACleanup();
#endif		 
		 return 0;
		 
}
