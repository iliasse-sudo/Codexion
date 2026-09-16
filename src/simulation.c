/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibaya <ibaya@student.1337.ma>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/16 11:30:00 by ibaya             #+#    #+#             */
/*   Updated: 2026/09/16 11:30:00 by ibaya            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	abort_threads(t_sim *sim, int count)
{
	int	i;

	pthread_mutex_lock(sim->state_mutex);
	sim->sim_stop = 1;
	pthread_mutex_unlock(sim->state_mutex);
	wake_all_coders(sim);
	i = 0;
	while (i < count)
	{
		pthread_join(sim->coders[i]->thread_id, NULL);
		i++;
	}
}

static int	create_coder_threads(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->nb_coders)
	{
		if (pthread_create(&sim->coders[i]->thread_id, NULL,
				coder_routine, sim->coders[i]))
		{
			abort_threads(sim, i);
			return (1);
		}
		i++;
	}
	return (0);
}

static void	join_threads(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->nb_coders)
	{
		pthread_join(sim->coders[i]->thread_id, NULL);
		i++;
	}
	pthread_join(sim->monitor_thread, NULL);
}

int	run_simulation(t_sim *sim)
{
	int	i;

	sim->start_time = get_time_in_ms();
	i = 0;
	while (i < sim->nb_coders)
	{
		sim->coders[i]->last_compile_start = sim->start_time;
		i++;
	}
	if (create_coder_threads(sim))
		return (1);
	if (pthread_create(&sim->monitor_thread, NULL, monitor_routine, sim))
	{
		abort_threads(sim, sim->nb_coders);
		return (1);
	}
	join_threads(sim);
	return (0);
}
