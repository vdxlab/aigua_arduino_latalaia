#include <EtherCard.h>
#include <LiquidCrystal.h>
#include <Ultrasonic.h>
#include <stdarg.h>

// Sensors
#define distancia_verd_max 85
#define distancia_verd_min 60
#define distancia_dalt_max 30
#define distancia_dalt_min 10
#define distancia_dalt_min_limit 5
Ultrasonic ultrasonic(32,33);  // trig, echo del densor del diposit de dalt
Ultrasonic ultrasonic2(30, 31);  // trig, echo del sensor del diposit d'aigua filtrada de color verd
Ultrasonic ultrasonic_aljub(34, 35);  // trig, echo del sensor aljub

int 
   rele1 = 22,
   releon1 = 24,
   rele2 = 26,
   releon2 = 28,
   sensorblau = 40,
   altura_on_iter = 0,
   start = 0,
   blau_ple = 0;
   
unsigned long 
  timer = 0, 
  blau_ple_timer = 0, 
  start_timer = 0,
  last_run_time = 0,
  current_time = 0,
  global_counter = 0;

long
  distancia_verd = 0,
  distancia_aljub = 0,
  distancia_dalt = 0;

// ETHERNET
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
static byte myip[] = { 192,168,1,100 };
byte Ethernet::buffer[500];
BufferFiller bfill;

// LCD 
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
int lcd_key     = 0;
int adc_key_in  = 0;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5
#define MAX_LCD_OPTS  5
int lcd_option = 0;

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
   }
}

int read_LCD_buttons()
{
 adc_key_in = analogRead(0);      // read the value from the sensor 
 // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
 // we add approx 50 to those values and check to see if we are close
 if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result 
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 195)  return btnUP; 
 if (adc_key_in < 380)  return btnDOWN; 
 if (adc_key_in < 555)  return btnLEFT; 
 if (adc_key_in < 790)  return btnSELECT;
 return btnNONE;  // when all others fail, return this...
}

void setup()
{
 Serial.begin(9600);   
 pinMode(rele1,OUTPUT);
 pinMode(rele2,OUTPUT);
 pinMode(releon1,OUTPUT);
 pinMode(releon2,OUTPUT);
 pinMode(sensorblau, INPUT_PULLUP); // 0/LOW = ON

 lcd.begin(16, 2);
 lcd.setCursor(0,0);
 if (ether.begin(sizeof Ethernet::buffer, mymac, 53) == 0) {
  lcd.print("ETHERNET ERROR");
  delay(5000);
 }
 ether.staticSetup(myip);
 lcd.print("La Talaia");
}

static word homePage() {
  long t = millis() / 1000;
  word h = t / 3600;
  byte m = (t / 60) % 60;
  byte s = t % 60;
  bfill = ether.tcpOffset();
  bfill.emit_p(PSTR(
    "HTTP/1.0 200 OK\r\n"
    "Content-Type: text/json\r\n"
    "Pragma: no-cache\r\n"
    "\r\n"
    "{\"distancia_dalt\":$D, \"distancia_verd\":$D,"
    "\"distancia_aljub\":$D, \"time\":$L}"),
      distancia_dalt, distancia_verd, distancia_aljub, t);
  return bfill.position();
}

void lcdmenu() {
  lcd.setCursor(0,0);
  switch (lcd_option) {
   case 0: {
    lcd.print("  <- Aigues ->    ");
    lcd.setCursor(0,1);
    lcd.print("Bombes:");
    if(start) lcd.print("ON  ");
    else lcd.print("OFF ");
    lcd.print(millis()/1000);
    break;
   }
   case 1: {
     lcd.print(" Diposit altura   ");
     lcd.setCursor(0,1);
     lcd.print("                 ");
     lcd.setCursor(0,1);
     lcd.print(distancia_dalt);
     break; 
   }
   case 2: {
     lcd.print(" Diposit filtre  ");
     lcd.setCursor(0,1);
     lcd.print("                 ");
     lcd.setCursor(0,1);
     lcd.print(distancia_verd);
     break;
   }
   case 3: {
     lcd.print("     Aljub      ");
     lcd.setCursor(0,1);
     lcd.print("                 ");
     lcd.setCursor(0,1);
     lcd.print(distancia_aljub);
     break;
   }
   case 4: {
     lcd.print("  Encen bombes   ");
     lcd.setCursor(0,1);
     lcd.print("SELECT per iniciar");
     break;
   }
 }
}

static void runlcd() {
 lcd.setCursor(0,0);
 lcd_key = read_LCD_buttons();  // read the buttons
 switch (lcd_key) {
   case btnRIGHT: {
     if (lcd_option == MAX_LCD_OPTS-1)
       lcd_option = 0;
     else lcd_option++;
     lcd.clear();
     break;
     }
   case btnLEFT: {
     if ( lcd_option == 0 )
        lcd_option = MAX_LCD_OPTS-1;
     else lcd_option--;
     lcd.clear();
     break;
     }
   case btnSELECT: {
     if(lcd_option == 4) {
       start = 1;
       lcd_option = 0;
     }
     lcd.clear();
     break;
     }
 }
 lcdmenu();
}

static void netrun() {
 word len = ether.packetReceive();
 word pos = ether.packetLoop(len); 
 if (pos)  // check if valid tcp data is received
    ether.httpServerReply(homePage());  
}

static void readsensors() {
  if(global_counter%10 == 0) {
    distancia_dalt = ultrasonic.Ranging(CM);
    Serial.print("distancia_dalt: ");
    Serial.println(distancia_dalt);
    distancia_verd = ultrasonic2.Ranging(CM);
    Serial.print("distancia_verd: ");
    Serial.println(distancia_verd);
    distancia_aljub = ultrasonic_aljub.Ranging(CM);
    Serial.print("distancia_aljub: ");
    Serial.println(distancia_aljub);
    blau_ple = ( digitalRead(sensorblau) == 0 );
  }
}

void loop() {
  // Milisegons des de inici arduino (absolut)
  timer = millis();
  // Prenem mesures als diposits
  readsensors();
  netrun();
  runlcd();
  Serial.println(lcd_option);
 
  // Si deposit dalt buit i han pasat +10 segons des de ultim funcionament
  if ( ! start && distancia_dalt >= distancia_dalt_max  && last_run_time + 10000 < timer ) {
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
    } else if ((distancia_verd < distancia_verd_min+10 || blau_ple) && ! blau_ple_timer) {
      // Paramos bomba Aljub si deposito verde lleno
     blau_ple_timer = timer;
     if (blau_ple)
         prints("info: deposit_blau full");
     else
          prints("info: deposit_verd full");
     bomba_aljub(0);
    }    
  }

  // Bomba Altura (funcionament autonom)
  if ( distancia_verd > 5 && distancia_verd <= distancia_verd_min 
      && distancia_dalt >= distancia_dalt_min_limit ) {
    if ( altura_on_iter > 2 ) bomba_altura(1);
    else altura_on_iter++;
  } 
  
  if ( distancia_verd > distancia_verd_max || distancia_dalt < distancia_dalt_min_limit) {
    altura_on_iter = 0;
    bomba_altura(0);
  }

  // Finalitzem funcionament (parem altura per seguretat tambe)
  if ( distancia_dalt < distancia_dalt_min && start ) {
       start = 0;
       blau_ple_timer = 0;
       last_run_time = timer;
       start_timer = 0;
       bomba_aljub(0);
       bomba_altura(0);
   }
   global_counter++;
   delay(500);
}

