/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obamzuro <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/07 18:05:21 by obamzuro          #+#    #+#             */
/*   Updated: 2018/10/14 23:25:35 by obamzuro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.h"

/*
** Change total stats and compress stats for server
*/

void	change_metadata(t_metadata *metadata,
		uint16_t totalReceived, uint16_t totalSent,
		uint16_t compressReceived, uint16_t compressSent)
{
	metadata->totalReceived += totalReceived;
	metadata->totalSent += totalSent;
	metadata->compressReceived += compressReceived;
	metadata->compressSent += compressSent;
}

/*
** Generate header with certain error
*/

void	send_error_header(int clientfd, t_metadata *metadata,
		enum e_errors error)
{
	t_msgheader	responseHeader;
	ssize_t		sendReturned;

	printf("clientfd #%d - error #%d\n", clientfd, error);
	bzero(&responseHeader, sizeof(responseHeader));
	responseHeader.magic = htonl(MAGIC);
	responseHeader.status = htons(error);
	sendReturned = send(clientfd, &responseHeader, sizeof(responseHeader), 0);
	change_metadata(metadata, 0, sendReturned, 0, 0);
}
