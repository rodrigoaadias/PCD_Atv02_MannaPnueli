
public class TrafficController {
	public int esperaDIR = 0;
	//Indica qntd de carros esperando pela DIREITA
	
	public int esperaESQ = 0;
	//Indica qntd de carros esperando pela ESQUERDA
	
	public int semafaro = 0;  
	//Conta quantos carros passaram em sequência de cada lado
	//Negativo para contar carros da esquerda/vermelhos
	//Positivo para contar carros da direita/azuis
	
	public int carroIN = 0;		
	//Indica se os carros que estão passando na monte são da:
	//Esquerda / vermelhos(-)
	//Direita / Azuis(+)

	
	public synchronized void enterLeft() {
		
		esperaESQ++; //Indica o mais um carro quer entrar deste lado

		if(carroIN>0||(semafaro<=-5&&esperaDIR>0)){	//Bloqueia processo se:
			try{																			//->Carro oposto estiver passando pela ponte
				wait();																	//OU
			}																					//->%carros já passaram em sequência e há um oposto esperando.				
			catch (Exception ie) {}
		}

		carroIN--;		//indica que um carro deste lado está passando pela ponte
		
		semafaro--;		//Indica que mais um carro deste lado passou em sequência
		
		esperaESQ--;	//Indica que o carro não está mais aguardando para entrar na ponte.
	}

	
	public synchronized void leaveRight() {
		
		carroIN++;			//Indica que um carro está deixando a ponte.
		
		if(carroIN==0){							//Libera processo em espera se:
			semafaro = 0;							//Não houver carros na ponte
			try{
					notify();
			}
			catch (Exception ie) {}
		}
	}


	//Processo análogo aos métodos anteriores:
	
	public synchronized void enterRight() {
		esperaDIR++;
		if(carroIN<0||(semafaro>=5&&esperaESQ>0)){
			try{
				wait();
			}
			catch (Exception ie) {}
		}
		carroIN++;
		semafaro++;
		esperaDIR--;
	}
	
	public synchronized void leaveLeft() {
		carroIN--;
		if(carroIN==0){
			semafaro = 0;
			try{
					notify();
			}
			catch (Exception ie) {}
		}
	}

}