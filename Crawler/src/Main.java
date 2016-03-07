
public class Main {
	public static void main (String [] args) throws Exception {
       
        Crawler c = new Crawler();
        if(c.cargarMap()){
        	System.out.println("Existe!");
        	//System.out.println(System.getProperty("user.home"));
        	//c.stringVocabulario();
        	System.out.println("Empieza a buscar...");
        	c.buscar();
        	
        }else{
        	System.out.println("Creando mapa...");
        	String home = System.getProperty("user.home");
        	c.crearVocabulario("/Users/daniel/Documents/UnEx/RWB/carpeta");
        	//c.crearVocabulario(home);
            c.guardarMap();
            System.out.println("He terminado...");
            //c.stringArchivos();
            //c.stringVocabulario();
            System.out.println("Empieza a buscar...");
            c.buscar();
        }
        
        
	}
}
