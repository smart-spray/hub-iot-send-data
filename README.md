# Hub-iot-send-data


*******
## Conteúdo
 0. [Introdução](#Introducao)
 1. [Pulverização](#Pulverização)
 2. [Tanque](#Tanque)
 3. [Enviando dados para ESP32](#Esp32)
 4. [Documentação Azure](#Azure)

*******

<div id='Introducao'/> 

# Introducao
  Nesse repositório está o código dos dois dispositivos embarcado do SmartSpray.
  Está divido em quatro partes pulverização, tanque, envio dados para ESP32 e a referência da documentação da Azure.

---
<div id='Pulverização'/> 

# Pulverização
  As informações dos sensores de vazão começam a ser enviadas quando é recebido a mensagem 'P'.<br/>
  Durante o proceso quando é recebido a mensagem 'S', o processo é interrompido. 

---
<div id='Tanque'/> 

# Tanque
  A cada 7 segundos é enviado dados dos sensores para nuvem, o processo de pulverização começa quando é recebida a mensagem 'P'.<br/>
  Durante o proceso quando é recebido a mensagem 'S', o processo é interrompido. 
  

---
<div id='Esp32'>
  
# Enviando dados para ESP32
  Para que funcionasse o envio de dados para ESP32, foi necessário fazer a alteração de dois arquivos da biblioteca o Azure IoT.
  Leia o arquivo Readme que está dentro da pasta 'lib' para mais informações.
  
  Além disso, foi usado um software em python para que na fase inicial dos testes pudessemos simular o comando que viria posteriormente do apk.
  
  
--- 
<div id='Azure'>
  
# Azure
  Acesse a documentação do Azure para ver detalhes como enviar dados para seu dispostivo IoT: <br/>
  https://docs.microsoft.com/pt-br/azure/iot-hub/iot-hub-python-python-c2d <br/>
  
  Documentação do sdk do Azure IoT:<br/>
  https://github.com/espressif/esp-azure <br/>
  
