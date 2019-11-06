//Libs do espnow e wifi
#include <esp_now.h>
#include <WiFi.h>
 
//Pino utilizado para leitura
//e que será enviado o valor
#define PIN 2
#define PIN2 18
 
//Canal usado para conexão
#define CHANNEL 1
 
//Se MASTER estiver definido 
//o compilador irá compilar o Master.ino
//Se quiser compilar o Slave.ino remova ou
//comente a linha abaixo
#define MASTER
 
//Estrutura com informações
//sobre o próximo peer
esp_now_peer_info_t peer;

//Função para inicializar o modo station
void modeStation(){
    //Colocamos o ESP em modo station
    WiFi.mode(WIFI_STA);
    //Mostramos no Monitor Serial o Mac Address 
    //deste ESP quando em modo station
    Serial.print("Mac Address in Station: "); 
    Serial.println(WiFi.macAddress());
}

//Função de inicialização do ESPNow
void InitESPNow() {
    //Se a inicialização foi bem sucedida
    if (esp_now_init() == ESP_OK) {
        Serial.println("ESPNow Init Success");
    }
    //Se houve erro na inicialização
    else {
        Serial.println("ESPNow Init Failed");
        ESP.restart();
    }
}

//Função que adiciona um novo peer
//através de seu endereço MAC
void addPeer(uint8_t *peerMacAddress){
    //Informamos o canal
    peer.channel = CHANNEL;
    //0 para não usar criptografia ou 1 para usar
    peer.encrypt = 0;
    //Copia o endereço do array para a estrutura
    memcpy(peer.peer_addr, peerMacAddress, 6);
    //Adiciona o slave
    esp_now_add_peer(&peer);
}

//Função que irá enviar o valor para 
//o peer que tenha o mac address especificado
void send(const uint8_t *value, uint8_t *peerMacAddress) {
    esp_err_t result = esp_now_send(peerMacAddress, value, sizeof(value));
    Serial.print("Send Status: ");
    //Se o envio foi bem sucedido
    if (result == ESP_OK) {
        Serial.println("Success");
    }
    //Se aconteceu algum erro no envio
    else {
        Serial.println("Error");
    }
}

uint8_t peerMacAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};


void setup() {
  Serial.begin(115200);
 
  //Chama a função que inicializa o modo station
  modeStation();
 
  //Chama a função que inicializa o ESPNow
  InitESPNow();
 
  //Adiciona o peer
  addPeer(peerMacAddress);

  //Registra o callback que nos informará
  //que recebemos dados.
  //A função que será executada
  //é onDataRecv e está declarada mais abaixo
  esp_now_register_recv_cb(onDataRecv);
 
  //Registra o callback que nos informará 
  //sobre o status do envio.
  //A função que será executada é onDataSent
  //e está declarada mais abaixo
  esp_now_register_send_cb(OnDataSent);
   
  //Colocamos o pino em modo de leitura
  pinMode(PIN, INPUT);

  //Colocamos o pino como saída
  pinMode(PIN2, OUTPUT);
 
  //Lê o valor do pino e envia
  readAndSend();
}

//Função responsável pela
//leitura do pino e envio
//do valor para o peer
void readAndSend(){
  //Lê o valor do pino
  uint8_t value = digitalRead(PIN);
  //Envia o valor para o peer
  send(&value, peerMacAddress);
}

//Função que serve de callback para nos avisar
//que recebemos dados
void onDataRecv(const uint8_t *mac_addr, const uint8_t *value, int len) {
  Serial.println("Recebendo dado");
  //Coloca o valor recebido na saída do pino
  digitalWrite(PIN2, *value);
  //Envia o valor lido para o próximo esp
  //Se este for o último, comente esta linha antes de compilar
  //send(value, peerMacAddress);
}

//Função que serve de callback para nos avisar
//sobre a situação do envio que fizemos
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
 
  //Quando recebemos o resultado do último envio
  //já podemos ler e enviar novamente
  readAndSend();
}

//Não precisamos fazer nada no loop
//pois sempre que recebemos o feedback
//do envio através da função OnDataSent
//nós enviamos os dados novamente,
//fazendo com que os dados estejam sempre
//sendo enviados em sequência
void loop() {
}
