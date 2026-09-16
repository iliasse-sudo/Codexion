/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibaya <ibaya@student.1337.ma>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 07:11:05 by ibaya             #+#    #+#             */
/*   Updated: 2026/09/14 16:13:01 by ibaya            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <time.h>
# include <unistd.h>
# include <stdio.h>

typedef enum e_scheduler {
	FIFO,
	EDF
}	t_scheduler;

typedef struct s_request {
	int			coder_id;
	long long	arrival_time;
	long long	deadline;
}	t_request;

typedef struct s_heap {
	t_request	*array;
	int			size;
	int			capacity;
	t_scheduler	scheduler;
}	t_heap;

typedef struct s_dongle {
	int				id;
	int				is_held;
	pthread_mutex_t	*mutex;
	long long		available_at;
	t_heap			*heap;
}	t_dongle;

typedef struct s_coder {
	int				id;
	pthread_t		thread_id;
	int				compiles_done;
	long long		last_compile_start;
	pthread_mutex_t	*lock;
	pthread_cond_t	*cond;
	t_dongle		*left_dongle;
	t_dongle		*right_dongle;
	struct s_sim	*sim;
}	t_coder;

typedef struct s_sim {
	int				nb_coders;
	long long		time_to_burnout;
	long long		time_to_compile;
	long long		time_to_debug;
	long long		time_to_refactor;
	int				nb_compiles_req;
	long long		dongle_cooldown;
	int				heap_cap;
	t_scheduler		scheduler;

	long long		start_time;
	int				sim_stop;

	pthread_mutex_t	*write_mutex;
	pthread_mutex_t	*state_mutex;

	t_coder			**coders;
	t_dongle		**dongles;
}	t_sim;

typedef struct s_alloc
{
	void			*ptr;
	struct s_alloc	*next;
}	t_alloc;

typedef struct s_mutex_alloc {
	pthread_mutex_t			*mutex_ptr;
	struct s_mutex_alloc	*next;
}	t_mutex_alloc;

typedef struct s_cond_alloc {
	pthread_cond_t			*cond_ptr;
	struct s_cond_alloc		*next;
}	t_cond_alloc;

typedef struct s_allocs_tracker {
	t_alloc			*allocs_head;
	t_alloc			*allocs_end;
	t_mutex_alloc	*mutexes_head;
	t_mutex_alloc	*mutexes_end;
	t_cond_alloc	*conds_head;
	t_cond_alloc	*conds_end;
}	t_allocs_tracker;

// parsing stuff
long long			ft_atoll(const char *str);
int					is_numeric(const char *str);
int					check_num_args(char **argv);
int					check_scheduler(char *scheduler, t_sim *sim);
int					print_errors(int error_code, int arg);
int					parse_args(int argc, char **argv, t_sim *sim);

// heap stuff
int					init_heap(t_allocs_tracker *allocs, t_heap *heap,
						int capacity, t_scheduler type);
void				free_heap(t_heap *heap);
void				sift_up(t_heap *heap, int index);
void				sift_down(t_heap *heap, int index);
void				heap_push(t_heap *heap, t_request req);
t_request			heap_pop(t_heap *heap);
void				swap_requests(t_request *a, t_request *b);
int					compare_requests(t_request *a, t_request *b,
						t_scheduler type);

// init stuff
void				ft_destroy_mutexes(t_allocs_tracker *allocs);
void				ft_destroy_conds(t_allocs_tracker *allocs);
void				ft_free(t_allocs_tracker *allocs);
t_allocs_tracker	*init_alloc_saver(void);
void				*ft_malloc(t_allocs_tracker *allocs, size_t size);
pthread_mutex_t		*create_mutex(t_allocs_tracker *allocs);
pthread_cond_t		*create_cond(t_allocs_tracker *allocs);
t_dongle			**init_dongles(t_sim *sim, t_allocs_tracker *allocs);
t_coder				**init_coders_structs(t_sim *sim, t_allocs_tracker *allocs);
t_sim				*init_sim(int argc, char **argv, t_allocs_tracker *allocs);

// time stuff
long long			get_time_in_ms(void);
int					has_sim_stopped(t_sim *sim);
void				ft_usleep(long long duration_ms, t_sim *sim);

#endif