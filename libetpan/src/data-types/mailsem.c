/*
 * libEtPan! -- a mail stuff library
 *
 * Copyright (C) 2001, 2005 - DINH Viet Hoa
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the libEtPan! project nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * $Id: mailsem.c,v 1.1.1.1 2005/03/18 20:17:31 zautrix Exp $
 */

#include "mailsem.h"
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

enum {
  SEMKIND_SEMOPEN,
  SEMKIND_SEMINIT,
};

#define SEMNAME_LEN 64

struct mailsem * mailsem_new(void)
{
  struct mailsem * sem;
  int r;
  
  sem = malloc(sizeof(* sem));
  if (sem == NULL)
    goto err;
  
  r = sem_init(sem->sem_sem, 0, 0);
  if (r < 0) {
    char name[SEMNAME_LEN];
    
    free(sem->sem_sem);
    
    snprintf(name, sizeof(name), "sem-%p", sem);
    
#ifndef __CYGWIN__
    sem->sem_sem = sem_open(name, O_CREAT, 0600, 0);
    if (sem->sem_sem == NULL)
      goto err;
    
    sem->sem_kind = SEMKIND_SEMOPEN;
#else
    goto err;
#endif
  }
  else {
    sem->sem_kind = SEMKIND_SEMINIT;
  }
  
  return sem;
  
 err:
  return NULL;
}

void mailsem_free(struct mailsem * sem)
{
  if (sem->sem_kind == SEMKIND_SEMOPEN) {
    char name[SEMNAME_LEN];
    
#ifndef __CYGWIN__
    sem_close((sem_t *) sem->sem_sem);
    snprintf(name, sizeof(name), "sem-%p", sem);
    sem_unlink(name);
#endif
  }
  else {
    sem_destroy((sem_t *) sem->sem_sem);
    free(sem->sem_sem);
  }
  free(sem);
}

int mailsem_up(struct mailsem * sem)
{
  return sem_wait((sem_t *) sem->sem_sem);
}

int mailsem_down(struct mailsem * sem)
{
  return sem_post((sem_t *) sem->sem_sem);
}