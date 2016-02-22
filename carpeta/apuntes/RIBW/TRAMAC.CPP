#include "TRAMAC.H"
#include "COMDOS.H"
#include "STDIO.H"
//
#include <DIR.H>
#include <DOS.H>
#include <FSTREAM.H>
#include <STRING.H>
#include <iostream.H>

/*
 * Autores:
 * Cristina Extremera Romero
 * Alberto Ricci Vázquez
 */

TramaC::TramaC(){
	S = 22;
	D = 'T';
	N = '0';
	BCE = 1;
}

void TramaC::Enviar(TCom &Puerto){
	char i;
	
	printf("Elija qué trama desea: \n");
	printf("1: Trama ENQ \n");
	printf("2: Trama EOT \n");
	printf("3: Trama ACK \n");
	printf("4: Trama NACK \n");
	
	i=getch();
	
	switch (i){
		case '1':
			C = 05;
			break;
		case '2':
			C = 04;
			break;
		case '3':
			C = 06;
			break;
		case '4':
			C = 21;
			break;
		default:
			printf("Número no válido\n");
	}
	
	Puerto << S;
	Puerto << D;
	Puerto << C;
	Puerto << N;
	
	switch(C){
		case 05:
			printf("Trama ENQ enviada \n");
			break;
		case 04:
			printf("Trama EOT enviada \n");
			break;
		case 06:
			printf("Trama ACK enviada \n");
			break;
		case 21:
			printf("Trama NACK enviada \n");
			break;
	}
}

void TramaC::EnviarDatos(TCom &Puerto, char cadena[], int longitud){
	int j=0, cont = 0;
	C=02;

	//printf("El mensaje tiene %i caracteres\n", longitud);
	
	while(j != longitud){
		Datos[cont] = cadena[j];
		cont ++;
		j++;
		if (j == longitud){
			L = cont;
			Datos[cont] = '\0';
			cont = 0;
			Puerto << S;
			Puerto << D;
			Puerto << C;
			Puerto << N;
			Puerto << L;
			Puerto << Datos;
			Puerto << BCE;
		} 
		else {
			if(cont == 254){ 
				L = cont;
				Datos[cont] = '\0';
				cont = 0;
				Puerto << S;
				Puerto << D;
				Puerto << C;
				Puerto << N;
				Puerto << L;
				Puerto << Datos;
				Puerto << BCE;
			}
		}
		
	}

}

void TramaC::EnviarFichero(TCom &Puerto, int &campo, int &control, int &contDatos, int &fichero, char nombre[], ofstream &f, int &apertura){
	int done;
	struct ffblk ffblk;
	char ruta[23] = "Ficheros\\";
	char cadena[255];
	int envio = 1; //esta variable nos servirá por si el buffer estaba lleno y todavía no habíamos enviado la información guardada
	char salida;
	TramaC aux; //esta trama la usaremos como auxiliar para poder recibir mientras enviamos el fichero

	done = findfirst("Ficheros\\*.txt", &ffblk, 0);
	
	if (done != 0){
		printf("No se encuentra ningún fichero");
	} else {
		ifstream lectura;
		strcat(ruta, ffblk.ff_name);
		
		lectura.open(ruta);
		
		if(!lectura.eof()){
			Puerto << '$';
			Puerto << ffblk.ff_name;
			int salir = 0;
			printf("Enviando fichero...\n");
			while(!lectura.eof() && salir == 0){
				C=02;
				
				if (Puerto.OcupacionBufferEntrada()!=0){ //comprobamos si tenemos algo que recibir
					Puerto >> salida;
					if (salida == '#'){
						printf("El receptor ha cortado el envío\n");
						salir = 1;
					} else {
						aux.Recibir(campo, salida, control, contDatos, fichero, nombre, f, apertura);
					}
				}
				
				if (envio == 1){
					lectura.getline(cadena, 255, EOF);
					strcpy(Datos, cadena);//Datos = cadena;
					L = strlen(Datos);
					envio = 0;	//ponemos envio a 0 para que no entre otra vez en este while hasta que no se envien estos datos
				}
				if ( (Puerto.OcupacionBufferSalida()+L+6) <1024 && salir == 0){ //comprobamos si tenemos espacio en nuestro buffer, si lo hay, enviamos
					Puerto << S;
					Puerto << D;
					Puerto << C;
					Puerto << N;
					Puerto << L;
					Puerto << Datos;
					Puerto << BCE;	
					envio = 1; //como ya hemos enviado, podemos volver a poner envio a 1
				}
				if (Puerto.OcupacionBufferEntrada()!=0){ //comprobamos si tenemos algo que recibir
					Puerto >> salida;
					if (salida == '#'){
						printf("El receptor ha cortado el envío\n");
						salir = 1;
					} else {
						aux.Recibir(campo, salida, control, contDatos, fichero, nombre, f, apertura);
					}
				}
				if(kbhit()){
					char s = getch();
					if (s == 27){
						salir = 1;
						printf("Envío interrumpido\n");
					}
				}
			}
			lectura.close();
			Puerto << '#';
			printf("Fichero enviado\n");
		}
	}
	
}

void TramaC::Recibir(int &campo, char salida, int &control, int &contDatos, int &fichero, char nombre[], ofstream &f, int &apertura){
	
	switch (campo){
		case 1:
			if (salida == 22){ //Compruebo si estoy recibiendo una trama
				control = 0;
				contDatos = 0;
				Datos[0]='\0';
				S = salida;
				campo++;
			} else {
				if(salida == '$'){ //compruebo si voy a recibir un fichero
					campo = 8;
					contDatos = 0;
					printf("Recibiendo fichero...\n");
				}
			}
			break;
		case 2:
			D = salida;
			campo++;
			break;
		case 3:
			C = salida;
			if (salida != 02){
				control = 1;
			}
			campo++;
			break;
		case 4:
			N = salida;
			if (control == 1){
				campo=1;
				switch (C){
					case 05:
						printf("Trama ENQ recibida\n");
						break;
					case 04:
						printf("Trama EOT recibida\n");
						break;
					case 06:
						printf("Trama ACK recibida\n");
						break;
					case 21:
						printf("Trama NACK recibida\n");
						break;
				}			
			} else {
				campo++;
			}
			break;
		case 5:
			L=(unsigned char) salida;
			//printf("El mensaje tiene %d caracteres\n", L);
			campo++;
			break;
		case 6:
			Datos[contDatos] = salida;
			if (contDatos < (L-1)){
				contDatos++;
			} else {
				contDatos++;
				Datos[contDatos] = '\0';
				campo++;
			}
			break;
		case 7: 
			BCE = salida;
			if (fichero == 1){
				campo = 8;
				//almacenar los datos en el fichero:
				int i = 0;
				while (i <L){
					f.put(Datos[i]);
					i++;
				}
			} else {
				campo = 1;
				ProcesarTrama();
			}
			break;
		case 8:
			if(salida != 22 && fichero == 0){ //leeré el nombre del fichero hasta que me llegue el sincronismo,  
			//y comprobamos que fichero sea 0 para que cuando sea 1 (que significa que estamos ya tratando el contenido del fichero) ya no entre en este if
				nombre[contDatos] = salida; 
				contDatos++;
			} else {
				if ( salida == '#'){ //compruebo si he terminado de recibir el fichero
					fichero = 0; //ponemos fichero a 0 porque ya no estamos tratando un fichero, hemos terminado
					apertura = 1; //reiniciamos apertura a 1
					campo = 1; //reiniciamos campo a 1
					f.close(); //cerramos flujo
					printf("Fichero recibido\n");
				} else {
					if (apertura >= 1){ //comprobamos si ya hemos abierto el flujo
						nombre[contDatos]='\0';
						f.open(nombre);
						apertura--;
					}
					fichero = 1; //estamos tratando el contenido del fichero
					campo = 2; //recibimos tramas de datos e inicializamos las variables de las tramas
					control = 0;
					contDatos = 0;
					Datos[0]='\0';
					S = salida;
				}
			}
			break;
		case 9:
			printf("Recepción interrumpida\n");
			fichero = 0; //ponemos fichero a 0 porque ya no estamos tratando un fichero, hemos terminado
			apertura = 1; //reiniciamos apertura a 1
			campo = 1; //reiniciamos campo a 1
			f.close(); //cerramos flujo
			break;
	  }
}

void TramaC::ProcesarTrama(){
	printf("%s" , Datos);
}


//----------------------------------------------------------- PROTOCOLO MAESTRO-ESCLAVO -----------------------------------------------------------//

void TramaC::IniciarProtocolo(TCom &Puerto, int &campo, char salida, int &contDatos, int &fichero, char nombre[], ofstream &f, int &apertura){
	printf("¿Que desea ser?\n 1. Maestro\n 2.Esclavo \n");
	
	char i=getch();
	
	switch(i){
		case '1':
			printf("Usted ha elegido maestro \n");
			EleccionOperacion(Puerto, campo, salida, contDatos, fichero, nombre, f, apertura);
			break;
		case '2':
			printf("Usted ha elegido esclavo \n");
			Esclavo(Puerto, campo, salida, contDatos, fichero, nombre, f, apertura);
			break;
	}
}

void TramaC::EleccionOperacion(TCom &Puerto, int &campo, char salida, int &contDatos, int &fichero, char nombre[], ofstream &f, int &apertura){
	printf("¿Que operación quiere realizar?\n");
	printf("1. Selección\n");
	printf("2. Sondeo\n");

	char i=getch();
	
	switch(i){
		case '1':
			printf("Iniciando selección... \n");
			SeleccionMaestro(Puerto);
			break;
		case '2':
			printf("Iniciando sondeo... \n");
			SondeoMaestro(Puerto, campo, salida, contDatos, fichero, nombre, f, apertura);
			break;
	}
}


void TramaC::Esclavo(TCom &Puerto, int &campo, char salida, int &contDatos, int &fichero, char nombre[], ofstream &f, int &apertura){
	unsigned char n, d;

	while (ENQRecibida(Puerto, d, n) != 1){
		//printf("b \n");
	}
	
	printf("Recibido 	%c		ENQ		%c \n", d, n);
	
	if (d == 'R'){
		SeleccionEsclavo(Puerto, campo, salida, contDatos, fichero, nombre, f, apertura);
	}
	if (d == 'T'){
		SondeoEsclavo(Puerto);
	}
}


int TramaC::ConfirmacionRecibida(TCom &Puerto, unsigned char &d, unsigned char &n){
	char sal;
	int i = 1, recibida = 0, correcto = 1, terminado = 1;
	//si recibida = 0 -> no he recibido nada
	//			  = 1 -> si recibo ACK
	//			  = 2 -> si recibo NACK
	
	while(Puerto.OcupacionBufferEntrada() != 0 && correcto == 1 && terminado == 1){
		Puerto >> sal;
	//	printf(" %c \n", sal);
		switch(i){
			case 1:
				if (sal != 22) correcto = 0;
				break;
			case 2:
				if (sal != 'T' && sal != 'R'){
					correcto = 0;
				} else {
					d = sal; //d la utilizaremos para mostrarlo posteriormente por pantalla
				}
				break;
			case 3:
				if (sal == 06){
					recibida = 1; // ACK
				} else {
					if (sal == 21){ // NACK
						recibida = 2;
					} else {
						correcto = 0;
					}
				}
				break;
			case 4:
				if (sal != '0' && sal != '1'){
					correcto = 0;
					recibida = 0;
				} else {
					n = sal;
					terminado = 0;
				}
				break;
		}
		i++;
	}
	return recibida;
}

int TramaC::ENQRecibida(TCom &Puerto, unsigned char &d, unsigned char &n){
	char sal;
	int i = 1, recibida = 0, correcto = 1;
	//si recibida = 0 -> no he recibido nada
	//			  = 1 -> si recibo ENQ
	
	while(Puerto.OcupacionBufferEntrada() != 0 && correcto == 1){
		Puerto >> sal;
		switch(i){
			case 1:
				if (sal != 22) correcto = 0;
				break;
			case 2:
				if (sal != 'T' && sal != 'R'){
					correcto = 0;
				} else {
					d = sal;
				}
				break;
			case 3:
				if (sal == 05){
					recibida = 1;
				} else {
					correcto = 0;
				}
				break;
			case 4:
				if (sal != '0' && sal != '1'){
					correcto = 0;
					recibida = 0;
				} else {
					n = sal;
				}
				break;
		}
		i++;
	}
	
	return recibida;
}

int TramaC::EOTRecibida(TCom &Puerto, unsigned char &d, unsigned char &n){
	char sal;
	int i = 1, recibida = 0, correcto = 1;
	//si recibida = 0 -> no he recibido nada
	//			  = 1 -> si recibo EOT
	
	while(Puerto.OcupacionBufferEntrada() != 0 && correcto == 1){
		Puerto >> sal;
		switch(i){
			case 1:
				if (sal != 22) correcto = 0;
				break;
			case 2:
				if (sal != 'T' && sal != 'R'){
					correcto = 0;
				} else {
					d=sal;
				}
				break;
			case 3:
				if (sal == 04){
					recibida = 1;
				} else {
					correcto = 0;
				}
				break;
			case 4:
				if (sal != '0' && sal != '1'){
					correcto = 0;
					recibida = 0;
				} else {
					n=sal;
				}
				break;
		}
		i++;
	}
	
	return recibida;
}

void TramaC::EstablecimientoSeleccion(TCom &Puerto){
	TramaC t;
	unsigned char d, n;
	
	t.D = 'R';
	t.C = 05;//ENQ
	
	Puerto << t.S;
	Puerto << t.D;
	Puerto << t.C;
	Puerto << t.N;
	
	printf("Enviado		%c		ENQ		%c \n", t.D, t.N);
	
	while ( ConfirmacionRecibida(Puerto, d, n) != 1){
		//printf("b \n");
	}
	printf("Recibido 	%c		ACK		%c \n", d, n);
}

void TramaC::EstablecimientoSondeo(TCom &Puerto){
	TramaC t;
	unsigned char d, n;

	t.C = 05;
	
	Puerto << t.S;
	Puerto << t.D;
	Puerto << t.C;
	Puerto << t.N;
	
	printf("Enviado		%c		ENQ		%c \n", t.D, t.N);
	
	while ( ConfirmacionRecibida(Puerto, d, n) != 1){
	
	}
	printf("Recibido 	%c		ACK		%c \n", d, n);
}


void TramaC::Liberacion(TCom &Puerto){
	TramaC t;
	unsigned char d, n;

	t.D = 'R';
	t.C = 04; //EOT
	
	Puerto << t.S;
	Puerto << t.D;
	Puerto << t.C;
	Puerto << t.N;
	
	printf("Enviado		%c		EOT		%c \n", t.D, t.N);
	
	while ( ConfirmacionRecibida(Puerto, d, n) != 1){
	
	}
	printf("Recibido 	%c		ACK		%c \n", d, n);
}

void TramaC::LiberacionSondeo(TCom &Puerto){
	TramaC t;
	unsigned char d, n;
	int r;

	t.D = 'T';
	t.C = 04; //EOT
	
	Puerto << t.S;
	Puerto << t.D;
	Puerto << t.C;
	Puerto << t.N;
	
	printf("Enviado		%c		EOT		%c \n", t.D, t.N);
	
	while ( (r = ConfirmacionRecibida(Puerto, d, n)) != 1){
		if (r == 2){
			printf("Recibido 	%c		NACK		%c \n", d, n);
			if (n == '0'){
				n = '1';
			} else {
				n = '0';
			}
			t.N = n;
			Puerto << t.S;
			Puerto << t.D;
			Puerto << t.C;
			Puerto << t.N;
			printf("Enviado		%c		EOT		%c \n", t.D, t.N);
		}
	}
	printf("Recibido 	%c		ACK		%c \n", d, n);
}


void TramaC::SeleccionMaestro(TCom &Puerto){
	int done, error, f6 = 0;
	struct ffblk ffblk;
	char ruta[23] = "Ficheros\\";
	char cadena[255];
	int envio = 1; //esta variable nos servirá por si el buffer estaba lleno y todavía no habíamos enviado la información guardada
	char salida, car, dato;
	unsigned char d, n;
	
	if (kbhit()){
      car=getch();
	  if(car == 64){
		f6 = 1;
	  }
	}
	
	//enviar trama ENQ con D=R
	EstablecimientoSeleccion(Puerto);

	D ='R';
	N = '0';
	
	done = findfirst("Ficheros\\*.txt", &ffblk, 0);
	
	if (kbhit()){
      car=getch();
	  if(car == 64){
		f6 = 1;
	  }
	}
	
	if (done != 0){
		printf("No se encuentra ningún fichero");
	} else {
		ifstream lectura;
		strcat(ruta, ffblk.ff_name);
		
		lectura.open(ruta);
		
		if(!lectura.eof()){
			Puerto << '$';
			Puerto << ffblk.ff_name;
			while(!lectura.eof()){
				if (kbhit()){
					car=getch();
					if(car == 64){
						f6 = 1;
					}
				}
				C=02;
				if (envio == 1){
					lectura.getline(cadena, 255, EOF);
					strcpy(Datos, cadena);//Datos = cadena;
					L = strlen(Datos);
					unsigned XOR= Datos[0]; 
					for(int i=1; i<L; i++){ 
					    XOR = XOR ^ Datos[i]; 
					}							
					if (kbhit()){
						car=getch();
						if(car == 64){
							f6 = 1;
						}
					}
					if (XOR == 0 || XOR == '$' || XOR == '#'){ 
						XOR = 1;								
					}											
					BCE = XOR;									
					envio = 0;	//ponemos envio a 0 para que no entre otra vez en este while hasta que no se envien estos datos
				}
				if ( (Puerto.OcupacionBufferSalida()+L+6) <1024){ //comprobamos si tenemos espacio en nuestro buffer, si lo hay, enviamos
					if (f6 == 1){
						dato = Datos[0];
						Datos[0] = 'ç';
						f6 = 0;
					}
					Puerto << S;
					Puerto << D;
					Puerto << C;
					Puerto << N;
					Puerto << L;
					Puerto << Datos;
					Puerto << BCE;	
					envio = 1; //como ya hemos enviado, podemos volver a poner envio a 1
					
					//mostrar por pantalla: enviado/recibido + D (R o T) + control (ENQ, ACK, STX..) + N + BCE (solo si es de texto)
					printf("Enviado		%c		STX		%c		%u \n", D, N, BCE); 
					//while no reciba ack
					while ( (error = ConfirmacionRecibida(Puerto, d, n)) != 1){ 
						if (error == 2){
							Datos[0] = dato;
							printf("Recibido 	%c		NACK		%c \n", d, n);
							Puerto << S;
							Puerto << D;
							Puerto << C;
							Puerto << N;
							Puerto << L;
							Puerto << Datos;
							Puerto << BCE;
							printf("Enviado		%c		STX		%c		%u \n", D, N, BCE);
						}
					}
					printf("Recibido 	%c		ACK		%c \n", d, n);
					
					if (N == '0'){
						N = '1';
					} else {
						N = '0';
					}
				}
/*				if (Puerto.OcupacionBufferEntrada()!=0){ //comprobamos si tenemos algo que recibir
					Puerto >> salida;
					if (salida == '#'){
						printf("El receptor ha cortado el envío\n");
						salir = 1;
					} 
				}
				if(kbhit()){
					char s = getch();
					if (s == 27){
						salir = 1;
						printf("Envío interrumpido\n");
					}
				}*/
				
			}
			lectura.close();
			Puerto << '#';
			//EOT
			Liberacion(Puerto);
			printf("Fichero enviado\n");
		}
	}
}


void TramaC::SeleccionEsclavo(TCom &Puerto, int &campo, char salida, int &contDatos, int &fichero, char nombre[], ofstream &f, int &apertura){
	unsigned bceAux;
	TramaC t; // enviamos ACK
	
	t.C = 06;
	t.D = 'R';
	
	Puerto << t.S;
	Puerto << t.D;
	Puerto << t.C;
	Puerto << t.N;
	
	printf("Enviado		%c		ACK		%c \n", t.D, t.N);
	
	int fin = 0;
	while (fin == 0){
		if (Puerto.OcupacionBufferEntrada()!=0){
		Puerto >> salida;
		
		switch (campo){
			case 1:
				if (salida == 22){ //Compruebo si estoy recibiendo una trama
					contDatos = 0;
					Datos[0]='\0';
					S = salida;
					campo++;
				} else {
					if(salida == '$'){ //compruebo si voy a recibir un fichero
						campo = 8;
						contDatos = 0;
					}
				}
				break;
			case 2:
				D = salida;
				campo++;
				break;
			case 3:
				C = salida;
				campo++;
				break;
			case 4:
				N = salida;
				campo++;
				break;
			case 5:
				L=(unsigned char) salida;
				campo++;
				break;
			case 6:
				Datos[contDatos] = salida;
				if (contDatos < (L-1)){
					contDatos++;
				} else {
					contDatos++;
					Datos[contDatos] = '\0';
					unsigned XOR= Datos[0]; 
					for(int i=1; i<L; i++){ 
					    XOR = XOR ^ Datos[i]; 
					}							
					if (XOR == 0 || XOR == '$' || XOR == '#'){ 
						XOR = 1;								
					}											
					bceAux = XOR;
					campo++;
				}
				break;
			case 7: 
				BCE = salida;
				if (salida != bceAux){ 
					campo = 1;
					printf("Recibido	%c		STX		%c		%u \n", D, N, BCE);
					
			 // enviamos ACK
					t.N = N;
					t.C = 21;
			 
					Puerto << t.S;
					Puerto << t.D;
					Puerto << t.C;
					Puerto << t.N;
			
					printf("Enviado		%c		NACK		%c \n", t.D, t.N);	
				} else {
				if (fichero == 1){
					campo = 8;
					//almacenar los datos en el fichero:
					int i = 0;
					while (i <L){
						f.put(Datos[i]);
						i++;
						//printf("escribiendo \n");
					}
					
					printf("Recibido	%c		STX		%c		%u \n", D, N, BCE);
					
			 // enviamos ACK
					t.N = N;
					t.C = 06;
			 
					Puerto << t.S;
					Puerto << t.D;
					Puerto << t.C;
					Puerto << t.N;
			
					printf("Enviado		%c		ACK		%c \n", t.D, t.N);	
				}
				}
				break;
			case 8:
				if(salida != 22 && fichero == 0){ //leeré el nombre del fichero hasta que me llegue el sincronismo,  
				//y comprobamos que fichero sea 0 para que cuando sea 1 (que significa que estamos ya tratando el contenido del fichero) ya no entre en este if
					nombre[contDatos] = salida; 
					contDatos++;
				} else {
					if ( salida == '#'){ //compruebo si he terminado de recibir el fichero
						fichero = 0; //ponemos fichero a 0 porque ya no estamos tratando un fichero, hemos terminado
						apertura = 1; //reiniciamos apertura a 1
						campo = 1; //reiniciamos campo a 1
						f.close(); //cerramos flujo
						
						//Liberacion:
						unsigned char d, n;
						while (EOTRecibida(Puerto, d, n) != 1){
						
						}
						printf("Recibido	%c		EOT		%c \n", d, n);
						// enviamos ACK
						t.C = 06;
						t.N = n; ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						Puerto << t.S;
						Puerto << t.D;
						Puerto << t.C;
						Puerto << t.N;
						printf("Enviado		%c		ACK		%c \n", t.D, t.N);
						printf("Fichero recibido\n");
						fin = 1;
					} else {
						if (apertura >= 1){ //comprobamos si ya hemos abierto el flujo
							nombre[contDatos]='\0';
							f.open(nombre);
							apertura--;
						}
						fichero = 1; //estamos tratando el contenido del fichero
						campo = 2; //recibimos tramas de datos e inicializamos las variables de las tramas
						contDatos = 0;
						Datos[0]='\0';
						S = salida;
					}
				}	
				break;
		}
	  }
	}
}


void TramaC::SondeoMaestro(TCom &Puerto, int &campo, char salida, int &contDatos, int &fichero, char nombre[], ofstream &f, int &apertura){
	unsigned bceAux;
	
	TramaC t; //ACK
	t.C = 06;
	
	EstablecimientoSondeo(Puerto);
	
	int fin = 0;
	while (fin == 0){
	
		if (Puerto.OcupacionBufferEntrada()!=0){
		
		Puerto >> salida;
		
		switch (campo){
			case 1:
				if (salida == 22){ //Compruebo si estoy recibiendo una trama
					contDatos = 0;
					Datos[0]='\0';
					S = salida;
					campo++;
				} else {
					if(salida == '$'){ //compruebo si voy a recibir un fichero
						campo = 8;
						contDatos = 0;
					}
				}
				break;
			case 2:
				D = salida;
				campo++;
				break;
			case 3:
				C = salida;
				campo++;
				break;
			case 4:
				N = salida;
				campo++;
				break;
			case 5:
				L=(unsigned char) salida;
				campo++;
				break;
			case 6:
				Datos[contDatos] = salida;
				if (contDatos < (L-1)){
					contDatos++;
				} else {
					contDatos++;
					Datos[contDatos] = '\0';
					unsigned XOR= Datos[0]; 
					for(int i=1; i<L; i++){ 
					    XOR = XOR ^ Datos[i]; 
					}							
					if (XOR == 0 || XOR == '$' || XOR == '#'){ 
						XOR = 1;								
					}											
					bceAux = XOR;

					campo++;
				}
				break;
			case 7: 
				BCE = salida;
				
				if (salida != bceAux){ 
					campo = 1;
					printf("Recibido	%c		STX		%c		%u \n", D, N, BCE);
					
			 // enviamos ACK
					t.N = N;
					t.C = 21;
			 
					Puerto << t.S;
					Puerto << t.D;
					Puerto << t.C;
					Puerto << t.N;
			
					printf("Enviado		%c		NACK		%c \n", t.D, t.N);
				} else {
					if (fichero == 1){
						campo = 8;
						//almacenar los datos en el fichero:
						int i = 0;
						while (i <L){
							f.put(Datos[i]);
							i++;
						}
					
						printf("Recibido	%c		STX		%c		%u \n", D, N, BCE);
					
				// enviamos ACK
						t.N = N;
						t.C =06;
				
						Puerto << t.S;
						Puerto << t.D;
						Puerto << t.C;
						Puerto << t.N;
				
						printf("Enviado		%c		ACK		%c \n", t.D, t.N);	
					}
				}
				break;
			case 8:
				if(salida != 22 && fichero == 0){ //leeré el nombre del fichero hasta que me llegue el sincronismo,  
				//y comprobamos que fichero sea 0 para que cuando sea 1 (que significa que estamos ya tratando el contenido del fichero) ya no entre en este if
					nombre[contDatos] = salida; 
					contDatos++;
				} else {
					if ( salida == '#'){ //compruebo si he terminado de recibir el fichero
						fichero = 0; //ponemos fichero a 0 porque ya no estamos tratando un fichero, hemos terminado
						apertura = 1; //reiniciamos apertura a 1
						campo = 1; //reiniciamos campo a 1
						f.close(); //cerramos flujo
						
						//Liberacion:
						int finalizar = 0;
						while (finalizar == 0){
							unsigned char d, n;
							while (EOTRecibida(Puerto, d, n) != 1){
							
							}
							printf("Recibido	%c		EOT		%c \n", d, n);
							printf("El esclavo desea finalizar la llamada, ¿desea terminarla? \n 1. Si \n 2. No \n");
							char ter = getch();
							switch(ter){
								case '1':
									finalizar = 1;
									t.N = n;
									Puerto << t.S;
									Puerto << t.D;
									Puerto << t.C;
									Puerto << t.N;
									printf("Enviado		%c		ACK		%c \n", t.D, t.N);
									printf("Fichero recibido\n");
									fin = 1;
									break;
								case '2':
									TramaC tn;
									tn.C = 21;
									tn.N = n;
									Puerto << tn.S;
									Puerto << tn.D;
									Puerto << tn.C;
									Puerto << tn.N;
									printf("Enviado		%c		NACK		%c \n", tn.D, tn.N);
									break;
							}
							
						}
						
					} else {
						if (apertura >= 1){ //comprobamos si ya hemos abierto el flujo
							nombre[contDatos]='\0';
							f.open(nombre);
							apertura--;
						}
						fichero = 1; //estamos tratando el contenido del fichero
						campo = 2; //recibimos tramas de datos e inicializamos las variables de las tramas
						contDatos = 0;
						Datos[0]='\0';
						S = salida;
					}
				}	
				break;
		}
	  }
	}
}


void TramaC::SondeoEsclavo(TCom &Puerto){
	int done, error, f6 = 0;
	struct ffblk ffblk;
	char ruta[23] = "Ficheros\\";
	char cadena[255];
	int envio = 1; //esta variable nos servirá por si el buffer estaba lleno y todavía no habíamos enviado la información guardada
	char salida, car, dato;
	unsigned char d, n;
	TramaC t; // enviamos ACK
	
	if (kbhit()){
      car=getch();
	  if(car == 64){
		f6 = 1;
	  }
	}
	
	t.C = 06;
	t.D = 'T';
	
	Puerto << t.S;
	Puerto << t.D;
	Puerto << t.C;
	Puerto << t.N;
	
	printf("Enviado		%c		ACK		%c \n", t.D, t.N);

	D = 'T';
	N = '0'; ///////////////////////////////////////////////////////////////////////////////////////////////////////
	
	done = findfirst("Ficheros\\*.txt", &ffblk, 0);
	
	if (kbhit()){
	  car=getch();
	  if(car == 64){
		f6 = 1;
	  }
	}
	
	if (done != 0){
		printf("No se encuentra ningún fichero");
	} else {
		ifstream lectura;
		strcat(ruta, ffblk.ff_name);
		
		lectura.open(ruta);
		
		if(!lectura.eof()){
			Puerto << '$';
			Puerto << ffblk.ff_name;
			while(!lectura.eof()){
				if (kbhit()){
					car=getch();
					if(car == 64){
						f6 = 1;
					}
				}
				C=02;
				if (envio == 1){
					lectura.getline(cadena, 255, EOF);
					strcpy(Datos, cadena);//Datos = cadena;
					L = strlen(Datos);
					unsigned XOR= Datos[0]; 
					for(int i=1; i<L; i++){ 
					    XOR = XOR ^ Datos[i]; 
					}							
					if (kbhit()){
						car=getch();
						if(car == 64){
							f6 = 1;
						}
					}
					if (XOR == 0 || XOR == '$' || XOR == '#'){ 
						XOR = 1;								
					}											
					BCE = XOR;
					envio = 0;	//ponemos envio a 0 para que no entre otra vez en este while hasta que no se envien estos datos
				}
				if ( (Puerto.OcupacionBufferSalida()+L+6) <1024){ //comprobamos si tenemos espacio en nuestro buffer, si lo hay, enviamos
					if (f6 == 1){
						dato = Datos[0];
						Datos[0] = 'ç';
						f6 = 0;
					}
					
					Puerto << S;
					Puerto << D;
					Puerto << C;
					Puerto << N;
					Puerto << L;
					Puerto << Datos;
					Puerto << BCE;	
					envio = 1; //como ya hemos enviado, podemos volver a poner envio a 1
					
					//mostrar por pantalla: enviado/recibido + D (R o T) + control (ENQ, ACK, STX..) + N + BCE (solo si es de texto)
					printf("Enviado		%c		STX		%c		%u \n", D, N, BCE);
					//while no reciba ack
					while ( (error = ConfirmacionRecibida(Puerto, d, n)) != 1){
						if (error == 2){
							Datos[0] = dato;
							printf("Recibido 	%c		NACK		%c \n", d, n);
							Puerto << S;
							Puerto << D;
							Puerto << C;
							Puerto << N;
							Puerto << L;
							Puerto << Datos;
							Puerto << BCE;
							printf("Enviado		%c		STX		%c		%u \n", D, N, BCE);
						}
					}
					printf("Recibido 	%c		ACK		%c \n", d, n);
					
					if (N == '0'){
						N = '1';
					} else {
						N = '0';
					}
				}
				
			}
			lectura.close();
			Puerto << '#';
			//EOT
			LiberacionSondeo(Puerto);
			printf("Fichero enviado\n");
		}
	}
}

TramaC::~TramaC(){

}
