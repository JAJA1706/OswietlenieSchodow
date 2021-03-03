#include "czujniki.h"
#define CZAS_DO_RESETU_SYSTEMU 50000

const int czujnikZmierzchu = 3;
const int wlacznikSwiatel = 45;
unsigned long timerResetu; //timer resetu calego systemu
unsigned long timerPrzelaczenia; //timer zastepujacy klasyczne delay() ktorego nie mozna uzyc ze wzgledu na calkowite zatrzymanie systemu
unsigned long timerPrzelaczenia2;
unsigned long timerDol, timerGora;  //timery sluzace do resetu czujnikow
int skroceniePrzelaczeniaDol, skroceniePrzelaczeniaGora; //wartosc o ktora bedziemy zmniejszac czas_przelaczenia swiatel
int doDolu, doGory;  //ile osob wchodzi po schodach w danym kierunkum   
int j, k = 22, l = 38; //zmienne zastepujace klasyczna petle for ktorych czasami nie mozemy uzyc

bool status[4] = {false, false, false ,false};  //status mowi nam czy czujnik zostal w danym momencie miniety
bool reset = false; //reset calego systemu w trakcie
bool wystapiloPrzelaczenie = false;
bool wystapiloPrzelaczenie2 = false;
bool zapalanieOdDolu = false;
bool zapalanieOdGory = false;
bool gaszenieOdDolu = false;
bool gaszenieOdGory = false;
bool zmierzch = false;

void setup() {
 Serial.begin (9600);
 pinMode(5, OUTPUT); //trig0
 pinMode(4, INPUT); //echo0
 pinMode(7, OUTPUT); //trig1
 pinMode(6, INPUT); //echo1
 pinMode(9, OUTPUT); //trig2
 pinMode(8, INPUT); //echo2
 pinMode(11, OUTPUT); //trig3
 pinMode(10, INPUT); //echo3
 
 pinMode(45, INPUT_PULLUP ); //wlacznikSwiatel, jezeli HIGH to wlaczone
 pinMode(3, INPUT ); //czujnik zmierzchu
 
/*for( int i=22; i<39; i++ ){
   pinMode(i, OUTPUT);
 }*/
  
 pinMode(22, OUTPUT); //pin'y swiatel, jezeli LOW to wlaczone
 pinMode(23, OUTPUT);
 pinMode(24, OUTPUT);
 pinMode(25, OUTPUT);
 pinMode(26, OUTPUT);
 pinMode(27, OUTPUT);
 pinMode(28, OUTPUT);
 pinMode(29, OUTPUT);
 pinMode(30, OUTPUT);
 pinMode(31, OUTPUT);
 pinMode(32, OUTPUT);
 pinMode(33, OUTPUT);
 pinMode(34, OUTPUT);
 pinMode(35, OUTPUT);
 pinMode(36, OUTPUT);
 pinMode(37, OUTPUT);
 pinMode(38, OUTPUT);
 for( int i=22; i<39; i++ ){
   digitalWrite( i, HIGH ); 
 }
}
 
void loop() {
  if( digitalRead( wlacznikSwiatel ) == HIGH || ( digitalRead( wlacznikSwiatel ) == LOW && zmierzch )){
    unsigned long aktualnyCzas;
    sprawdzStanCzujnikow();
    
    aktualnyCzas = millis();
    if( zapalanieOdDolu && timerPrzelaczenia + CZAS_PRZELACZENIA - skroceniePrzelaczeniaDol < aktualnyCzas ){
      digitalWrite( k, LOW );
      wystapiloPrzelaczenie = true;
      
      if( k >= l ){
        zapalanieOdDolu = false;
        zapalanieOdGory = false;
        k = 21; //bo zara zwiekszymy o 1
        l = 38;
      }
      k++;
    }
    
    if( zapalanieOdGory && timerPrzelaczenia2 + CZAS_PRZELACZENIA - skroceniePrzelaczeniaGora < aktualnyCzas ){
      digitalWrite( l, LOW );
      wystapiloPrzelaczenie2 = true;
      
      if( l <= k ){
        zapalanieOdGory = false;
        zapalanieOdDolu = false;
        k = 22;
        l = 39; //bo zara zmniejszymy o 1
      }
      l--;
    }
    
    if( gaszenieOdGory && timerPrzelaczenia + CZAS_PRZELACZENIA < aktualnyCzas ){
      digitalWrite( j, HIGH );
      wystapiloPrzelaczenie = true;
      
      if( j == 22 ){
        gaszenieOdGory = false;
      }
      else if( zapalanieOdDolu && j-1 <= k )
        gaszenieOdGory = false;
      j--;
    }
    
    if( gaszenieOdDolu && timerPrzelaczenia + CZAS_PRZELACZENIA < aktualnyCzas ){
      digitalWrite( j, HIGH );
      wystapiloPrzelaczenie = true;
      
      if( j == 38 ){
        gaszenieOdDolu = false;
      }
      else if( zapalanieOdGory && j+1 >= l )
        gaszenieOdDolu = false;
      j++;
    }
    
    //po zbyt dlugim czasie nieaktywnosci reset calego systemu
    if( timerResetu && timerResetu + CZAS_DO_RESETU_SYSTEMU < aktualnyCzas ){ //pierwszy warunek "timerResetu" sluzy mi tylko do przyspieszenia dzialania systemu
      reset = true;
      j = 22;
      timerResetu = 0;
    }
    
    if( reset && timerPrzelaczenia + CZAS_PRZELACZENIA < aktualnyCzas ){
        digitalWrite( j, HIGH );
        digitalWrite( 60-j, HIGH );
        wystapiloPrzelaczenie = true;
        
        if( j == 30 ){
          for( int i = 0; i < 4; i++ )
            status[i] = false;
          doGory = 0;
          doDolu = 0;
          timerGora = 0;
          timerDol = 0;
          reset = false;
        }
        j++;
    }
    
    if( wystapiloPrzelaczenie ){
      if( k != 31 )
        timerPrzelaczenia = aktualnyCzas;
      else
        timerPrzelaczenia = aktualnyCzas + CZAS_PRZELACZENIA - skroceniePrzelaczeniaDol;
      wystapiloPrzelaczenie = false;
    }
    if( wystapiloPrzelaczenie2 ){
      if( l != 29 )
        timerPrzelaczenia2 = aktualnyCzas;
      else
        timerPrzelaczenia2 = aktualnyCzas + CZAS_PRZELACZENIA - skroceniePrzelaczeniaGora;
      wystapiloPrzelaczenie2 = false;
    }
  }
  
  if( zmierzch && digitalRead(czujnikZmierzchu) == LOW ){ //zapobieganie zmiany stanu czujnikaZmierzchu dopoki wszystkie swiatla nie zgasna
    bool czyZgaszone = true;
    for( int i = 22; i < 39; i++ ){
      if( digitalRead( i ) == LOW )
        czyZgaszone = false;
    }
    if( czyZgaszone )
      zmierzch = false;
  }
  else if( !zmierzch && digitalRead(czujnikZmierzchu) == HIGH ){
    zmierzch = true;
  }
} 