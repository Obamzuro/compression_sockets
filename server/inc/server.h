/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obamzuro <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/07 18:06:26 by obamzuro          #+#    #+#             */
/*   Updated: 2018/10/15 00:20:07 by obamzuro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __SERVER_H
# define __SERVER_H
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <strings.h>
# include <sys/socket.h>
# include <sys/wait.h>
# include <netinet/in.h>
# include <netdb.h>
# include <pthread.h>
# include <ctype.h>
# include <signal.h>
# include "libft.h"
# define BACKLOG 4
# define AMOUNT_REQ_CODES 4
# define MAGIC 0x53545259
# define MAX_PAYLOAD_LEN 1 << 15
# define MAX_RATIO 100

enum				e_errors
{
	OK = 0,
	UKNOWN_ERROR,
	MESSAGE_TOO_LARGE,
	UNSUPPORTED_REQUEST_TYPE,
	HEADER_WRONG_LEN = 33,
	HEADER_WRONG_MAGIC,
	PAYLOAD_WRONG_LEN,
	PAYLOAD_NOT_ALPHA,
	PAYLOAD_ZERO_LEN
};

typedef struct		s_metadata
{
	uint32_t	totalReceived;
	uint32_t	totalSent;
	uint32_t	compressReceived;
	uint32_t	compressSent;
}					t_metadata;

typedef struct		s_stats
{
	uint32_t		received;
	uint32_t		sent;
}					t_stats;

typedef struct		s_msgheader
{
	uint32_t		magic;
	uint16_t		payloadlen;
	uint16_t		status;
}					t_msgheader;

enum				e_commandcode
{
	PING = 1,
	GETSTATS,
	RESETSTATS,
	COMPRESS,
};

typedef struct		s_responsemap
{
	enum e_commandcode	code;
	enum e_errors		(*func)(int clientfd,
			t_msgheader *msgheader, t_metadata *metadata);
}					t_responsemap;

enum e_errors		response_ping(int clientfd, t_msgheader *reqheader, t_metadata *metadata);
enum e_errors		response_compress(int clientfd, t_msgheader *reqheader, t_metadata *metadata);
enum e_errors		response_getstats(int clientfd, t_msgheader *reqheader, t_metadata *metadata);
enum e_errors		response_resetstats(int clientfd, t_msgheader *reqheader, t_metadata *metadata);

void				handling_newclient(int clientfd);
void				send_error_header(int clientfd, t_metadata *metadata, enum e_errors error);
void				change_metadata(t_metadata *metadata,
					uint16_t totalReceived, uint16_t totalSent,
					uint16_t compressReceived, uint16_t compressSent);
#endif
