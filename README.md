platform - macOS High Sierra v.10.13.4

Was implemented client-server architecture.
In folder server you can find Makefile and implemented server written on C.
And in folder client another Makefile with client on C for testing server.

Also I used my own written library - libft for using some special and convenient functions:
char ** ft_strsplit(char *line, char delimiter) - function for splitting line,
delimited by separator, into several sublines.
char *ft_itoa(int) - function, that transforms integer to string
AND returns its new allocated string (unlike itoa() with needed buffer in params)

- I couldn't understand the logic of calculating stats
what real situation after ResetStats response(is it zero stats after receiving
this request or after responsing to client)
and same for other requests
- Is needed null-termnator after responses/requests
- Is server should be demon
- Is needed separated process for every client

I think, if I had more time, I would deal with select()/epoll()