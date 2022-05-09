
//************************
// TESTE SENSOR DE VAZÃO COM ESP32
#include <WiFi.h>
#include "Esp32MQTTClient.h"

#define GPIO_ENTRADA 5
#define GPIO_ESQUERDA 25
#define GPIO_DIREITA 4
#define LED_UMA_OBSTRUCAO 22
#define LED_DUAS_OBSTRUCAO 23

float vazaoEntrada = 0.0;  // Variável para armazenar o valor em L/min
float mediaEntrada = 0;    // Variável para tirar a média a cada 1 minuto
int contaPulsoEntrada = 0; // Variável para a quantidade de pulsos

float vazaoEsquerda = 0.0;
float mediaEsquerda = 0;
int contaPulsoEsquerda = 0;

float vazaoDireita = 0.0;
float mediaDireita = 0;
int contaPulsoDireita = 0;
//int i = 0; // Variável para contagem

int contaTempo = 0;
char mensagem[40];

const float FATOR_CALIBRACAO = 5.6;
const float FATOR_CALIBRACAO_E = 5.6;
const float FATOR_CALIBRACAO_D = 5.6;
int dezSegundosIniciais = 0;

const int fatorDeAcerto = 5;

// Please input the SSID and password of WiFi
const char *ssid = "";
const char *password = "";

static const char *connectionString = "";
static bool hasIoTHub = false;
char *p;

void incpulso()
{
  contaPulsoEntrada++; // Incrementa a variável de contagem dos pulsos
}

void incpulsoEsquerda()
{
  contaPulsoEsquerda++; // Incrementa a variável de contagem dos pulsos sensor da esquerda
}

void incpulsoDireita()
{
  contaPulsoDireita++; // Incrementa a variável de contagem dos pulsos sensor da direita
}

void setup()
{
  Serial.begin(115200); // Inicia a serial com um baud rate de 9600  

  // pinMode(LED_ALERTA, OUTPUT);
  pinMode(LED_UMA_OBSTRUCAO, OUTPUT);
  pinMode(LED_DUAS_OBSTRUCAO, OUTPUT);

  pinMode(GPIO_ENTRADA, INPUT);
  attachInterrupt(digitalPinToInterrupt(GPIO_ENTRADA), incpulso, RISING); // Configura o pino 5 para trabalhar como interrupção

  pinMode(GPIO_ESQUERDA, INPUT);
  attachInterrupt(digitalPinToInterrupt(GPIO_ESQUERDA), incpulsoEsquerda, RISING);

  pinMode(GPIO_DIREITA, INPUT);
  attachInterrupt(digitalPinToInterrupt(GPIO_DIREITA), incpulsoDireita, RISING);
  // attachInterrupt(GPIO_ENTRADA, incpulso, RISING);
  Serial.println("\n\nInicio\n\n"); // Imprime Inicio na serial
  // Parte de WiFi
  //******************************************************
  Serial.println("Starting connecting WiFi.");
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  if (!Esp32MQTTClient_Init((const uint8_t *)connectionString))
  {
    hasIoTHub = false;
    Serial.println("Initializing IoT hub failed.");
    return;
  }
  hasIoTHub = true;
  dezSegundosIniciais =millis();
  //******************************************************
}

void loop()
{
  contaPulsoEntrada = 0;  // Zera a variável para contar os giros por segundos
  contaPulsoEsquerda = 0; // Zera a variável para contar os giros por segundos
  contaPulsoDireita = 0;  // Zera a variável para contar os giros por segundos
  char *estaPulverizando = "false";
  p = "P";
//
//  sei();                       // Habilita interrupção
//  delay(1000 * fatorDeAcerto); // Aguarda 1 segundo

  // Se alguma vez já tiver recebido mensagem...

  if (p != NULL)
  {
    // Imprime o valor de p
    Serial.println("Imprimindo valor de p...");
    Serial.println(p);

    // Atualiza o valor de p
    // p = GetMessage();
    p = "P";

    // estaPulverizando = "true";
  }
  else
  { // mantém o 'i' em zero enquanto não chega mensagem
    // i=0;
    estaPulverizando = "false";
  }

  if (p != NULL && (strcmp(p, "P") == 0))
  {
    estaPulverizando = "true";
    
    sei();                       // Habilita interrupção
    delay(1000 * fatorDeAcerto); // Aguarda 1 segundo
    // if (i % 5 == 0)
    if (true)
    {
      vazaoEntrada = (contaPulsoEntrada) / (FATOR_CALIBRACAO * fatorDeAcerto); // Converte para L/min
      mediaEntrada = mediaEntrada + vazaoEntrada;                              // Soma a vazão para o calculo da mediaEntrada

      vazaoEsquerda = (contaPulsoEsquerda) / (FATOR_CALIBRACAO_E * fatorDeAcerto);
      mediaEsquerda = mediaEsquerda + vazaoEsquerda;

      vazaoDireita = (contaPulsoDireita) / (FATOR_CALIBRACAO_D * fatorDeAcerto);
      mediaDireita = mediaDireita + vazaoDireita;
      Serial.print(contaPulsoEntrada); // Imprime o contaPulsoEntrada i (segundos)
      Serial.println(" - pulsos entrada");
      Serial.print(contaPulsoEsquerda); // Imprime o contaPulsoEntrada i (segundos)
      Serial.println(" - pulsos esquerda");
      Serial.print(contaPulsoDireita); // Imprime o contaPulsoEntrada i (segundos)
      Serial.println(" - pulsos direita");
    }
    cli(); // Desabilita interrupção

    //i += fatorDeAcerto;

    //if (i == 10 && dezSegundosIniciais == 0)
    if (millis()- dezSegundosIniciais > 10000)
    {
      dezSegundosIniciais = 1;
    }

    // if (i % 5 == 0)
    if (true)
    {
      Serial.print(vazaoEntrada);  // Imprime na serial o valor da vazão
      Serial.println(" L/min - "); // Imprime L/min

      Serial.print(vazaoEsquerda); // Imprime na serial o valor da vazão
      Serial.println(" L/min - "); // Imprime L/min

      Serial.print(vazaoDireita);  // Imprime na serial o valor da vazão
      Serial.println(" L/min - "); // Imprime L/min

      Serial.print(dezSegundosIniciais/1000);     // Imprime a contagem i (segundos)
      Serial.println("s"); // Imprime s indicando que está em segundos
    }
    // int statusOp = 1;
    String valorOp = "ok";
    char *setorOp = "null";
    int quantidadeObstruida = 0;
    bool enviaMensagem = false;

    // if (vazaoEntrada > 0.5 && hasIoTHub && (i % 5 == 0) && dezSegundosIniciais == 1)
    if (vazaoEntrada > 0.5 && hasIoTHub && true && dezSegundosIniciais == 1)
    {
      bool umObstruidoEsq = ((vazaoEsquerda >= 0.73 * vazaoDireita) && (vazaoEsquerda <= 0.85 * vazaoDireita));
      bool doisObstruidoEsq = ((vazaoEsquerda >= 0.332 * vazaoDireita) && (vazaoEsquerda <= 0.48 * vazaoDireita));

      bool umObstruidoDir = ((vazaoDireita >= 0.73 * vazaoEsquerda) && (vazaoDireita <= 0.85 * vazaoEsquerda));
      bool doisObstruidoDir = ((vazaoDireita >= 0.332 * vazaoEsquerda) && (vazaoDireita <= 0.48 * vazaoEsquerda));

      if (umObstruidoEsq)
      {
        enviaMensagem = true;
        quantidadeObstruida = 1;
        setorOp = "A";
        valorOp = "nok";
        Serial.println("1º if");
        digitalWrite(LED_UMA_OBSTRUCAO, HIGH);
        digitalWrite(LED_DUAS_OBSTRUCAO, LOW);
        Serial.println("Atenção, pode haver um 1 bico obstruído no setor A (Esquerda)!");
        //delay(1000);
      }
      else if (doisObstruidoEsq)
      {
        enviaMensagem = true;
        quantidadeObstruida = 2;
        setorOp = "A";
        valorOp = "nok";
        Serial.println("2º if");
        digitalWrite(LED_UMA_OBSTRUCAO, LOW);
        digitalWrite(LED_DUAS_OBSTRUCAO, HIGH);
        Serial.println("Atenção, pode haver um 2 bicos obstruídos no setor A (Esquerda)!");
        //delay(1000);
      }
      else if (umObstruidoDir)
      {
        enviaMensagem = true;
        quantidadeObstruida = 1;
        setorOp = "B";
        valorOp = "nok";
        Serial.println("3º if");
        digitalWrite(LED_UMA_OBSTRUCAO, HIGH);
        digitalWrite(LED_DUAS_OBSTRUCAO, LOW);
        Serial.println("Atenção, pode haver um 1 bico obstruído no setor B (Direita)!");
        //delay(1000);
      }
      else if (doisObstruidoDir)
      {
        enviaMensagem = true;
        quantidadeObstruida = 2;
        setorOp = "B";
        valorOp = "nok";
        Serial.println("4º if");
        digitalWrite(LED_UMA_OBSTRUCAO, LOW);
        digitalWrite(LED_DUAS_OBSTRUCAO, HIGH);
        Serial.println("Atenção, pode haver um 2 bicos obstruídos no setor B (Direita)!");
        //delay(1000);
      }
      else
      {
        // digitalWrite(LED_ALERTA, LOW);
        digitalWrite(LED_UMA_OBSTRUCAO, LOW);
        digitalWrite(LED_DUAS_OBSTRUCAO, LOW);
        Serial.println("Eu to no else");
        valorOp = "ok";
        quantidadeObstruida = 0;
        setorOp = "null";
        // delay(200);
      }
    }
    else
    {
      quantidadeObstruida = 0;
      setorOp = "null";
      valorOp = "nok";
    }

    // if ((i % 5 == 0))
    if (true)
    {
      sprintf(mensagem, "{\"deviceId\": \"P01D02\",\"pulverizacao\": %s,\"sensor1\": %.2f,\"sensor2\": %.2f,\"sensor3\": %.2f,\"valor\": \"%s\", \"setor\": \"%s\",\"quantidadeObstruida\": %d}",
              estaPulverizando, vazaoEntrada, vazaoEsquerda, vazaoDireita, valorOp, setorOp, quantidadeObstruida);
      Serial.println(mensagem);

      if (Esp32MQTTClient_SendEvent(mensagem))
      {
        Serial.println("Enviando dados com sucesso");
      }
      else
      {
        Serial.println("Falha no envio dos dados...");
      }
      enviaMensagem = false;
    }

/*
      if (i == 60)
    {
      mediaEntrada = mediaEntrada / 60 * fatorDeAcerto; // Tira a mediaEntrada dividindo por 60
      Serial.print("\nmediaEntrada por minuto = ");     // Imprime a frase mediaEntrada por minuto =
      Serial.print(mediaEntrada);                       // Imprime o valor da mediaEntrada
      Serial.println(" L/min - ");                      // Imprime L/min
      mediaEntrada = 0;                                 // Zera a variável mediaEntrada para uma nova contagem

      mediaEsquerda = mediaEsquerda / 60 * fatorDeAcerto;
      Serial.print("\nmediaEsquerda por minuto = ");
      Serial.print(mediaEsquerda);
      Serial.println(" L/min - ");
      mediaEsquerda = 0;

      mediaDireita = mediaDireita / 60 * fatorDeAcerto;
      Serial.print("\nmediaDireita por minuto = ");
      Serial.print(mediaDireita);
      Serial.println(" L/min - ");
      mediaDireita = 0;

      i = 0;                            // Zera a variável i para uma nova contagem
      Serial.println("\n\nInicio\n\n"); // Imprime Inicio indicando que a contagem iniciou
    }
*/
  }
}
