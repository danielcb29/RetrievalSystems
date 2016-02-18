import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;

public class Instancias {
	public static void main (String [] args) throws Exception {
        if (args.length<1) {
                System.out.println("ERROR. formato: >java ListIt nombre_archivo");
                return;
        }
        File fichero = new File(args[0]);
        if (!fichero.exists() || !fichero.canRead()) {
                System.out.println("No puedo leer " + fichero);
                return;
        }
        if (fichero.isDirectory()) {
                String [] listaFicheros = fichero.list();
                for (int i=0; i<listaFicheros.length; i++)
                        System.out.println(listaFicheros[i]);
        }
        else try {
                FileReader fr = new FileReader(fichero);
                BufferedReader br = new BufferedReader(fr);
                String linea;
                while ((linea=br.readLine()) != null)
                        System.out.println(linea);
             }
             catch (FileNotFoundException fnfe) {
                System.out.println("Fichero desaparecido en combate  ;-)");
             }
 }
}

