/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibaya <ibaya@student.1337.ma>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/16 04:47:00 by ibaya             #+#    #+#             */
/*   Updated: 2026/09/16 11:48:44 by ibaya            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	get_ordered_dongles(t_coder *coder, t_dongle **d1, t_dongle **d2)
{
	if (coder->left_dongle->id < coder->right_dongle->id)
	{
		*d1 = coder->left_dongle;
		*d2 = coder->right_dongle;
	}
	else
	{
		*d1 = coder->right_dongle;
		*d2 = coder->left_dongle;
	}
}

static int	is_dongle_ready(t_dongle *d, int coder_id, long long now)
{
	if (d->is_held)
		return (0);
	if (now < d->available_at)
		return (0);
	if (d->heap->size <= 0 || d->heap->array[0].coder_id != coder_id)
		return (0);
	return (1);
}

static int	try_acquire_both(t_coder *coder)
{
	t_dongle	*d1;
	t_dongle	*d2;
	long long	now;
	int			acquired;

	get_ordered_dongles(coder, &d1, &d2);
	pthread_mutex_lock(d1->mutex);
	pthread_mutex_lock(d2->mutex);
	now = get_time_in_ms();
	acquired = 0;
	if (is_dongle_ready(d1, coder->id, now)
		&& is_dongle_ready(d2, coder->id, now))
	{
		d1->is_held = 1;
		d2->is_held = 1;
		heap_pop(d1->heap);
		heap_pop(d2->heap);
		acquired = 1;
	}
	pthread_mutex_unlock(d2->mutex);
	pthread_mutex_unlock(d1->mutex);
	return (acquired);
}

int	grab_dongles(t_coder *coder)
{
	if (coder->left_dongle == coder->right_dongle)
		return (ft_usleep(coder->sim->time_to_burnout + 100, coder->sim), 0);
	push_dongle_requests(coder);
	while (!has_sim_stopped(coder->sim))
	{
		if (try_acquire_both(coder))
		{
			pthread_mutex_lock(coder->lock);
			coder->last_compile_start = get_time_in_ms();
			pthread_mutex_unlock(coder->lock);
			print_status(coder, "has taken a dongle");
			print_status(coder, "has taken a dongle");
			return (1);
		}
		wait_for_dongles(coder);
	}
	return (0);
}

void	drop_dongles(t_coder *coder)
{
	long long	cd_end;

	cd_end = get_time_in_ms() + coder->sim->dongle_cooldown;
	release_dongle(coder->sim, coder->left_dongle, cd_end);
	release_dongle(coder->sim, coder->right_dongle, cd_end);
}
