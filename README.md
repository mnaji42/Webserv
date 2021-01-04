# Webserv

## Usage

To start the server:
```sh
make
./webserv
```

To display the configuration file on startup:
```sh
./webserv --display-config
```

## Some test commands

Upload a file
```sh
curl -v http://localhost:8080/ --upload-file Makefile
# will do a PUT /Makefile HTTP/1.1
```

Upload a file with Transfer-Encoding: chunked
```sh
curl -v http://localhost:8080/ -H 'Transfer-Encoding: chunked' --upload-file Makefile
```

Upload with Content-Encoding: gzip and Transfer-Encoding: chunked
```sh
cat Makefile | gzip | curl -X PUT localhost:8080/upload \
  -H 'Content-Type: text/plain' \
  -H 'Transfer-Encoding: chunked' \
  -H 'Content-Encoding: gzip' \
  --data-binary @-
```

## RFCs

- [7230: Hypertext Transfer Protocol (HTTP/1.1): Message Syntax and Routing](https://tools.ietf.org/html/rfc7230)
- [3875: The Common Gateway Interface (CGI) Version 1.1](https://tools.ietf.org/html/rfc3875)
- [3986: Uniform Resource Identifier (URI): Generic Syntax](https://tools.ietf.org/html/rfc3986)
