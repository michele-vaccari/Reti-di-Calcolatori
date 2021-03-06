#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define DIM 256

int main(int argc, char** argv)
{
	int err;
	struct addrinfo hints; /* direttive per getaddrinfo */
	struct addrinfo *res, *ptr;
	char *host_remoto; /* nome host remoto */
	char *servizio_remoto; /* numero porta servizio remoto */
	int sd, nread;
	char buf[DIM], mese[DIM], anno[DIM], localita[DIM];
	int i=1; /* per contare quante volte viene fatta la connect */
	char *ackVer="ack\n";

	/* Controllo argomenti */
	if (argc != 3)
	{
		printf("Uso: pacchetti_vacanze <server> <porta>\n");
		exit(1);
	}
	
	for(;;) /* Ciclo infinito, 'fine' per uscire*/
	{
		/* Costruzione dell'indirizzo */
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET; /* Uso indirizzi famiglia AF_INET */
		hints.ai_socktype = SOCK_STREAM; /* Uso una socket di tipo stream */
		
		/* Leggo il mese */
		printf("Inserisci il mese ('fine' per uscire):\n");
		scanf("%s", mese); /* ho il terminatore */

		if(strcmp(mese, "fine") == 0)
		{
			close(sd);
			printf("Hai scelto di terminare il programma.\n");
			break;
		}
	
		/* Leggo l'anno */
		printf("Inserisci l'anno ('fine' per uscire):\n");
		scanf("%s", anno); /* ho il terminatore */

		if(strcmp(anno, "fine") == 0)
		{
			close(sd);
			printf("Hai scelto di terminare il programma.\n");
			break;
		}
	
		/* Leggo la localita */
		printf("Inserisci la localita ('fine' per uscire):\n");
		scanf("%s", localita); /* ho il terminatore */
		
		if(strcmp(localita, "fine") == 0)
		{
			close(sd);
			printf("Hai scelto di terminare il programma.\n");
			break;
		}

		/* Risoluzione dell'host */
		host_remoto = argv[1];
		servizio_remoto = argv[2];

		if ((err = getaddrinfo(host_remoto, servizio_remoto, &hints, &res)) != 0)
		{
			fprintf(stderr, "Errore risoluzione nome: %s\n", gai_strerror(err));
			exit(2);
		}

		for (ptr = res; ptr != NULL; ptr = ptr->ai_next)
		{	/* se socket fallisce salto direttamente alla prossima iterazione */
			sd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

			if (sd < 0)
			{
				fprintf(stderr, "creazione socket fallita\n");
				continue;
			}
			/* se connect funziona esco dal ciclo */
			if (connect(sd, ptr->ai_addr, ptr->ai_addrlen) == 0)
			{
				printf("Connect riuscita al tentativo %d\n",i);
				break;
			}
			i++;
			close(sd);
		}

		/* Verifica sul risultato restituito da getaddrinfo */
		if (ptr == NULL)
		{
			fprintf(stderr, "Errore risoluzione nome: nessun indirizzo corrispondente trovato\n");
			exit(3);
		}
		
		/* libero la memoria allocata da getaddrinfo */
		freeaddrinfo(res);

		/* Mando la mese al server */
		if (write(sd, mese, strlen(mese)) < 0)
		{
			perror("write mese");
			exit(4);
		}

		/* Leggo l'ack */
		memset(buf, 0, sizeof(buf));
		if (read(sd, buf, DIM) < 0)
		{
			perror("read ack");
			exit(5);
		}
	
		if (strcmp(buf, ackVer) != 0)
		{
			close(sd);
			printf("errore nell'ack\n");
			exit(6);
		}

		/* Mando l'anno al server */
		if (write(sd, anno, strlen(anno)) < 0)
		{
			perror("write anno");
			exit(7);
		}

		/* Leggo l'ack */
		memset(buf, 0, sizeof(buf));
		if (read(sd, buf, DIM) < 0)
		{
			perror("read ack");
			exit(8);
		}
		
		if (strcmp(buf, ackVer) != 0)
		{
			close(sd);
			printf("errore nell'ack\n");
			exit(9);
		}

		/* Mando la localita al server */
		if (write(sd, localita, strlen(localita)) < 0)
		{
			perror("write localita");
			exit(10);
		}

		/* Ricezione risultato */
	
		/* Svuoto il buffer di printf perima di iniziare a scrivere sullo standard output con write */
		fflush(stdout);

		memset(buf, 0, sizeof(buf));
		while ((nread = read(sd, buf, sizeof(buf))) > 0)
		{
			if (write(1, buf, nread) < 0)
			{
				perror("write su stdout");
				exit(11);
			}
		}

		/* Controllo errori di lettura */
		if (nread < 0)
		{
			perror("read del risultato");
			exit(12);
		}
		
		close(sd);
	}
	return 0;
}
