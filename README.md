platform - macOS High Sierra v.10.13.4

Was implemented client-server architecture.
In folder [server] you can find Makefile and implemented server written on C. 
And in folder [client] - another Makefile with client on C for testing server.

Also I used my own written library - libft for using some special and convenient functions:

char ** ft_strsplit(char *line, char delimiter) - function for splitting line,
delimited by separator, into several sublines.

char *ft_itoa(int) - function, that transforms integer to string
AND returns its new allocated string (unlike itoa() with needed buffer in params)

...

My own errors:
HEADER_WRONG_LEN - 33, if header has wrong amount of bytes
HEADER_WRONG_MAGIC - 34, if header has wrong magic
PAYLOAD_WRONG_LEN - 35, if payload length in header greater than amount of bytes read for payload message
PAYLOAD_NOT_ALPHA - 36, if payload consists of not only a-z letters
PAYLOAD_ZERO_LEN - 37, if payload length is zero

Assumptions while implementing solution:
- I couldn't understand the logic of calculating stats,
what real situation after ResetStats response(zero stats after receiving
this request or after responsing to client?)
and same for other requests
- Is needed null-termnator after responses/requests?
- Is server should be demon?
- Is needed separated process for every client?

I think, if I had more time, I would deal with select()/epoll()
