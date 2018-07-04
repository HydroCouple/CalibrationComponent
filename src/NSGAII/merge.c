/* Routine for mergeing two populations */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "global.h"
# include "rand.h"

/* Routine to merge two populations into one */
void merge(NSGAIIProject *project,  population *pop1, population *pop2, population *pop3)
{
    int i, k;
    for (i=0; i<project->popsize; i++)
    {
        copy_ind (project, &(pop1->ind[i]), &(pop3->ind[i]));
    }
    for (i=0, k=project->popsize; i<project->popsize; i++, k++)
    {
        copy_ind (project, &(pop2->ind[i]), &(pop3->ind[k]));
    }
    return;
}

/* Routine to copy an individual 'ind1' into another individual 'ind2' */
void copy_ind (NSGAIIProject *project,  individual *ind1, individual *ind2)
{
    int i, j;
    ind2->rank = ind1->rank;
    ind2->constr_violation = ind1->constr_violation;
    ind2->crowd_dist = ind1->crowd_dist;
    if (project->nreal!=0)
    {
        for (i=0; i<project->nreal; i++)
        {
            ind2->xreal[i] = ind1->xreal[i];
        }
    }
    if (project->nbin!=0)
    {
        for (i=0; i<project->nbin; i++)
        {
            ind2->xbin[i] = ind1->xbin[i];
            for (j=0; j<project->nbits[i]; j++)
            {
                ind2->gene[i][j] = ind1->gene[i][j];
            }
        }
    }
    for (i=0; i<project->nobj; i++)
    {
        ind2->obj[i] = ind1->obj[i];
    }
    if (project->ncon!=0)
    {
        for (i=0; i<project->ncon; i++)
        {
            ind2->constr[i] = ind1->constr[i];
        }
    }
    return;
}
