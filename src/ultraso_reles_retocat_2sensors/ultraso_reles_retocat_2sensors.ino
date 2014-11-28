#include <Ultrasonic.h>
#include <Ultrasonic.h>
#include <stdarg.h>

#define distancia_verd_max 80
#define distancia_verd_min 50
#define distancia_dalt_max 40
#define distancia_dalt_min 20
#define distancia_dalt_min_limit 5

Ultrasonic ultrasonic(11,12);  // trig, echo del densor del diposit de dalt
Ultrasonic ultrasonic2(5, 6);  // trig, echo del sensor del diposit d'aigua filtrada de color verd

int 
   rele1 = 7,
   releon1 = 8,
   rele2 = 9,
   releon2 = 10,
   sensoron = 4,
   sensorblau = 3,
   altura_on_iter = 0,
   start = 0;

unsigned long 
  timer = 0, 
  blau_ple_timer = 0, 
  start_timer = 0,
  last_run_time = 0,
  current_time = 0;

void prints(char *msg) {
       Serial.print(current_time);
       Serial.print(" ");
       Serial.println(msg);
}

void bomba_altura(unsigned int action) {
   if (action) {
     digitalWrite(releon2,HIGH);  // Activamos el relé 2 y la bomba Altura
     digitalWrite(rele2,HIGH);  // Activamos el relé 2 y la bomba Altura
     prints("bomba_altura: on ");
   }
   else {
     digitalWrite(rele2,LOW);
     digitalWrite(releon2,LOW);
     prints("bomba_altura: off ");
   }
}

void bomba_aljub(unsigned int action) {
   if (action) {
     digitalWrite(releon1,HIGH);  // Activamos el relé 1 y la bomba Aljub
     digitalWrite(rele1,HIGH);  // Activamos el relé 1 y la bomba Aljub
     prints("bomba_aljub: on");
   }
   else {
     digitalWrite(releon1,LOW);
     digitalWrite(rele1,LOW);
     prints("bomba_aljub: off");
   }
}

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
  if ( ! start && distanciax >= distancia_dalt_max  && last_run_time + 10000 < timer ) {
     start = 1;
     prints("info: start");
  }

  if ( start ) {
    if (start_timer < 1) start_timer = timer;
    current_time = timer-start_timer;
    
    // Bomba Aljub ON si deposito verde vacio
    if (distancia_verd >= distancia_verd_min && ! blau_ple && (timer > blau_ple_timer + 10000 )) {
     bomba_aljub(1);
     blau_ple_timer = 0;
    }
    
    // Paramos bomba Aljub si deposito verde lleno
    if ((distancia_verd < distancia_verd_min+10 || blau_ple) && ! blau_ple_timer) {
     blau_ple_timer = timer;
     if (blau_ple)
         prints("info: deposit_blau full");
     else
          prints("info: deposit_verd full");
     bomba_aljub(0);
    }    
  }

  // Bomba Altura (funcionament autonom)
  if ( altura_on_iter > 2 && distancia_verd <= distancia_verd_min && distanciax >= distancia_dalt_min_limit ) {
    altura_on_iter++;
    bomba_altura(1);
  }
  if ( distancia_verd > distancia_verd_max || distanciax < distancia_dalt_min_limit) {
    altura_on_iter = 0;
    bomba_altura(0);
  }

  // Finalitzem funcionament (parem altura per seguretat tambe)
  if ( distanciax < distancia_dalt_min && start ) {
       start = 0;
       blau_ple_timer = 0;
       last_run_time = timer;
       start_timer = 0;
       bomba_aljub(0);
       bomba_altura(0);
   }

   if ( ! start ) prints("bomba_aljub: off");
   delay(1500);
}

