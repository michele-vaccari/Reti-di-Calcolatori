import java.io.*;
import java.net.*;

public class Client
{
	static boolean isNumber(String num)
	{
		try
		{
			Integer.parseInt(num);
		}
		catch (NumberFormatException ne)
		{
			return false;
		}
		return true;
	}
	
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
				/* Leggo l'argomento */
				System.out.println("Inserisci l'argomento ('fine' per uscire): ");
				String argomento = userIn.readLine();

				if (argomento.equals("fine"))
				{
					System.out.println("Hai scelto di terminare il programma.");
					break;
				}
				
				/* Leggo la data */
				System.out.println("Inserisci la data ('fine' per uscire): ");
				String data = userIn.readLine();

				if (data.equals("fine"))
				{
					System.out.println("Hai scelto di terminare il programma.");
					break;
				}
				
				/* Leggo il numero */
				System.out.println("Inserisci il numero ('fine' per uscire): ");
				String numero = userIn.readLine();

				if (numero.equals("fine"))
				{
					System.out.println("Hai scelto di terminare il programma.");
					break;
				}
				
				while(true)
				{					
					if (!isNumber(numero))
					{
						System.out.println("Devi inserire un numero intero! Numero: ");
						numero = userIn.readLine();
					}
					else
						break;
				}
				
				/* Creo una nuova socket (effettua la risoluzione dei nomi e crea la socket) */
				Socket s = new Socket(argv[0], Integer.parseInt(argv[1]));
			
				/* Creo stream di lettura e scrittura per il server */
				BufferedReader fromServer = new BufferedReader(new InputStreamReader(s.getInputStream(), "UTF-8"));
				BufferedWriter toServer = new BufferedWriter(new OutputStreamWriter(s.getOutputStream(), "UTF-8"));

				/* Invio l'argomento */
				toServer.write(argomento);
				toServer.flush();

				String buff = fromServer.readLine();

				if(!buff.equals("ack"))
				{
					System.err.println("Errore lettura Ack dal server");
					System.exit(2);
				}

				/* Invio la data */
				toServer.write(data);
				toServer.flush();

				buff = fromServer.readLine();

				if(!buff.equals("ack"))
				{
					System.err.println("Errore lettura Ack dal server");
					System.exit(3);
				}

				/* Invio numero */
				toServer.write(numero);
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
