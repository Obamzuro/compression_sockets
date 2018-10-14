/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obamzuro <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/07 18:05:21 by obamzuro          #+#    #+#             */
/*   Updated: 2018/10/14 23:53:27 by obamzuro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.h"

/*
** Waiting for the new connection and
** create new process for handling client
*/

void	listening(int servfd)
{
	pthread_t			ntid;
	int					clientfd;
	pid_t				pid;

	while (1)
	{
		if ((clientfd = accept(servfd, NULL, NULL)) < 0)
		{
			perror("server: accept");
			continue ;
		}
		printf("new client connected\n");
		pid = fork();
		if (pid < 0)
			perror("fork");
		else if (pid == 0)
			handling_newclient(clientfd);
		close(clientfd);
	}
}

void	signal_handler(int sig)
{
	if (sig == SIGCHLD)
		wait(NULL);
}

/*
** Handle SIGCHLD signal for prevent
** creating zombie-processes
*/

void	setup_signal_handler(void)
{
	struct sigaction act;

	bzero(&act, sizeof(act));
	act.sa_handler = signal_handler;
	act.sa_flags |= SA_RESTART;
	sigaction(SIGCHLD, &act, 0);
}

/*
** Init server socket
*/

int		main(int argc, char **argv)
{
	int					servfd;
	struct sockaddr_in	sockaddr;

	if (argc < 2 || (!atoi(argv[1]) && strcmp(argv[1], "0")))
	{
		fprintf(stderr, "Usage: ./server [port]\n");
		exit(EXIT_FAILURE);
	}
	if ((servfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}
	bzero(&sockaddr, sizeof(sockaddr));
	sockaddr.sin_family = PF_INET;
	sockaddr.sin_port = htons(atoi(argv[1]));
	if (INADDR_ANY)
		sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(servfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0)
	{
		perror("bind");
		exit(EXIT_FAILURE);
	}
	setup_signal_handler();
	listen(servfd, BACKLOG);
	listening(servfd);
}
