/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response_compress.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obamzuro <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/13 23:50:41 by obamzuro          #+#    #+#             */
/*   Updated: 2018/10/14 23:25:44 by obamzuro         ###   ########.fr       */
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
** endReturnedLine - end of returnedLine
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
** Compressing payload and send header with compressed data
*/

static void		compress_and_response_payload(int clientfd,
		t_msgheader *receivedHeader, char *receivedStr,
		t_metadata *metadata)
{
	t_msgheader		responseHeader;
	char			*compressedStr;
	uint16_t		compressedStrLen;
	char			*dataSent;
	ssize_t			sendReturned;

	compressedStr = compress_data(receivedStr, receivedHeader->payloadlen);
	compressedStrLen = strlen(compressedStr);

	bzero(&responseHeader, sizeof(responseHeader));
	responseHeader.magic = htonl(MAGIC);
	responseHeader.payloadlen = htons(compressedStrLen);

	dataSent = (char *)malloc(sizeof(t_msgheader) + compressedStrLen);
	memcpy(dataSent, &responseHeader, sizeof(responseHeader));
	memcpy(dataSent + sizeof(responseHeader), compressedStr, compressedStrLen);
	sendReturned = send(clientfd, dataSent, sizeof(t_msgheader) + compressedStrLen, 0);

	change_metadata(metadata, 0, sendReturned, 0, sendReturned - sizeof(t_msgheader));
	free(dataSent);
	free(compressedStr);
}

/*
** Receive payload and check recv() returned value:
** -1
** if value less than specified in header
*/

static enum e_errors	recv_str(int clientfd, t_metadata *metadata,
		uint16_t receivedStrLen, char **receivedStr)
{
	ssize_t			recvReturned;

	*receivedStr = (char *)malloc(receivedStrLen + 1);
	recvReturned = recv(clientfd, *receivedStr, receivedStrLen, 0);
	change_metadata(metadata, recvReturned, 0, recvReturned, 0);
	if (recvReturned == -1)
	{
		free(*receivedStr);
		close(clientfd);
		printf("clientfd #%d - closed after failing recv()\n", clientfd);
		exit(EXIT_FAILURE);
	}
	if (recvReturned < receivedStrLen)
	{
		send_error_header(clientfd, metadata, PAYLOAD_WRONG_LEN);
		change_metadata(metadata, recvReturned, 0, 0, 0);
		free(*receivedStr);
		return (PAYLOAD_WRONG_LEN);
	}
	(*receivedStr)[receivedStrLen] = '\0';
	return (OK);
}

/*
** Check if string contains
** only lowercase english chars
*/

static int		is_strlowercase(char *str)
{
	while (*str)
		if (!islower(*str++))
			return (0);
	return (1);
}

/*
** Check case if payloadLen bigger than limit of payloadLen
** Check case if payloadlen is 0
*/

static enum e_errors	check_header(int clientfd,
		t_msgheader *receivedHeader, t_metadata *metadata)
{
	if (receivedHeader->payloadlen > MAX_PAYLOAD_LEN)
	{
		send_error_header(clientfd, metadata, MESSAGE_TOO_LARGE);
		return (MESSAGE_TOO_LARGE);
	}
	if (!receivedHeader->payloadlen)
	{
		send_error_header(clientfd, metadata, PAYLOAD_ZERO_LEN);
		return (PAYLOAD_ZERO_LEN);
	}
	return (OK);
}

/*
** Response Compress RC:
** - check header
** - receive payload
** - check payload
** - compress payload (in func compress_and_response_payload())
** - send payload (in func compress_and_response_payload())
*/

enum e_errors	response_compress(int clientfd, t_msgheader *receivedHeader,
		t_metadata *metadata)
{
	char			*receivedStr;
	enum e_errors	error;

	if ((error = check_header(clientfd, receivedHeader, metadata)) != OK)
		return (error);
	if ((error = recv_str(clientfd, metadata,
				receivedHeader->payloadlen, &receivedStr)) != OK)
		return (error);
	if (!is_strlowercase(receivedStr))
	{
		send_error_header(clientfd, metadata, PAYLOAD_NOT_ALPHA);
		change_metadata(metadata, strlen(receivedStr), 0, 0, 0);
		free(receivedStr);
		return (PAYLOAD_NOT_ALPHA);
	}
	compress_and_response_payload(clientfd,
			receivedHeader, receivedStr, metadata);
	free(receivedStr);
	return (OK);
}
