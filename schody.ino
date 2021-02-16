#define ODLEGLOSC_CZUJNIKOW 17
#define ODLEGLOSC_CZUJNIKOW_OD_SCIANY 55
#define ODLEGLOSC_CZUJNIKOW_MAX 89
#define CZAS_UTRZYMANIA_STANU 1500
#define CZAS_DO_RESETU_CZUJNIKOW 2500
#define CZAS_DO_RESETU_SYSTEMU 50000
#define CZAS_PRZELACZENIA 400

const int trigPin[4] = {5, 7, 9, 11};
const int echoPin[4] = {4, 6, 8, 10};
const int czujnikZmierzchu = 3;
const int przelacznik = 45;
unsigned long timerResetu; //timer resetu calego systemu
unsigned long timerPrzelaczenia; //timer zastepujacy klasyczne delay() ktorego nie mozna uzyc ze wzgledu na calkowite zatrzymanie systemu
unsigned long timerPrzelaczenia2;
unsigned long ostatnioUzywany[4] = {0, 0, 0, 0};  //timery zapobiegajace wielokrotnej zmianie statusu czujnika
unsigned long timerDol, timerGora;  //timery sluzace do resetu czujnikow
int tajnaWartoscDol, tajnaWartoscGora; //wartosc o ktora bedziemy zmniejszac czas_przelaczenia swiatel
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
 
 pinMode(45, INPUT_PULLUP ); //przelacznik
 pinMode(3, INPUT ); //czujnik zmierzchu
 
/*for( int i=22; i<39; i++ ){
   pinMode(i, OUTPUT);
 }*/
  
 pinMode(22, OUTPUT);
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
  if( digitalRead( przelacznik ) == HIGH || ( digitalRead( przelacznik ) == LOW && zmierzch )){
    unsigned long aktualnyCzas;
    sprawdzStanCzujnikow();
    
    aktualnyCzas = millis();
    if( zapalanieOdDolu && timerPrzelaczenia + CZAS_PRZELACZENIA - tajnaWartoscDol < aktualnyCzas ){
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
    
    if( zapalanieOdGory && timerPrzelaczenia2 + CZAS_PRZELACZENIA - tajnaWartoscGora < aktualnyCzas ){
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
        timerPrzelaczenia = aktualnyCzas + CZAS_PRZELACZENIA - tajnaWartoscDol;
      wystapiloPrzelaczenie = false;
    }
    if( wystapiloPrzelaczenie2 ){
      if( l != 29 )
        timerPrzelaczenia2 = aktualnyCzas;
      else
        timerPrzelaczenia2 = aktualnyCzas + CZAS_PRZELACZENIA - tajnaWartoscGora;
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


/* -------------------------------------------------------------------------------------------
-----------------------------------------CZUJNIKI---------------------------------------------
--------------------------------------------------------------------------------------------*/


void sprawdzStanCzujnikow(){
  unsigned long aktualnyCzas = millis();
  int odleglosc;

  odleglosc = zmierzOdleglosc(0);
  if( ostatnioUzywany[0] + CZAS_UTRZYMANIA_STANU < aktualnyCzas && (odleglosc < ODLEGLOSC_CZUJNIKOW_OD_SCIANY || odleglosc > ODLEGLOSC_CZUJNIKOW_MAX )) //wchodzimy w warunek jesli czujnik cos wykryl + nie byl uzywany ostatnio
    if( status[1] && doDolu > 0 ){  //jesli przed nim wykryl cos czujnik1 to wiemy ze ktos zszedl z gory
      doDolu--;
      if( doDolu == 0 && doGory == 0 && !reset ){
        j = 38;
        gaszenieOdGory = true;
      }
      status[1] = false;
      ostatnioUzywany[0] = aktualnyCzas;
      timerResetu = aktualnyCzas;
    }
    else{
      status[0] = true; //jesli czujnik1 nic nie wykryl wczesniej to zaznaczamy czujnik0 jako aktywny
      digitalWrite( 22, LOW );
      wystapiloPrzelaczenie = true;
      timerDol = aktualnyCzas; //odpalamy timer ktory policzy nam czas w jakim pokonana zostala odleglosc pomiedzy czujnikiem 0 i 1
      ostatnioUzywany[0] = aktualnyCzas;
    }
    
    
  //powtorzyc to samo dla kazdego kierunku

  odleglosc = zmierzOdleglosc(1);
  if( ostatnioUzywany[1] + CZAS_UTRZYMANIA_STANU < aktualnyCzas && (odleglosc < ODLEGLOSC_CZUJNIKOW_OD_SCIANY || odleglosc > ODLEGLOSC_CZUJNIKOW_MAX ) )
    if( status[0] ){

      
      doGory++;
      if( doGory == 1 ){
        float predkoscDol;
        
        zapalanieOdDolu = true;
        k = 23;
        predkoscDol = aktualnyCzas - ostatnioUzywany[0];
        predkoscDol = ODLEGLOSC_CZUJNIKOW / predkoscDol; //mamy predkosc w cm/ms
        predkoscDol = predkoscDol * 10; //a teraz w m/s
        if( predkoscDol >= 1 && predkoscDol <= 3 )
          predkoscDol = CZAS_PRZELACZENIA / predkoscDol; //taki trik
        else
          predkoscDol = CZAS_PRZELACZENIA;
        
        if( !gaszenieOdDolu && !gaszenieOdGory )
          tajnaWartoscDol = CZAS_PRZELACZENIA - predkoscDol; //? wymysl co tu
        else
          tajnaWartoscDol = 0;
      }
      status[0] = false;
      ostatnioUzywany[1] = aktualnyCzas;
      timerResetu = aktualnyCzas;
    }
    else{
      status[1] = true;
      timerDol = aktualnyCzas;
      ostatnioUzywany[1] = aktualnyCzas;
    }

  odleglosc = zmierzOdleglosc(2);
  if( ostatnioUzywany[2] + CZAS_UTRZYMANIA_STANU < aktualnyCzas && (odleglosc < ODLEGLOSC_CZUJNIKOW_OD_SCIANY || odleglosc > ODLEGLOSC_CZUJNIKOW_MAX ))
    if( status[3] ){
      
      doDolu++;
      if( doDolu == 1 ){
        float predkoscGora;
        
        zapalanieOdGory = true;
        l = 37;
        predkoscGora = aktualnyCzas - ostatnioUzywany[3];
        predkoscGora = ODLEGLOSC_CZUJNIKOW / predkoscGora;
        predkoscGora = predkoscGora * 10;
        if( predkoscGora >= 1 && predkoscGora <= 3)
          predkoscGora = CZAS_PRZELACZENIA / predkoscGora; //taki trik
        else
          predkoscGora = CZAS_PRZELACZENIA;
          
        if( !gaszenieOdDolu && !gaszenieOdGory )
          tajnaWartoscGora = CZAS_PRZELACZENIA - predkoscGora; //wymysl co tu
        else
          tajnaWartoscGora = 0;
      }
      status[3] = false;
      ostatnioUzywany[2] = aktualnyCzas;
      timerResetu = aktualnyCzas;
    }
    
    else{
      status[2] = true;
      timerGora = aktualnyCzas;
      ostatnioUzywany[2] = aktualnyCzas;
    }

  odleglosc = zmierzOdleglosc(3);
  if( ostatnioUzywany[3] + CZAS_UTRZYMANIA_STANU < aktualnyCzas && (odleglosc < ODLEGLOSC_CZUJNIKOW_OD_SCIANY || odleglosc > ODLEGLOSC_CZUJNIKOW_MAX )  )
    if( status[2] && doGory > 0 ){
      doGory--;
      if( doGory == 0 && doDolu == 0 && !reset ){
        j = 22;
        gaszenieOdDolu = true;
      }
      status[2] = false;
      ostatnioUzywany[3] = aktualnyCzas;
      timerResetu = aktualnyCzas;
    }
    else{
      digitalWrite( 38, LOW );
      wystapiloPrzelaczenie = true;
      status[3] = true;
      timerGora = aktualnyCzas;
      ostatnioUzywany[3] = aktualnyCzas;
    }
    
    
  //przy zbyt dlugim oczekiwaniu reset statusow
  if( timerDol && timerDol + CZAS_DO_RESETU_CZUJNIKOW < aktualnyCzas ){
    if( digitalRead( 22 ) == LOW && digitalRead( 23 ) == HIGH )
     digitalWrite( 22, HIGH );
    status[0] = false;
    status[1] = false;
    timerDol = 0;
  }
  
  if( timerGora && timerGora + CZAS_DO_RESETU_CZUJNIKOW < aktualnyCzas ){
    if( digitalRead( 38 ) == LOW && digitalRead( 37 ) == HIGH )
     digitalWrite( 38, HIGH );
    status[2] = false;
    status[3] = false;
    timerGora = 0;
  }
  
}
 
int zmierzOdleglosc( int nrCzujnika ) {
  unsigned long czas;
  int dystans;
 
  digitalWrite( trigPin, LOW );
  delayMicroseconds(2);
  digitalWrite( trigPin[nrCzujnika], HIGH );
  delayMicroseconds(15);
  digitalWrite( trigPin[nrCzujnika], LOW );
  
  digitalWrite( echoPin[nrCzujnika], HIGH );
  czas = pulseIn( echoPin[nrCzujnika], HIGH );
  dystans = czas / 58;
 
  return dystans;
}