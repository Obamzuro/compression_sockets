/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obamzuro <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/07 20:17:30 by obamzuro          #+#    #+#             */
/*   Updated: 2018/10/19 18:40:47 by obamzuro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.h"

int g_servfd;

void	requesting(int servfd, char *cmdLine)
{
	int		i;
	char	**cmdLineDivided;
	t_requestmap	requests[AMOUNT_REQ_CODES] =
	{
		{PING, "Ping", request_ping},
		{GETSTATS, "GetStats", request_getstats},
		{RESETSTATS, "ResetStats", request_resetstats},
		{COMPRESS, "Compress", request_compress}
	};

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
		if (!strcmp(cmdLineDivided[0], requests[i].commandName))
		{
			requests[i].func(servfd, cmdLineDivided);
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
	printf("\nclosing %d socket\n", g_servfd);
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
		if (get_next_line(STDIN_FILENO, &cmdLine) < 0 || !cmdLine)
			break ;
		requesting(servfd, cmdLine);
		free(cmdLine);
	}
	close(servfd);
	return (0);
}
