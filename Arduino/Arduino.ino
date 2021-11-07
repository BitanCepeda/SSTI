#include <SPI.h>      // incluye libreria bus SPI
#include <MFRC522.h>      // incluye libreria especifica para MFRC522
#include <LiquidCrystal.h>
#include <Servo.h>      // incluye libreria de Servo


#define pin1 41
#define pin2 40
#define RST_PIN  9      // constante para referenciar pin de reset
#define SS_PIN 53       // constante para referenciar pin de slave select

Servo servo1;     // crea objeto
Servo servo2;     // crea objeto
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
MFRC522 mfrc522(SS_PIN, RST_PIN); // crea objeto mfrc522 enviando pines de slave select y reset

int d=0;
int s_analogica_mq135=0;
int CO2=38;
int gas=39;
int PINSERVO = 22;
int PINSERVO2 = 14;
int SENSOR;    // variable almacena valor leido de entrada analogica A0
float TEMPERATURA;  // valor de temperatura en grados centigrados
float SUMA;    // valor de la suma de las 5 lecturas de temperatura
byte LecturaUID[4];         // crea array para almacenar el UID leido
byte Usuario1[4]= {0x49 , 0x69 , 0x07 , 0x94} ;    // UID de tarjeta leido en programa 1
byte Usuario2[4]= {0x23 , 0x72 , 0x5C  , 0x1B} ;    // UID de llavero leido en programa 1
byte Usuario3[4]= {0xA9 , 0xD9 , 0xF7 , 0x97} ;    // UID de tarjeta leido en programa 1
byte Usuario4[4]= {0x13 , 0x47 , 0xF6 , 0x38} ;
String humo;// variable donde guarda el humo tring
String temp;//variable guarda temperatura en string

void setup() {
  Serial.begin(9600);     // inicializa comunicacion por monitor serie a 9600 bps
  pinMode(pin1, OUTPUT);
  pinMode(47,OUTPUT);
  pinMode(pin2, OUTPUT);
  pinMode(CO2,OUTPUT);
  pinMode(gas,OUTPUT);
  servo1.attach(PINSERVO);
  servo2.attach(PINSERVO2); 
  SPI.begin();        // inicializa bus SPI
  digitalWrite(47,LOW);
  lcd.begin(16,2);
  lcd.clear();
  mfrc522.PCD_Init();     // inicializa modulo lector
  lcd.setCursor(5,0);
  lcd.print("Listo");
  delay(1000);
  lcd.clear();
  servo1.write(0);    // ubica el servo a 0 grados
  servo2.write(0);    // ubica el servo a 0 grados
}

void loop() {
/////////seguridad contra incendio 
  s_analogica_mq135 = analogRead(0); 
  humo= s_analogica_mq135;
  String a = "3 "+humo;
  Serial.println(a);
  delay(500);

  
      
//temperatura
  SUMA = 0;          // valor inicial de SUMA en cero
  
  for (int i=0; i < 5; i++){      // bucle que repite 5 veces
    SENSOR = analogRead(A8);      // lectura de entrada analogica A0  
    TEMPERATURA = ((SENSOR * 5000.0) / 1023) / 10;// formula para convertir valor leido
            // de entrada A0 en grados centigrados
    SUMA = TEMPERATURA + SUMA;      // suma de cada lectura de temperatura
    delay(200);     // demora de medio seg. entre lecturas
  }
  temp = SUMA/5.0, 1;
  Serial.println("2 "+temp);      // imprime en monitor serial valor promedio de
           // las 5 lecturas de temperatura con un decimal
  
 
  //dioxido de carbono
  if(s_analogica_mq135<120)
  {
  
  digitalWrite(CO2,LOW);
  digitalWrite(gas,LOW);
  delay(50);
  
  }

  
 //dioxido de carbono
  if(s_analogica_mq135>=120 && s_analogica_mq135<=350)
  {
   delay(200);
  Serial.println("advertencia");
  lcd.setCursor(0,0);
  lcd.print("Precaucion Humo");
  digitalWrite(CO2,HIGH);
  digitalWrite(gas,LOW);
  delay(2000);
  lcd.clear();  
  
  }

  
  //gas propano y butano
  if(s_analogica_mq135 >= 351)
  {
   delay(200);
  Serial.println("advertencia");
  lcd.setCursor(0,0);
  lcd.print("Alerta posible");
  lcd.setCursor(4,1);
  lcd.print("Incendio");
  digitalWrite(CO2,LOW);
  digitalWrite(gas,HIGH);
  delay(6000);
  lcd.clear();
  }

  //gas propsno y butano + temperatura alta
  
  if(s_analogica_mq135 >= 351 && TEMPERATURA >= 65)
  {
   delay(200);
  Serial.println("incendio");
  lcd.setCursor(0,0);
  lcd.print("ALERTA INCENDIO");
  lcd.setCursor(4,1);
  lcd.print("INMINENTE");
  servo1.write(180);    // ubica el servo a 180 grados
  digitalWrite(CO2,HIGH);
  digitalWrite(gas,HIGH);
  servo1.write(180);
  servo2 .write(180);
  delay(7000);
  lcd.clear();
  d=1;
  }

 if( d == 1 ){
  delay(200);
  Serial.println("incendio");
  lcd.setCursor(0,0);
  lcd.print("ALERTA INCENDIO");
  lcd.setCursor(4,1);
  lcd.print("INMINENTE");
  digitalWrite(47,HIGH);
  delay(2000);
  digitalWrite(47,LOW);
  delay(1000);
 }
 
  //////control de personal
  if ( ! mfrc522.PICC_IsNewCardPresent())   // si no hay una tarjeta presente
    return;           // retorna al loop esperando por una tarjeta
  
  if ( ! mfrc522.PICC_ReadCardSerial())     // si no puede obtener datos de la tarjeta
    return;           // retorna al loop esperando por otra tarjeta

        
    for (byte i = 0; i < mfrc522.uid.size; i++) { // bucle recorre de a un byte por vez el UID
      LecturaUID[i]=mfrc522.uid.uidByte[i];     // almacena en array el byte del UID leido      
      }
          
             // imprime un espacio de tabulacion             
                    
     if(comparaUID(LecturaUID, Usuario1)){    // llama a funcion comparaUID con Usuario1
        digitalWrite(pin1,HIGH);
        lcd.setCursor(0,0);
        lcd.print("Acceso concedido");
        lcd.setCursor(0,1);
        lcd.print("Usuario 1");
        servo();
        digitalWrite(pin1,LOW);
        lcd.clear();
        delay(200);
        Serial.println("1 Alejandro");
        delay(200);
        }// si retorna verdadero muestra texto bienvenida
     else if(comparaUID(LecturaUID, Usuario2)){ // llama a funcion comparaUID con Usuario2
        digitalWrite(pin1,HIGH);
        lcd.setCursor(0,0);
        lcd.print("Acceso concedido");
        lcd.setCursor(0,1);
        lcd.print("Usuario 2");
        servo1.write(180);    // ubica el servo a 180 grados
        delay(2000);    // espera de 5 seg.
        lcd.clear();
        servo1.write(0);
        digitalWrite(pin1,LOW);
        delay(200);
        Serial.println("1 Brayan");
        delay(200);} // si retorna verdadero muestra texto bienvenida
     else if(comparaUID(LecturaUID, Usuario3)){    // llama a funcion comparaUID con Usuario1
        digitalWrite(pin1,HIGH);
        lcd.setCursor(0,0);
        lcd.print("Acceso concedido");
        lcd.setCursor(0,1);
        lcd.print("Usuario 3");
        servo1.write(180);    // ubica el servo a 180 grados
        delay(2000);      // espera de 5 seg.
        digitalWrite(pin1,LOW);
        servo1.write(0);
        lcd.clear();
        delay(200);
        Serial.println("1 Fabián"); 
        delay(200);}// si retorna verdadero muestra texto bienvenida
     else if(comparaUID(LecturaUID, Usuario4)){ // llama a funcion comparaUID con Usuario2
        digitalWrite(pin1,HIGH);
        lcd.setCursor(0,0);
        lcd.print("Acceso concedido");
        lcd.setCursor(0,1);
        lcd.print("Usuario 4");
        servo1.write(180);    // ubica el servo a 180 grados
        delay(2000);    // espera de 5 seg.
        digitalWrite(pin1,LOW);
        servo1.write(0);
        lcd.clear();
        delay(200);
        Serial.println("1 Rubiano");
        delay(200);} // si retorna verdadero muestra texto bienvenida
      else {          // si retorna falso
        digitalWrite(pin2,HIGH);
        lcd.setCursor(0,0);
        lcd.print("Acceso Denegado");
        lcd.setCursor(0,1);
        lcd.print("              ");
        delay(2000);
        digitalWrite(pin2,LOW);
        lcd.clear();
        Serial.println("No te conozco");}    // muestra texto equivalente a acceso denegado          
                  
    
      mfrc522.PICC_HaltA();     // detiene comunicacion con tarjeta                


}

void servo()
{
  servo1.write(180);    // ubica el servo a 180 grados
  delay(2000);
  servo1.write(0);
  }

boolean comparaUID(byte lectura[],byte usuario[]) // funcion comparaUID
{
  for (byte i=0; i < mfrc522.uid.size; i++){    // bucle recorre de a un byte por vez el UID
  if(lectura[i] != usuario[i])        // si byte de UID leido es distinto a usuario
    return(false);          // retorna falso
  }
  return(true);           // si los 4 bytes coinciden retorna verdadero
}
