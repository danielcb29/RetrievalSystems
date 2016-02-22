
public class Main {
	public static void main (String [] args) throws Exception {
        if (args.length<1) {
                System.out.println("ERROR. formato: >java ListIt nombre_archivo");
                return;
        }
        Crawler c = new Crawler();
        if(c.cargarMap()){
        	System.out.println(c.getVocabulario().toString());
        	System.out.println("Existe!");
        }else{
        	c.crearVocabulario("/Users/daniel/Documents/UnEx/RWB/carpeta");
            c.guardarMap();
        }
        
        
	}
}
