//programa SalvarObjeto.java

import java.io.*;
import java.util.*;

public class SalvarObjeto {
        public static void main (String args[]) {
                Hashtable h = new Hashtable();
                h.put("String","Luis Rodr�guez Dur�n");
                h.put("Integer",new Integer(2));
                h.put("Double",new Double(0.96));
                try {
                        FileOutputStream fos = new FileOutputStream("h.ser");
                        ObjectOutputStream oos = new ObjectOutputStream(fos);
                        oos.writeObject(h);
                }
                catch (Exception e) { System.out.println(e); }
        }
        public String guardarMap(){
        	return null;
        }
}
