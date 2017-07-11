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
				/* Leggo il mese */
				System.out.println("Inserisci il mese ('fine' per uscire): ");
				String mese = userIn.readLine();

				if (mese.equals("fine"))
				{
					System.out.println("Hai scelto di terminare il programma.");
					break;
				}
				
				/* Leggo l'anno */
				System.out.println("Inserisci l'anno ('fine' per uscire): ");
				String anno = userIn.readLine();

				if (anno.equals("fine"))
				{
					System.out.println("Hai scelto di terminare il programma.");
					break;
				}
				
				while(true)
				{					
					if (!isNumber(anno))
					{
						System.out.println("Devi inserire un anno! Anno: ");
						anno = userIn.readLine();
					}
					else
						break;
				}
				
				/* Leggo la destinazione */
				System.out.println("Inserisci la destinazione ('fine' per uscire): ");
				String destinazione = userIn.readLine();

				if (destinazione.equals("fine"))
				{
					System.out.println("Hai scelto di terminare il programma.");
					break;
				}
				
				/* Creo una nuova socket (effettua la risoluzione dei nomi e crea la socket) */
				Socket s = new Socket(argv[0], Integer.parseInt(argv[1]));
			
				/* Creo stream di lettura e scrittura per il server */
				BufferedReader fromServer = new BufferedReader(new InputStreamReader(s.getInputStream(), "UTF-8"));
				BufferedWriter toServer = new BufferedWriter(new OutputStreamWriter(s.getOutputStream(), "UTF-8"));

				/* Invio il mese */
				toServer.write(mese);
				toServer.flush();

				String buff = fromServer.readLine();

				if(!buff.equals("ack"))
				{
					System.err.println("Errore lettura Ack dal server");
					System.exit(2);
				}

				/* Invio l'anno */
				toServer.write(anno);
				toServer.flush();

				buff = fromServer.readLine();

				if(!buff.equals("ack"))
				{
					System.err.println("Errore lettura Ack dal server");
					System.exit(3);
				}

				/* Invio la destinazione */
				toServer.write(destinazione);
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
	}
}
