#ifndef CZUJNIKI_H
#define CZUJNIKI_H

#define CZAS_UTRZYMANIA_STANU 1500
#define ODLEGLOSC_CZUJNIKOW_OD_SCIANY 55
#define ODLEGLOSC_CZUJNIKOW_MAX 89
#define ODLEGLOSC_CZUJNIKOW 17
#define CZAS_DO_RESETU_CZUJNIKOW 2500
#define CZAS_PRZELACZENIA 400

const int trigPin[4] = {5, 7, 9, 11};
const int echoPin[4] = {4, 6, 8, 10};
unsigned long ostatnioUzywany[4] = {0, 0, 0, 0};  //timery zapobiegajace wielokrotnej zmianie statusu czujnika

extern unsigned long timerResetu;
extern unsigned long timerDol, timerGora;
extern int doDolu, doGory;
extern int j, k, l;
extern int skroceniePrzelaczeniaDol, skroceniePrzelaczeniaGora;

extern bool status[4];
extern bool reset;
extern bool gaszenieOdDolu;
extern bool gaszenieOdGory;
extern bool wystapiloPrzelaczenie;
extern bool zapalanieOdDolu;
extern bool zapalanieOdGory;


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
          predkoscDol = CZAS_PRZELACZENIA / predkoscDol;
        else
          predkoscDol = CZAS_PRZELACZENIA;
        
        if( !gaszenieOdDolu && !gaszenieOdGory )
          skroceniePrzelaczeniaDol = CZAS_PRZELACZENIA - predkoscDol; //prawdopodobnie wymagane dodanie odpowiedniego wspolczynnika
        else
          skroceniePrzelaczeniaDol = 0;
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
          predkoscGora = CZAS_PRZELACZENIA / predkoscGora;
        else
          predkoscGora = CZAS_PRZELACZENIA;
          
        if( !gaszenieOdDolu && !gaszenieOdGory )
          skroceniePrzelaczeniaGora = CZAS_PRZELACZENIA - predkoscGora; //prawdopodobnie wymagane dodanie odpowiedniego wspolczynnika
        else
          skroceniePrzelaczeniaGora = 0;
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

#endif //CZUJNIKI_H