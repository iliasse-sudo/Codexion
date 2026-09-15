/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   inits.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibaya <ibaya@student.1337.ma>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/14 17:21:15 by ibaya             #+#    #+#             */
/*   Updated: 2026/09/14 17:55:42 by ibaya            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_dongle	**init_dongles(t_sim *sim, t_allocs_tracker *allocs)
{
	t_dongle		**dongles;
	t_heap			*heap;
	pthread_mutex_t	*mtx;
	int				i;

	dongles = ft_malloc(allocs, sizeof(t_dongle *) * sim->nb_coders);
	if (!dongles)
		return (NULL);
	i = 0;
	while (i < sim->nb_coders)
	{
		dongles[i] = ft_malloc(allocs, sizeof(t_dongle));
		heap = ft_malloc(allocs, sizeof(t_heap));
		mtx = create_mutex(allocs);
		if (!dongles[i] || !heap || !mtx)
			return (NULL);
		dongles[i]->id = i;
		dongles[i]->available_at = 0;
		dongles[i]->heap = heap;
		dongles[i]->mutex = *mtx;
		if (init_heap(allocs, heap, sim->heap_cap, sim->scheduler))
			return (NULL);
		i++;
	}
	return (dongles);
}

t_allocs_tracker	*init_alloc_saver(void)
{
	t_allocs_tracker	*allocs;
	t_alloc				*node;

	allocs = malloc(sizeof(*allocs));
	node = malloc(sizeof(*node));
	if (!allocs || !node)
	{
		free(allocs);
		free(node);
		return (NULL);
	}
	memset(allocs, 0, sizeof(*allocs));
	memset(node, 0, sizeof(*node));
	node->ptr = NULL;
	node->next = NULL;
	allocs->allocs_head = node;
	allocs->allocs_end = node;
	allocs->mutexes_head = NULL;
	allocs->mutexes_end = NULL;
	return (allocs);
}

t_coder	**init_coders_structs(t_sim *sim, t_allocs_tracker *allocs)
{
	t_coder	**coders;
	int		i;

	coders = ft_malloc(allocs, sizeof(t_coder *) * sim->nb_coders);
	if (!coders)
		return (NULL);
	i = 0;
	while (i < sim->nb_coders)
	{
		coders[i] = ft_malloc(allocs, sizeof(t_coder));
		if (!coders[i])
			return (NULL);
		coders[i]->id = i + 1;
		coders[i]->compiles_done = 0;
		coders[i]->last_compile_start = 0;
		coders[i]->sim = sim;
		coders[i]->left_dongle = sim->dongles[i];
		coders[i]->right_dongle = sim->dongles[(i + 1) % sim->nb_coders];
		i++;
	}
	return (coders);
}

t_sim	*init_sim(int argc, char **argv, t_allocs_tracker *allocs)
{
	t_sim	*sim;

	sim = ft_malloc(allocs, sizeof(*sim));
	if (!sim)
		return (NULL);
	if (parse_args(argc, argv, sim))
		return (NULL);
	if (pthread_mutex_init(&sim->write_mutex, NULL))
		return (NULL);
	if (pthread_mutex_init(&sim->state_mutex, NULL))
		return (pthread_mutex_destroy(&sim->write_mutex), NULL);
	sim->dongles = init_dongles(sim, allocs);
	if (!sim->dongles)
		return (NULL);
	sim->coders = init_coders_structs(sim, allocs);
	if (!sim->coders)
		return (NULL);
	return (sim);
}
