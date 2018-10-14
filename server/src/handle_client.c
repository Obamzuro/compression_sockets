/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obamzuro <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/07 18:05:21 by obamzuro          #+#    #+#             */
/*   Updated: 2018/10/15 00:16:12 by obamzuro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.h"

/*
** Check received header
*/

static enum e_errors	check_receivedHeader(int clientfd, t_metadata *metadata,
		t_msgheader *receivedHeader)
{
	if (receivedHeader->magic != MAGIC)
	{
		send_error_header(clientfd, metadata, HEADER_WRONG_MAGIC);
		return (HEADER_WRONG_MAGIC);
	}
	return (OK);
}

/*
** Check returned value of function recv()
*/

static enum e_errors	check_recvReturned(int clientfd, t_metadata *metadata,
		ssize_t recvReturned)
{
	if (!recvReturned)
	{
		close(clientfd);
		printf("client socket closed\n");
		exit(EXIT_SUCCESS);
	}
	else if (recvReturned == -1)
	{
		close(clientfd);
		printf("client socket closed after failing recv()\n");
		exit(EXIT_FAILURE);
	}
	if (recvReturned < sizeof(t_msgheader))
	{
		send_error_header(clientfd, metadata, HEADER_WRONG_LEN);
		return (HEADER_WRONG_LEN);
	}
	return (OK);
}

/*
** Receive header and check it
*/

static enum e_errors	recv_header(int clientfd, t_metadata *metadata,
		t_msgheader *receivedHeader)
{
	ssize_t			recvReturned;
	enum e_errors	error;

	recvReturned = recv(clientfd, receivedHeader,
				sizeof(*receivedHeader), 0);
	change_metadata(metadata, recvReturned, 0, 0, 0);
	if ((error = check_recvReturned(clientfd, metadata, recvReturned)) != OK)
		return (error);
	receivedHeader->status = ntohs(receivedHeader->status);
	receivedHeader->payloadlen = ntohs(receivedHeader->payloadlen);
	receivedHeader->magic = ntohl(receivedHeader->magic);
	if ((error = check_receivedHeader(clientfd, metadata, receivedHeader)) != OK)
		return (error);
	return (OK);
}

/*
** Handle new connection with client:
** - Waiting for receiving new header
** with RequestCode from client
** - Call function for corresponding RC
** or send error header to client
*/

void				handling_newclient(int clientfd)
{
	t_msgheader		receivedHeader;
	t_metadata		metadata;
	int				i;
	enum e_errors	error;
	t_responsemap	responses[AMOUNT_REQ_CODES] =
	{
		{PING, response_ping},
		{COMPRESS, response_compress},
		{GETSTATS, response_getstats},
		{RESETSTATS, response_resetstats}
	};

	bzero(&metadata, sizeof(metadata));
	while (1)
	{
		if ((error = recv_header(clientfd, &metadata, &receivedHeader)) != OK)
			continue ;
		i = -1;
		while (++i != AMOUNT_REQ_CODES)
			if (receivedHeader.status == responses[i].code)
				break ;

		// if request was found
		if (i != AMOUNT_REQ_CODES)
		{
			printf("RC #%d received \n", responses[i].code);
			error = responses[i].func(clientfd, &receivedHeader, &metadata);
			printf("processed RC with status %d\n", error);
		}
		else
			send_error_header(clientfd, &metadata, UNSUPPORTED_REQUEST_TYPE);
	}
	exit(UKNOWN_ERROR);
}
