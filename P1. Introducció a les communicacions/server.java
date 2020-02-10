package serversocket;
import java.io.*;
import java.net.*;
import java.util.Scanner; 
import java.util.logging.Level; 
import java.util.logging.Logger;

public class Main {
	/**
	* @param args the command line arguments */
	public static void main(String[] args) { // TODO code application logic here int contador = 0;
		try {
			ServerSocket server = new ServerSocket(8189); 
			while (true) {
				Socket socket = server.accept(); 
				System.out.println("Client numero " + contador); 
				Runnable r = new ServerManagement(socket, contador); 
				Thread t = new Thread(r);
				t.start();
				contador++;
			}
		} catch (IOException ioe) {
			ioe.printStackTrace(); 
		}
	}		 
}

class ServerManagement implements Runnable {
	public ServerManagement(Socket i, int c) { 
		socket = i;
		contador = c;
	}

	public void run() { 
		try {
			try {
				InputStream entrada = socket.getInputStream(); 
				OutputStream salida = socket.getOutputStream(); 
				Scanner in = new Scanner(entrada);

				PrintWriter out = new PrintWriter(salida,true); 
				out.println("Servidor connectat. Escriu BYE per sortir"); 
				boolean fin = true;
				while (fin && in.hasNextLine()){
					String linia = in.nextLine(); 
						out.println("ECHO: "+linia); 
						if(linia.trim().equals("BYE")) fin = false; 
				}
	
			} finally {
				socket.close(); 
			}
		} catch (IOException ex) { 
			Logger.getLogger(ServerManagement.class.getName()).log(Level.SEVERE, null, ex);
		} 
	}
	
	private Socket socket;
	private int contador; 
}
