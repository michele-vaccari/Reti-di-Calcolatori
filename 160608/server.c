#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

#define N 256

/* Gestore del segnale SIGCHLD */
void handler (int s)
{
	int status;
	while (waitpid(-1, &status, WNOHANG) > 0);
}

int main (int argc, char** argv)
{
	struct addrinfo hints, *res; /* direttive per getaadrinfo */
	int err, sd, ns, pid_f, pid_n1, pid_n2, on, n1n2[2], n1f[2];
	char argomento[N], data[N], numero[N], db_path[N];
	const char *ack = "ack\n"; /* creando l'ack aggiungo \n per consentire al client Java la readLine */

	/* Controllo argomenti */
	if(argc != 2)
	{
		printf("Uso: ./server <porta>\n");
		exit(1);
	}

	/* Gestore dei segnali */
	signal(SIGCHLD, handler);

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC; /* Uso indirizzi famiglia UNSPEC */
	hints.ai_socktype = SOCK_STREAM; /* Uso una socket di tipo stream */
	hints.ai_flags = AI_PASSIVE;

	if ((err = getaddrinfo(NULL, argv[1], &hints, &res)) != 0)
	{
		/* Non faccio la gestione degli errori con la perror perchè non è una system call ma una funzione di libreria */
		fprintf(stderr, "Errore setup indirizzo bind: %s\n", gai_strerror(err));
		exit(2);
	}
	
	if ((sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0)
	{
		perror("Errore in socket");
		exit(3);
	}

	on = 1;

	if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
	{
		perror("setsockopt");
		exit(4);
	}

	if (bind(sd, res->ai_addr, res->ai_addrlen) < 0)
	{
		perror("Errore in bind");
		exit(5);
	}
	
	/* Libero la memoria allocata da getaddrinfo */
	freeaddrinfo(res);

	if(listen(sd, SOMAXCONN) < 0)
	{
		perror("listen");
		exit(6);
	}

	signal(SIGCHLD, handler);

	/* Attendo i client... */
	for(;;)
	{
		printf("Server in ascolto...\n");
		
		if((ns = accept(sd, NULL, NULL)) < 0)
		{
			/* Ignoro gli errori di tipo interruzione da segnale */
			if (errno == EINTR)
				continue;
			/* Gestisco tutte le altre tipologie di errore */
			perror("accept");
			exit(7);
		}

		/* Generazione di un figlio */
		if ((pid_f = fork()) < 0)
		{
			perror("fork");
			exit(8);
		}
		else if (pid_f == 0)
		{
			/* Chiudo la socket passiva */
			close(sd);

			/* figlio */

			/* Leggi l'argomento */
			memset(argomento, 0, sizeof(argomento));
			if (read(ns, argomento, sizeof(argomento)-1) < 0)
			{
				perror("read argomento");
				exit(9);
			}

			/* Mando ack */
			if (write(ns, ack, strlen(ack)) < 0)
			{
				perror("write ack");
				exit(10);
			}

			/* Leggi la data */
			memset(data, 0, sizeof(data));
			if (read(ns, data, sizeof(data)-1) < 0)
			{
				perror("read data");
				exit(11);
			}

			/* Mando ack */
			if (write(ns, ack, strlen(ack)) < 0)
			{
				perror("write ack");
				exit(11);
			}

			/* Leggi il numero */
			memset(numero, 0, sizeof(numero));
			if (read(ns, numero, sizeof(numero)-1) < 0)
			{
				perror("read numero");
				exit(12);
			}
			
			/* Memorizzo il path */
			memset(db_path, 0, sizeof(db_path));
			/* snprintf(db_path, sizeof(db_path)-1, "/var/local/news/%s.txt", data); */
			snprintf(db_path, sizeof(db_path)-1, "%s.txt", data); /* per il test */

			/* Pipe */
			if (pipe(n1f) < 0)
			{
				perror("pipe n1f");
				exit(13);
			}

			if ((pid_n1 = fork()) < 0)
			{
				perror("seconda fork");
				exit(14);
			}
			else if (pid_n1 == 0)
			{
				/* Nipote N1 */

				/* Chiudo i descrittori che non servono */
				close(ns);
				close(n1f[0]);

				/* Ridireziono lo stdout */
				close(1);
				dup(n1f[1]);
				close(n1f[1]);

				/* Pipe */
				pipe(n1n2);
				
				if ((pid_n2 = fork()) < 0)
				{
					perror("terza fork");
					exit(16);
				}
				else if (pid_n2 == 0)
				{
					/* Nipote N2 */

					/* Chiudo i descrittori che non servono */
					close(n1n2[0]);

					/* Ridireziono lo stdout */
					close(1);
					dup(n1n2[1]);
					close(n1n2[1]);

					/* Eseguo la grep e rimando i risultati a nipote 1 */
					execlp("grep", "grep", argomento, db_path, NULL);
					perror("exec grep");
					exit(17);
				}
				/* Nipote N1 */

				/* Chiudo i descrittori che non servono */
				close(n1n2[1]);

				/* Ridireziono lo stdin */
				close(0);
				dup(n1n2[0]);
				close(n1n2[0]);
				
				/* Eseguo la sort decrescente e rimando i risultati al figlio */
				execlp("sort", "sort", "-rn", NULL);
				perror("exec sort");
				exit(15);
				
			}

			/* Figlio */

			/* Chiudo i descrittori non necessari */
			close(n1f[1]);

			/* Ridireziono stdin */
			close(0);
			dup(n1f[0]);
			close(n1f[0]);

			/* Ridireziono stdout */
			close(1);
			dup(ns);
			close(ns);

			execlp("head", "head", "-n", numero, NULL); /* head */
			perror("exec head");
			exit(18);
		}
		/* Padre */
		close(ns);
	}
	return 0;
}
