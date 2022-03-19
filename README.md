# gpio_client_server
Często mój komp się wyłącza sam po obejrzeniu jakiegoś filmu w nocy, chciałem by był wyłączany także sprzęt muzyczny, bufor lampowy itd.

Wykorzystałem do tego serwer działający pod Odroid C4, coś podobnego do Rasberry, odpowiedni kawałek elektroniki z przekaźnikiem, podłączenie pod GPIO i te oprogramowanie pozwoliło mi na osiągnięcie celu.

Serwer dba o część elektryczną, a w C program komunikuje się na wybranym porcie i wysyła żądania. Reszta to już kwestia konfiguracji, kiedy i jak ma to być odpalane, ręcznie czy automatycznie, przy wyłączaniu komputera i włączaniu. To już sprawa indywidualna.

Kod jest prosty, kto chce niech korzysta ;)
