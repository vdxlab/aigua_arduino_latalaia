#include <Ultrasonic.h>


#include <Ultrasonic.h>
#include <stdarg.h>
/*
void SerialPrintf(char *fmt, ... )
{
   char tmp[128]; // resulting string limited to 128 chars

   va_list args;
   va_start (args, fmt );
   vsnprintf(tmp, 128, fmt, args);
   va_end (args);

   Serial.print(tmp);
}
*/

Ultrasonic ultrasonic(11,12);  // trig, echo del densor del diposit de dalt
Ultrasonic ultrasonic2(5, 6);  // trig, echo del sensor del diposit d'aigua filtrada de color verd

int 
   rele1 = 7,
   releon1 = 8,
   rele2 = 9,
   releon2 = 10,
   sensoron = 4,
   sensorblau = 3;

unsigned long 
  timer = 0, 
  blau_ple_timer = 0, 
  start_timer = 0,
  last_run_time = 0,
  current_time = 0;

void setup() 
{   
  // initialize the digital pin as an output.
    Serial.begin(9600);
    
    pinMode(rele1,OUTPUT);
    pinMode(rele2,OUTPUT);
    pinMode(releon1,OUTPUT);
    pinMode(releon2,OUTPUT);
    pinMode(sensoron,OUTPUT);
    pinMode(sensorblau, INPUT_PULLUP); // 0/LOW = ON
    digitalWrite(sensoron,HIGH);    
}

void loop()
{
  // Prenem mesures als diposits
  long 
     distanciax = ultrasonic.Ranging(CM),
     distancia_verd = ultrasonic2.Ranging(CM);
  int blau_ple = ! digitalRead(sensorblau);
  Serial.print("distancia_dalt: ");
  Serial.println(distanciax);
  Serial.print("distancia_verd: ");
  Serial.println(distancia_verd);
  
  // Milisegons des de inici arduino (absolut)
  timer = millis();
  
  // Si deposit dalt buit i han pasat +10 segons des de ultim funcionament
  if ( distanciax > 30 && last_run_time + 10000 < timer )
  {
    if (start_timer < 1) start_timer = timer;
    current_time = timer-start_timer;
    
    // Bomba Aljub ON si deposito verde vacio
    if (distancia_verd >= 60 && ! blau_ple && (blau_ple_timer + 5000 < timer ))
    {
     digitalWrite(releon1,HIGH);  // Activamos el relé 1 y la bomba Aljub
     digitalWrite(rele1,HIGH);  // Activamos el relé 1 y la bomba Aljub
     Serial.print(current_time);
     Serial.println(" bomba_aljub: on");
     blau_ple_timer = 0;
    }
    
    // Paramos bomba Aljub si deposito verde lleno
    if ((distancia_verd < 50 || blau_ple) && (blau_ple_timer == 0))
    {
     digitalWrite(releon1,LOW);
     digitalWrite(rele1,LOW);
     blau_ple_timer = timer;
     if  (blau_ple) {
         Serial.print(current_time);
         Serial.println(" deposit_blau: full");
     }
     else
          Serial.print(current_time);
          Serial.println("deposit_verd: full");
     Serial.print(current_time);
     Serial.println(" aljub: off");
    }
    
    // Bomba Altura
     if ( distancia_verd < 50 )
     {
       digitalWrite(releon2,HIGH);  // Activamos el relé 2 y la bomba Altura
       digitalWrite(rele2,HIGH);  // Activamos el relé 2 y la bomba Altura
       Serial.print(current_time);
       Serial.println(" bomba_altura: on ");
      }
      
     if ( distancia_verd > 70 )
     {
       digitalWrite(rele2,LOW);  // Activamos el relé 2 y la bomba Altura
       digitalWrite(releon2,LOW);  // Activamos el relé 2 y la bomba Altura
       Serial.print(current_time);
       Serial.println(" bomba_altura: off ");
      }
      
  }  
  else
  {
       blau_ple_timer = 0;
       last_run_time = current_time;
       start_timer = 0;
       distanciax = ultrasonic.Ranging(CM);
       digitalWrite(rele1,LOW);   // Desactivamos el relé 1 y la bomba Aljub
       digitalWrite(releon1,LOW);   // Desactivamos el relé 1 y la bomba Aljub
       Serial.print(current_time);
       Serial.println(" bomba_aljub: off");
       
       digitalWrite(rele2,LOW);   // Desactivamos el relé 2 y la bomba Altura
       digitalWrite(releon2,LOW);   // Desactivamos el relé 2 y la bomba Altura
       Serial.print(current_time);
       Serial.println(" bomba_altura: off");
  }
     
  delay(2000);
}

