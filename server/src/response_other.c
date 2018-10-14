/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response_other.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obamzuro <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/13 23:50:26 by obamzuro          #+#    #+#             */
/*   Updated: 2018/10/14 23:28:35 by obamzuro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.h"

/*
** Response ResetStats RC
*/

enum e_errors	response_resetstats(int clientfd,
		t_msgheader *receivedHeader, t_metadata *metadata)
{
	t_msgheader				responseHeader;
	ssize_t					sendReturned;

	bzero(&responseHeader, sizeof(responseHeader));
	responseHeader.magic = htonl(MAGIC);
	bzero(metadata, sizeof(*metadata));
	sendReturned = send(clientfd, &responseHeader, sizeof(responseHeader), 0);
	change_metadata(metadata, 0, sendReturned, 0, 0);
	return (OK);
}

/*
** Response Ping RC
*/

enum e_errors	response_ping(int clientfd,
		t_msgheader *receivedHeader, t_metadata *metadata)
{
	t_msgheader		responseHeader;
	ssize_t					sendReturned;

	bzero(&responseHeader, sizeof(responseHeader));
	responseHeader.magic = htonl(MAGIC);
	sendReturned = send(clientfd, &responseHeader, sizeof(responseHeader), 0);
	change_metadata(metadata, 0, sendReturned, 0, 0);
	return (OK);
}

/*
** Response GetStats RC
** P.S (ratio can't be in stats struct because of
** structure alignment)
*/

enum e_errors	response_getstats(int clientfd,
		t_msgheader *receivedHeader, t_metadata *metadata)
{
	t_msgheader				responseHeader;
	t_stats					stats;
	unsigned char			ratio;
	char					*dataSent;
	ssize_t					sendReturned;

	bzero(&responseHeader, sizeof(responseHeader));
	responseHeader.magic = htonl(MAGIC);
	if (!metadata->compressReceived)
		ratio = 0;
	else
		ratio = (unsigned char)((double)metadata->compressSent /
				metadata->compressReceived * MAX_RATIO);
	stats.received = htonl(metadata->totalReceived);
	stats.sent = htonl(metadata->totalSent);

	dataSent = (char *)malloc(sizeof(t_msgheader) + sizeof(t_stats) + sizeof(ratio));
	memcpy(dataSent, &responseHeader, sizeof(responseHeader));
	memcpy(dataSent + sizeof(responseHeader), &stats, sizeof(stats));
	memcpy(dataSent + sizeof(responseHeader) + sizeof(stats), &ratio, sizeof(ratio));

	sendReturned = send(clientfd, dataSent, sizeof(responseHeader) + sizeof(stats) + sizeof(ratio), 0);
	change_metadata(metadata, 0, sendReturned, 0, 0);
	free(dataSent);
	return (OK);
}
