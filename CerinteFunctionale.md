# Cerinte Functionale - PCD

## Server
Permite:
- un singur admin conectat la un moment dat
- unul sau mai multi clienti normali INET socket TCP & UDP
- unul sau mai multi clienti web SOAP / REST (OPTIONAL)

Comunicare:
- trebuie sa contina identificatori unici pentru fiecare tip de client e.q. admin, user INET, user web etc.
- sincrona / asincrona pentru nivel B / C respectiv

## Admin
Permite:
- sa genereze rapoarte pe baza numarului de clienti / comenzilor lor
- - Exemplu: durata medie de executie comanda, comada ce ruleaza in momentul de fata pe un client, istoric comenzi
- sa dea kick / ban la useri
- sa intrerupa o comanda in curs de rulare

Comunicare:
- sincrona

## Client INET socket
Trebuie implementat atat in C cat si in alt limbaj (PYTHON)

Trimite fisier video la care poate sa adauge:
- blur
- upscale
- downscale
- object tracking

## Client Web
