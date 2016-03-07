import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.PrintWriter;
import java.nio.file.Files;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Scanner;
import java.util.Set;
import java.util.StringTokenizer;
import java.util.TreeMap;
import java.util.Vector;

public class Crawler {
	
	private Map vocabulario;
	private Vector<Archivo> archivos;
	
	Crawler(){
		this.vocabulario = new TreeMap();
		this.archivos = new Vector<Archivo>();
	}
	
	public void mapearArchivo (BufferedReader br, String url) throws IOException {
        String linea;
        Archivo archivo = new Archivo(url,0);
        int contador = archivos.size();
        while ( (linea = br.readLine () ) != null) {
                StringTokenizer st = new StringTokenizer (linea, ",.;(){}=+\"\'#&%?¿!¡*<>:-// ");
                while (st.hasMoreTokens () ) {
                		archivo.aumentarTotalTerminos(); //si encontramos un nuevo termino, aumentamos el nt
                        String s = st.nextToken();
                        Object o = this.vocabulario.get(s);
                        if (o == null){
                        	this.vocabulario.put (s, new Ocurr(contador)); //si no existe la ocurrencia creamos el objeto y lo metemos al mapa
                        }
                        else {
                                Ocurr ocurr = (Ocurr) o;
                                ocurr.putOcurrencia(contador); //si ya existe la ocurrencia aumentamos el ftt y el ftl
                        }
                }
        }
        archivos.add(archivo);
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
                mapearArchivo(br,fichero.toString());
                
        }
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
	
	public void stringArchivos(){
		for(int i=0; i< this.archivos.size();i++){
			System.out.println("Pos: "+i+" url:"+this.archivos.get(i).getUrl());
		}
	}
	
	public void buscar() throws IOException{
		BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
		System.out.println("Digita una palabra o %fin para terminar");
        String s = "";
        while (!s.equals("%fin")){
        	 s = br.readLine();
        	 Object o = this.vocabulario.get(s);
        	 if (o==null){
        		 System.out.println("El termino no existe en el vocabulario...");
        	 }else{
        		 Ocurr ocurr = (Ocurr) o;
        		 System.out.println("El numero de veces que aparece el termino es: "+ocurr.getFtt());
        		 Map ocurrencias = ocurr.getOcurrencias();
        		 List claves = new ArrayList (ocurrencias.keySet ());
                 Collections.sort (claves);
                 Iterator i = claves.iterator ();
                 while (i.hasNext ()) {
                         int k = (int)i.next ();
                         String url = this.archivos.get(k).getUrl();
                         Valor_Ocurr val = (Valor_Ocurr) ocurrencias.get (k);
                         System.out.println(url + " : " + val.getFtl());
                 }
                 
        	 }
        	 System.out.println("Digita una palabra o %fin para terminar");
        	 
        }
        System.out.println("La ejecucion ha terminado!");
	}
	
}
