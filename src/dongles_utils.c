/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibaya <ibaya@student.1337.ma>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/16 09:50:00 by ibaya             #+#    #+#             */
/*   Updated: 2026/09/16 09:50:00 by ibaya            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	push_dongle_requests(t_coder *coder)
{
	t_request	req;

	req.coder_id = coder->id;
	req.arrival_time = get_time_in_ms();
	req.deadline = 0;
	if (coder->sim->scheduler == EDF)
	{
		pthread_mutex_lock(coder->lock);
		req.deadline = coder->last_compile_start + coder->sim->time_to_burnout;
		pthread_mutex_unlock(coder->lock);
	}
	pthread_mutex_lock(coder->left_dongle->mutex);
	heap_push(coder->left_dongle->heap, req);
	pthread_mutex_unlock(coder->left_dongle->mutex);
	pthread_mutex_lock(coder->right_dongle->mutex);
	heap_push(coder->right_dongle->heap, req);
	pthread_mutex_unlock(coder->right_dongle->mutex);
}

long long	get_cooldown_wait(t_coder *coder)
{
	long long	now;
	long long	wait_ms;

	now = get_time_in_ms();
	wait_ms = 0;
	pthread_mutex_lock(coder->left_dongle->mutex);
	if (coder->left_dongle->available_at > now)
		wait_ms = coder->left_dongle->available_at - now;
	pthread_mutex_unlock(coder->left_dongle->mutex);
	pthread_mutex_lock(coder->right_dongle->mutex);
	if (coder->right_dongle->available_at - now > wait_ms)
		wait_ms = coder->right_dongle->available_at - now;
	pthread_mutex_unlock(coder->right_dongle->mutex);
	return (wait_ms);
}

void	wait_for_dongles(t_coder *coder)
{
	long long	wait_ms;

	wait_ms = get_cooldown_wait(coder);
	if (wait_ms > 0)
		ft_usleep(wait_ms, coder->sim);
	else
	{
		pthread_mutex_lock(coder->lock);
		pthread_cond_wait(coder->cond, coder->lock);
		pthread_mutex_unlock(coder->lock);
	}
}

void	release_dongle(t_sim *sim, t_dongle *dongle, long long cd_end)
{
	int	next_id;

	next_id = 0;
	pthread_mutex_lock(dongle->mutex);
	dongle->is_held = 0;
	dongle->available_at = cd_end;
	if (dongle->heap->size > 0)
		next_id = dongle->heap->array[0].coder_id;
	pthread_mutex_unlock(dongle->mutex);
	if (next_id > 0 && next_id <= sim->nb_coders)
	{
		pthread_mutex_lock(sim->coders[next_id - 1]->lock);
		pthread_cond_signal(sim->coders[next_id - 1]->cond);
		pthread_mutex_unlock(sim->coders[next_id - 1]->lock);
	}
}
