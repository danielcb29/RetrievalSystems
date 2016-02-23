import java.io.Serializable;
import java.util.Map;
import java.util.TreeMap;

public class Ocurr implements Serializable {
	private Integer ftt;
	private Map ocurrencia;

	//Constructor caso no existe ocurr
	public Ocurr(String url, Integer nt){
		this.ocurrencia = new TreeMap();
		this.ftt = 0;
		putOcurrencia(url,nt);
	}
	
	public void putOcurrencia(String url,Integer nt){
		this.ftt++;
		Object o = this.ocurrencia.get(url);
		if(o == null){
			Valor_Ocurr v = new Valor_Ocurr(nt);
			this.ocurrencia.put(url,v);
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
	
}
