/******************************************************************************** 
 * FichContPalabras.java														*
 *  Contabiliza palabras contenidas en un fichero								*
 * (i) Félix R. Rodríguez, GIM, Universidad de Extremadura, 2009				*
 * 						   http://gim.unex.es/felixr							*
 *******************************************************************************/

import java.io.*;
import java.util.*;

public class FichContPalabras {
        public Map mapearArchivo (BufferedReader br) throws IOException {
                
                Map map = new TreeMap ();
                String linea;

                while ( (linea = br.readLine () ) != null) {
                        StringTokenizer st = new StringTokenizer (linea);
                        while (st.hasMoreTokens () ) {
                                String s = st.nextToken();
                                Object o = map.get(s);
                                if (o == null) map.put (s, new Integer (1));
                                else {
                                        Integer cont = (Integer) o;
                                        map.put (s, new Integer (cont.intValue () + 1));
                                }
                        }
                }
                br.close ();

                List claves = new ArrayList (map.keySet ());
                Collections.sort (claves);

                return map;

        }
}
