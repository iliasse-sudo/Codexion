/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   allocs.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibaya <ibaya@student.1337.ma>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/14 17:22:15 by ibaya             #+#    #+#             */
/*   Updated: 2026/09/16 04:37:54 by ibaya            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	*ft_malloc(t_allocs_tracker *allocs, size_t size)
{
	void	*new;
	t_alloc	*new_alloc;

	new_alloc = malloc(sizeof(*new_alloc));
	new = malloc(size);
	if (!new || !new_alloc)
	{
		free(new);
		new = NULL;
		free(new_alloc);
		new_alloc = NULL;
		return (NULL);
	}
	memset(new, 0, size);
	memset(new_alloc, 0, sizeof((*new_alloc)));
	new_alloc->ptr = new;
	new_alloc->next = NULL;
	allocs->allocs_end->next = new_alloc;
	allocs->allocs_end = new_alloc;
	return (new);
}

pthread_mutex_t	*create_mutex(t_allocs_tracker *allocs)
{
	pthread_mutex_t	*new;
	t_mutex_alloc	*new_mutex;

	new = malloc(sizeof(*new));
	new_mutex = malloc(sizeof(*new_mutex));
	if (!new || !new_mutex)
		return (free(new), free(new_mutex), NULL);
	if (pthread_mutex_init(new, NULL))
		return (free(new), free(new_mutex), NULL);
	memset(new_mutex, 0, sizeof(*new_mutex));
	memset(new, 0, sizeof(*new));
	new_mutex->mutex_ptr = new;
	new_mutex->next = NULL;
	if (!allocs->mutexes_head)
		allocs->mutexes_head = new_mutex;
	else
		allocs->mutexes_end->next = new_mutex;
	allocs->mutexes_end = new_mutex;
	return (new);
}

pthread_cond_t	*create_cond(t_allocs_tracker *allocs)
{
	pthread_cond_t	*new;
	t_cond_alloc	*new_cond;

	new = malloc(sizeof(*new));
	new_cond = malloc(sizeof(*new_cond));
	if (!new || !new_cond)
		return (free(new), free(new_cond), NULL);
	if (pthread_cond_init(new, NULL))
		return (free(new), free(new_cond), NULL);
	memset(new_cond, 0, sizeof(*new_cond));
	memset(new, 0, sizeof(*new));
	new_cond->cond_ptr = new;
	new_cond->next = NULL;
	if (!allocs->conds_head)
		allocs->conds_head = new_cond;
	else
		allocs->conds_end->next = new_cond;
	allocs->conds_end = new_cond;
	return (new);
}
