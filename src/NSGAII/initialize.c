/* Data initializtion routines */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "global.h"
# include "rand.h"

/* Function to initialize a population randomly */
void initialize_pop (NSGAIIProject *project, population *pop)
{
    int i;
    for (i=0; i<project->popsize; i++)
    {
        initialize_ind (project, &(pop->ind[i]));
    }
    return;
}

/* Function to initialize an individual randomly */
void initialize_ind (NSGAIIProject *project, individual *ind)
{
    int j, k;

    if (project->nreal > 0)
    {
        for (j=0; j<project->nreal; j++)
        {
            ind->xreal[j] = rndreal (project->min_realvar[j], project->max_realvar[j]);
        }
    }

    if (project->nbin > 0)
    {
        for (j=0; j<project->nbin; j++)
        {
            for (k= 0; k < project->nbits[j]; k++)
            {
                if (randomperc() <= 0.5)
                {
                    ind->gene[j][k] = 0;
                }
                else
                {
                    ind->gene[j][k] = 1;
                }
            }
        }
    }
    return;
}
