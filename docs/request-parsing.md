# HTTP Request Parsing

## HTTP request structure
```bash

<request-line>\r\n
<header 1>: <value>\r\n
...
<heander n>: <value>\r\n
\r\n
<body>

# Request line: <method> <path> <version>

# Without body
GET /docs HTTP/1.1\r\n
Host: example.com\r\n
User-Agent: curl/8.1.0\r\n
Content-Length: 27\r\n
\r\n

# With body
GET /docs HTTP/1.1\r\n
Host: example.com\r\n
User-Agent: curl/8.1.0\r\n
Content-Length: 27\r\n
\r\n
body-sdfsdfgsdfg

```

## Parsing function

library- string.h

char* strchr(char* str, char ch) -> returns pointer to 1st occurrence of ch in str

char* strstr(char* str, char* s) -> returns pointer to 1st occurrence of s in str

Always check value returned by strchr(), strstr() as it may be NULL

null terminator- '\0' -> sets the end of a string

```bash
char s[] = "GET /docs"
printf("%s", s); -> GET /docs

char* p = strchr(s, ' '); -> *p = ' '
*p = '\0'; -> s = "GET\0/docs"

printf("%s", s); -> GET
```

The full request is stored in a char array- buffer.
```bash
char buffer[1024];

# Parsing request line
char* method = buffer; # Pointer to 1st character

char* path = strchr(method, ' '); # Pointer to 1st occurrence of ' ', starting from method
*path = '\0'; # Change ' ' to '\0' (null terminator) to terminate method
path += 1; # Move pointer to next address- 1st char of path

char* version = strchr(path, ' '); # Pointer to 1st ' ' starting from path
*version = '\0'; # Set null terminator at the end of path
version += 1; # Move pointer

# Parsing headers
char* key = header; # Pointer to start of a line

char* value = strstr(key, ": "); # Pointer to ": "
*value = '\0'; # Terminate key
value += 1;

char* next_line = strstr(value, "\r\n");
*next_line = '\0'; # Terminate value
next_line += 2;

```