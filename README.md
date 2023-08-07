# gpio_client_server
Często mój komp się wyłącza sam po obejrzeniu jakiegoś filmu w nocy, chciałem by był wyłączany także sprzęt muzyczny, bufor lampowy itd.

Wykorzystałem do tego serwer działający pod Odroid C4, coś podobnego do Rasberry, odpowiedni kawałek elektroniki z przekaźnikiem, podłączenie pod GPIO i te oprogramowanie pozwoliło mi na osiągnięcie celu.

Serwer dba o część elektryczną, a w C program komunikuje się na wybranym porcie i wysyła żądania. Reszta to już kwestia konfiguracji, kiedy i jak ma to być odpalane, ręcznie czy automatycznie, przy wyłączaniu komputera i włączaniu. To już sprawa indywidualna.

Kod jest prosty, kto chce niech korzysta ;)


Jakiś czas temu na moim serwerze stary sposób odczytywania/zapisywania stanu GPIO oparty na ręcznym otwieraniu GPIO przestał działać. Robię to w tej chwili za pomocą dostępnych programów konsolowych, co ciekawe zapis jest prosty, zaś odczyt utrudniony. Zastosowałem więc sztuczkę polegającą na tym, że nie odczytuję stanu, tylko go zapisuję, zaś to co zostaje zapisane (zmienione) zapisuję jednocześnie do pliku i w razie potrzeby odczytania wcześniej ustawionego stanu GPIO, odczytuję tylko wartość tego pliku. To działa mi wyśmienicie. Oczywiście starsza wersja kodu jest dostępna w postaci zakomentowanej, jak ktoś będzie chciał skorzystać nie powinno być z tym problemu.
