/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibaya <ibaya@student.1337.ma>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/16 11:15:00 by ibaya             #+#    #+#             */
/*   Updated: 2026/09/16 11:15:00 by ibaya            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	is_coder_done(t_coder *coder)
{
	int	done;

	done = 0;
	if (coder->sim->nb_compiles_req <= 0)
		return (0);
	pthread_mutex_lock(coder->lock);
	if (coder->compiles_done >= coder->sim->nb_compiles_req)
		done = 1;
	pthread_mutex_unlock(coder->lock);
	return (done);
}

static int	coder_cycle(t_coder *coder)
{
	if (!grab_dongles(coder))
		return (0);
	print_status(coder, "is compiling");
	ft_usleep(coder->sim->time_to_compile, coder->sim);
	drop_dongles(coder);
	pthread_mutex_lock(coder->lock);
	coder->compiles_done++;
	pthread_mutex_unlock(coder->lock);
	if (has_sim_stopped(coder->sim))
		return (0);
	print_status(coder, "is debugging");
	ft_usleep(coder->sim->time_to_debug, coder->sim);
	if (has_sim_stopped(coder->sim))
		return (0);
	print_status(coder, "is refactoring");
	ft_usleep(coder->sim->time_to_refactor, coder->sim);
	return (1);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	if (coder->sim->nb_compiles_req == 0)
		return (NULL);
	if (coder->sim->nb_coders > 1 && coder->id % 2 != 0)
		ft_usleep(coder->sim->time_to_compile / 2, coder->sim);
	while (!has_sim_stopped(coder->sim))
	{
		if (is_coder_done(coder))
			break ;
		if (!coder_cycle(coder))
			break ;
	}
	return (NULL);
}
