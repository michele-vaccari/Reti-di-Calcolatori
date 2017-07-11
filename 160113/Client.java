import java.io.*;
import java.net.*;

public class Client
{
	public static void main(String[] argv)
	{
		/* Controllo argomenti */
		if (argv.length != 2)
		{
			System.err.println("Uso corretto: java Client server porta");
			System.exit(1);
		}

		try
		{			
			/* Creo stream di lettura per l'utente */
			BufferedReader userIn = new BufferedReader(new InputStreamReader(System.in));

			while (true)
			{
				/* Leggo il nome del database */
				System.out.println("Inserisci il nome del database ('fine' per uscire): ");
				String nomeDB = userIn.readLine();

				if (nomeDB.equals("fine"))
				{
					System.out.println("Hai scelto di terminare il programma.");
					break;
				}
				
				/* Leggo il tipo componente */
				System.out.println("Inserisci il tipo componente ('fine' per uscire): ");
				String tipoComponente = userIn.readLine();

				if (tipoComponente.equals("fine"))
				{
					System.out.println("Hai scelto di terminare il programma.");
					break;
				}

				/* Leggo il sistema operativo */
				System.out.println("Inserisci il sistema operativo ('fine' per uscire): ");
				String sistemaOP = userIn.readLine();

				if (sistemaOP.equals("fine"))
				{
					System.out.println("Hai scelto di terminare il programma.");
					break;
				}
				
				/* Creo una nuova socket (effettua la risoluzione dei nomi e crea la socket) */
				Socket s = new Socket(argv[0], Integer.parseInt(argv[1]));
			
				/* Creo stream di lettura e scrittura per il server */
				BufferedReader fromServer = new BufferedReader(new InputStreamReader(s.getInputStream(), "UTF-8"));
				BufferedWriter toServer = new BufferedWriter(new OutputStreamWriter(s.getOutputStream(), "UTF-8"));

				/* Invio nome del database */
				toServer.write(nomeDB);
				toServer.flush();

				String buff = fromServer.readLine();

				if(!buff.equals("ack"))
				{
					System.err.println("Errore lettura Ack dal server");
					System.exit(2);
				}

				/* Invio tipo componente */
				toServer.write(tipoComponente);
				toServer.flush();

				buff = fromServer.readLine();

				if(!buff.equals("ack"))
				{
					System.err.println("Errore lettura Ack dal server");
					System.exit(3);
				}

				/* Invio sistema operativo */
				toServer.write(sistemaOP);
				toServer.flush();

				/* Ricevi e stampa output */
				while ((buff = fromServer.readLine()) != null)
				{
					System.out.println(buff);
				}
				
				/* Chiudo la socket (non lo fa automaticamente il garbadge collector) */
				s.close();
			}
		}
		
		catch (IOException e)
		{
			System.err.println(e.getMessage());
			System.exit(4);
		}
		
		/* L'IOException gestisce già le eccezioni seguenti */
		/*
		catch (UnknownHostException e)
		{
			System.err.println(e.getMessage());
			e.printStackTrace();
			System.exit(100);
		}

		catch (Exception e)
		{
			System.err.println(e.getMessage());
			e.printStackTrace();
			System.exit(101);
		} */
	}
}
