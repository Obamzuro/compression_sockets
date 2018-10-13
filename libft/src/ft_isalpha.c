/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isalpha.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obamzuro <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/23 18:30:32 by obamzuro          #+#    #+#             */
/*   Updated: 2018/10/10 22:25:33 by obamzuro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_isalpha(int c)
{
	return ((c <= 0132 && c >= 0101) || (c >= 0141 && c <= 0172));
}

int	ft_isalphastr(char *str)
{
	if (!str)
		return (0);
	while (*str)
	{
		if (!ft_isalpha(*str))
			return (0);
		++str;
	}
	return (1);
}
