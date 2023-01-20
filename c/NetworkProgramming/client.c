/**
        client.c

        the client file
        
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
#include <errno.h>

#define BUFFER_SIZE 1024

/* struct to hold the DATAMESSAGE */
struct DATAMESSAGE{
    char OriginID[100];				/* the ID of the node base station/sensor that initially sent the data message */
    char NextID[100];				/* the ID that the data message is currently being sent to */
    char DestinationID[100];		/* the ID of the final destination that the data message is trying to be sent to */
    int HopListLength;				/* is the length of [HopList] */
    char HopList[20][100];			/* contains the IDs that have been used so far to deliver the data message */
    								/*this shouldnt be 20 it should be HopListLength*/
};

/* struct to hold the client */
struct CLIENT{
    char id[100];                   /* the ID of the client */
    int xpos;                       /* the x position of the client */
    int ypos;                       /* the y position of the client */
    int range;                      /* the range of the client */
};

typedef struct DATAMESSAGE DATAMESSAGE;
typedef struct CLIENT client;

/**
    parse_space

    a function to parse an input string
    separating the string into an array of strings (words)
    split up at the spaces in the original input
    params: input- the input string we are parsing
            current- the current index
            space_count- the number of spaces in the input string
                    (this will be known based on what message type it is)
    returns: command- the array of words in the string
**/
char** parse_space(char* input, int* current, int space_count){
    char ** command = calloc(space_count+1, sizeof(char *));
    char temp[100];
    memset(temp, '\0', 100);
    int count = 0;
    int curr = 0;
    while (*current < strlen(input)){
        command[curr] = calloc(1024, sizeof(char));
        if (input[*current] == ' '){
            memcpy(command[curr], temp, strlen(temp));
            curr += 1;
            memset(temp, '\0', 100);
            count = 0;
        }
        else{
            temp[count] = input[*current];
            count += 1;
        }
        *current += 1;
    }
    memcpy(command[curr], temp, strlen(temp));
    curr += 1;
    memset(temp, '\0', 100);
    count = 0;
    return command;
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
int calculate_distance(double x1, double y1, double x2, double y2){
    return sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
}


/**
    execute_command

    a function to check the input from stdin (MOVE, QUIT, SENDDATA, WHERE)
    to determine what to send to the server

    params: input- the input from stdin
            c_data- the client we are receiveing input for
            sock- the socket descriptor number
            server- the server we are sending to
    returns: an int to determine whether to continue in the main function or not
                0 if we are quitting
**/
int execute_command(char* input, client* c_data, int sock, struct sockaddr_in server){
    if (input[strlen(input)-1] == '\n'){
        input[strlen(input)-1] = '\0'; 
    }
    int i = 0;
    /** comm is an array that hold parsed input, each index is a word of input
        comm[0] = (first word of input like MOVE, WHERE, QUIT, SENDDATA)
        comm[1] etc are the words that follow that command **/
    char** comm = parse_space(input, &i, 10);
    /*check what the first word of input is*/
    if (strcmp(comm[0], "MOVE") == 0){
        /*MOVE the node to the given input position*/
        int newx = atoi(comm[1]);
        int newy = atoi(comm[2]);
        c_data->xpos = newx;
        c_data->ypos = newy;

        char new_loc_mesg[1024];
        memset(new_loc_mesg, '\0', 1024);

        snprintf(new_loc_mesg, 1024, "UPDATEPOSITION %s %d %d %d", c_data->id, c_data->range, c_data->xpos, c_data->ypos);
        /*tell the server where the node was moved to*/
        sendto(sock, new_loc_mesg, strlen(new_loc_mesg), 0 ,(struct sockaddr *)&server, sizeof(server));
        return 1;
    }
    if (strcmp(comm[0], "QUIT") == 0){
        /*we return 0 here to tell the main function that we are stopping*/
        return 0;
    }
    if (strcmp(comm[0], "WHERE") == 0){
        /*WHERE will prompt the server to send back THERE, which is the position of the input node*/
        sendto(sock, input, strlen(input), 0, (struct sockaddr *)&server, sizeof(server));
        char where_str[1024];
        memset(where_str, '\0', 1024);
        recv(sock, where_str, 1024, 0);
    }
    if (strcmp(comm[0], "SENDDATA") == 0){
        /*
        Send a where command to get all reachable stations
        If one is destination ID, send off to to that station
        Otherwise send to nearest base station
        */
        /* get the destination */
        char* dest = comm[1];

        char loc_mesg[1024];
        memset(loc_mesg, '\0', 1024);
        snprintf(loc_mesg, 1024, "UPDATEPOSITION %s %d %d %d", c_data->id, c_data->range, c_data->xpos, c_data->ypos);
        /* send the UPDATEPOSITION message to the server */
        sendto(sock, loc_mesg, strlen(loc_mesg), 0 ,(struct sockaddr *)&server, sizeof(server));
        /*buffer filled with base_stations*/
        char base_buff[1024];
        memset(base_buff, '\0', 1024);
        recv(sock, base_buff, 1024, 0);
        /*find the amount of spaces in base_buff to use in parce_space*/
        int base_space = 0;
        for (int i = 0; i < strlen(base_buff); i++){
            if (base_buff[i] == ' '){
                base_space += 1;
            }
        }

        int i = 0;
        /* reach is an array of strings that make up the base_buff
            reach[0] is the REACHABLE command
            reach[1] is the number of reachable nodes
            reach[2] is the id of a reachable node
            reach[3] is the x position of a reachable node
            reach[4] is the y position of a reachable node */
        char ** reach = parse_space(base_buff, &i, base_space);
        char min_id[1024];
        memset(min_id, '\0', 1024);
        int min_dist = 1000000;

        int found = 0;

        for (int i = 2; i < base_space; i+=3){
            char * id = reach[i];
            int x = atoi(reach[i+1]);
            int y = atoi(reach[i+2]);
            /*check if the id is the same as the destination, therefore it was reached*/
            if (strcmp(id, dest) == 0){
                char data_msg[1024];
                memset(data_msg, '\0', 1024);
                snprintf(data_msg, 1024, "DATAMESSAGE %s %s %s 1 %s", c_data->id, dest, dest, c_data->id);
                sendto(sock, data_msg, strlen(data_msg), 0, (struct sockaddr *)&server, sizeof(server));
                found = 1;
            }
            else{
                /*not the destination id but see if it is the closest within range*/
                int dist = calculate_distance(c_data->xpos, c_data->ypos, x, y);
                if (dist < min_dist){
                    memset(min_id, '\0', 1024);
                    memcpy(min_id, id, strlen(id));
                }
            }
        }

        if (found == 0){
            /*the destination was not found*/
            char data_msg[1024];
            memset(data_msg, '\0', 1024);
            snprintf(data_msg, 1024, "DATAMESSAGE %s %s %s 1 %s", c_data->id, min_id, dest, c_data->id);
            sendto(sock, data_msg, strlen(data_msg), 0, (struct sockaddr *)&server, sizeof(server));
        }

        printf("%s: Sent a new message bound for %s.\n", c_data->id, dest);

        return 2;
    }
    return 3;
}


/**
    check_there

    a function to check if the client is part of the network of nodes
    params: d_msg- the DATAMESSAGE
            c_reach- the current list of nodes
            d_count- the amount of spaces
            c_count- the amount of spaces in the reachable list
            options- the nodes that have not yet been traveled to
    returns: count- an int of how many nodes can be reached
                returns 0 if no other nodes are reachable
**/
int check_there(char** d_msg, char** c_reach, int d_count, int c_count, client * options){
    int count = 0;
    for (int i = 5; i < d_count; i++){
        for (int j = 2; i < c_count; i+= 3){
            if (strcmp(d_msg[i], c_reach[j]) == 0){
                memset(options[count].id, '\0', 100);
                memcpy(options[count].id, c_reach[j], strlen(c_reach[j]));
                options[count].xpos = atoi(c_reach[j+1]);
                options[count].ypos = atoi(c_reach[j+2]);
                count += 1;
            }
        }
    }
    return count;
}


/**
    main

    creates the ports and connections, and makes use of all other functions
    params: argc- the number of command line arguments
            argv- the array of command line arguments as strings
    returns: EXIT_SUCCESS on successful exit
**/
int main(int argc, char * argv[]){
    /* argv the command line arguments are
        argv[0] = ./client.out
        argv[1] = [control address] - the address or hostname the control server can be reached at
        argv[2] = [control port] is the port the control server will be listening on for
        argv[3] = [SensorID] is the ID of the sensor and only contains letters and numbers.
        argv[4] = [SensorRange] is an integer value for the maximum communication range of the sensor
        argv[5] = [InitalXPosition] is the initial x-coordinate of the sensor
        argv[6] = [InitialYPosition] is the initial y-coordinate of the sensor */
    if (argc != 7){
        fprintf(stderr, "Incorrect amount of arguments\n");
    }

    /* set up the control port*/
    char* control_addr = argv[1]; //plug into getaddr to get ip address then connect
    int control_port = atoi(argv[2]);
    /* read in to make the client*/
    char* id = argv[3];
    int range = atoi(argv[4]);
    int xpos = atoi(argv[5]);
    int ypos = atoi(argv[6]);

    struct addrinfo hints;
    struct addrinfo *res;

    client sensor_client;
    memset(sensor_client.id, '\0', 100);
    memcpy(sensor_client.id, id, strlen(id));
    sensor_client.xpos = xpos;
    sensor_client.ypos = ypos;
    sensor_client.range = range;

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(control_addr, NULL, &hints, &res);

    char address[1024];
    memset(address, '\0', 1024);

    getnameinfo(res->ai_addr, res->ai_addrlen, address, sizeof(address), NULL, 0, NI_NUMERICHOST);

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0x00, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(control_port);
    inet_pton(AF_INET, address, &serv_addr.sin_addr);

    /*try to connect to the server, check if it failed*/
    int success = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    if (success != 0){
        fprintf(stderr, "Connect failed, errno is %d, string is %s\n", errno, strerror(errno));
    }

    char loc_mesg[1024];
    memset(loc_mesg, '\0', 1024);

    snprintf(loc_mesg, 1024, "UPDATEPOSITION %s %d %d %d", id, range, sensor_client.xpos, sensor_client.ypos);
    /*the initial message that needs to be sent is an UPDATEPOSITION message*/
    sendto(sock, loc_mesg, strlen(loc_mesg), 0 ,(struct sockaddr *)&serv_addr, sizeof(serv_addr));

    /*variable to check if the connection is closed at any point*/
    int closed = 0;

    while(1){
        fd_set readfds;
        char buffer[BUFFER_SIZE];
        memset(buffer, '\0', 1024);
        int server_sd = sock;

        struct timeval timeout;

        FD_ZERO(&readfds);
        if (closed == 0){
            FD_SET(server_sd, &readfds);
        }
        FD_SET(fileno(stdin), &readfds);

        select(FD_SETSIZE, &readfds, NULL, NULL, &timeout);

        if (FD_ISSET(server_sd, &readfds)){
            /*read in from stdin*/
            int bytes = recv(server_sd, buffer, BUFFER_SIZE, 0);
            if (bytes == 0){
                closed = 1;
                continue;
            }

            char ** data_str;
            /*find the amount of spaces in the input to properly parse_space it*/
            int space_count = 0;
            for (int i = 0; i < strlen(buffer); i++){
                if (buffer[i] == ' '){
                    space_count += 1;
                }
            }

            int i = 0;
            /* data_str is an array of strings of the individual input words, a DATAMESSAGE
                data_str[0] is DATAMESSAGE
                data_str[1] is the originID
                data_str[2] is the nextID
                data_str[3] is the destinationID
                data_str[4] is the hop list length
                data_str[5] is the hoplist */
            data_str = parse_space(buffer, &i, space_count);

            if (strcmp(data_str[0], "DATAMESSAGE") != 0){
                continue;
            }

            char* dest = data_str[3];
            char* origin = data_str[1];
            /*check if the destination and client are the same*/
            if (strcmp(dest, sensor_client.id) == 0){
                printf("%s: Message from %s to %s successfully received.\n", data_str[3], data_str[1], data_str[3]);
            }

            else{
                /*we need to forward the message*/
                printf("%s: Message from %s to %s forwarded through %s.\n", sensor_client.id, origin, dest, sensor_client.id);
                char loc_mesg[1024];
                memset(loc_mesg, '\0', 1024);
                snprintf(loc_mesg, 1024, "UPDATEPOSITION %s %d %d %d", id, range, sensor_client.xpos, sensor_client.ypos);
                /*send an update position message*/
                sendto(sock, loc_mesg, strlen(loc_mesg), 0 ,(struct sockaddr *)&serv_addr, sizeof(serv_addr));
                
                char reachable[1024];
                recv(server_sd, buffer, BUFFER_SIZE, 0);
                int amount = 0;
                int reach_space = 0;
                for (int i = 0; i < strlen(reachable); i++){
                    if (reachable[i] == ' '){
                        reach_space += 1;
                    }
                }
                /* current is an array of strings that are the REACHABLE command
                    current[0] is the REACHABLE command
                    current[1] is the number of reachable nodes
                    current[2] is the id of a reachable node
                    current[3] is the x position of a reachable node
                    current[4] is the y position of a reachable node */
                char** current = parse_space(reachable, &amount, reach_space);

                int hop_len = atoi(data_str[4]);

                char where_mesg[1024];
                memset(where_mesg, '\0', 1024);
                snprintf(where_mesg, 1024, "WHERE %s", dest);
                sendto(sock, where_mesg, strlen(where_mesg), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
                /*what you receive back is a THERE message*/
                char there_mesg[1024];
                memset(there_mesg, '\0', 1024);
                recv(sock, there_mesg, 1024, 0);

                int i = 0;
                /* there is the array of words in a THERE message
                    there[0] = THERE command
                    there[1] = nodeID
                    there[2] = x position of the node
                    there[3] = y position of the node */
                char** there = parse_space(there_mesg, &i, 3);

                int dest_x = atoi(there[2]);
                int dest_y = atoi(there[3]);
                /*calculate the distance between the sendor client and what is THERE */
                int dist_betw = calculate_distance(sensor_client.xpos, sensor_client.ypos, dest_x, dest_y);

                if (dist_betw < sensor_client.range){
                    /*can send if within range*/
                    hop_len += 1;
                    char final_mesg[1024];
                    memset(final_mesg, '\0', 1024);
                    snprintf(final_mesg, 1024, "DATAMESSAGE %s %s %s %d", origin, dest, dest, hop_len);
                    for (int i = 5; i < hop_len-1; i++){
                        strcat(final_mesg, data_str[i]);
                    }
                    strcat(final_mesg, sensor_client.id);
                    sendto(sock, final_mesg, strlen(final_mesg), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
                }
                else{
                    /*not within range*/
                    client * not_in = calloc(20, sizeof(client));
                    /*check if the THERE node is reachable*/
                    int count = check_there(data_str, current, space_count, reach_space, not_in);
                    if (count == 0){
                        printf("Message from %s to %s could not be delivered\n", origin, dest);
                    }
                    else{
                        /*node is reachable*/
                        char min_id[1024];
                        int min_dist = 1000000;
                        for (int i = 0; i < count; i++){
                            int dist = calculate_distance(sensor_client.xpos, sensor_client.ypos, not_in[count].xpos, not_in[count].ypos);
                            /*find the shortest distance*/
                            if (dist < min_dist){
                                memset(min_id, '\0', 1024);
                                memcpy(min_id, not_in[count].id, strlen(not_in[count].id));
                            }
                            else if (dist == min_dist){
                                if (not_in[count].id < min_id){
                                    memset(min_id, '\0', 1024);
                                    memcpy(min_id, not_in[count].id, strlen(not_in[count].id));
                                }
                            }
                        }
                        hop_len += 1;
                        char next_mesg[1024];
                        memset(next_mesg, '\0', 1024);
                        snprintf(next_mesg, 1024, "DATAMESSAGE %s %s %s %d", origin, min_id, dest, hop_len);
                        for (int i = 5; i < hop_len-1; i++){
                            strcat(next_mesg, data_str[i]);
                        }
                        strcat(next_mesg, min_id);
                        sendto(sock, next_mesg, strlen(next_mesg), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
                    }
                }
            }
        }
        else if (FD_ISSET(fileno(stdin), &readfds)){
            /*check if we need to continue or not*/
            fgets(buffer, 1024, stdin);
            int cont_or_not = execute_command(buffer, &sensor_client, sock, serv_addr);
            if (cont_or_not == 0){
                /*done*/
                break;
            }
        }
    }
    return EXIT_SUCCESS;
}
