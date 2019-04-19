# Assignment 9
## JT Mundi

Dayserve.c and daytime.c using the socket routines discussed in the slides. dayserve.c is a server process that listens on an ephemeral TCP port for a connection. When it receives a connection, it logs the hostname of the client to stdout and writes the current date and time to the client (as text) via the socket connection. 

Port used 49999

### Compile
```make```

### Running Server
```./Server```

### Running Client
```./Client hostname```

