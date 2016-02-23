import java.io.Serializable;

public class Valor_Ocurr implements Serializable {
	private Integer ftl;
	private Integer nt;
	
	public Valor_Ocurr(Integer nt){
		this.ftl = new Integer(1);
		this.nt = nt;
	}
	
	public void aumentarFTL(){
		this.ftl+= new Integer(1);
	}

	@Override
	public String toString() {
		return "Valor_Ocurr [ftl=" + ftl + ", nt=" + nt + "]";
	}
	
	
}
