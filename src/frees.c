/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   frees.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibaya <ibaya@student.1337.ma>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/14 17:24:03 by ibaya             #+#    #+#             */
/*   Updated: 2026/09/14 17:55:37 by ibaya            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	ft_destroy_mutexes(t_allocs_tracker *allocs)
{
	t_mutex_alloc	*tmp;

	if (!allocs->mutexes_head)
		return ;
	tmp = allocs->mutexes_head;
	allocs->mutexes_end = NULL;
	while (tmp->next)
	{
		tmp = allocs->mutexes_head->next;
		pthread_mutex_destroy(allocs->mutexes_head->mutex_ptr);
		free(allocs->mutexes_head->mutex_ptr);
		allocs->mutexes_head->mutex_ptr = NULL;
		free(allocs->mutexes_head);
		allocs->mutexes_head = tmp;
	}
	pthread_mutex_destroy(allocs->mutexes_head->mutex_ptr);
	free(allocs->mutexes_head->mutex_ptr);
	allocs->mutexes_head->mutex_ptr = NULL;
	free(allocs->mutexes_head);
	tmp = NULL;
}

void	ft_free(t_allocs_tracker *allocs)
{
	t_alloc	*tmp;

	tmp = allocs->allocs_head;
	allocs->allocs_end = NULL;
	while (tmp->next)
	{
		tmp = allocs->allocs_head->next;
		free(allocs->allocs_head->ptr);
		allocs->allocs_head->ptr = NULL;
		free(allocs->allocs_head);
		allocs->allocs_head = tmp;
	}
	free(allocs->allocs_head->ptr);
	allocs->allocs_head->ptr = NULL;
	free(allocs->allocs_head);
	tmp = NULL;
	ft_destroy_mutexes(allocs);
	free(allocs);
	allocs = NULL;
}
