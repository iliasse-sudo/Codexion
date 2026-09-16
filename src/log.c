/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibaya <ibaya@student.1337.ma>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/16 04:45:00 by ibaya             #+#    #+#             */
/*   Updated: 2026/09/16 04:45:00 by ibaya            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	print_status(t_coder *coder, const char *action)
{
	long long	timestamp;

	pthread_mutex_lock(coder->sim->write_mutex);
	pthread_mutex_lock(coder->sim->state_mutex);
	if (!coder->sim->sim_stop)
	{
		timestamp = get_time_in_ms() - coder->sim->start_time;
		printf("%lld %d %s\n", timestamp, coder->id, action);
	}
	pthread_mutex_unlock(coder->sim->state_mutex);
	pthread_mutex_unlock(coder->sim->write_mutex);
}
