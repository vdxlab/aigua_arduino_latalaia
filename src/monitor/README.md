serial.lua
--

Llegeix el port serie on està l'ardunio connectat i emmagatzema les dades en un fitxer temporal a /tmp/serial.log


aigua.lua
--

Llegeix les dades de /tmp/serial.log i les serveix en format JSON i compatible amb crides HTTP
També emmagatzema les dades en format JSON en el directori /root/db per poder accedir a l'històric

