
public class Archivo {
	private String url;
	private int totalTerminos; //cuantos terminos en total tiene el archivo
	
	public Archivo(String url,int totalTerminos){
		this.url = url;
		this.totalTerminos = totalTerminos;
	}
	
	public String getUrl() {
		return url;
	}
	public void setUrl(String url) {
		this.url = url;
	}
	public int getTotalTerminos() {
		return totalTerminos;
	}
	public void setTotalTerminos(int totalTerminos) {
		this.totalTerminos = totalTerminos;
	}
	
	public void aumentarTotalTerminos(){
		this.totalTerminos++;
	}
	
}
