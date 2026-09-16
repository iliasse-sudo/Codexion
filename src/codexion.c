/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibaya <ibaya@student.1337.ma>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/10 18:37:28 by ibaya             #+#    #+#             */
/*   Updated: 2026/09/16 00:16:44 by ibaya            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char **argv)
{
	char				*sched;
	t_sim				*sim;
	t_allocs_tracker	*allocs;

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
	ft_free(allocs);
	return (0);
}
