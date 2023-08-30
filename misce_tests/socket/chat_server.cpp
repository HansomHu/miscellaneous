#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <errno.h>
#include <memory.h>
#include <unistd.h>

int create_socket1() {
#define SERVER_PORT 12345
    int rc = -1;
    int on = 1;
    int listen_sd = -1;
    struct sockaddr_in   addr;
    listen_sd = socket(AF_INET, SOCK_STREAM, 0);
   if (listen_sd < 0)
   {
      perror("socket() failed");
      exit(-1);
   }
   rc = setsockopt(listen_sd, SOL_SOCKET,  SO_REUSEADDR, (char *)&on, sizeof(on));
   if (rc < 0)
   {
      perror("setsockopt() failed");
      close(listen_sd);
      exit(-1);
   }
   fcntl(listen_sd, F_SETFL, fcntl(listen_sd, F_GETFL, 0) | O_NONBLOCK);  // Non-blocking mode

   memset(&addr, 0, sizeof(addr));
   addr.sin_family      = AF_INET;
   addr.sin_addr.s_addr = htonl(INADDR_ANY);
   addr.sin_port        = htons(SERVER_PORT);
   rc = bind(listen_sd, (struct sockaddr *)&addr, sizeof(addr));
   if (rc < 0)
   {
      perror("bind() failed");
      close(listen_sd);
      exit(-1);
   }

   rc = listen(listen_sd, 32);
   if (rc < 0)
   {
      perror("listen() failed");
      close(listen_sd);
      exit(-1);
   }
   return listen_sd;
}

int main(int argc, char** argv)
{
    int listenfd = create_socket1();
    printf("create_socket listenfd is %d\n",listenfd);
    #define TRUE 1
    #define FALSE 0
    int i, len, rc, on = 1;
    int max_sd, new_sd;
    int desc_ready, end_server = 0;
    int close_conn;
    char buffer[10240];
    struct timeval timeout;
    fd_set master_set, working_set;
    
    FD_ZERO(&master_set);
    max_sd = listenfd;
    FD_SET(listenfd, &master_set);
    
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    /*************************************************************/
    /* Loop waiting for incoming connects or for incoming data   */
    /* on any of the connected sockets.                          */
    /*************************************************************/
    do {
        /**********************************************************/
        /* Copy the master fd_set over to the working fd_set.     */
        /**********************************************************/
        memcpy(&working_set, &master_set, sizeof(master_set));

        /**********************************************************/
        /* Call select() and wait 3 minutes for it to complete.   */
        /**********************************************************/
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        printf("Waiting on select()...\n");
        rc = select(max_sd + 1, &working_set, NULL, NULL, &timeout);

        /**********************************************************/
        /* Check to see if the select call failed.                */
        /**********************************************************/
        if (rc < 0) {
            perror("  select() failed");
            break;
        }

        /**********************************************************/
        /* Check to see if the 3 minute time out expired.         */
        /**********************************************************/
        if (rc == 0) {
            printf("  select() timed out.  continue program.\n");
            continue;
            // break;
        }

        /**********************************************************/
        /* One or more descriptors are readable.  Need to         */
        /* determine which ones they are.                         */
        /**********************************************************/
        desc_ready = rc;
        for (i = 0; i <= max_sd && desc_ready > 0; ++i) {
            /*******************************************************/
            /* Check to see if this descriptor is ready            */
            /*******************************************************/
            if (FD_ISSET(i, &working_set)) {
                /****************************************************/
                /* A descriptor was found that was readable - one   */
                /* less has to be looked for.  This is being done   */
                /* so that we can stop looking at the working set   */
                /* once we have found all of the descriptors that   */
                /* were ready.                                      */
                /****************************************************/
                desc_ready -= 1;

                /****************************************************/
                /* Check to see if this is the listening socket     */
                /****************************************************/
                if (i == listenfd) {
                    printf("  Listening socket is readable\n");
                    /*************************************************/
                    /* Accept all incoming connections that are      */
                    /* queued up on the listening socket before we   */
                    /* loop back and call select again.              */
                    /*************************************************/
                    do {
                        /**********************************************/
                        /* Accept each incoming connection.  If       */
                        /* accept fails with EWOULDBLOCK, then we     */
                        /* have accepted all of them.  Any other      */
                        /* failure on accept will cause us to end the */
                        /* server.                                    */
                        /**********************************************/
                        fcntl(listenfd, F_SETFL, fcntl(listenfd, F_GETFL, 0) | O_NONBLOCK);  // Non-blocking mode
                        new_sd = accept(listenfd, NULL, NULL);
  
                        if (new_sd < 0) {
                            if (errno != EWOULDBLOCK) {
                                perror("  accept() failed");
                                end_server = TRUE;
                            }
                            break;
                        }
                        fcntl(new_sd, F_SETFL, fcntl(new_sd, F_GETFL, 0) | O_NONBLOCK);  // Non-blocking mode

                        /**********************************************/
                        /* Add the new incoming connection to the     */
                        /* master read set                            */
                        /**********************************************/
                        printf("  New incoming connection - %d\n", new_sd);
                        FD_SET(new_sd, &master_set);
                        if (new_sd > max_sd)
                            max_sd = new_sd;

                        /**********************************************/
                        /* Loop back up and accept another incoming   */
                        /* connection                                 */
                        /**********************************************/
                    } while (new_sd != -1);
                }

                /****************************************************/
                /* This is not the listening socket, therefore an   */
                /* existing connection must be readable             */
                /****************************************************/
                else {
                    printf("  Descriptor %d is readable\n", i);
                    close_conn = FALSE;
                    /*************************************************/
                    /* Receive all incoming data on this socket      */
                    /* before we loop back and call select again.    */
                    /*************************************************/
                    do {
                        /**********************************************/
                        /* Receive data on this connection until the  */
                        /* recv fails with EWOULDBLOCK.  If any other */
                        /* failure occurs, we will close the          */
                        /* connection.                                */
                        /**********************************************/
                        printf("start to recv\n");
                        rc = recv(i, buffer, sizeof(buffer), 0);
                        printf("recv done\n");
                        if (rc < 0) {
                            if (errno != EWOULDBLOCK) {
                                perror("  recv() failed");
                                close_conn = TRUE;
                            }
                            printf("recv() would block, \n");
                            break;
                        }

                        /**********************************************/
                        /* Check to see if the connection has been    */
                        /* closed by the client                       */
                        /**********************************************/
                        if (rc == 0) {
                            printf("  Connection closed\n");
                            close_conn = TRUE;
                            break;
                        }

                        /**********************************************/
                        /* Data was received                          */
                        /**********************************************/
                        len = rc;
                        printf("  %d bytes received\n", len);

                        /**********************************************/
                        /* Echo the data back to the client           */
                        /**********************************************/
                        char body[] = "{\"result\":\"receive message success, select\"}\r\n";
                        char message[] = "HTTP/1.1 200 OK\r\nContent-Type: application/json;charset=UTF-8\r\nContent-Length: 45\r\n\r\n{\"result\":\"receive message success, select\"}\r\n";
                        rc = send(i, message, strlen(message), 0);
                        if (rc < 0) {
                            perror("  send() failed");
                            close_conn = TRUE;
                            break;
                        } else {
                            printf("send success: %d bytes\n", rc);
                            // break;
                        }

                    } while (TRUE);

                    /*************************************************/
                    /* If the close_conn flag was turned on, we need */
                    /* to clean up this active connection.  This     */
                    /* clean up process includes removing the        */
                    /* descriptor from the master set and            */
                    /* determining the new maximum descriptor value  */
                    /* based on the bits that are still turned on in */
                    /* the master set.                               */
                    /*************************************************/
                    if (close_conn) {
                        close(i);
                        FD_CLR(i, &master_set);
                        if (i == max_sd) {
                            while (FD_ISSET(max_sd, &master_set) == FALSE)
                                max_sd -= 1;
                        }
                    }
                } /* End of existing connection is readable */
            } /* End of if (FD_ISSET(i, &working_set)) */
        } /* End of loop through selectable descriptors */

    } while (end_server == FALSE);

    /*************************************************************/
    /* Clean up all of the sockets that are open                 */
    /*************************************************************/
    for (i = 0; i <= max_sd; ++i) {
        if (FD_ISSET(i, &master_set))
            close(i);
    }    
}