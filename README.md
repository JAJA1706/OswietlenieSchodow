# OswietlenieSchodow
Program dla zestawu płytki Arduino MEGA 2560 (lub innej od odpowiedniej ilości wejść), 4 czujników odlegósci HC-SR04 oraz oświetlenia LED na każdym stopniu schodów.

Jego działanie polega na wykryciu osoby mijającej czujniki i zapalaniu po kolei oświetlenia LED na stopniach w kierunku poruszania się, uwzgledniając prędkość danej postaci.  
Po minięciu czujników po drugiej stronie, oświetlenie zacznie gasnąć po kolei, począwszy od uprzednio jako pierwszego zapalonego.  
Program uwzględnia przypadek gdy wiele osób znajduje się na schodach.