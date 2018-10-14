/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obamzuro <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/07 20:17:30 by obamzuro          #+#    #+#             */
/*   Updated: 2018/10/15 00:20:09 by obamzuro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.h"

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
