/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obamzuro <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/07 18:06:26 by obamzuro          #+#    #+#             */
/*   Updated: 2018/10/14 21:22:35 by obamzuro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CLIENT_H
# define __CLIENT_H
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <strings.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netdb.h>
# include <pthread.h>
# include <ctype.h>
# include <signal.h>
# include "libft.h"
# define AMOUNT_REQ_CODES 4
# define MAGIC 0x53545259

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

typedef struct		s_requestmap
{
	enum e_commandcode	code;
	char				*commandName;
	void				(*func)(int servfd, char **cmdLineDivided);
}					t_requestmap;

void				request_ping(int servfd, char **cmdLineDivided);
void				request_compress(int servfd, char **cmdLineDivided);
void				request_getstats(int sockfd, char **cmdLineDivided);
void				request_resetstats(int sockfd, char **cmdLineDivided);
#endif
