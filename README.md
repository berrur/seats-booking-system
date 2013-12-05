seats-booking-system
====================

Specifica della tesina:
-----------------------

Sistema di prenotazione posti remoto.


Realizzazione di un sistema di prenotazione posti per una sala
cinematografica. Un processo su una macchina server gestisce una mappa di
posti per una sala cinematografica. Ciascun posto e' caratterizzato da un
numero di fila, un numero di poltrona ed un *FLAG* indicante se il posto
e' gia' stato prenotato o meno. Il server accetta e processa sequenzialmente le
richieste di prenotazione di posti da uno o piu' client (residenti, in
generale, su macchine diverse).


Un client deve fornire ad un utente le seguenti funzioni:
<ol>
<li>Visualizzare la mappa dei posti in modo da individuare quelli ancora
disponibili.</li>
<li>Inviare al server l'elenco dei posti che si intende prenotare (ciascun
posto da prenotare viene ancora identificato tramite numero di fila e numero di
poltrona).</li>
<li>Attendere dal server la conferma di effettuata prenotazione.</li>
<li>Disdire una prenotazione per cui si possiede un codice.</li>

