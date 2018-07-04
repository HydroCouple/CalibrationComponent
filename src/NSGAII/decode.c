/* Routines to decode the population */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "global.h"
# include "rand.h"

/* Function to decode a population to find out the binary variable values based on its bit pattern */
void decode_pop (NSGAIIProject *project, population *pop)
{
    int i;
    if (project->nbin!=0)
    {
        for (i=0; i<project->popsize; i++)
        {
            decode_ind (project, &(pop->ind[i]));
        }
    }
    return;
}

/* Function to decode an individual to find out the binary variable values based on its bit pattern */
void decode_ind (NSGAIIProject *project, individual *ind)
{
    int j, k;
    int sum;
    if (project->nbin!=0)
    {
        for (j=0; j<project->nbin; j++)
        {
            sum=0;
            for (k=0; k<project->nbits[j]; k++)
            {
                if (ind->gene[j][k]==1)
                {
                    sum += pow(2,project->nbits[j]-1-k);
                }
            }
            ind->xbin[j] = project->min_binvar[j] + (double)sum*(project->max_binvar[j] - project->min_binvar[j])/(double)(pow(2,project->nbits[j])-1);
        }
    }
    return;
}
