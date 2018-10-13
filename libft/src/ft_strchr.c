/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obamzuro <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/22 20:25:54 by obamzuro          #+#    #+#             */
/*   Updated: 2018/09/21 22:17:47 by obamzuro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strchr(const char *s, int c)
{
	while (*s != (char)c)
		if (!*s++)
			return (NULL);
	return (char *)(s);
}

char	*ft_strchr_str(const char *s, const char *s2)
{
	while (!ft_is_char_in_str(*s, s2))
		if (!*s++)
			return (NULL);
	return (char *)(s);
}
