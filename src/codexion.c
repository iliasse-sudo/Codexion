/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibaya <ibaya@student.1337.ma>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/10 18:37:28 by ibaya             #+#    #+#             */
/*   Updated: 2026/09/14 10:14:10 by ibaya            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	ft_free(allocs_tracker_t *allocs)
{
	alloc_t	*tmp;

	tmp = allocs->allocs_head;
	allocs->allocs_end = NULL;
	while (tmp->next)
	{
		tmp = allocs->allocs_head->next;
		free(allocs->allocs_head->ptr);
		allocs->allocs_head->ptr = NULL;
		free(allocs->allocs_head);
		allocs->allocs_head = NULL;
		allocs->allocs_head = tmp;
	}
	free(allocs->allocs_head->ptr);
	allocs->allocs_head->ptr = NULL;
	free(allocs->allocs_head);
	tmp = NULL;
	free(allocs);
	allocs = NULL;
}

allocs_tracker_t	*init_alloc_saver(void)
{
	allocs_tracker_t	*allocs;
	alloc_t				*node;

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

void	*ft_malloc(allocs_tracker_t *allocs, size_t size)
{
	void	*new;
	alloc_t	*new_alloc;

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

pthread_mutex_t	*create_mutex(allocs_tracker_t *allocs)
{
	pthread_mutex_t	*new;
	mutex_alloc_t	*new_mutex;

	new = malloc(sizeof(*new));
	new_mutex = malloc(sizeof(*new_mutex));
	if (!new || !new_mutex)
	{
		free(new);
		free(new_mutex);
		return (NULL);
	}
	if (pthread_mutex_init(new, NULL))
	{
		free(new);
		free(new_mutex);
		return (NULL);
	}
	memset(new_mutex, 0, sizeof(*new_mutex));
	new_mutex->mutex_ptr = new;
	new_mutex->next = NULL;
	if (!allocs->mutexes_head)
	{
		allocs->mutexes_head = new_mutex;
		allocs->mutexes_end = new_mutex;
	}
	else
	{
		allocs->mutexes_end->next = new_mutex;
		allocs->mutexes_end = new_mutex;
	}
	return (new);
}

t_dongle	**init_dongles(t_sim *sim, allocs_tracker_t *allocs)
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
		if (!dongles[i] || !heap)
			return (NULL);
		mtx = create_mutex(allocs);
		if (!mtx)
			return (NULL);
		dongles[i]->id = i;
		dongles[i]->available_at = 0;
		dongles[i]->heap = heap;
		dongles[i]->mutex = *mtx;
		if (init_heap(heap, sim->nb_coders, sim->scheduler))
			return (NULL);
		i++;
	}
	return (dongles);
}

t_coder	**init_coders_structs(t_sim *sim, allocs_tracker_t *allocs)
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

t_sim	*init_sim(int argc, char **argv, allocs_tracker_t *allocs)
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
	{
		pthread_mutex_destroy(&sim->write_mutex);
		return (NULL);
	}
	sim->dongles = init_dongles(sim, allocs);
	if (!sim->dongles)
		return (NULL);
	sim->coders = init_coders_structs(sim, allocs);
	if (!sim->coders)
		return (NULL);
	return (sim);
}

int	main(int argc, char **argv)
{
	char				*sched;
	t_sim				*sim;
	allocs_tracker_t	*allocs;

	allocs = init_alloc_saver();
	if (!allocs)
		return (1);
	sim = init_sim(argc, argv, allocs);
	if (!sim)
	{
		ft_free(allocs);
		return (1);
	}
	printf("Successfully parsed inputs!\n");
	printf("Coders: %d\n", sim->nb_coders);
	printf("Burnout: %lld ms\n", sim->time_to_burnout);
	sched = "fifo";
	if (sim->scheduler == EDF)
		sched = "edf";
	printf("Scheduler: %s\n", sched);
	return (0);
}
