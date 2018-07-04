/* Mutation routines */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "global.h"
# include "rand.h"

/* Function to perform mutation in a population */
void mutation_pop (NSGAIIProject *project, population *pop)
{
    int i;
    for (i=0; i< project->popsize; i++)
    {
        mutation_ind(project, &(pop->ind[i]));
    }
    return;
}

/* Function to perform mutation of an individual */
void mutation_ind (NSGAIIProject *project, individual *ind)
{
    if ( project->nreal!=0)
    {
        real_mutate_ind(project, ind);
    }
    if ( project->nbin!=0)
    {
        bin_mutate_ind(project, ind);
    }
    return;
}

/* Routine for binary mutation of an individual */
void bin_mutate_ind (NSGAIIProject *project, individual *ind)
{
    int j, k;
    double prob;
    for (j=0; j< project->nbin; j++)
    {
        for (k=0; k< project->nbits[j]; k++)
        {
            prob = randomperc();
            if (prob <= project->pmut_bin)
            {
                if (ind->gene[j][k] == 0)
                {
                    ind->gene[j][k] = 1;
                }
                else
                {
                    ind->gene[j][k] = 0;
                }
                 project->nbinmut+=1;
            }
        }
    }
    return;
}

/* Routine for real polynomial mutation of an individual */
void real_mutate_ind (NSGAIIProject *project, individual *ind)
{
    int j;
    double rnd, delta1, delta2, mut_pow, deltaq;
    double y, yl, yu, val, xy;
    for (j=0; j< project->nreal; j++)
    {
        if (randomperc() <=  project->pmut_real)
        {
            y = ind->xreal[j];
            yl =  project->min_realvar[j];
            yu =  project->max_realvar[j];
            delta1 = (y-yl)/(yu-yl);
            delta2 = (yu-y)/(yu-yl);
            rnd = randomperc();
            mut_pow = 1.0/( project->eta_m+1.0);
            if (rnd <= 0.5)
            {
                xy = 1.0-delta1;
                val = 2.0*rnd+(1.0-2.0*rnd)*(pow(xy,( project->eta_m+1.0)));
                deltaq =  pow(val,mut_pow) - 1.0;
            }
            else
            {
                xy = 1.0-delta2;
                val = 2.0*(1.0-rnd)+2.0*(rnd-0.5)*(pow(xy,( project->eta_m+1.0)));
                deltaq = 1.0 - (pow(val,mut_pow));
            }
            y = y + deltaq*(yu-yl);
            if (y<yl)
                y = yl;
            if (y>yu)
                y = yu;
            ind->xreal[j] = y;
             project->nrealmut+=1;
        }
    }
    return;
}
