/*
 * This file is a part of Jing.
 *
 * Copyright (c) 2015 The Jing Authors.
 *
 * For the full copyright and license information, please view the LICENSE file
 * that was distributed with this source code.
 */

#include "node.h"

/*
 * Create a `procedure` node.
 */
struct node *
node_proc_new(struct symbol *sym, struct node *args)
{
    struct node_proc *proc = bmalloc(sizeof(*proc));
    proc->type = NODE_PROC;
    proc->args = args;
    proc->sym = sym;
    proc->body = NULL;

    symtab_set_type(sym, TPROC);
    symtab_set_def(sym, (struct node *)proc);

    return (struct node *)proc;
}

/*
 * Assign `procedure` body.
 */
void
node_proc_body(struct node *n, struct node *body)
{
    struct node_proc *proc = (struct node_proc *)n;
    proc->body = (struct node_list *)body;
}

/*
 * Create a reference to a symbol
 */
struct node *
node_symref_new(struct symbol *sym, struct node *args)
{
    struct node_symref *symref;

    assert(args);

    symref = bmalloc(sizeof(*symref));
    symref->type = NODE_SYMREF;
    symref->sym = sym;
    symref->args = (struct node_list *)args;

    return (struct node *)symref;
}

/*
 * Create a new statement list.
 */
struct node *
node_list_new(void)
{
    struct node_list *list = bmalloc(sizeof(*list));
    list->type = NODE_LIST;
    array_init(&(list->ary), node_cmp, node_free);

    return (struct node *)list;
}

/*
 * Add a statement to the list.
 */
void
node_list_add(struct node *n, struct node *el)
{
    struct node_list *list = (struct node_list *)n;

    array_push(&list->ary, el);
}

/*
 * Create a common declaration, one of `action`, `fluent`, `prolog`.
 */
struct node *
node_comdcl_new(enum type stype, struct symbol *sym, uint8_t arity)
{
    struct node_comdcl *dcl = bmalloc(sizeof(*dcl));

    dcl->type = NODE_COMDCL;
    dcl->arity = arity;
    dcl->sym = sym;

    symtab_set_type(sym, stype);
    symtab_set_def(sym, (struct node *)dcl);

    return (struct node *)dcl;
}

/*
 * Create a search node. Contains a list of statements.
 */
struct node *
node_search_new(struct node *body)
{
    struct node_search *search = bmalloc(sizeof(*search));
    search->type = NODE_SEARCH;

    if (body) {
        search->body = (struct node_list *)body;
    }

    return (struct node *)search;
}

/*
 * Create an `if` node.
 */
struct node *
node_if_new(struct node *cond, struct node *then, struct node *elseif_list,
        struct node *alt)
{
    struct node_if *nif = bmalloc(sizeof(*nif));
    nif->type = NODE_IF;
    nif->cond = cond;
    nif->then = (struct node_list *)then;
    nif->elseif_list = (struct node_list *)elseif_list;
    nif->alt = (struct node_list *)alt;

    return (struct node *)nif;
}

/*
 * Create an `interrupt` node.
 */
struct node *
node_interrupt_new(struct node *cond, struct node *body)
{
    struct node_cond_block *interrupt = node_cond_block_new(cond, body);
    interrupt->type = NODE_INTERRUPT;

    return (struct node *)interrupt;
}

/*
 * Create a `while` node.
 */
struct node *
node_while_new(struct node *cond, struct node *body)
{
    struct node_cond_block *nwhile = node_cond_block_new(cond, body);
    nwhile->type = NODE_WHILE;

    return (struct node *)nwhile;
}

/*
 * Create a node with a condition and a block of code.
 */
struct node_cond_block *
node_cond_block_new(struct node *cond, struct node *body)
{
    struct node_cond_block *cond_block = bmalloc(sizeof(*cond_block));
    cond_block->cond = cond;
    cond_block->body = (struct node_list *)body;

    return cond_block;
}

/*
 * Node value `true`.
 */
struct node *
node_get_true(void)
{
    static struct node ntrue;

    ntrue.type = NODE_VAL;
    ntrue.val.vtype = VAL_BOOL;
    ntrue.val.u.bool_val = 1;

    return &ntrue;
}

/*
 * Node value `false`.
 */
struct node *
node_get_false(void)
{
    static struct node nfalse;

    nfalse.type = NODE_VAL;
    nfalse.val.vtype = VAL_BOOL;
    nfalse.val.u.bool_val = 1;

    return &nfalse;
}

/*
 * Compare node `xa` to `xb`.
 */
int
node_cmp(const void *xa, const void *xb)
{
    /* FIXME: implement comparison */

    (void) xa;
    (void) xb;

    return 0;
}

/*
 * Free node resources.
 */
void
node_free(void *del)
{
    struct node *n = (struct node *)del;

    if (!n) {
        return;
    }

    switch (n->type) {
    case NODE_PROC:
        {
            struct node_proc *proc = (struct node_proc *)n;
            if (proc->body) {
                node_free(proc->body);
            }
        }
        break;
    case NODE_LIST:
        {
            struct node_list *list = (struct node_list *)n;
            array_destroy(&(list->ary));
        }
        break;
    case NODE_SYMREF:
        {
            struct node_symref *ref = (struct node_symref *)n;
            node_free(ref->args);
        }
        break;
    case NODE_COMDCL:
        /* unused */
        break;
    case NODE_VAL:
        {
            /* bool value is static */
            if (VAL_BOOL == n->val.vtype) {
                return;
            }
        }
        break;
    case NODE_SEARCH:
        {
            struct node_search *search = (struct node_search *)n;
            if (search->body) {
                node_free(search->body);
            }
        }
        break;
    case NODE_IF:
        {
            struct node_if *nif = (struct node_if *)n;
            node_free(nif->cond);
            node_free(nif->then);
            node_free(nif->elseif_list);
            node_free(nif->alt);
        }
        break;
    case NODE_INTERRUPT:
    case NODE_WHILE:
        {
            struct node_cond_block *cond_block = (struct node_cond_block *)n;
            node_free(cond_block->cond);
            node_free(cond_block->body);
        }
        break;
    case NODE_NIL:
    default:
        fprintf(stderr, "node_free: type error\n");
    }

    free(n);
}
