/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obamzuro <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/07 18:05:21 by obamzuro          #+#    #+#             */
/*   Updated: 2018/10/14 00:11:46 by obamzuro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.h"

/*
** Check received Header that sent client
*/

static int			check_receivedHeader(int clientfd, t_metadata *metadata,
		t_msgheader *receivedHeader)
{
	if (receivedHeader->magic != MAGIC)
	{
		send_error_header(clientfd, HEADER_WRONG_MAGIC);
		change_metadata(metadata, sizeof(t_msgheader), sizeof(t_msgheader), 0, 0);
		return (-1);
	}
	if (receivedHeader->status > AMOUNT_REQ_CODES || !receivedHeader->status)
	{
		send_error_header(clientfd, UNSUPPORTED_REQUEST_TYPE);
		change_metadata(metadata, sizeof(t_msgheader), sizeof(t_msgheader), 0, 0);
		return (-1);
	}
	return (0);
}

/*
** Check returned value of function recv
*/

static int			check_recvReturned(int clientfd, t_metadata *metadata,
		int recvReturned)
{
	if (recvReturned == -1)
	{
		send_error_header(clientfd, RECV_ERROR);
		return (-1);
	}
	if (recvReturned < sizeof(t_msgheader))
	{
		send_error_header(clientfd, HEADER_WRONG_LEN);
		change_metadata(metadata, recvReturned, sizeof(t_msgheader), 0, 0);
		return (-1);
	}
	return (0);
}

/*
** Recv and check header that sent client
*/

static t_msgheader	*recv_header(int clientfd, t_metadata *metadata)
{
	t_msgheader		*receivedHeader;
	int				recvReturned;

	receivedHeader = (t_msgheader *)malloc(sizeof(t_msgheader));
	recvReturned = recv(clientfd, receivedHeader, sizeof(*receivedHeader), 0);
	if (check_recvReturned(clientfd, metadata, recvReturned))
	{
		free(receivedHeader);
		return (NULL);
	}
	receivedHeader->status = ntohs(receivedHeader->status);
	receivedHeader->payloadlen = ntohs(receivedHeader->payloadlen);
	receivedHeader->magic = ntohl(receivedHeader->magic);
	if (check_receivedHeader(clientfd, metadata, receivedHeader))
	{
		free(receivedHeader);
		return (NULL);
	}
	return (receivedHeader);
}

/*
** Handle new connection with client
** in the new thread
*/

void				*handling_newclient(void *arg)
{
	t_msgheader		*receivedHeader;
	t_metadata		*metadata;
	t_threadinfo	threadInfo;
	int				clientfd;
	int				i;
	t_responsemap	responses[AMOUNT_REQ_CODES] =
	{
		{PING, response_ping},
		{COMPRESS, response_compress},
		{GETSTATS, response_getstats},
		{RESETSTATS, response_resetstats}
	};


	threadInfo = *((t_threadinfo *)arg);
	clientfd = threadInfo.clientfd;
	metadata = threadInfo.metadata;
	if (!(receivedHeader = recv_header(clientfd, metadata)))
		return (NULL);
	i = -1;
	while (++i != AMOUNT_REQ_CODES)
	{
		if (receivedHeader->status == responses[i].code)
		{
			responses[i].func(clientfd, receivedHeader, metadata);
			break ;
		}
	}
	if (i != AMOUNT_REQ_CODES && receivedHeader->status != RESETSTATS)
		change_metadata(metadata, sizeof(t_msgheader), sizeof(t_msgheader), 0, 0);
	close(clientfd);
	free(receivedHeader);
	return (NULL);
}
