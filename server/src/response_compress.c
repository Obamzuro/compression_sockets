/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response_compress.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obamzuro <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/13 23:50:41 by obamzuro          #+#    #+#             */
/*   Updated: 2018/10/13 23:50:45 by obamzuro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.h"

/*
** If amount of repeating letters in payload
** on special position(data) > 2 - 
** write compressed record option into the new string.
** ft_itoa() - variation of itoa() function that
** internally calculates needed length for malloced buffer
** and works in 10th number system
*/

static void		memcpy_compressed_case(int amountRepeating,
		char **endReturnedLine, char *data)
{
	char		*number;
	size_t		numberLen;

	number = ft_itoa(amountRepeating);
	numberLen = strlen(number);
	memcpy(*endReturnedLine, number, numberLen);
	*endReturnedLine += numberLen;
	memcpy(*endReturnedLine, data, 1);
	++*endReturnedLine;
	free(number);
}

/*
** Compression algorithm on data - payload string
** with datalen - length of payload
** amountRepeating - amount of repeating letters on (char *)data position
*/

char			*compress_data(char *data, uint16_t datalen)
{
	char		*returnedLine;
	char		*endReturnedLine;
	size_t		amountRepeating;

	returnedLine = (char *)malloc(sizeof(*data) * (datalen + 1));
	endReturnedLine = returnedLine;
	amountRepeating = 1;
	while (*data)
	{
		if (*data == *(data + 1))
			++amountRepeating;
		else
		{
			if (amountRepeating > 2)
				memcpy_compressed_case(amountRepeating, &endReturnedLine, data);
			else
			{
				memcpy(endReturnedLine, data - amountRepeating + 1, amountRepeating);
				endReturnedLine += amountRepeating;
			}
			amountRepeating = 1;
		}
		++data;
	}
	return (returnedLine);
}

/*
** Compressing payload and sent header with compressed data
*/

static void		response_compressed_payload(int clientfd,
		t_msgheader *receivedHeader, char *receivedStr,
		t_metadata *metadata)
{
	t_msgheader		responseHeader;
	char			*compressedStr;
	uint16_t		compressedStrLen;
	char			*dataSent;

	compressedStr = compress_data(receivedStr, receivedHeader->payloadlen);
	compressedStrLen = strlen(compressedStr);
	bzero(&responseHeader, sizeof(responseHeader));
	responseHeader.magic = htonl(MAGIC);
	responseHeader.payloadlen = htons(compressedStrLen);
	dataSent = (char *)malloc(sizeof(t_msgheader) + compressedStrLen);
	memcpy(dataSent, &responseHeader, sizeof(responseHeader));
	memcpy(dataSent + sizeof(responseHeader), compressedStr, compressedStrLen);
	send(clientfd, dataSent, sizeof(t_msgheader) + compressedStrLen, 0);
	change_metadata(metadata, receivedHeader->payloadlen, compressedStrLen,
			receivedHeader->payloadlen, compressedStrLen);
	free(dataSent);
	free(compressedStr);
}

/*
** Receive payload and check recv() returned value:
** -1
** if value less than specified in header
*/

static char		*recv_str(int clientfd, uint16_t receivedStrLen,
		t_metadata *metadata)
{
	char			*receivedStr;
	int				recvReturned;

	receivedStr = (char *)malloc(receivedStrLen + 1);
	recvReturned = recv(clientfd, receivedStr, receivedStrLen, 0);
	if (recvReturned == -1)
	{
		send_error_header(clientfd, RECV_ERROR);
		free(receivedStr);
		return (NULL);
	}
	if (recvReturned < receivedStrLen)
	{
		send_error_header(clientfd, PAYLOAD_WRONG_LEN);
		change_metadata(metadata, recvReturned, 0, 0, 0);
		free(receivedStr);
		return (NULL);
	}
	receivedStr[receivedStrLen] = '\0';
	return (receivedStr);
}

/*
** Check if string contains only lowercase english chars
*/

static int		is_strlowercase(char *str)
{
	while (*str)
		if (!islower(*str++))
			return (0);
	return (1);
}

/*
** Response Compress RC
** check case if payloadLen bigger than limit of payloadLen
** check case if payloadlen is 0
** recv payload and check error cases from recv_str() func
** check if received payload consists of only lowercase english chars
*/

void			response_compress(int clientfd,
		t_msgheader *receivedHeader, t_metadata *metadata)
{
	char			*receivedStr;

	if (receivedHeader->payloadlen > MAX_PAYLOAD_LEN)
	{
		send_error_header(clientfd, MESSAGE_TOO_LARGE);
		return ;
	}
	if (!receivedHeader->payloadlen)
	{
		send_error_header(clientfd, PAYLOAD_ZERO_LEN);
		return ;
	}
	if (!(receivedStr = recv_str(clientfd, receivedHeader->payloadlen, metadata)))
		return ;
	if (!is_strlowercase(receivedStr))
	{
		send_error_header(clientfd, PAYLOAD_NOT_ALPHA);
		change_metadata(metadata, strlen(receivedStr), 0, 0, 0);
		free(receivedStr);
		return ;
	}
	response_compressed_payload(clientfd,
			receivedHeader, receivedStr, metadata);
	free(receivedStr);
}
