/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obamzuro <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/07 20:17:30 by obamzuro          #+#    #+#             */
/*   Updated: 2018/10/14 23:36:30 by obamzuro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.h"

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
