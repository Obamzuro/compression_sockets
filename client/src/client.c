/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obamzuro <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/07 20:17:30 by obamzuro          #+#    #+#             */
/*   Updated: 2018/10/14 22:36:59 by obamzuro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.h"

volatile sig_atomic_t g_servfd;

t_requestmap	g_requests[AMOUNT_REQ_CODES] =
{
	{PING, "Ping", request_ping},
	{GETSTATS, "GetStats", request_getstats},
	{RESETSTATS, "ResetStats", request_resetstats},
	{COMPRESS, "Compress", request_compress}
};

void	print_help(void)
{
	printf("Commands:\n\
Ping\nGetStats\nResetStats\nCompress [line]\n");
}

void	print_header(t_msgheader *header)
{
	header->magic = ntohl(header->magic);
	header->payloadlen = ntohs(header->payloadlen);
	header->status = ntohs(header->status);
	printf("magic = %x\npayloadlen = %d\nstatus = %d\n", header->magic,
			header->payloadlen, header->status);
}

void	request_resetstats(int servfd, char **cmdLineDivided)
{
	char				buf[BUFSIZ];
	int					bytes;
	t_msgheader			reqheader;

	bzero(&reqheader, sizeof(reqheader));
	reqheader.magic = htonl(MAGIC);
	reqheader.status = htons(RESETSTATS);
	send(servfd, &reqheader, sizeof(reqheader), 0);
	bytes = recv(servfd, buf, BUFSIZ, 0);
	print_header((t_msgheader *)buf);
}

void	print_stats(char *data)
{
	uint32_t		receivedLen;
	uint32_t		sendLen;
	unsigned char	ratio;

	receivedLen = ntohl(*(uint32_t *)data);
	data += sizeof(uint32_t);
	sendLen = ntohl(*(uint32_t *)data);
	data += sizeof(uint32_t);
	ratio = *data;
	printf("receivedLen = %d\nsendLen = %d\nratio = %d\n",
			receivedLen, sendLen, ratio);
}

void	request_getstats(int servfd, char **cmdLineDivided)
{
	char				buf[BUFSIZ];
	int					bytes;
	t_msgheader			reqheader;

	bzero(&reqheader, sizeof(reqheader));
	reqheader.magic = htonl(MAGIC);
	reqheader.status = htons(GETSTATS);
	send(servfd, &reqheader, sizeof(reqheader), 0);
	bytes = recv(servfd, buf, BUFSIZ, 0);
	print_header((t_msgheader *)&buf);
	if (bytes != sizeof(t_msgheader) + 9)
		return ;
	print_stats((char *)(buf + sizeof(t_msgheader)));
}

void	request_ping(int servfd, char **cmdLineDivided)
{
	char				buf[BUFSIZ];
	int					bytes;
	t_msgheader			reqheader;

	bzero(&reqheader, sizeof(reqheader));
	reqheader.magic = htonl(MAGIC);
	reqheader.status = htons(PING);
	send(servfd, &reqheader, sizeof(reqheader), 0);
	bytes = recv(servfd, buf, BUFSIZ, 0);
	print_header((t_msgheader *)buf);
}

void	request_compress(int servfd, char **cmdLineDivided)
{
	t_msgheader		reqheader;
	char			buf[BUFSIZ];
	int				bytes;
	char			*dataSent;
	char			*payload;
	uint16_t		payloadLen;

	payload = cmdLineDivided[1];
	if (!payload)
	{
		print_help();
		return ;
	}
	payloadLen = strlen(payload);
	reqheader.magic = htonl(MAGIC);
	reqheader.payloadlen = htons(payloadLen);
	reqheader.status = htons(COMPRESS);
	dataSent = (char *)malloc(sizeof(reqheader) + payloadLen);
	memcpy(dataSent, &reqheader, sizeof(reqheader));
	memcpy(dataSent + sizeof(reqheader), payload, payloadLen);
	send(servfd, dataSent, sizeof(reqheader) + payloadLen, 0);
	free(dataSent);
	bytes = recv(servfd, buf, BUFSIZ, 0);
	print_header((t_msgheader *)buf);
	if (bytes > sizeof(t_msgheader))
		printf("Compressed payload = %.*s\n", (int)(bytes - sizeof(t_msgheader)),
				buf + sizeof(t_msgheader));

}

void	requesting(int servfd, char *cmdLine)
{
	int		i;
	char	**cmdLineDivided;

	cmdLineDivided = ft_strsplit(cmdLine, ' ');
	if (!cmdLineDivided[0])
	{
		print_help();
		free(cmdLineDivided);
		return ;
	}
	i = 0;
	while (i != AMOUNT_REQ_CODES)
	{
		if (!strcmp(cmdLineDivided[0], g_requests[i].commandName))
		{
			g_requests[i].func(servfd, cmdLineDivided);
			break ;
		}
		++i;
	}
	// if didn't find a request cmdname
	if (i == AMOUNT_REQ_CODES)
		print_help();
	free(cmdLineDivided);
}

int		get_serv_entity(int argc, char **argv,
		int *servfd, struct sockaddr_in *sockaddr)
{
	struct hostent		*hostEntity;

	if (argc < 3 || (!atoi(argv[2]) && strcmp(argv[2], "0")))
	{
		fprintf(stderr, "Usage: ./client [host] [port]\n");
		return (1);
	}
	if ((*servfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket");
		return (1);
	}
	if ((hostEntity = gethostbyname(argv[1])) == NULL)
	{
		herror(argv[1]);
		return (1);
	}
	bzero(sockaddr, sizeof(*sockaddr));
	sockaddr->sin_family = AF_INET;
	sockaddr->sin_port = htons(atoi(argv[2]));
	memcpy(&sockaddr->sin_addr, hostEntity->h_addr_list[0], hostEntity->h_length);
	return (0);
}

void	signal_handler(int sig)
{
	if (sig == SIGINT)
		close(g_servfd);
	printf("closing %d socket\n", g_servfd);
}

void	setup_signal_handler(void)
{
	struct sigaction act;

	bzero(&act, sizeof(act));
	act.sa_handler = signal_handler;
	sigaction(SIGINT, &act, 0);
}

int		main(int argc, char **argv)
{
	int					servfd;
	struct sockaddr_in	sockaddr;
	char				*cmdLine;
	size_t				cmdLineLen;

	if (get_serv_entity(argc, argv, &servfd, &sockaddr))
		return (-1);
	if (connect(servfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0)
	{
		perror("connect");
		return (-1);
	}
	printf("opened %d socket\n", servfd);
	g_servfd = servfd;
	setup_signal_handler();
	while (1)
	{
		fprintf(stderr, "$> ");
		cmdLine = NULL;
		if (get_next_line(STDIN_FILENO, &cmdLine) < 0)
			break ;
		requesting(servfd, cmdLine);
		free(cmdLine);
	}
	close(servfd);
	return (0);
}
