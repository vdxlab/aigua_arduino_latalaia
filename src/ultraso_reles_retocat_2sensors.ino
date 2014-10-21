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
   sensoron = 4;

void setup() 
{   
  // initialize the digital pin as an output.
    Serial.begin(9600);
    
    pinMode(rele1,OUTPUT);
    pinMode(rele2,OUTPUT);
    pinMode(releon1,OUTPUT);
    pinMode(releon2,OUTPUT);
    pinMode(sensoron,OUTPUT);
    digitalWrite(sensoron,HIGH);    
}

void loop()
{
  long 
     distanciax = ultrasonic.Ranging(CM),      // Prenem la mesura al diposit de dalt
     distancia_verd = ultrasonic2.Ranging(CM);
  Serial.print("distanciax: ");
  Serial.println(distanciax);
  Serial.print("distancia_verd: ");
  Serial.println(distancia_verd);
  
  if ( distanciax > 30 )
  {
    // Bomba Aljub ON si deposito verde vacio
    if (distancia_verd >= 40)
    {
     digitalWrite(releon1,HIGH);  // Activamos el relé 1 y la bomba Aljub
     digitalWrite(rele1,HIGH);  // Activamos el relé 1 y la bomba Aljub
     Serial.println("Bomba Aljub ON ");
    }
    
    // Paramos bomba Aljub si deposito verde lleno
    if (distancia_verd < 40)
    {
     digitalWrite(releon1,LOW);  // Activamos el relé 1 y la bomba Aljub
     digitalWrite(rele1,LOW);  // Activamos el relé 1 y la bomba Aljub
     Serial.println("Bomba Aljub OFF, verde lleno ");
    }
    
    // Bomba Altura
     if ( distancia_verd < 50 )
     {
       digitalWrite(releon2,HIGH);  // Activamos el relé 2 y la bomba Altura
       digitalWrite(rele2,HIGH);  // Activamos el relé 2 y la bomba Altura
       Serial.println("Bomba Altura ON ");
       delay(1000);
      }
      
     if ( distancia_verd > 70 )
     {
       digitalWrite(rele2,LOW);  // Activamos el relé 2 y la bomba Altura
       digitalWrite(releon2,LOW);  // Activamos el relé 2 y la bomba Altura
       Serial.println("Bomba Altura OFF ");
      }
      
      
      
  }  
  if ( distanciax <= 15)
  {
       distanciax = ultrasonic.Ranging(CM);
       digitalWrite(rele1,LOW);   // Desactivamos el relé 1 y la bomba Aljub
       digitalWrite(releon1,LOW);   // Desactivamos el relé 1 y la bomba Aljub
       Serial.println("Bomba Aljub OFF");
       
       digitalWrite(rele2,LOW);   // Desactivamos el relé 2 y la bomba Altura
       digitalWrite(releon2,LOW);   // Desactivamos el relé 2 y la bomba Altura
       Serial.println("Bomba Altura OFF");
     
  }
     
  delay(1000);         // espera 30 minutos = 30 · 60 · 1000
}

