//**************************** ECHO CLIENT CODE **************************
// The echo client client.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/socket.h>
#include <netdb.h>

#define MAX 256

// Define variables
struct hostent *hp;              
struct sockaddr_in  server_addr;

char *cmd, cwd[128], *pathname, line[MAX], ans[MAX];

int sock, r;
int SERVER_IP, SERVER_PORT; 

// clinet initialization code

int client_init(char *argv[])
{
  printf("======= clinet init ==========\n");

  printf("1 : get server info\n");
  hp = gethostbyname(argv[1]);
  if (hp==0){
     printf("unknown host %s\n", argv[1]);
     exit(1);
  }

  SERVER_IP   = *(long *)hp->h_addr;
  SERVER_PORT = atoi(argv[2]);

  printf("2 : create a TCP socket\n");
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock<0){
     printf("socket call failed\n");
     exit(2);
  }

  printf("3 : fill server_addr with server's IP and PORT#\n");
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = SERVER_IP;
  server_addr.sin_port = htons(SERVER_PORT);

  // Connect to server
  printf("4 : connecting to server ....\n");
  r = connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (r < 0){
     printf("connect failed\n");
     exit(1);
  }

  printf("5 : connected OK to \007\n"); 
  printf("---------------------------------------------------------\n");
  printf("hostname=%s  IP=%s  PORT=%d\n", 
          hp->h_name, inet_ntoa(SERVER_IP), SERVER_PORT);
  printf("---------------------------------------------------------\n");

  printf("========= init done ==========\n");
}

int getpathname()
{
	pathname = strtok(0, " ");
	if (pathname[0] == 0)
	{
		printf("No pathname specified!");
		return 0;
	}
	return 1;
}

void ls()
{

}

int main(int argc, char *argv[ ])
{
  int n;

  if (argc < 3){
     printf("Usage : client ServerName SeverPort\n");
     exit(1);
  }

  client_init(argv);

  printf("********  processing loop  *********\n");
  while (1)
  {
	  // memset line, answer, cwd
	  memset(line, 0, sizeof(line));
	  memset(ans, 0, sizeof(ans));
	  memset(cwd, 0, sizeof(cwd));

	  // get input
    printf("shell> ");
    bzero(line, MAX);                // zero out line[ ]
    fgets(line, MAX, stdin);         // get a line (end with \n) from stdin

    line[strlen(line)-1] = 0;        // kill \n at end
	getcwd(cwd, MAX);				 // update client working directory
	if (line[0] != 0)                // if line is not null, do a command
	{
		////////// backup the input //////////
		char input[MAX];
		strcpy(input, line);

		////////// determine what the command is //////////
		cmd = strtok(line, " ");
		//// built-in commands
		// exit
		if (!strcmp(cmd, "exit"))
			exit(1);
		//// local commands
		// pwd
		else if (!strcmp(cmd, "lpwd"))
		{
			printf("******** BEGIN lpwd COMMAND ********\n");
			printf("%s", cwd);
			printf("\n******** END lpwd COMMAND ********\n");
		}
		// cat
		else if (!strcmp(cmd, "lcat"))
		{
			printf("******** BEGIN lcat COMMAND ********\n");
			if (getpathname())
			{
				int fd, i, n;
				char buf[1024];
				fd = open(pathname, O_RDONLY);

				while (n = read(fd, buf, 1024))
				{
					for (i = 0; i < n; i++)
					{
						if (buf[i] == '\n')
						{
							putchar('\n');
							putchar('\r');
						}
						else
							putchar(buf[i]);
					}
				}
			}
			printf("\n******** END lcat COMMAND ********\n");
		}
		// ls
		else if (!strcmp(cmd, "lls"))
		{
			printf("******** BEGIN lls COMMAND ********\n");
			pathname = strtok(0, " ");
			// if no argument, set argument to current cwd
			if (strcmp(pathname, "") == 0)
			{
				strcpy(pathname, cwd);
			}
			ls();
			printf("\n******** END lls COMMAND ********\n");
		}
		// cd
		else if (!strcmp(cmd, "lcd"))
		{
			printf("******** BEGIN lcd COMMAND ********\n");
			if (getpathname())
			{
				chdir(pathname);
			}
			printf("\n******** END lcd COMMAND ********\n");
		}
		// mkdir
		else if (!strcmp(cmd, "lmkdir"))
		{
			printf("******** BEGIN lmkdir COMMAND ********\n");
			if (getpathname())
			{
				mkdir(pathname, 0755);
			}
			printf("\n******** END lmkdir COMMAND ********\n");
		}
		// rmdir
		else if (!strcmp(cmd, "lrmdir"))
		{
			printf("******** BEGIN lrmdir COMMAND ********\n");
			if (getpathname())
			{
				rmdir(pathname);
			}
			printf("\n******** END lrmdir COMMAND ********\n");
		}
		// rm
		else if (!strcmp(cmd, "lrm"))
		{
			printf("******** BEGIN lrm COMMAND ********\n");
			if (getpathname())
			{
				unlink(pathname);
			}
			printf("\n******** END lrm COMMAND ********\n");
		}
		//// server commands
		// pwd
		else if (!strcmp(cmd, "pwd"))
		{
			printf("******** BEGIN pwd COMMAND ********\n");

			// Send ENTIRE line to server
			n = write(sock, input, MAX);
			printf("client: wrote n=%d bytes; line=(%s)\n", n, input);

			// Read a line from sock and show it
			n = read(sock, ans, MAX);
			printf("client: read  n=%d bytes; echo=(%s)\n", n, ans);

			printf("\n******** END pwd COMMAND ********\n");
		}
		// ls
		else if (!strcmp(cmd, "ls"))
		{
			printf("******** BEGIN ls COMMAND ********\n");

			// Send ENTIRE line to server
			n = write(sock, input, MAX);
			printf("client: wrote n=%d bytes; line=(%s)\n", n, input);

			// Read a line from sock and show it
			n = read(sock, ans, MAX);
			printf("client: read  n=%d bytes; echo=(%s)\n", n, ans);

			printf("\n******** END ls COMMAND ********\n");
		}
		// cd
		else if (!strcmp(cmd, "cd"))
		{
			printf("******** BEGIN cd COMMAND ********\n");

			if (getpathname())
			{
				// Send ENTIRE line to server
				n = write(sock, input, MAX);
				printf("client: wrote n=%d bytes; line=(%s)\n", n, input);

				// Read a line from sock and show it
				n = read(sock, ans, MAX);
				printf("client: read  n=%d bytes; echo=(%s)\n", n, ans);
			}
			printf("\n******** END cd COMMAND ********\n");
		}
		// mkdir
		else if (!strcmp(cmd, "mkdir"))
		{
			printf("******** BEGIN mkdir COMMAND ********\n");

			if (getpathname())
			{
				// Send ENTIRE line to server
				n = write(sock, input, MAX);
				printf("client: wrote n=%d bytes; line=(%s)\n", n, input);

				// Read a line from sock and show it
				n = read(sock, ans, MAX);
				printf("client: read  n=%d bytes; echo=(%s)\n", n, ans);
			}
			printf("\n******** END mkdir COMMAND ********\n");
		}
		// rmdir
		else if (!strcmp(cmd, "rmdir"))
		{
			printf("******** BEGIN rmdir COMMAND ********\n");

			if (getpathname())
			{
				// Send ENTIRE line to server
				n = write(sock, input, MAX);
				printf("client: wrote n=%d bytes; line=(%s)\n", n, input);

				// Read a line from sock and show it
				n = read(sock, ans, MAX);
				printf("client: read  n=%d bytes; echo=(%s)\n", n, ans);
			}
			printf("\n******** END rmdir COMMAND ********\n");
		}
		// rm
		else if (!strcmp(cmd, "rm"))
		{
			printf("******** BEGIN rm COMMAND ********\n");

			if (getpathname())
			{
				// Send ENTIRE line to server
				n = write(sock, input, MAX);
				printf("client: wrote n=%d bytes; line=(%s)\n", n, input);

				// Read a line from sock and show it
				n = read(sock, ans, MAX);
				printf("client: read  n=%d bytes; echo=(%s)\n", n, ans);
			}
			printf("\n******** END rm COMMAND ********\n");
		}
		// get
		else if (!strcmp(cmd, "get"))
		{
			printf("******** BEGIN get COMMAND ********\n");

			if (getpathname())
			{
				// Send ENTIRE line to server
				n = write(sock, input, MAX);
				printf("client: wrote n=%d bytes; line=(%s)\n", n, input);

				// Read a line from sock and show it
				n = read(sock, ans, MAX);
				printf("client: read  n=%d bytes; echo=(%s)\n", n, ans);
			}
			printf("\n******** END get COMMAND ********\n");
		}
		// put
		else if (!strcmp(cmd, "put"))
		{
			printf("******** BEGIN put COMMAND ********\n");

			if (getpathname())
			{
				// Send ENTIRE line to server
				n = write(sock, input, MAX);
				printf("client: wrote n=%d bytes; line=(%s)\n", n, input);

				// Read a line from sock and show it
				n = read(sock, ans, MAX);
				printf("client: read  n=%d bytes; echo=(%s)\n", n, ans);
			}
			printf("\n******** END put COMMAND ********\n");
		}
		printf("\n");
	}
  }
}


