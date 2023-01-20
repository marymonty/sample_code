/**
		server.c

		the server file

**/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <math.h>

/*global variable needed for sizing arrays
	hopefully there will never be an instance where there
	are more than 27 base_stations in a file or 27 clients*/
#define num_base_stations 27
#define num_clients 27

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ STRUCTS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/*struct to hold the THERE message*/
struct THERE{
	char message[100];				/* to put THERE */
	char id[100];					/* nodeID of the base station or sensor */
	float xpos;					/* x position of base station / sensor */
	float ypos;					/* y position of the base station / sensor */
};


/*struct of a base station*/
struct base_station{
	char BaseID[100];			/*unique string identifier for the base station, only contains letters and numbers*/
	float XPos;				/*floating point number representing the x-coordinate of this base station*/
	float YPos;				/*floating point number representing the y-coordinate of this base station*/
	int NumLinks;				/*the degree of the base station (how many other base stations its connected to*/
	char ListOfLinks[num_base_stations][100];		/*list of base station IDs that this base station is connected to*/
};


/*struct of a client*/
struct client{
	char id[128];								/* the id of the client */
	struct sockaddr_in client;					/* the sockaddr of the client */
	int sock_desc;								/* the socket descriptor the client is on */
	float xpos;									/* the x position of the client */
	float ypos;									/* the y position of the client */
	float range;								/* the range the client has for other sensors */
	int active;									/* int to tell if the client is active */
	int num_reachable;							/* the amount of nodes reachable from this client */
	char reachable_list[num_clients][100];		/* the list of reachable nodes from this client */
};


typedef struct THERE THERE;
typedef struct base_station base_station;
typedef struct client client;
base_station* base_station_arr;				/* the array of base_stations (read in from command line file) */
client* client_arr;							/* the array of clients */


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ MAKING MESSAGES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/**
	make_THERE

	function to make a THERE message
	THERE [NodeID] [XPosition] [YPosition]
	params: id- the nodeID
			x- the x position of the node
			y- the y position of the node
	returns: void	
**/
void make_THERE(char* id, float x, float y, int sd, struct sockaddr_in s_c) {
	// THERE there_message;
	// memcpy(there_message.message, "THERE", sizeof("THERE"));
	// memcpy(there_message.id, id, strlen(id));
	// there_message.xpos = x;
	// there_message.ypos = y;
	char mesg[1024];
	memset(mesg, '\0', 1024);
	snprintf(mesg, 1024, "THERE %s %lf %lf", id, x, y);
	sendto(sd, mesg, strlen(mesg), 0, (struct sockaddr *)&s_c, sizeof(s_c));
}







/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ PRINT FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/**
	print_base_stations

	probably just a funtion for testing purposes
	prints out all the base stations currently in the 
	base_station_arr (all the base stations from the input file)
	params: none
	returns: none
**/
void print_base_stations() {
	int i = 0;
	while(i < num_base_stations) {
		printf("base station %d:   %s\n", i, base_station_arr[i].BaseID);
		i += 1;
	}
}

/**
	print_clients

	probably just a funtion for testing purposes
	prints out all the clients currently in the 
	client_arr (all the client sensors)
	params: none
	returns: none
**/
void print_clients() {
	int i = 0;
	while(i < num_clients) {
		printf("client %d:   %s\n", i, client_arr[i].id);
		i += 1;
	}
}


/**
	print_reachable_list

	prints out the client's reachable list
	params: c- the client
	returns: none
**/
void print_reachable_list(client c) {
	int i = 0;
	while(i < num_clients) {
		printf("client %s reachable list index %d:   %s\n", c.id, i, c.reachable_list[i]);
		i += 1;
	}
}




/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ PARSING FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/**
	parse_space

	a function to parse an input string
	separating the string into an array of strings (words)
	split up at the spaces in the original input
	params: input- the input string we are parsing
			space_count- the number of spaces in the input string
					(this will be known based on what message type it is)
	returns: command- the array of words in the string
**/
char** parse_space(char* input, int space_count){
    char ** command = calloc(space_count+1, sizeof(char *));
    char temp[100];
    memset(temp, '\0', 100);
    int count = 0;
    int curr = 0;
    int current = 0;
    while (current < strlen(input)){
        command[curr] = calloc(1024, sizeof(char));
        if (input[current] == ' '){
            memcpy(command[curr], temp, strlen(temp));
            curr += 1;
            memset(temp, '\0', 100);
            count = 0;
        }
        else{
            temp[count] = input[current];
            count += 1;
        }
        current += 1;
    }
    memcpy(command[curr], temp, strlen(temp));
    curr += 1;
    memset(temp, '\0', 100);
    count = 0;
    return command;
}


/**
	read_base_station_file

	a function to read in the base station file and put all information into
	base_station structs and put in the base_station_arr
	params: base_station_file- the file that holds the base station info
	return: none
	changes: base_station_arr
**/
void read_base_station_file(FILE* base_station_file) {
	base_station_arr = calloc(num_base_stations ,sizeof(base_station));
	for(int i = 0; i < num_base_stations; i++){
		memset(base_station_arr[i].BaseID, '\0', 1024);
	}
	char buffer[512];
	buffer[511] = '\n';
	int count = 0;
	int end_count = 100;
	int listOfLinks_index = 0;
	int base_station_arr_index = 0;
	base_station base;
	/* go through the file word by word*/
	while(fscanf(base_station_file, "%s", buffer) != EOF){
		count += 1;
		/* the first word is the name of the base_station*/
		if (count == 1) {
			memcpy(base.BaseID, buffer, strlen(buffer));
		}
		/* second word is the x position of the base_station*/
		else if (count == 2) {
			base.XPos = atof(buffer);
		}
		/* third word is the y position of the base station */
		else if (count == 3) {
			base.YPos = atof(buffer);
		}
		/* forth word is how many other base stations the current station is connected to*/
		else if (count == 4) {
			int connections = atoi(buffer);
			/*this tells us how many more words are in this line in the file*/
			end_count = count+connections;
			/*this is the number of links for the base station*/
			base.NumLinks = connections;
		}
		/*fifth + words are the ids of the base stations the current station is connected to */
		else if (count > 4 || count == end_count) {
			/*put the connected base stations into the ListOfLinks for the current base station*/
			memcpy(base.ListOfLinks[listOfLinks_index], buffer, sizeof(buffer));
			listOfLinks_index += 1;
			if (count == end_count) {
				/*that was the last part of this base_station*/
				end_count = 100;
				count = 0;
				base_station_arr[base_station_arr_index] = base;
				base_station_arr_index += 1;
				listOfLinks_index = 0;
			}
		}
		#ifdef DEBUG_MODE
		printf("buffer: %s\n", buffer);
		#endif
	}
	#ifdef DEBUG_MODE
	print_base_stations();
	#endif
}


/**
	get_first_word

	function to get the first word of input, which should be
	one of the specified commands (SENDDATA, QUIT, WHERE, UPDATEPOSITION, NextID)
	params: input- the input from stdin
	returns: the first word of the input (broken at the first space in the string)
**/
char* get_first_word(char* input) {
	/*need to make a copy of input string so we arent loosing some of the input*/
	char input_copy[strlen(input)];
	memcpy(input_copy, input, strlen(input));
	char delim[] = " ";
	char* first_word = calloc(100, sizeof(char));
	first_word = strtok(input_copy, delim);
	return first_word;
}






/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ CLIENT CALLED FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/**
	check_SENDDATA

	a function to check the following condition: 
	SENDDATA [OriginID] [destination ID] causes a new DATAMESSAGE to be created. If the
	[OriginID] is CONTROL then when deciding the next hop, all base stations should be considered
	reachable, and the [NextID] must be a base station. If the [OriginID] is a base station, then the
	next hop should be decided based on what is reachable from the base station with that [BaseID]. The
	[OriginID] will never be a sensor in this command.
	params: input - the input from stdin
	returns:
**/
void check_SENDDATA(char* input) {
	/* input_arr
		input_arr[0] = SENDDATA
		input_arr[1] = OriginID
		input_arr[2] = destinationID */
	char** input_arr = parse_space(input, 2);

	if (strcmp(input_arr[1], "CONTROL") == 0) {
		/*all base stations are considered reachable
			NextID must be a base station*/
		/*lucas say you can send it to anywhere*/
		if (strncmp(input_arr[1], "base_station", strlen("base_station")) == 0){
			//printf("Sent to %s\n", input_arr[1]);
		}
		else{
			//client dest;
			for (int i = 0; i < num_clients; i++){
				if (strcmp(client_arr[i].id, input_arr[2]) == 0){
					if (client_arr[i].num_reachable == 0){
						/*could not send the message*/
						//printf("Could not send message\n");
					}
					else{
						int base_found = 0;
						char clients[num_clients][100];
						for(int j = 0; j < num_clients; j++){
							memset(clients[i], '\0', 100);
						}
						for (int j = 0; j < client_arr[i].num_reachable; j++){
							if (strncmp(client_arr[i].reachable_list[j], "base_station", strlen("base_station")) == 0){
								/*Send directly to client*/
								base_found = 1;
							}
							else{
								memcpy(clients[i], client_arr[i].reachable_list[j], strlen(client_arr[i].reachable_list[j]));
							}
						}
						if (base_found == 0){
							/*
							check other clients to see if can access, if can't then crash
							*/
						}
					}
				}
			}
		}

	}
	else if (strncmp(input_arr[1], "base_station", strlen("base_station")) == 0) {
		/*next hop decided based on what is reachable for 
			base_station with that BaseID*/
		/*greedy approach in choosing where to send next*/
	}
}



/**
	calculate_distance

	a function to calculate the cartesian distance between two points
	(the distance between points (X1, Y1) and (X2, Y2) is given by the Pythagorean theorem:
		     ______________________
		d = √((x2−x1)^2+(y2−y1)^2)
	params: x1- x of point 1
			y1- y of point 1
			x2- x of point 2
			y2- y of point 2
	returns: dist- the distance between points
**/
float calculate_distance(float x1, float y1, float x2, float y2) {
	return sqrt( ((x2-x1)*(x2-x1)) + ((y2-y1)*(y2-y1)) );
}

/**
	populate_reachable_list

	a function to go through the clients to see what sensors
	can reach what other sensors / base stations
	params: c- the client we are measuring against
	returns:
**/
void populate_reachable_list(client* c) {
	int num_reached = 0;
	int i = 0;
	int j = 0;
	/*go through the clients to check if they are reachable*/
	while (i < num_clients) {
		if (client_arr[i].active != 0 && client_arr[i].id[0] != '\0') {
			/*the client exists*/
			if (strcmp(client_arr[i].id, c->id) != 0) {
				/*the current client is different from the input client*/
				/*check if this client is within range of client c*/
				float dist = calculate_distance(c->xpos, c->ypos, client_arr[i].xpos, client_arr[i].ypos);
				if (dist <= c->range) {
					/*the client is within range of c*/
					num_reached += 1;
					memcpy(c->reachable_list[j], client_arr[i].id, sizeof(client_arr[i].id));
					j += 1;
				}
			}
		}
		i += 1;
	}
	/*go through the base stations to check if they are reachable*/
	i = 0;
	j = 0;
	while (i < num_base_stations) {
 		if (strcmp(base_station_arr[i].BaseID, "") != 0) {
 			/*the base_station exists*/
 			if (strcmp(base_station_arr[i].BaseID, c->id) != 0) {
 				/*the current base station is different from the input client*/
 				//check if this base station is within range of client c
 				float dist = calculate_distance(c->xpos, c->ypos, base_station_arr[i].XPos, base_station_arr[i].YPos);
 				if (dist <= c->range) {
	 				/*the client is within range of c*/
 					num_reached += 1;
					memcpy(c->reachable_list[j], base_station_arr[i].BaseID, sizeof(base_station_arr[i].BaseID));
	 				j += 1;
	 			}
	 		}
	 	}
	 	i += 1;
	}
	c->num_reachable = num_reached;

}


/**
	check_UPDATEPOSITION

	a function used when UPDATEPOSITION is called
	UPDATEPOSITION call looks like this:
	UPDATEPOSITION  [SensorID] [SensorRange] [CurrentXPosition] [CurrentYPosition] 
	params: input- that update position whole call
	returns: REACHABLE message:
				REACHABLE [NumReachable] [ReachableList]
**/
void check_UPDATEPOSITION(char* input, int s_d, struct sockaddr_in cli) {
	/*parse the input*/
	/*there will be four spaces in updateposition*/
	#ifdef DEBUG_MODE
	printf("IN UPDATE POSTIITON CALL\n");
	#endif
	char** input_arr = parse_space(input, 4);
	int i = 0;
	int changed = 0;
	//printf("id is %s\n", input_arr[1]);
	while (i < num_clients) {
		if (strcmp(client_arr[i].id, input_arr[1]) == 0) {
			/*the sensor we are changing position of is in the client array*/
			/*just update the range, x, and y */
			client_arr[i].range = atoi(input_arr[2]);
			client_arr[i].xpos = atoi(input_arr[3]);
			client_arr[i].ypos = atoi(input_arr[4]);
			/*repopulate the reachable list since the range and position changed*/
			populate_reachable_list(&client_arr[i]);
			changed = 1;
			break;
		}
		else if (strcmp(client_arr[i].id, "") == 0 ) {
			/*there are no more clients in client_arr */ 
			break;
		}
		i += 1;
	}
	if (!changed) {
		/*the client is not in the client array yet*/
		/*make a new client*/
		client c;
		memcpy(c.id, input_arr[1], sizeof(char)*strlen(input_arr[1]));
		c.id[strlen(input_arr[1])] = '\0';
		c.range = atoi(input_arr[2]);
		c.xpos = atoi(input_arr[3]);
		c.ypos = atoi(input_arr[4]);
		/* fill the reachable list with other clients / base stations
			with the function populate_reachable_list*/
		c.client = cli;
		c.sock_desc = s_d;
		c.active = 1;
		populate_reachable_list(&c);
		#ifdef DEBUG_MODE
		print_reachable_list(c);
		#endif
		client_arr[i] = c;
	}
	#ifdef DEBUG_MODE
	print_clients();
	#endif
	char reach_mesg[1024];
	snprintf(reach_mesg, 1024, "REACHABLE %d", client_arr[i].num_reachable);
	for (int j = 0; j < client_arr[i].num_reachable; j++){
		strcat(reach_mesg, " ");
		strcat(reach_mesg, client_arr[i].reachable_list[j]);
	}
	sendto(client_arr[i].sock_desc, reach_mesg, strlen(reach_mesg), 0, (struct sockaddr *)&client_arr[i].client, sizeof(client_arr[i].client));
}


/**
	check_WHERE

	a function called when the server recieves WHERE
	If the server receives a WHERE message from a sensor, it should respond with a
	message in the following format: THERE [NodeID] [XPosition] [YPosition] 
	where [XPosition] and [YPosition] are the x and	y-coordinates of the requested 
	base station/sensor and [NodeID] is its ID. 
	params: input- the input from the client
	returns: 
**/
void check_WHERE(char* input, int sd, struct sockaddr_in s_cli) {
	/* input_arr
		input_arr[0] = WHERE
		input_arr[1] = [SensorID/BaseID] */
	char** input_arr = parse_space(input, 1);
	int i;
	/*check if the id is a base station id*/
	if (strncmp(input_arr[1], "base_station", strlen("base_station")) == 0) {
		/*go through the base_stations to check if input is there*/
		i = 0;
		while(i < num_base_stations) {
			if (strcmp(base_station_arr[i].BaseID, input_arr[1]) == 0) {
				make_THERE(base_station_arr[i].BaseID, base_station_arr[i].XPos, base_station_arr[i].YPos, sd, s_cli);
			}
			i += 1;
		}
	}
	i = 0;
	/*go through the clients to check if input is there*/
	while (i < num_clients) {
		if (strcmp(client_arr[i].id, input_arr[1]) == 0) {
			make_THERE(client_arr[i].id, client_arr[i].xpos, client_arr[i].ypos, sd, s_cli);
		}
		i += 1;
	}
}


/**
	check_QUIT

	a function to close all sockets and stop the server
	params: none
	returns: none
**/
void check_QUIT() {
	int i = 0;
	while (i < num_clients){	
		/*close all client socket descriptors*/
		if (strcmp(client_arr[i].id, "") != 0) {
	        close(client_arr[i].sock_desc);	
	        client_arr[i].active = 0;
    	}
        i += 1;
    }
    /*free everything that was dynamically allocated */
    free(client_arr);
    free(base_station_arr);
}


/**
	check_input

	function to figure out what the input is asking the server to do
	params: input- the input from stdin
	returns: 
**/
int check_input(char* input, int sd, struct sockaddr_in s_cli) {
	/*check that the input is not empty or null*/
	if (strlen(input) == 0 || input == NULL) {
		return 0;
	}
	/*get the first word of the input*/
	char first_word[100];
	memcpy(first_word, get_first_word(input), sizeof(char)*100);
	#ifdef DEBUG_MODE
	printf("first word: %s\n", first_word);
	#endif
	if (first_word[strlen(first_word)-1] == '\n'){
		first_word[strlen(first_word)-1] = '\0';
	}
	if (strcmp(first_word, "SENDDATA") == 0) {
		/*creates a DATAMESSAGE to send back to the client*/
		#ifdef DEBUG_MODE
		printf("SENDDATA match\n");
		#endif
		check_SENDDATA(input);
	}
	else if (strcmp(first_word, "QUIT") == 0) {
		/*causes the server to clean up any memory and any sockets that are in use and terminate.*/
		#ifdef DEBUG_MODE
		printf("QUIT match\n");
		#endif
		check_QUIT();
		return 0;
	}
	else if (strcmp(first_word, "WHERE") == 0) {
		/*returns a THERE message to the client*/
		#ifdef DEBUG_MODE
		printf("WHERE match\n");
		#endif
		check_WHERE(input, sd, s_cli);

	}
	else if (strcmp(first_word, "UPDATEPOSITION") == 0) {
		/*updates the position of the client (or creates the new client) and send back a THERE message*/
		#ifdef DEBUG_MODE
		printf("UPDATEPOSITION match\n");
		#endif
		check_UPDATEPOSITION(input, sd, s_cli);
	}
	else {
		/*check for a matching NextID that is a sensor's ID
			the server then acts as a sensor to sensor relay
			and should deliver the message to the destination*/
		/*count the spaces so we can parse*/
		int space_count = 0;
		for (int i = 0; i < strlen(input); i++){
			space_count += 1;
		}
		/* d_m is an array of the input (a datamessage)
			d_m[0] is DATAMESSAGE
			d_m[1] is the originID
			d_m[2] is the nextID
			d_m[3] is the destinationID
			d_m[4] is the hop list length
			d_m[5] is the hoplist */
		char** d_m = parse_space(input, space_count);
		int found = 0;
		if (strcmp(d_m[2], d_m[3]) == 0){
			/*when nextID and destinationID are the same place, the message was received*/
			printf("%s: Message from %s to %s successfully received.\n", d_m[3], d_m[1], d_m[3]);
			found = 1;
		}
		else{
			/*nextID and destinationID are not the same so we are forwarding through nextID*/
			printf("%s: Message from %s to %s being forwarded through %s\n", d_m[2], d_m[1], d_m[3], d_m[2]);
		}
		while(found != 1){
			/*while the destinationID was not yet found*/
			if (strcmp(d_m[2], d_m[3]) == 0){
				/*check if nextID and destinationID are the same*/
				printf("%s: Message from %s to %s successfully received.\n", d_m[3], d_m[1], d_m[3]);
				found = 1;
			}
			else if (strncmp(d_m[3], "base_station", strlen("base_station")) == 0){
				if (strncmp(d_m[2], "base_station", strlen("base_station")) == 0){
					/* both the nextID and destinationID are base stations*/
					client min_client;
					for (int i = 0; i < num_base_stations; i++){
						/*go through the base stations to find a match*/
						int compare = strcmp(d_m[2], base_station_arr[i].BaseID);
						if (compare == 0){
							/*base station match*/
							base_station curr = base_station_arr[i];
							char min_id[1024];
							memset(min_id, '\0', 1024);
							int min_dist = 10000000;
							for (int j = 0; j < curr.NumLinks; j++){
								char* poss_id = base_station_arr[i].ListOfLinks[j];
								for (int k = 0; k < num_base_stations; k++){
									if (strcmp(poss_id, base_station_arr[k].BaseID) == 0 && found == 0){
										if (strcmp(poss_id, d_m[3]) == 0){
											/*found a matching base station*/
											memset(min_id, '\0', 1024);
											memcpy(min_id, poss_id, strlen(poss_id));
											found = 1;
											break;
										}
										base_station poss = base_station_arr[k];
										if (strcmp(poss.BaseID, curr.BaseID) != 0){
											/*check if the base station is in range*/
											int dist = calculate_distance(curr.XPos, curr.YPos, poss.XPos, poss.YPos);
											if (dist < min_dist){
												memset(min_id, '\0', 1024);
												memcpy(min_id, poss.BaseID, strlen(poss.BaseID));
												min_dist = dist;
											}
											else if (dist == min_dist){
												if (strcmp(poss.BaseID, min_id) < 0){
													/*update the minimum distanced base station*/
													memset(min_id, '\0', 1024);
													memcpy(min_id, poss.BaseID, strlen(poss.BaseID));
												}
											}
										}
									}
								}
							}
							for (int j = 0; j < num_clients; j++){
								/*go through the clients*/
							}
							if (found == 1){
								/*we found the path*/
								printf("%s: Message from %s to %s successfully received.\n", d_m[3], d_m[1], d_m[3]);
								break;
							}
							else{
								/*we did not find the path yet*/
								int hop_len = atoi(d_m[4]);
								hop_len += 1;
								char new_d_m[1024];
								memset(new_d_m, '\0', 1024);
								snprintf(new_d_m, 1024, "DATAMESSAGE %s %s %s %d", d_m[1], min_id, d_m[3], hop_len);
								for (int i = 5; i < hop_len-1; i++){
									strcat(new_d_m, " ");
									strcat(new_d_m, d_m[i]);
								}
								strcat(new_d_m, " ");
								strcat(new_d_m, curr.BaseID);
								space_count += 1;
								/*repeat the process with the nextID*/
								d_m = parse_space(new_d_m, space_count);
							}
						}
					}
				}
				else{
					/*client curr;
					for (int i = 0; i < num_clients; i++){
						if (strcmp(d_m[3], client_arr[i].id) == 0){
							curr = client_arr[i];
							break;
						}
					}
					for (int i = 0; i < curr.num_reachable; i++){
						curr[i]
						for (int j = 0; j < num_clients; j++){
							if (strcmp)
						}
					}*/
				}
			}
			else{
			}
		}
	}
	return 1;
}




/**
	main
	a function to call the other functions and create the server
	params: argc- the number of command line args
			argv- the array of command line args
	return: EXIT_SUCCESS on success
**/
int main(int argc, char** argv) {
	/**
		the command line args should be [control port] [base station file]
			• [control port] is the port the server should listen on for sensors to connect to.
			• [base station file] is the name of the file the server should read to get information about base
				stations and the base station network.
	**/
	if (argc != 3) {
		fprintf(stderr, "Invalid command line arguments\n");
		exit(0);
	}

	/*make the array of clients and reset the memory in it*/
	client_arr = calloc(num_clients, sizeof(client));
	for (int i = 0; i < num_clients; i++){
		memset(client_arr[i].id, '\0', 128);
		client_arr[i].sock_desc = 0;
		client_arr[i].xpos = 0;
		client_arr[i].ypos = 0;
		client_arr[i].range = 0;
		client_arr[i].active = 0;
		client_arr[i].num_reachable = 0;
		for (int j = 0; j < num_clients; j++){
			memset(client_arr[i].reachable_list[j], '\0',100);
		}
	}

	/*read in the base station file*/
	FILE *base_station_file = fopen(argv[2], "r");
	if (base_station_file == NULL) {
		fprintf(stderr, "Could not open base station file\n");
		exit(0);
	}
	else {
		read_base_station_file(base_station_file);
	}

	/*read in the port from the command line*/
	int port = atoi(argv[1]);
	#ifdef DEBUG_MODE
	printf("port = %d\n", port);
	#endif

	/*setting up the port*/
	int amount_clients = 0;
	int master_sock = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in control;
	control.sin_family = PF_INET;
	control.sin_addr.s_addr = INADDR_ANY;
	int control_size = sizeof(control);
	unsigned short control_port = port;
	control.sin_port = htons(control_port);

	/*try to bind and check for failure*/
	int success = bind(master_sock, (struct sockaddr *)&control, control_size);

	if (success != 0){
		fprintf(stderr, "BIND Error\n");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in sensor_client;
	int sen_len = sizeof(sensor_client);

	/*need to listen since we are using select*/
	int lis = listen(master_sock, 5);
	
	if (lis < 0){
		fprintf(stderr, "LISTEN Error\n");
		exit(EXIT_FAILURE);
	}

	while(1){
		struct timeval t_out;
		fd_set readfds;
		char s_msg[1024];
		int s_sd;
		memset(s_msg, '\0', 1024);

		FD_ZERO(&readfds);
		FD_SET(master_sock, &readfds);
		FD_SET(fileno(stdin), &readfds);

		for (int i = 0; i < amount_clients; i++){
			s_sd = client_arr[i].sock_desc;

			if(client_arr[i].active == 1){
				FD_SET(s_sd, &readfds);
				/*client is active*/
			}
		}

		/*select so that we can have multiple clients at once*/
		select(FD_SETSIZE, &readfds, NULL, NULL, &t_out);

		if (FD_ISSET(master_sock, &readfds)){
			/*new client connected*/
			int new_conn = accept(master_sock, (struct sockaddr*)&sensor_client, (socklen_t *)&sen_len);
			client new_sensor;
			new_sensor.client = sensor_client;
			new_sensor.sock_desc = new_conn;
			new_sensor.active = 1;
			memset(new_sensor.id, '\0', 128);
			for(int i = 0; i < num_clients; i++){
				memset(new_sensor.reachable_list[i], '\0', 100);
			}
			client_arr[amount_clients] = new_sensor;
			amount_clients += 1;
		}
		else if (FD_ISSET(fileno(stdin), &readfds)){
			fgets(s_msg, 1024, stdin);
			if (strcmp(s_msg, "QUIT\n") == 0){
				check_QUIT();
				break;
			}
			/*get message from stdin (i dont think server gets any stdin)*/
			int quit = check_input(s_msg, client_arr[0].sock_desc, client_arr[0].client);
			if (quit == 0){
				/*we need to check if we quit so that we can stop the server*/
				break;
			}
		}
		else{
			for (int i = 0; i < amount_clients; i++){
				int sd = client_arr[i].sock_desc;
				if (FD_ISSET(sd, &readfds)){
					/*we received a message from the client*/
					recv(sd, s_msg, 1024, 0);
					/*the first message should be an update position message*/
					check_input(s_msg, client_arr[i].sock_desc, client_arr[i].client);
				}
			}
		}
	}
	return EXIT_SUCCESS;
}
