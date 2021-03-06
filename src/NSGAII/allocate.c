/* Memory allocation and deallocation routines */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "global.h"
# include "rand.h"

void allocate_memory_project(NSGAIIProject **project)
{
    *project = (NSGAIIProject*)malloc(sizeof(NSGAIIProject));
    (*project)->nreal = 0;
    (*project)->nbin = 0;
    (*project)->ncon = 0;
    (*project)->popsize = 0;
    (*project)->ngen = 0;
    (*project)->nbinmut = 0;
    (*project)->nrealmut = 0;
    (*project)->nbincross = 0;
    (*project)->nrealcross = 0;
    (*project)->nbits = NULL;
    (*project)->min_realvar = NULL;
    (*project)->max_realvar = NULL;
    (*project)->min_binvar = NULL;
    (*project)->max_binvar = NULL;
    (*project)->bitlength = 0;
    (*project)->currentGen = 0;
    (*project)->max_nbits = 0;
    (*project)->fpt_initialPop = NULL;
    (*project)->fpt_finalPop = NULL;
    (*project)->fpt_finalFeasiblePop = NULL;
    (*project)->fpt_allPop = NULL;
    (*project)->fpt_modelParams = NULL;
}

void deallocate_memory_project(NSGAIIProject *project)
{
  if(project)
  {
    if (project->nreal > 0)
    {
      free(project->min_realvar); project->min_realvar = NULL;
      free(project->max_realvar); project->max_realvar = NULL;
    }

    if (project->nbin > 0)
    {
      free(project->min_binvar); project->min_binvar = NULL;
      free(project->max_binvar); project->max_binvar = NULL;
      free(project->nbits); project->nbits = NULL;
    }

    if(project->fpt_initialPop)
    {
      fflush(project->fpt_initialPop);
      fclose(project->fpt_initialPop);
    }

    if(project->fpt_finalPop)
    {
      fflush(project->fpt_finalPop);
      fclose(project->fpt_finalPop);
    }

    if(project->fpt_finalFeasiblePop)
    {
      fflush(project->fpt_finalFeasiblePop);
      fclose(project->fpt_finalFeasiblePop);
    }

    if(project->fpt_allPop)
    {
      fflush(project->fpt_allPop);
      fclose(project->fpt_allPop);
    }

    if(project->fpt_modelParams)
    {
      fflush(project->fpt_modelParams);
      fclose(project->fpt_modelParams);
    }

    free(project);
    project = NULL;
  }
}

/* Function to allocate memory to a population */
void allocate_memory_pop (NSGAIIProject *project, population *pop, int size)
{
  int i;
  pop->ind = (individual *)malloc(size*sizeof(individual));

  for (i= 0; i < size; i++)
  {
    allocate_memory_ind (project, &(pop->ind[i]));
  }

  return;
}

/* Function to allocate memory to an individual */
void allocate_memory_ind (NSGAIIProject *project, individual *ind)
{
  int j;

  if (project->nreal > 0)
  {
     ind->xreal = (double *)malloc(project->nreal*sizeof(double));
  }
  else
  {
    ind->xreal = NULL;
  }

  if (project->nbin > 0)
  {
    ind->xbin = (double *)malloc(project->nbin * sizeof(double));
    ind->gene = (int **)malloc(project->nbin * sizeof(int*));

    for (j = 0; j < project->nbin; j++)
    {
      ind->gene[j] = (int *) malloc (project->nbits[j]*sizeof(int));
    }
  }
  else
  {
    ind->xbin = NULL;
    ind->gene = NULL;
  }


  ind->obj = (double *)malloc(project->nobj*sizeof(double));

  if (project->ncon > 0)
  {
    ind->constr = (double *)malloc(project->ncon*sizeof(double));
  }
  else
  {
    ind->constr = NULL;
  }

  return;
}

/* Function to deallocate memory to a population */
void deallocate_memory_pop (NSGAIIProject *project, population *pop, int size)
{
  int i;

  for (i=0; i<size; i++)
  {
    deallocate_memory_ind (project, &(pop->ind[i]));
  }

  free (pop->ind);
  pop->ind = NULL;

  return;
}

/* Function to deallocate memory to an individual */
void deallocate_memory_ind (NSGAIIProject *project, individual *ind)
{
  int j;

  if (project->nreal > 0)
  {
    free(ind->xreal);
    ind->xreal = NULL;
  }

  if (project->nbin > 0)
  {

    for (j=0; j<project->nbin; j++)
    {
      free(ind->gene[j]);
      ind->gene[j] = NULL;
    }

    free(ind->xbin);
    ind->xbin = NULL;

    free(ind->gene);
    ind->gene =NULL;
  }

  free(ind->obj);
  ind->obj = NULL;

  if (project->ncon > 0)
  {
    free(ind->constr);
    ind->constr = NULL;
  }

  return;
}
