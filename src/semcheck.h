/*
 * This file is a part of Jing.
 *
 * Copyright (c) 2015 The Jing Authors.
 *
 * For the full copyright and license information, please view the LICENSE file
 * that was distributed with this source code.
 */

#ifndef SEMCHECK_H
#define SEMCHECK_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>

#include "jing.h"

void
semcheck_init(FILE *fp);

void
semcheck_walk(struct node *root);

void
semcheck_chk_node(struct node *n);

void
semcheck_chk_proc(struct node_proc *proc);

void
semcheck_chk_list(struct node_list *list);

void
semcheck_chk_symref(struct node_symref *ref);

void
semcheck_chk_search(struct node_search *search);

void
semcheck_chk_if(struct node_if *nif);

#endif /* SEMCHECK_H */
