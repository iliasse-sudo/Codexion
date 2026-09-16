/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibaya <ibaya@student.1337.ma>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/16 11:20:00 by ibaya             #+#    #+#             */
/*   Updated: 2026/09/16 11:20:00 by ibaya            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	wake_all_coders(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->nb_coders)
	{
		pthread_mutex_lock(sim->coders[i]->lock);
		pthread_cond_signal(sim->coders[i]->cond);
		pthread_mutex_unlock(sim->coders[i]->lock);
		i++;
	}
}

static void	announce_burnout(t_sim *sim, int id, long long now)
{
	long long	timestamp;

	timestamp = now - sim->start_time;
	pthread_mutex_lock(sim->write_mutex);
	pthread_mutex_lock(sim->state_mutex);
	sim->sim_stop = 1;
	printf("%lld %d burned out\n", timestamp, id);
	pthread_mutex_unlock(sim->state_mutex);
	pthread_mutex_unlock(sim->write_mutex);
	wake_all_coders(sim);
}

static int	check_burnout(t_sim *sim, int i)
{
	long long	last_start;
	long long	now;

	now = get_time_in_ms();
	pthread_mutex_lock(sim->coders[i]->lock);
	if (sim->nb_compiles_req > 0
		&& sim->coders[i]->compiles_done >= sim->nb_compiles_req)
	{
		pthread_mutex_unlock(sim->coders[i]->lock);
		return (0);
	}
	last_start = sim->coders[i]->last_compile_start;
	pthread_mutex_unlock(sim->coders[i]->lock);
	if (now - last_start > sim->time_to_burnout)
	{
		announce_burnout(sim, sim->coders[i]->id, now);
		return (1);
	}
	return (0);
}

static int	check_all_done(t_sim *sim)
{
	int	i;

	if (sim->nb_compiles_req <= 0)
		return (0);
	i = 0;
	while (i < sim->nb_coders)
	{
		pthread_mutex_lock(sim->coders[i]->lock);
		if (sim->coders[i]->compiles_done < sim->nb_compiles_req)
		{
			pthread_mutex_unlock(sim->coders[i]->lock);
			return (0);
		}
		pthread_mutex_unlock(sim->coders[i]->lock);
		i++;
	}
	pthread_mutex_lock(sim->state_mutex);
	sim->sim_stop = 1;
	pthread_mutex_unlock(sim->state_mutex);
	wake_all_coders(sim);
	return (1);
}

void	*monitor_routine(void *arg)
{
	t_sim	*sim;
	int		i;

	sim = (t_sim *)arg;
	while (!has_sim_stopped(sim))
	{
		if (check_all_done(sim))
			break ;
		i = 0;
		while (i < sim->nb_coders)
		{
			if (check_burnout(sim, i))
				return (NULL);
			i++;
		}
		usleep(500);
	}
	return (NULL);
}
