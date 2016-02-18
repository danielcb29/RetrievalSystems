//programa CargarObjeto.java

import java.io.*;
import java.util.*;

public class CargarObjeto {
        public static void main (String args[]) {
                try {
                        FileInputStream fis = new FileInputStream("h.ser");
                        ObjectInputStream ois = new ObjectInputStream(fis);
                        Hashtable h = (Hashtable) ois.readObject();
                        System.out.println(h.toString());
                }
                catch (Exception e) { System.out.println(e); }
        }
}
