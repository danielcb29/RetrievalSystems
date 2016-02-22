import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.nio.file.Files;
import java.util.Map;
import java.util.StringTokenizer;
import java.util.TreeMap;

public class Crawler {
	
	private Map vocabulario;
	
	Crawler(){
		this.vocabulario = new TreeMap();
	}
	
	public void mapearArchivo (BufferedReader br) throws IOException {
        String linea;

        while ( (linea = br.readLine () ) != null) {
                StringTokenizer st = new StringTokenizer (linea, ",.;(){}=+\"\'#&%?¿!¡*<>:-// ");
                while (st.hasMoreTokens () ) {
                        String s = st.nextToken();
                        Object o = this.vocabulario.get(s);                        
                        if (o == null) this.vocabulario.put (s, new Integer (1));
                        else {
                                Integer cont = (Integer) o;
                                this.vocabulario.put (s, new Integer (cont.intValue () + 1));
                        }
                }
        }
        br.close ();

	}
	public boolean cargarMap(){
		try {
			FileInputStream fis = new FileInputStream("dicc.ser");
			ObjectInputStream ois = new ObjectInputStream(fis);
			this.vocabulario = (Map) ois.readObject();
			return true;
	    }
	    catch (Exception e) { 
	    	return false; 
	    }
		
	}
	public void guardarMap(){
    	try {
            FileOutputStream fos = new FileOutputStream("dicc.ser");
            ObjectOutputStream oos = new ObjectOutputStream(fos);
            oos.writeObject(this.vocabulario);
        }
        catch (Exception e) { System.out.println(e); }
    }
	
	public void crearVocabulario(String dir) throws IOException{
		File fichero = new File(dir);
		if(fichero.isHidden()){
			return;
		}
        if (!fichero.exists() || !fichero.canRead()) {
                System.out.println("No puedo leer " + fichero);
                return;
        }
        if (fichero.isDirectory()) {
        		//System.out.println(fichero.toString());
                String [] listaFicheros = fichero.list();
                
                for (int i=0; i<listaFicheros.length; i++){
                        crearVocabulario(dir+"/"+listaFicheros[i]);
                }
        }
        else{ 
        		//System.out.println(fichero.toString());
                FileReader fr = new FileReader(fichero);
                BufferedReader br = new BufferedReader(fr);
                mapearArchivo(br);
                
        }
	}
	
	public Map getVocabulario(){
		return this.vocabulario;
	}

	
}
