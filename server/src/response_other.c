/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response_other.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obamzuro <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/13 23:50:26 by obamzuro          #+#    #+#             */
/*   Updated: 2018/10/13 23:59:28 by obamzuro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.h"

/*
** Response ResetStats RC
*/

void	response_resetstats(int clientfd,
		t_msgheader *receivedHeader, t_metadata *metadata)
{
	t_msgheader				responseHeader;
	extern pthread_mutex_t	g_metadataLock;

	bzero(&responseHeader, sizeof(responseHeader));
	responseHeader.magic = htonl(MAGIC);
	pthread_mutex_lock(&g_metadataLock);
	bzero(metadata, sizeof(*metadata));
	pthread_mutex_unlock(&g_metadataLock);
	send(clientfd, &responseHeader, sizeof(responseHeader), 0);
}

/*
** Response Ping RC
*/

void	response_ping(int clientfd,
		t_msgheader *receivedHeader, t_metadata *metadata)
{
	t_msgheader		responseHeader;

	bzero(&responseHeader, sizeof(responseHeader));
	responseHeader.magic = htonl(MAGIC);
	send(clientfd, &responseHeader, sizeof(responseHeader), 0);
}

/*
** Response GetStats RC
** (ratio can't be in stats struct because of
** structure alignment)
*/

void	response_getstats(int clientfd,
		t_msgheader *receivedHeader, t_metadata *metadata)
{
	t_msgheader				responseHeader;
	t_stats					stats;
	unsigned char			ratio;
	char					*dataSent;
	extern pthread_mutex_t	g_metadataLock;

	bzero(&responseHeader, sizeof(responseHeader));
	responseHeader.magic = htonl(MAGIC);
	pthread_mutex_lock(&g_metadataLock);
	if (!metadata->compressReceived)
		ratio = 0;
	else
		ratio = (unsigned char)((double)metadata->compressSent /
				metadata->compressReceived * MAX_RATIO);
	printf("ratio = %d", ratio);
	stats.received = htonl(metadata->totalReceived);
	stats.sent = htonl(metadata->totalSent);
	dataSent = (char *)malloc(sizeof(t_msgheader) + sizeof(t_stats) + sizeof(ratio));
	memcpy(dataSent, &responseHeader, sizeof(responseHeader));
	memcpy(dataSent + sizeof(responseHeader), &stats, sizeof(stats));
	memcpy(dataSent + sizeof(responseHeader) + sizeof(stats), &ratio, sizeof(ratio));
	send(clientfd, dataSent, sizeof(responseHeader) + sizeof(stats) + sizeof(ratio), 0);
	pthread_mutex_unlock(&g_metadataLock);
	change_metadata(metadata, 0, sizeof(stats) + sizeof(ratio), 0, 0);
	free(dataSent);
}
