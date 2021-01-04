#!/bin/bash
 éçà

 
printf "\
GET / HTTP/1.1\r\n\
Host: Unicorn\r\n\
Hi: Yes\r\n\
Oui: Non\r\n\
\r\n\
" | nc localhost 8080

printf "\
GET /test.sh HTTP/1.1\r\n\
\r\n\
" | nc localhost 8080

CONTENT=`seq 0 99`

printf "\
POST /upload HTTP/1.1\r\n\
\r\n\
Bonjour! voici du contenu:\n\
${CONTENT}\n\
" | nc localhost 8080
