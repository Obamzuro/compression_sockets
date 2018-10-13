/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obamzuro <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/07 18:05:21 by obamzuro          #+#    #+#             */
/*   Updated: 2018/10/13 23:57:52 by obamzuro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.h"

pthread_mutex_t	g_metadataLock = PTHREAD_MUTEX_INITIALIZER;

/*
** Change total bytes stats and compress stats for server
*/

void	change_metadata(t_metadata *metadata,
		uint16_t totalReceived, uint16_t totalSent,
		uint16_t compressReceived, uint16_t compressSent)
{
	pthread_mutex_lock(&g_metadataLock);
	metadata->totalReceived += totalReceived;
	metadata->totalSent += totalSent;
	metadata->compressReceived += compressReceived;
	metadata->compressSent += compressSent;
	pthread_mutex_unlock(&g_metadataLock);
}

/*
** Generate header with certain error
*/

void	send_error_header(int clientfd, enum e_errors error)
{

	t_msgheader	responseHeader;

	printf("\n ERROR #%d\n", error);
	bzero(&responseHeader, sizeof(responseHeader));
	responseHeader.magic = htonl(MAGIC);
	responseHeader.status = htons(error);
	send(clientfd, &responseHeader, sizeof(responseHeader), 0);
}

/*
** Waiting for connection with server socket
*/

void	listening(int servfd)
{
	t_metadata			metadata;
	pthread_t			ntid;
	t_threadinfo		threadInfo;
	int					clientfd;

	bzero(&threadInfo, sizeof(threadInfo));
	bzero(&metadata, sizeof(metadata));
	threadInfo.metadata = &metadata;
	while (1)
	{
		if ((clientfd = accept(servfd, NULL, NULL)) < 0)
		{
			perror("server: accept");
			continue ;
		}
		threadInfo.clientfd = clientfd;
		pthread_create(&ntid, NULL, &handling_newclient, &threadInfo);
	}
}

/*
** Init server socket
*/

int		main(int argc, char **argv)
{
	int					servfd;
	struct sockaddr_in	sockaddr;

	if (argc < 2 || !atoi(argv[1]))
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
	listen(servfd, BACKLOG);
	listening(servfd);
}
