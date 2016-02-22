//programa SalvarObjeto.java

import java.io.*;
import java.util.*;

public class SalvarObjeto {
        public void guardarMap(Map vocabulario){
        	try {
                FileOutputStream fos = new FileOutputStream("dicc.ser");
                ObjectOutputStream oos = new ObjectOutputStream(fos);
                oos.writeObject(vocabulario);
	        }
	        catch (Exception e) { System.out.println(e); }
        }
}
