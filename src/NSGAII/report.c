/* Routines for storing population data into files */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "global.h"
# include "rand.h"

/* Function to print the information of a population in a file */
void report_pop (NSGAIIProject *project,  population *pop, FILE *fpt)
{
  int i, j, k;

  for (i = 0; i <  project->popsize; i++)
  {
    for (j= 0; j <  project->nobj; j++)
    {
      fprintf(fpt,"%e, ",pop->ind[i].obj[j]);
    }

    if ( project->ncon > 0)
    {
      for (j = 0; j <  project->ncon; j++)
      {
        fprintf(fpt,"%e, ",pop->ind[i].constr[j]);
      }
    }

    if ( project->nreal > 0)
    {
      for (j = 0; j <  project->nreal ; j++)
      {
        fprintf(fpt,"%e, ",pop->ind[i].xreal[j]);
      }
    }

    if ( project->nbin > 0)
    {
      for (j = 0; j <  project->nbin; j++)
      {
        for (k = 0; k <  project->nbits[j]; k++)
        {
          fprintf(fpt,"%d, ",pop->ind[i].gene[j][k]);
        }
      }
    }

    fprintf(fpt,"%e, ",pop->ind[i].constr_violation);
    fprintf(fpt,"%d, ",pop->ind[i].rank);
    fprintf(fpt,"%e, ",pop->ind[i].crowd_dist);
    fprintf(fpt,"%d\n",pop->generation);

  }

  return;
}

/* Function to print the information of feasible and non-dominated population in a file */
void report_feasible (NSGAIIProject *project,  population *pop, FILE *fpt)
{
  int i, j, k;

  for (i = 0; i <  project->popsize; i++)
  {
    if (pop->ind[i].constr_violation == 0.0 && pop->ind[i].rank==1)
    {
      for (j = 0; j <  project->nobj; j++)
      {
        fprintf(fpt,"%e, ",pop->ind[i].obj[j]);
      }
      if ( project->ncon > 0)
      {
        for (j=0; j< project->ncon; j++)
        {
          fprintf(fpt,"%e, ",pop->ind[i].constr[j]);
        }
      }
      if ( project->nreal > 0)
      {
        for (j=0; j< project->nreal; j++)
        {
          fprintf(fpt,"%e, ",pop->ind[i].xreal[j]);
        }
      }
      if ( project->nbin > 0)
      {
        for (j=0; j< project->nbin; j++)
        {
          for (k=0; k< project->nbits[j]; k++)
          {
            fprintf(fpt,"%d, ",pop->ind[i].gene[j][k]);
          }
        }
      }

      fprintf(fpt,"%e, ",pop->ind[i].constr_violation);
      fprintf(fpt,"%d, ",pop->ind[i].rank);
      fprintf(fpt,"%e\n",pop->ind[i].crowd_dist);
    }
  }
  return;
}
