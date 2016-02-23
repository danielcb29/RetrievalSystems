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
import java.util.Iterator;
import java.util.Map;
import java.util.Scanner;
import java.util.Set;
import java.util.StringTokenizer;
import java.util.TreeMap;
import java.util.Vector;

public class Crawler {
	
	private Map vocabulario;
	private Vector<String> urls;
	
	Crawler(){
		this.vocabulario = new TreeMap();
		this.urls = new Vector<String>();
	}
	
	public void mapearArchivo (BufferedReader br, Integer nt, String url) throws IOException {
        String linea;
        this.urls.add(url);
        while ( (linea = br.readLine () ) != null) {
                StringTokenizer st = new StringTokenizer (linea, ",.;(){}=+\"\'#&%?¿!¡*<>:-// ");
                while (st.hasMoreTokens () ) {
                        String s = st.nextToken();
                        Object o = this.vocabulario.get(s);
                        if (o == null){
                        	this.vocabulario.put (s, new Ocurr(this.urls.indexOf(url),nt));
                        }
                        else {
                                Ocurr ocurr = (Ocurr) o;
                                ocurr.putOcurrencia(this.urls.indexOf(url), nt);
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
                mapearArchivo(br,countWords(fichero),fichero.toString());
                
        }
	}
	
	public Integer countWords(File fl) throws IOException{
		int result = 0;
		Scanner in = new Scanner(new FileInputStream(fl));
	    while(in.hasNextLine())  {
	        String line = in.nextLine();
	        result += new StringTokenizer(line, ",.;(){}=+\"\'#&%?¿!¡*<>:-// ").countTokens();
	    }
        return new Integer(result);

	}
	public Map getVocabulario(){
		return this.vocabulario;
	}

	public void stringVocabulario(){
		Set a = this.vocabulario.keySet();
		Iterator i = a.iterator();
		while(i.hasNext()) {
			String setElement = (String) i.next();
	        //String setElement = this.urls.get(((Integer) i.next()).intValue());
	        System.out.println(setElement+": "+this.vocabulario.get(setElement));
	    }
		
	}
	
}
