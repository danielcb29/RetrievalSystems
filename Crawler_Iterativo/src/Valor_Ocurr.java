import java.io.Serializable;

public class Valor_Ocurr implements Serializable {
	private Integer ftl; //cuentas veces aparece una palabra en el archivo
	//private int archivo;
	
	public Valor_Ocurr(int archivo){
		this.ftl = new Integer(1);
		//this.archivo = archivo;
	}
	
	public void aumentarFTL(){
		this.ftl+= new Integer(1);
	}

	@Override
	public String toString() {
		return "Valor_Ocurr [ftl=" + ftl + "]";
	}

	public Integer getFtl() {
		return ftl;
	}

	public void setFtl(Integer ftl) {
		this.ftl = ftl;
	}

	
	
}
