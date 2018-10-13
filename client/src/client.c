/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obamzuro <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/07 20:17:30 by obamzuro          #+#    #+#             */
/*   Updated: 2018/10/13 23:57:53 by obamzuro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.h"

t_requestmap	g_requests[AMOUNT_REQ_CODES] =
{
	{PING, request_ping},
	{GETSTATS, request_getstats},
	{RESETSTATS, request_resetstats},
	{COMPRESS, request_compress}
};

void	request_resetstats(int servfd, char **argv)
{
	char				buf[BUFSIZ];
	int					bytes;
	t_msgheader			reqheader;

	bzero(&reqheader, sizeof(reqheader));
	reqheader.magic = htonl(MAGIC);
	reqheader.status = htons(RESETSTATS);
	send(servfd, &reqheader, sizeof(reqheader), 0);
	while ((bytes = recv(servfd, buf, BUFSIZ, 0)) > 0)
		write(1, buf, bytes);
}

void	request_getstats(int servfd, char **argv)
{
	char				buf[BUFSIZ];
	int					bytes;
	t_msgheader			reqheader;

	bzero(&reqheader, sizeof(reqheader));
	reqheader.magic = htonl(MAGIC);
	reqheader.status = htons(GETSTATS);
	send(servfd, &reqheader, sizeof(reqheader), 0);
	while ((bytes = recv(servfd, buf, BUFSIZ, 0)) > 0)
		write(1, buf, bytes);
}

void	request_ping(int servfd, char **argv)
{
	char				buf[BUFSIZ];
	int					bytes;
	t_msgheader		reqheader;

	bzero(&reqheader, sizeof(reqheader));
	reqheader.magic = htonl(MAGIC);
	reqheader.status = htons(PING);
	send(servfd, &reqheader, sizeof(reqheader), 0);
	while ((bytes = recv(servfd, buf, BUFSIZ, 0)) > 0)
		write(1, buf, bytes);
}

char	*get_file_content(char *filename)
{
	int		fd;
	char	*temp;
	char	*content;
	char	line[BUFSIZ + 1];
	int		ret;

	if (!(fd = open(filename, O_RDONLY)))
	{
		perror("open");
		exit(EXIT_FAILURE);
	}
	content = NULL;
	ft_bzero(&line, sizeof(line));
	while ((ret = read(fd, &line, BUFSIZ)) > 0)
	{
		temp = content;
		content = ft_strjoin(content, line);
		free(temp);
		ft_bzero(&line, sizeof(line));
	}
	if (ret == -1)
	{
		perror("read");
		exit(EXIT_FAILURE);
	}
	return (content);
}

void	request_compress(int servfd, char **argv)
{
	t_msgheader		reqheader;
	char			buf[BUFSIZ];
	int				bytes;
	char			*dataSent;
	char			*payload;
	uint16_t		sentStrLen;

	if (!argv[3] || (!strcmp(argv[3], "-f") && !argv[4]))
	{
		fprintf(stderr, "Request code \"Compress\" \
usage:\n ./client [host] 4 string\n\
or\n ./client [host] 4 -f file\n");
		exit(EXIT_FAILURE);
	}
	if (!strcmp(argv[3], "-f"))
		payload = get_file_content(argv[4]);
	else
		payload = argv[3];
	sentStrLen = strlen(payload);
	dataSent = (char *)malloc(sizeof(reqheader) + sentStrLen);
	reqheader.magic = htonl(MAGIC);
	reqheader.payloadlen = htons(sentStrLen);
	reqheader.status = htons(COMPRESS);
	memcpy(dataSent, &reqheader, sizeof(reqheader));
	memcpy(dataSent + sizeof(reqheader), payload, sentStrLen);
	send(servfd, dataSent, sizeof(reqheader) + sentStrLen, 0);
	free(dataSent);
	if (payload != argv[3])
		free(payload);
	while ((bytes = recv(servfd, buf, BUFSIZ, 0)) > 0)
		write(1, buf, bytes);
}

void	requesting(int servfd, char **argv)
{
	int		i;

	i = 0;
	while (i != AMOUNT_REQ_CODES)
	{
		if (argv[2][0] == g_requests[i].code + '0')
		{
			g_requests[i].func(servfd, argv);
			break ;
		}
		++i;
	}
	close(servfd);
}

int		main(int argc, char **argv)
{
	int					servfd;
	struct sockaddr_in	sockaddr;
	struct hostent		*hostEntity;

	if (argc < 3)
	{
		fprintf(stderr, "Usage: ./client [host] [request code]\n");
		exit(EXIT_FAILURE);
	}
	if ((servfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}
	if ((hostEntity = gethostbyname(argv[1])) == NULL)
	{
		herror(argv[1]);
		exit(EXIT_FAILURE);
	}
	bzero(&sockaddr, sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(4000);
	memcpy(&sockaddr.sin_addr, hostEntity->h_addr_list[0], hostEntity->h_length);
	if (connect(servfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0)
	{
		perror("connect");
		exit(EXIT_FAILURE);
	}
	requesting(servfd, argv);
	return (0);
}
