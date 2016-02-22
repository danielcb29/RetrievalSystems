import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.ObjectOutputStream;
import java.util.Map;

public class Crawler {
	
	public static void guardarMap(Map vocabulario){
    	try {
            FileOutputStream fos = new FileOutputStream("dicc.ser");
            ObjectOutputStream oos = new ObjectOutputStream(fos);
            oos.writeObject(vocabulario);
        }
        catch (Exception e) { System.out.println(e); }
    }
	
	public static void crearVocabulario(String dir) throws IOException{
		File fichero = new File(dir);
        if (!fichero.exists() || !fichero.canRead()) {
                System.out.println("No puedo leer " + fichero);
                return;
        }
        if (fichero.isDirectory()) {
                String [] listaFicheros = fichero.list();
                for (int i=0; i<listaFicheros.length; i++)
                        crearVocabulario(listaFicheros[i]);
        }
        else{ 
                FileReader fr = new FileReader(fichero);
                BufferedReader br = new BufferedReader(fr);
                FichContPalabras fich = new FichContPalabras();
                Map vocabulario =  fich.mapearArchivo(br);
                guardarMap(vocabulario);
        }
	}
	
	public static void main (String [] args) throws Exception {
        if (args.length<1) {
                System.out.println("ERROR. formato: >java ListIt nombre_archivo");
                return;
        }
        crearVocabulario(args[0]);
        
}
}
