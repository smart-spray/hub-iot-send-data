#include <WiFi.h>
#include "Esp32MQTTClient.h"


float ph;
float tb;
char mensagem[150];
char * p = NULL;
static const char* connectionString = "";
static bool hasIoTHub = false;


/* Software da Calda
   Lendo as entradas analógicas e imprimindo um json com os dados.   

   Componente          Pino na Esp32
   Sensor Turbidez  :  34
   Sensor PH        :  35
   Solenoide 2 (V1) :  23
   Solenoide 3 (V2) :  22
   Bomba            :  19
*/




void setup() {
 Serial.begin(115200);
 
  ConectaWifi();  

  if (!Esp32MQTTClient_Init((const uint8_t*)connectionString))
  {
    hasIoTHub = false;
    Serial.println("Initializing IoT hub failed.");
    Serial.println("start sending events.");
    return;
  }
  hasIoTHub = true;
 }

void loop() {
  

  if(hasIoTHub){     
    
    ph = CalculaPh(LeituraAnalogica(35));
    tb = CalculaTurbidez(LeituraAnalogica(34));
    delay(500);
    sprintf(mensagem,"{\"deviceId\": \"P01D01\",\"limpeza\": \"false\",\"pulverizacao\": \"false\",\"ph\": %f, \"tb\": %f}",ph,tb);
    Serial.println(mensagem);

     if (Esp32MQTTClient_SendEvent(mensagem))     
    {
      Serial.println("Sending data succeed");      
    }
    else
    {
      Serial.println("Failure...");
    }    
   delay(7000);


  // Se alguma vez já tiver recebido mensagem...
  if (p != NULL){
  //Imprime o valor de p  
  Serial.println("Imprimindo valor de p..."); 
  Serial.println(p);
  // Atualiza o valor de p
  p = GetMessage();    
  }
 
    
 //Se alguma vez já tiver recebido mensagem
 //E for igual a "L"
     
 if(p != NULL && (strcmp(p, "L") == 0)){
        Serial.println ("limpeza!");
        delay(500);
        ph = CalculaPh(LeituraAnalogica(35));
        tb = CalculaTurbidez(LeituraAnalogica(34)); 

        //while ( ((ph < 6) || (ph > 8) ) && ((tb < 900) || (tb > 1200))){
        while ((tb > 1500) ||  ((ph < 4) ||(ph > 9) )) {
        
              Serial.println("limpando...");
              DesligaSaidaDigital(19); //Desliga Bomba
              delay(100);
              LigaSaidaDigital(23); //Liga V1
              Serial.println ("Liga V1...");
              
              /*Espera encher o tanque 
              (verificar o tempo correto já que não temos sensor de nível)
              */
              delay(30000);
              
              DesligaSaidaDigital(23); //Desliga V1
              delay(1000);
              LigaSaidaDigital(19); //Liga Bomba 
              Serial.println ("Bomba ligada...");
              delay(500);

              /*Espera encher o tanque 
              (verificar o tempo correto já que não temos sensor de nível)
              */
              
              //Desliga V1 e Liga V2 para deixar a água sair nos pulverizadores

              LigaSaidaDigital(22); //Liga V2
              Serial.println ("Liga V2...");
              delay(700);

              // Faz as medições
              ph = CalculaPh(LeituraAnalogica(35));              
              tb = CalculaTurbidez(LeituraAnalogica(34));

              //Envia mensagem para nuvem

              sprintf(mensagem,"{\"deviceId\": \"P01D01\",\"limpeza\": \"true\",\"pulverizacao\": \"false\",\"ph\": %f, \"tb\": %f}",ph,tb);
              Serial.println(mensagem);


              if (Esp32MQTTClient_SendEvent(mensagem)){
                Serial.println("Sending data succeed");      
                }
                else
                {
                  Serial.println("Failure...");
                } 

              //Espera mensagem para parar a limpeza manualmente
              //Caso o HubIot estiver conectado e houver mensagem... 

              p = GetMessage();

              if(p != NULL && (strcmp(p, "S") == 0)){
                Serial.println("Limpeza parada pelo usuário...");
                DesligaSaidaDigital(19); //Desliga Bomba
                DesligaSaidaDigital(23); //Desliga V1            
                DesligaSaidaDigital(22); //Desliga V2                
                //força valor do tb e ph para parar a pulverização
                tb = 1000;
                ph = 7;
              }

        }//Fim do loop do while da pulverizacao
                   
      }//Fim da etapa da limpeza

 //Se alguma vez já tiver recebido mensagem
 //E for igual a "P"

  if(p != NULL && (strcmp(p, "P") == 0)){ 
     /*Analisa o ph, e a turbidez,
        a turbidez muito alta vai indicar que não há mais liquido no tanque
        isso vai indicar o fim da pulverização
      */

      Serial.println ("pulverizador!");
      delay(500); 

      tb = CalculaTurbidez(LeituraAnalogica(34));
            
      while (tb < 3000){//Loop pulverização
        Serial.println("Pulverizando..."); 
        ph = CalculaPh(LeituraAnalogica(35));
        tb = CalculaTurbidez(LeituraAnalogica(34));
    
        Serial.print("Valor do pH: ");
        Serial.println(ph);
    
        Serial.print("Valor do tb: ");
        Serial.println(tb);
    
        Serial.println("Ligando V1, V2 e a Bomba");
              
        DesligaSaidaDigital(23); //Desliga V1            
        LigaSaidaDigital(22); //Liga V2
        delay(700);
        LigaSaidaDigital(19); //Liga Bomba 
        delay(5000);

        //Envia mensagem para nuvem

        sprintf(mensagem,"{\"deviceId\": \"P01D01\",\"limpeza\": \"false\",\"pulverizacao\": \"true\",\"ph\": %f, \"tb\": %f}",ph,tb);
        Serial.println(mensagem);


        if (Esp32MQTTClient_SendEvent(mensagem)){
          Serial.println("Sending data succeed");      
          }
          else
          {
          Serial.println("Failure...");
          } 

       //Espera mensagem para parar a pulverização manualmente
       //Caso o HubIot estiver conectado e houver mensagem... 

        p = GetMessage();

        if(p != NULL && (strcmp(p, "S") == 0)){
          Serial.println("Pulverização parada pelo usuário...");
          DesligaSaidaDigital(19); //Desliga Bomba
          DesligaSaidaDigital(23); //Desliga V1                      
          DesligaSaidaDigital(22); //Desliga V2          
          //força valor do tb para parar a pulverização
          tb = 4000;
        }

         }//Fim do loop do while da pulverizacao
    }//Fim da etapa da pulverizacao
      
      else{
        Serial.println ("Tentando pegar mensagem da nuvem...");
        delay(2000);
        //Tenta pegar alguma coisa que supostamente chegou...        
        p = GetMessage();
        }

      delay(500);     
    }
  }

// Metodos utilizados nesse programa

//Função para leitura da tensao
float LeituraAnalogica(int pino){
  float tensao = 0; 
  for (int i = 0; i < 100; i++) {
    tensao += ((float)analogRead(pino) / 4095) * 3.3; //3.3 tensao máx de entrada   
  }

  // Realiza a média entre os valores lidos na função for acima
  tensao = tensao / 100;
  tensao = tensao * 1.538; // converte o valor para tensao de 5V
  tensao = ArredondarTensao(tensao,2);    
  return tensao;
}

// Calcula o pH
float CalculaPh(float tensao){  
  float ph = (-5.7*tensao) + 21.44;
  return ph;  
}

// Calcula NTU
float CalculaTurbidez(float tensao){
  float NTU;  
  if (tensao < 2.5) {
    NTU = 3000;
  }

  else if (tensao > 4.2) {
    NTU = 0;
  //voltagem = 4.2;
  }

  // Senão calcula o valor de NTU através da fórmula
  else {
    NTU = -1120.4 * pow(tensao,2) + (5742.3 * tensao) - 4353.8;
  }

 return NTU;       
}

// Função para arredondar valor analogico
float ArredondarTensao(float tensao, int decimal ) {    
  float multiplicador = powf( 10.0f, decimal );
  tensao = roundf( tensao * multiplicador ) / multiplicador;  
  return tensao;
}

// Liga saídas digitais  
void LigaSaidaDigital(int pino){
  int saida = pino;
  pinMode(saida, OUTPUT);  
  digitalWrite(saida, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);  
}

// Desliga saídas digitais  
void DesligaSaidaDigital(int pino){
  int saida = pino;
  pinMode(saida, OUTPUT);  
  digitalWrite(saida, LOW);   // turn the LED on (HIGH is the voltage level)
  delay(100); 
}



//Conectar no Wifi
void ConectaWifi() {  
  // Please input the SSID and password of WiFi
  const char* ssid     = "";
  const char* password = "";

  Serial.println("Starting connecting WiFi.");
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());  
  }
