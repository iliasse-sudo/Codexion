/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibaya <ibaya@student.1337.ma>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/15 22:46:59 by ibaya             #+#    #+#             */
/*   Updated: 2026/09/16 04:38:02 by ibaya            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long	get_time_in_ms(void)
{
	struct timeval	tv;
	long long		sec;
	long long		micro;

	if (gettimeofday(&tv, NULL))
		return (1);
	sec = (long long)tv.tv_sec * 1000;
	micro = (long long)tv.tv_usec / 1000;
	return (sec + micro);
}

int	has_sim_stopped(t_sim *sim)
{
	int	stopped;

	if (!sim)
		return (0);
	pthread_mutex_lock(sim->state_mutex);
	stopped = sim->sim_stop;
	pthread_mutex_unlock(sim->state_mutex);
	return (stopped);
}

void	ft_usleep(long long duration_ms, t_sim *sim)
{
	long long	start;

	start = get_time_in_ms();
	while ((get_time_in_ms() - start) < duration_ms)
	{
		if (has_sim_stopped(sim))
			break ;
		usleep(500);
	}
}
