# TODO

## Headers

Cocher apres verification de la RFC
(rien n'est coche pour l'instant meme si certains sont implementes)

Dans la RFC il y a des mots comme MUST, SHOULD, MAY en majuscule -- on doit implementer obligatoirement tous les MUST de la RFC
pour etre conforme.

- [ ] Accept-Charsets
-> not sure
- [ ] Accept-Language
-> not sure
- [ ] Allow
-> not sure
- [ ] Authorization
-> on gerera ca a la fin -- il faut implementer la HTTP Basic Auth (requis selon la RFC il me semble)
- [ ] Content-Language
- [ ] Content-Length
-> il faut gerer le Content-Length d'une requete -- fermer la connexion et la considerer invalide si elle
    envoie trop de contenu (a charles de gerer)
-> il faut aussi envoyer ce header avec la taille du corps de la requete si possible (la RFC ne le demande pas forcement
    mais on peut retourner la taille du fichier en faisant un stat dessus -- mais quand on utilise la compression -bonus-,
    on ne pourra pas renvoyer ce header car on a pas la taille du corps)
- [ ] Content-Location
-> not sure
- [x] Content-Type
- [x] Date
- [x] Host
- [x] Last-Modified
- [ ] Location
-> not sure
- [ ] Referer
-> not sure
- [ ] Retry-After
-> not sure
- [ ] Server
-> not sure
- [x] Transfer-Encoding
- [x] User-Agent
-> rien a faire a premiere vue
- [ ] WWW-Authenticate
-> meme chose que le header Authorization il me semble?

## Technical

- [x] It must be non blocking and use only 1 select for all the IO between the client andthe server (listens includes).
- [x] Select should check read and write at the same time.
- [x] Your server should never block and client should be bounce properly if necessary
-> pas sur de savoir ce que veut dire "bounce" dans ce context? mais normalement OK
- [x] You should never do a read operation or a write operation without going through select
- [ ] Checking the value of errno is strictly forbidden after a read or a write operation
-> techniquement on suit pas ce requirement actuellement car je lis les cas d'erreur et les affiche (je raise une SocketException)
    pour pouvoir debugger pendant le developpement si un read ou write fail
- [x] A request to your server should never hang forever
-> faut faire attention a ne pas avoir de boucle infinie, et normalement vu qu'on utilise pas de blocking IO c'est pas possible
- [x] You server should have default error pages if none are provided
- [ ] Your program should not leak and should never crash, (even when out of memory if all the initialisation is done)
-> il faut que je catch les erreurs d'allocation et kill la connexion dans ce cas
- [x] You can’t use fork for something else than CGI (like php or perl or ruby etc...)
- [ ] You can include and use everything in "iostream" "string" "vector" "list" "queue" "stack" "map" "algorithm"
-> j'ai des doutes sur les headers qu'on utilise du coup
- [ ] Your program should have a config file in argument or use a default path
-> on a pas fait le default path encore

## Config file

> In this config file we should be able to:
> (You should inspire yourself from the "server" part of nginxconfiguration file)

Pour la plupart, la configuration existe, mais pas encore implemente

- [x] choose the port and host of each "server"
- [x] setup the server_names or not
- [x] The first server for a host:port will be the default for this host:port (meaning it will answer to all request that doesn’t belong to an other server)
- [x] setup default error pages
- [x] limit client body size
- [x] setup routes with one or multiple of the following rules/configuration (routes wont be using regexp):
- [x] ∗define a list of accepted HTTP Methods for the route
- [x] ∗define a directory or a file from where the file should be search (for ex-ample if url / is rooted to /tmp/www, url /pouic/toto/pouet is /tm-p/www/pouic/toto/pouet)
- [x] ∗turn on or off directory listing
- [x] ∗default file to answer if the request is a directory
- [x] ∗execute CGI based on certain file extension (for example .php)
- [x] ∗make the route able to accept uploaded files and configure where it should be save
-> quelques petites choses a changer cote charles au niveau de la fermeture de la connexion apres que tout soit transfere
    mais normalement c'est implemente et ca marche de ce cote
    il faut ajouter la directive de configuration et la gerer dans la partie de medhi

## CGI

- [x] Because you wont call the cgi directly use the full path as PATH_INFO
- [x] Just remember that for chunked request, your server need to unchunked it and the CGI will expect EOF as end of the body.
- [ ] Same things for the output of the CGI. if no content_length is returned from the CGI, EOF will mean the end of the returned data.
-> presque bon
- [ ] Your program should set the following Meta-Variables
- [ ] AUTH_TYPE
-> voir la Basic HTTP Authorization
- [x] CONTENT_LENGTH
- [x] CONTENT_TYPE
- [x] GATEWAY_INTERFACE
- [x] PATH_INFO
- [x] PATH_TRANSLATED
- [ ] QUERY_STRING
- [ ] REMOTE_ADDR
- [ ] REMOTE_IDENT
-> voir la Basic HTTP Authorization
- [ ] REMOTE_USER
-> voir la Basic HTTP Authorization
- [x] REQUEST_METHOD
- [x] REQUEST_URI
- [x] SCRIPT_NAME
- [x] SERVER_NAME
- [x] SERVER_PORT
- [x] SERVER_PROTOCOL
- [x] SERVER_SOFTWARE
- [x] Your program should call the cgi with the file requested as first argument
- [ ] the cgi should be run in the correct directory for relative path file access
- [ ] your server should work with php-cgi
-> a tester

## BONUS

- [] Make pluggins loadable/unloadable through terminal, like other compression sys-tem, charset convertor and so on... (repeatable bonus)
-> "loadable/unloadable through terminal" => ca veut dire quoi exactement?
- [x] Your program can have workers define as:
    a worker can be either processes or threads (and use fork for them)
    a worker should not be spawn for each client and must able to take care of aninfinite number of requests
    workers are not mandatory
    you can use fork, wait, waitpid, wait3, wait4, dup, dup2, pipe or pthread_create,pthread_detach, pthread_join, pthread_mutex_init, pthread_mutex_destroy, pthread_mutex_lock, pthread_mutex_unlock
- [2/4] Add any number of the following in the configuration file:
    choose a number of worker (if your program implements workers)
    Configure plugins (works with pluggins see above)
    Make routes work with regexp
    Configure a proxy to an other http/https server
- [2/5] Use an internal module for php or any other language.
    it means you aren’t call-ing any external executable to generate a page with this language. (repeatable bonus)
