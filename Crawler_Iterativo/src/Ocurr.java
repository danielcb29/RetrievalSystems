import java.io.Serializable;
import java.util.Map;
import java.util.TreeMap;

public class Ocurr implements Serializable {
	private Integer ftt;
	private Map ocurrencia;

	//Constructor caso no existe ocurr
	public Ocurr(int archivo){
		this.ocurrencia = new TreeMap();
		this.ftt = 0;
		putOcurrencia(archivo);
	}
	
	/**
	 * Permite insertar nueva ocurrencia o actualizar el valor de ftt y ftl de una ocurrencia
	 * @param archivo
	 */
	public void putOcurrencia(int archivo){
		this.ftt++;
		Object o = this.ocurrencia.get(archivo);
		if(o == null){
			Valor_Ocurr v = new Valor_Ocurr(archivo);
			this.ocurrencia.put(archivo,v);
		}else{
			Valor_Ocurr valor= (Valor_Ocurr) o;
			valor.aumentarFTL();
		}
		
	}
	


	@Override
	public String toString() {
		return "Ocurr [ftt=" + ftt + ", ocurrencia=" + ocurrencia + "]";
	}

	public Integer getFtt() {
		return ftt;
	}

	public void setFtt(Integer ftt) {
		this.ftt = ftt;
	}
	
	public Map getOcurrencias(){
		return this.ocurrencia;
	}
}
