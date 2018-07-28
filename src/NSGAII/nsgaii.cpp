#include "stdafx.h"
#include "global.h"
#include "NSGAII/nsgaii.h"
#include "NSGAII/rand.h"
#include "progresschecker.h"
#include "optimizationalgorithm.h"
#include "spatial/geometryfactory.h"
#include "spatial/envelope.h"
#include "spatial/geometry.h"

#include <cstdio>

NSGAIIAlgorithm::NSGAIIAlgorithm(QObject *parent)
  : OptimizationAlgorithm(parent),
    m_NSGAIIProject(nullptr),
    parent_pop(nullptr),
    child_pop(nullptr),
    mixed_pop(nullptr),
    m_isDone(false)
{

}

NSGAIIAlgorithm::~NSGAIIAlgorithm()
{
  if(parent_pop)
  {
    deallocate_memory_pop(m_NSGAIIProject, parent_pop, m_NSGAIIProject->popsize);
    deallocate_memory_pop(m_NSGAIIProject, child_pop, m_NSGAIIProject->popsize);
    deallocate_memory_pop(m_NSGAIIProject, mixed_pop, 2 * m_NSGAIIProject->popsize);

    free(parent_pop); parent_pop = NULL;
    free(child_pop); child_pop = NULL;
    free(mixed_pop); mixed_pop = NULL;
  }

  deleteProject();
}

int NSGAIIAlgorithm::numVariables() const
{
  return nreal;
}

QString NSGAIIAlgorithm::variableName(int index) const
{
  return m_variableNames[index];
}

int NSGAIIAlgorithm::getVariableIndex(const QString &variableName) const
{
  for(size_t i = 0; i < m_variableNames.size(); i++)
  {
    QString variable = m_variableNames[i];

    if(!variable.compare(variableName, Qt::CaseSensitive))
    {
      return i;
    }
  }

  return -1;
}

QList<QSharedPointer<HCGeometry> > NSGAIIAlgorithm::variableGeometries(int index)
{
  QString variableId = m_variableNames[index];
  return m_variableGeometries[variableId.toStdString()];
}

int NSGAIIAlgorithm::numObjectives() const
{
  return nobj;
}

QString NSGAIIAlgorithm::objectiveName(int index) const
{
  return m_objectiveNames[index];
}

int NSGAIIAlgorithm::getObjectiveIndex(const QString &objectiveName) const
{
  for(size_t i = 0; i < m_objectiveNames.size(); i++)
  {
    QString objective = m_objectiveNames[i];

    if(!objective.compare(objectiveName, Qt::CaseSensitive))
    {
      return i;
    }
  }

  return -1;
}

QList<QSharedPointer<HCGeometry> > NSGAIIAlgorithm::objectiveGeometries(int index)
{
  QString objectiveId = m_objectiveNames[index];
  return m_objectiveGeometries[objectiveId.toStdString()];
}

int NSGAIIAlgorithm::numConstraints() const
{
  return ncon;
}

QString NSGAIIAlgorithm::constraintName(int index) const
{
  return m_constraintNames[index];
}

int NSGAIIAlgorithm::getConstraintIndex(const QString &constraintName) const
{
  for(size_t i = 0; i < m_constraintNames.size(); i++)
  {
    QString constraint = m_constraintNames[i];

    if(!constraint.compare(constraintName, Qt::CaseSensitive))
    {
      return i;
    }
  }

  return -1;
}

QList<QSharedPointer<HCGeometry>> NSGAIIAlgorithm::constraintGeometries(int index)
{
  QString constraintId = m_constraintNames[index];
  return m_constraintGeometries[constraintId.toStdString()];
}

bool NSGAIIAlgorithm::isDone() const
{
  return m_isDone;
}

void NSGAIIAlgorithm::prepareForInputFileRead()
{
  nreal = 0;
  nbin = 0;
  nobj = 0;
  ncon = 0;
  popsize = 0;
  ngen = 0;
  nbinmut = 0;
  nrealmut = 0;
  nbincross = 0;
  nrealcross = 0;
  bitlength = 0;
  max_nbits = 0;

  if(parent_pop)
  {
    deallocate_memory_pop(m_NSGAIIProject, parent_pop, m_NSGAIIProject->popsize);
    deallocate_memory_pop(m_NSGAIIProject, child_pop, m_NSGAIIProject->popsize);
    deallocate_memory_pop(m_NSGAIIProject, mixed_pop, 2 * m_NSGAIIProject->popsize);

    free(parent_pop); parent_pop = NULL;
    free(child_pop); child_pop = NULL;
    free(mixed_pop); mixed_pop = NULL;
  }

  m_inputFileInitialPop = "";
  m_inputFileFinalPop = "";
  m_inputFileFinalFeasiblePop = "";
  m_inputFileAllPop = "";
  m_inputFileModelParams = "";

  m_variableNames.clear();
  m_objectiveNames.clear();
  m_constraintNames.clear();

  min_realvar.clear();
  max_realvar.clear();

  //delete geometries

  m_delimiters = QRegExp("(\\,|\\t|\\;|\\s+)");
}

bool NSGAIIAlgorithm::processInputFileLine(const QString &currentFlag, const QString &line, QString &error)
{
  error = "";
  std::string flag = currentFlag.toStdString();
  auto currentInputFlagIter = m_inputFileFlags.find(flag);
  int currentInputFlag = currentInputFlagIter->second;

  switch (currentInputFlag)
  {
    case 1:
      {
        QStringList cols = line.split(m_delimiters,QString::SkipEmptyParts);

        if(cols.size() == 2)
        {

          std::string optionFlagS =  cols[0].toStdString();
          auto it = m_optionsFlags.find(optionFlagS);
          int optionFlag = it->second;

          bool valueOk;
          double value = cols[1].toDouble(&valueOk);

          if(valueOk)
          {
            switch (optionFlag)
            {
              case 1:
                {
                  seed = value;
                }
                break;
              case 2:
                {
                  popsize = value;
                }
                break;
              case 3:
                {
                  ngen = value;
                }
                break;
              case 4:
                {
                  nobj = value;
                }
                break;
              case 5:
                {
                  ncon = value;
                }
                break;
              case 6:
                {
                  nreal = value;
                }
                break;
              case 7:
                {
                  pmut_real = value;
                }
                break;
              case 8:
                {
                  pcross_real = value;
                }
                break;
              case 9:
                {
                  eta_m = value;
                }
                break;
              case 10:
                {
                  eta_c = value;
                }
                break;
            }
          }
          else
          {
            error = "Expected number for second argument";
            return false;
          }
        }
        else
        {
          error = "Two arguments expected";
          return false;
        }

      }
      break;
    case 2:
      {
        if(!readOptArg(line, 1, error))
        {
          return false;
        }
      }
      break;
    case 3:
      {
        if(!readOptArg(line, 2, error))
        {
          return false;
        }
      }
      break;
    case 4:
      {
        if(!readOptArg(line, 3, error))
        {
          return false;
        }
      }
      break;
    case 5:
      {
        if(!readOptArgGeometry(line, 1, error))
        {
          return false;
        }
      }
      break;
    case 6:
      {
        if(!readOptArgGeometry(line, 2, error))
        {
          return false;
        }
      }
      break;
    case 7:
      {
        if(!readOptArgGeometry(line, 2, error))
        {
          return false;
        }
      }
      break;
    case 8:
      {
        QStringList cols = line.split(m_delimiters,QString::SkipEmptyParts);

        if(cols.size() == 2)
        {
          std::string fileFlagS =  cols[0].toStdString();
          auto it = m_outputFileFlags.find(fileFlagS);
          int fileFlag = it->second;

          switch (fileFlag)
          {
            case 1:
              m_inputFileInitialPop = cols[1];
              break;
            case 2:
              m_inputFileFinalPop = cols[1];
              break;
            case 3:
              m_inputFileFinalFeasiblePop = cols[1];
              break;
            case 4:
              m_inputFileAllPop = cols[1];
              break;
            case 5:
              m_inputFileModelParams = cols[1];
              break;
          }
        }
        else
        {
          error = "Expected two arguments";
          return false;
        }

      }
      break;
  }

  return true;
}

bool NSGAIIAlgorithm::readOptArg(const QString &line, int type, QString &error)
{
  QStringList cols = line.split(m_delimiters,QString::SkipEmptyParts);

  switch (type)
  {
    case 1:
      {
        if(cols.size() == 3)
        {
          bool minOk, maxOk;
          QString variable = cols[0];
          double minV = cols[1].toDouble(&minOk);
          double maxV = cols[2].toDouble(&maxOk);

          if(minOk && maxOk)
          {
            m_variableNames.push_back(variable);
            min_realvar.push_back(minV);
            max_realvar.push_back(maxV);
          }
          else
          {
            error = "Expected numerica values";
            return false;
          }
        }
        else
        {
          error = "Expected 3 arguments";
          return false;
        }
      }
      break;
    case 2:
      {
        if(cols.size() == 1)
        {
          m_objectiveNames.push_back(cols[0]);
        }
        else
        {
          error = "Expected 1 argument";
          return false;
        }

      }
      break;
    case 3:
      {
        if(cols.size() == 1)
        {
          m_constraintNames.push_back(cols[0]);
        }
        else
        {
          error = "Expected 1 argument";
          return false;
        }

      }
      break;
  }

  return true;
}

bool NSGAIIAlgorithm::readOptArgGeometry(const QString &line, int type, QString &error)
{
  QStringList cols = line.split(m_delimiters,QString::SkipEmptyParts);

  if(cols.size() == 3)
  {
    QString name = cols[0];
    QString gtype = cols[1];
    QString gsource = cols[2];
    QList<HCGeometry*> geometries;

    if(!gtype.compare("SHAPEFILE", Qt::CaseInsensitive))
    {
      QFileInfo path = getAbsoluteFilePath(gsource);
      Envelope envp;

      if(!GeometryFactory::readGeometryFromFile(path.absoluteFilePath(), geometries, envp, error))
      {
        return false;
      }
    }
    else if (!gtype.compare("WKT", Qt::CaseInsensitive))
    {
      HCGeometry *geometry = GeometryFactory::importFromWkt(gsource);

      if(!geometry)
      {
        return false;
      }

      geometries.push_back(geometry);
    }

    QList<QSharedPointer<HCGeometry>> sharedGeoms;

    for(HCGeometry *geometry : geometries)
    {
      sharedGeoms.push_back(QSharedPointer<HCGeometry>(geometry));
    }

    switch (type)
    {
      case 1:
        {
          if(std::find(m_variableNames.begin() , m_variableNames.end(), name ) != m_variableNames.end())
          {
            m_variableGeometries[name.toStdString()] = sharedGeoms;
          }
        }
        break;
      case 2:
        {
          if(std::find(m_objectiveNames.begin() , m_objectiveNames.end(), name ) != m_objectiveNames.end())
          {
            m_objectiveGeometries[name.toStdString()] = sharedGeoms;
          }
        }
        break;
      case 3:
        {
          if(std::find(m_constraintNames.begin() , m_constraintNames.end(), name ) != m_constraintNames.end())
          {
            m_constraintGeometries[name.toStdString()] = sharedGeoms;
          }
        }
        break;
    }
  }
  else
  {
    error = "Expected 3 arguments";
    return false;
  }



  return true;
}

bool NSGAIIAlgorithm::initialize(QStringList &errors)
{
  deleteProject();

  allocate_memory_project(&m_NSGAIIProject);

  //testing remove
//  return true;

  if (seed <= 0.0 || seed >= 1.0)
  {
    errors.push_back("Entered seed value is wrong, seed value must be in (0,1)");
    return false;
  }

  if (popsize < 4 || (popsize % 4) != 0)
  {
    errors.push_back("population size read is : " + QString::number(popsize));
    errors.push_back("Wrong population size (needs to be a multiple of 4 entered), hence exiting");
    return false;
  }

  if (ngen < 1)
  {
    errors.push_back("number of generations read is : " + QString::number(ngen));
    errors.push_back("Wrong nuber of generations entered, hence exiting");
    return false;
  }

  if (nobj < 1)
  {
    errors.push_back("number of objectives entered is : " + QString::number(nobj));
    errors.push_back("Wrong number of objectives entered, hence exiting");
    return false;
  }
  else
  {
    if((int)m_objectiveNames.size() != nobj)
    {
      errors.push_back("Number of objective names does not match number of objectives specified");
      return false;
    }
  }

  if (ncon < 0)
  {
    errors.push_back("number of constraints entered is : " + QString::number(ncon));
    errors.push_back("Wrong number of constraints enetered, hence exiting");
    return false;
  }
  else
  {
    if((int)m_constraintNames.size() != ncon)
    {
      errors.push_back("Number of constraint names does not match number of constraint specified");
      return false;
    }
  }

  if (nreal <= 0)
  {
    errors.push_back("number of real variables entered is : " + QString::number(nreal));
    errors.push_back("Wrong number of variables entered, hence exiting");
    return false;
  }
  else
  {
    if((int)m_variableNames.size() != nreal)
    {
      errors.push_back("Number of variable names does not match number of variables specified");
      return false;
    }

    if(nreal != (int)min_realvar.size() || nreal != (int)max_realvar.size())
    {
      errors.push_back("The sizes of the min and max variables should be equal to the number of variables specified");
      return false;
    }

    if (pcross_real < 0.0 || pcross_real > 1.0)
    {
      errors.push_back("Probability of crossover entered is : " + QString::number(pcross_real));
      errors.push_back("Entered value of probability of crossover of real variables is out of bounds, hence exiting");
    }

    if (pmut_real < 0.0 || pmut_real > 1.0)
    {
      errors.push_back("Probability of mutation entered is : " + QString::number(pmut_real));
      errors.push_back("Entered value of probability of mutation of real variables is out of bounds, hence exiting");
      return false;
    }

    if (eta_c <= 0.0)
    {
      errors.push_back("The value entered is : " + QString::number(eta_c));
      errors.push_back("Wrong value of distribution index for crossover entered, hence exiting");
      return false;
    }

    if (eta_m <= 0.0)
    {
      errors.push_back("The value entered is : " + QString::number(eta_m));
      errors.push_back("Wrong value of distribution index for mutation entered, hence exiting");
      return false;
    }
  }

  m_NSGAIIProject->popsize = popsize;
  m_NSGAIIProject->ngen = ngen;
  m_NSGAIIProject->nobj = nobj;
  m_NSGAIIProject->ncon = ncon;
  m_NSGAIIProject->nreal = nreal;
  m_NSGAIIProject->min_realvar = (double *)malloc(nreal * sizeof(double));
  m_NSGAIIProject->max_realvar = (double *)malloc(nreal * sizeof(double));

  for (int i = 0; i < nreal; i++)
  {
    m_NSGAIIProject->min_realvar[i] = min_realvar[i];
    m_NSGAIIProject->max_realvar[i] = max_realvar[i];
  }

  m_NSGAIIProject->pcross_real = pcross_real;
  m_NSGAIIProject->pmut_real = pmut_real;
  m_NSGAIIProject->eta_c = eta_c;
  m_NSGAIIProject->eta_m = eta_m;

  progress()->reset(0.0, ngen);

  parent_pop = (population*) malloc(sizeof(population));
  child_pop  = (population*) malloc(sizeof(population));
  mixed_pop  = (population*) malloc(sizeof(population));

  parent_pop->generation = m_NSGAIIProject->currentGen;
  child_pop->generation = m_NSGAIIProject->currentGen;
  mixed_pop->generation = m_NSGAIIProject->currentGen;

  allocate_memory_pop(m_NSGAIIProject, parent_pop, m_NSGAIIProject->popsize);
  allocate_memory_pop(m_NSGAIIProject, child_pop, m_NSGAIIProject->popsize);
  allocate_memory_pop(m_NSGAIIProject, mixed_pop, 2 * m_NSGAIIProject->popsize);

  randomize();

  m_isDone = false;

  initialize_pop(m_NSGAIIProject, parent_pop);

  decode_pop(m_NSGAIIProject, parent_pop);

  setCurrentPopulation(parent_pop);

  return initializeOutputFiles(errors);

}

bool NSGAIIAlgorithm::prepareForEvaluation(QStringList &errors)
{
  errors.clear();

  if(m_NSGAIIProject->currentGen < m_NSGAIIProject->ngen)
  {
    if(m_NSGAIIProject->currentGen > 0)
    {
      selection(m_NSGAIIProject, parent_pop, child_pop);
      mutation_pop(m_NSGAIIProject, child_pop);
      decode_pop(m_NSGAIIProject, child_pop);
      setCurrentPopulation(child_pop);
    }

    return true;
  }

  return false;
}

bool NSGAIIAlgorithm::processEvaluationOutput(QStringList &errors)
{

  errors.clear();

  if(m_NSGAIIProject->currentGen < m_NSGAIIProject->ngen)
  {
    if(m_NSGAIIProject->currentGen == 0)
    {
      readObjectives(parent_pop);

      assign_rank_and_crowding_distance(m_NSGAIIProject, parent_pop);

      if(m_NSGAIIProject->fpt_initialPop)
      {
        report_pop(m_NSGAIIProject, parent_pop, m_NSGAIIProject->fpt_initialPop);
        fflush(m_NSGAIIProject->fpt_initialPop);

      }

      if(m_NSGAIIProject->fpt_allPop)
      {
        report_pop(m_NSGAIIProject, parent_pop, m_NSGAIIProject->fpt_allPop);
        fflush(m_NSGAIIProject->fpt_allPop);
      }
    }
    else
    {

      readObjectives(child_pop);

      merge(m_NSGAIIProject, parent_pop, child_pop, mixed_pop);

      fill_nondominated_sort(m_NSGAIIProject, mixed_pop, parent_pop);

      if(m_NSGAIIProject->fpt_allPop)
      {
        report_pop(m_NSGAIIProject, parent_pop, m_NSGAIIProject->fpt_allPop);
        fflush(m_NSGAIIProject->fpt_allPop);
      }
    }

    m_NSGAIIProject->currentGen++;
    parent_pop->generation = m_NSGAIIProject->currentGen;
    child_pop->generation = m_NSGAIIProject->currentGen;
    mixed_pop->generation = m_NSGAIIProject->currentGen;

    if(m_NSGAIIProject->currentGen == m_NSGAIIProject->ngen)
    {

      report_pop(m_NSGAIIProject, parent_pop, m_NSGAIIProject->fpt_finalPop);
      report_feasible(m_NSGAIIProject, parent_pop, m_NSGAIIProject->fpt_finalFeasiblePop);

      if (m_NSGAIIProject->nreal > 0)
      {
        fprintf(m_NSGAIIProject->fpt_modelParams, "\n Number of crossover of real variable = %d", m_NSGAIIProject->nrealcross);
        fprintf(m_NSGAIIProject->fpt_modelParams, "\n Number of mutation of real variable = %d", m_NSGAIIProject->nrealmut);
      }

      if (m_NSGAIIProject->nbin > 0)
      {
        fprintf(m_NSGAIIProject->fpt_modelParams, "\n Number of crossover of binary variable = %d", m_NSGAIIProject->nbincross);
        fprintf(m_NSGAIIProject->fpt_modelParams, "\n Number of mutation of binary variable = %d", m_NSGAIIProject->nbinmut);
      }

      if(m_NSGAIIProject->fpt_allPop)
      fflush(m_NSGAIIProject->fpt_allPop);

      if(m_NSGAIIProject->fpt_finalPop)
      fflush(m_NSGAIIProject->fpt_finalPop);

      if(m_NSGAIIProject->fpt_finalFeasiblePop)
      fflush(m_NSGAIIProject->fpt_finalFeasiblePop);

      if(m_NSGAIIProject->fpt_modelParams)
      fflush(m_NSGAIIProject->fpt_modelParams);

      m_isDone = true;
    }

    progress()->performStep(m_NSGAIIProject->currentGen);

    return true;
  }

  return false;
}

bool NSGAIIAlgorithm::finalize(QStringList &errors)
{
  errors.clear();

  if(parent_pop)
  {
    if(m_NSGAIIProject->fpt_finalPop)
      report_pop(m_NSGAIIProject, parent_pop, m_NSGAIIProject->fpt_finalPop);

    if(m_NSGAIIProject->fpt_finalPop)
      report_feasible(m_NSGAIIProject, parent_pop, m_NSGAIIProject->fpt_finalFeasiblePop);

    if(m_NSGAIIProject->fpt_modelParams)
    {
      if (m_NSGAIIProject->nreal > 0)
      {
        fprintf(m_NSGAIIProject->fpt_modelParams, "\n Number of crossover of real variable = %d", m_NSGAIIProject->nrealcross);
        fprintf(m_NSGAIIProject->fpt_modelParams, "\n Number of mutation of real variable = %d", m_NSGAIIProject->nrealmut);
      }

      if (m_NSGAIIProject->nbin > 0 )
      {
        fprintf(m_NSGAIIProject->fpt_modelParams, "\n Number of crossover of binary variable = %d", m_NSGAIIProject->nbincross);
        fprintf(m_NSGAIIProject->fpt_modelParams, "\n Number of mutation of binary variable = %d", m_NSGAIIProject->nbinmut);
      }
    }

    if (m_NSGAIIProject->nbin > 0)
    {
      for (int i = 0; i < m_NSGAIIProject->nbin; i++)
      {
        m_NSGAIIProject->bitlength += m_NSGAIIProject->nbits[i];
      }
    }

    deallocate_memory_pop(m_NSGAIIProject, parent_pop, m_NSGAIIProject->popsize);
    deallocate_memory_pop(m_NSGAIIProject, child_pop, m_NSGAIIProject->popsize);
    deallocate_memory_pop(m_NSGAIIProject, mixed_pop, 2 * m_NSGAIIProject->popsize);

    free(parent_pop); parent_pop = NULL;
    free(child_pop); child_pop = NULL;
    free(mixed_pop); mixed_pop = NULL;
  }

  m_isDone = false;

  deleteProject();

  return true;

}

bool NSGAIIAlgorithm::initializeOutputFiles(QStringList &errors)
{
  if(m_NSGAIIProject)
  {
    if(!m_inputFileInitialPop.isEmpty())
    {
      QFileInfo inputFile1(m_inputFileInitialPop);

      if(inputFile1.isRelative())
        inputFile1 = getAbsoluteFilePath(m_inputFileInitialPop);

      if(inputFile1.absoluteDir().exists() )
      {
        if(inputFile1.isDir())
          return true;

        m_NSGAIIProject->fpt_initialPop = fopen(inputFile1.absoluteFilePath().toStdString().c_str(),"w");
        fprintf(m_NSGAIIProject->fpt_initialPop, "# This file contains the data of initial population\n");
        fprintf(m_NSGAIIProject->fpt_initialPop, "# of objectives = %d, # of constraints = %d, # of real_var = %d, # of bits of bin_var = %d, constr_violation, rank, crowding_distance, generation\n",
                m_NSGAIIProject->nobj, m_NSGAIIProject->ncon, m_NSGAIIProject->nreal, m_NSGAIIProject->bitlength);

        printCustomHeader(m_NSGAIIProject->fpt_initialPop);
        fflush(m_NSGAIIProject->fpt_initialPop);
      }
      else
      {
        errors.push_back("Initial population output file path error");
        return false;
      }
    }

    if(!m_inputFileFinalPop.isEmpty())
    {
      QFileInfo inputFile2(m_inputFileFinalPop);

      if(inputFile2.isRelative())
        inputFile2 = getAbsoluteFilePath(m_inputFileFinalPop);

      if(inputFile2.absoluteDir().exists())
      {
        if(inputFile2.isDir())
          return true;

        m_NSGAIIProject->fpt_finalPop = fopen(inputFile2.absoluteFilePath().toStdString().c_str(),"w");
        fprintf(m_NSGAIIProject->fpt_finalPop, "# This file contains the data of final population\n");
        fprintf(m_NSGAIIProject->fpt_finalPop, "# of objectives = %d, # of constraints = %d, # of real_var = %d, # of bits of bin_var = %d, constr_violation, rank, crowding_distance, generation\n",
                m_NSGAIIProject->nobj, m_NSGAIIProject->ncon, m_NSGAIIProject->nreal, m_NSGAIIProject->bitlength);

        printCustomHeader(m_NSGAIIProject->fpt_finalPop);
        fflush(m_NSGAIIProject->fpt_finalPop);
      }
      else
      {
        errors.push_back("Final population output file path error");
        return false;
      }
    }

    if(!m_inputFileFinalFeasiblePop.isEmpty())
    {
      QFileInfo inputFile3(m_inputFileFinalFeasiblePop);

      if(inputFile3.isRelative())
        inputFile3 = getAbsoluteFilePath(m_inputFileFinalFeasiblePop);

      if(inputFile3.absoluteDir().exists())
      {
        if(inputFile3.isDir())
          return true;

        m_NSGAIIProject->fpt_finalFeasiblePop = fopen(inputFile3.absoluteFilePath().toStdString().c_str(),"w");
        fprintf(m_NSGAIIProject->fpt_finalFeasiblePop, "# This file contains the data of final feasible population (if found)\n");
        fprintf(m_NSGAIIProject->fpt_finalFeasiblePop, "# of objectives = %d, # of constraints = %d, # of real_var = %d, # of bits of bin_var = %d, constr_violation, rank, crowding_distance, generation\n",
                m_NSGAIIProject->nobj, m_NSGAIIProject->ncon, m_NSGAIIProject->nreal, m_NSGAIIProject->bitlength);
        printCustomHeader(m_NSGAIIProject->fpt_finalFeasiblePop);
        fflush(m_NSGAIIProject->fpt_finalFeasiblePop);
      }
      else
      {
        errors.push_back("Best population output file path error");
        return false;
      }
    }
    else
    {
      errors.push_back("Best population output file must be specified");
      return false;
    }

    if(!m_inputFileAllPop.isEmpty())
    {
      QFileInfo inputFile4(m_inputFileAllPop);

      if(inputFile4.isRelative())
        inputFile4 = getAbsoluteFilePath(m_inputFileAllPop);

      if(inputFile4.absoluteDir().exists())
      {
        if(inputFile4.isDir())
          return true;

        m_NSGAIIProject->fpt_allPop = fopen(inputFile4.absoluteFilePath().toStdString().c_str(),"w");
        fprintf(m_NSGAIIProject->fpt_allPop, "# This file contains the data of all generations\n");
        fprintf(m_NSGAIIProject->fpt_allPop, "# of objectives = %d, # of constraints = %d, # of real_var = %d, # of bits of bin_var = %d, constr_violation, rank, crowding_distance, generation\n",
                m_NSGAIIProject->nobj, m_NSGAIIProject->ncon, m_NSGAIIProject->nreal, m_NSGAIIProject->bitlength);
        printCustomHeader(m_NSGAIIProject->fpt_allPop);
        fflush(m_NSGAIIProject->fpt_allPop);
      }
      else
      {
        errors.push_back("All individuals output file path error");
        return false;
      }
    }

    if(!m_inputFileModelParams.isEmpty())
    {
      QFileInfo inputFile5(m_inputFileModelParams);

      if(inputFile5.isRelative())
        inputFile5 = getAbsoluteFilePath(m_inputFileModelParams);

      if(inputFile5.absoluteDir().exists())
      {
        if(inputFile5.isDir())
          return true;

        m_NSGAIIProject->fpt_modelParams = fopen(inputFile5.absoluteFilePath().toStdString().c_str(),"w");
        fprintf(m_NSGAIIProject->fpt_modelParams, "# This file contains information about inputs as read by the program\n");

        printf("\nInput data successfully read, now performing initialization\n");
        fprintf(m_NSGAIIProject->fpt_modelParams, "\n Population size = %d", m_NSGAIIProject->popsize);
        fprintf(m_NSGAIIProject->fpt_modelParams, "\n Number of generations = %d", m_NSGAIIProject->ngen);
        fprintf(m_NSGAIIProject->fpt_modelParams, "\n Number of objective functions = %d", m_NSGAIIProject->nobj);
        fprintf(m_NSGAIIProject->fpt_modelParams, "\n Number of constraints = %d", m_NSGAIIProject->ncon);
        fprintf(m_NSGAIIProject->fpt_modelParams, "\n Number of real variables = %d", m_NSGAIIProject->nreal);

        if (m_NSGAIIProject->nreal > 0)
        {
          for (int i = 0; i < m_NSGAIIProject->nreal; i++)
          {
            fprintf(m_NSGAIIProject->fpt_modelParams, "\n Lower limit of real variable %d = %e", i + 1, m_NSGAIIProject->min_realvar[i]);
            fprintf(m_NSGAIIProject->fpt_modelParams, "\n Upper limit of real variable %d = %e", i + 1, m_NSGAIIProject->max_realvar[i]);
          }

          fprintf(m_NSGAIIProject->fpt_modelParams, "\n Probability of crossover of real variable = %e", m_NSGAIIProject->pcross_real);
          fprintf(m_NSGAIIProject->fpt_modelParams, "\n Probability of mutation of real variable = %e", m_NSGAIIProject->pmut_real);
          fprintf(m_NSGAIIProject->fpt_modelParams, "\n Distribution index for crossover = %e", m_NSGAIIProject->eta_c);
          fprintf(m_NSGAIIProject->fpt_modelParams, "\n Distribution index for mutation = %e", m_NSGAIIProject->eta_m);
        }

        fprintf(m_NSGAIIProject->fpt_modelParams, "\n Number of binary variables = %d", m_NSGAIIProject->nbin);

        if (m_NSGAIIProject->nbin > 0)
        {
          for (int i = 0; i < m_NSGAIIProject->nbin; i++)
          {
            fprintf(m_NSGAIIProject->fpt_modelParams, "\n Number of bits for binary variable %d = %d", i + 1, m_NSGAIIProject->nbits[i]);
            fprintf(m_NSGAIIProject->fpt_modelParams, "\n Lower limit of binary variable %d = %e", i + 1, m_NSGAIIProject->min_binvar[i]);
            fprintf(m_NSGAIIProject->fpt_modelParams, "\n Upper limit of binary variable %d = %e", i + 1, m_NSGAIIProject->max_binvar[i]);
          }

          fprintf(m_NSGAIIProject->fpt_modelParams, "\n Probability of crossover of binary variable = %e", m_NSGAIIProject->pcross_bin);
          fprintf(m_NSGAIIProject->fpt_modelParams, "\n Probability of mutation of binary variable = %e", m_NSGAIIProject->pmut_bin);
        }

        fprintf(m_NSGAIIProject->fpt_modelParams, "\n Seed for random number generator = %e", seed);

        fflush(m_NSGAIIProject->fpt_modelParams);

      }
      else
      {
        errors.push_back("All individuals output file path error");
        return false;
      }
    }

    return true;
  }

  return false;
}

void NSGAIIAlgorithm::printCustomHeader(FILE *file)
{
//  fprintf(m_NSGAIIProject->fpt_modelParams, "\n Number of mutation of binary variable = %d", m_NSGAIIProject->nbinmut);

  for(int i = 0 ; i < nobj ; i++)
  {
     QString name = m_objectiveNames[i];
     fprintf(file, "Objective_%i-%s, ", i+1, name.toStdString().c_str());
  }

  for(int i = 0; i < ncon; i++)
  {
    QString name = m_constraintNames[i];
    fprintf(file, "Constraint_%i-%s, ", i+1, name.toStdString().c_str());
  }

  for(int i = 0; i < nreal; i++)
  {
    QString name = m_variableNames[i];
    fprintf(file, "Variables_%i-%s, ", i+1, name.toStdString().c_str());
  }

  fprintf(file, "constr_violation, rank, crowding_distance, generation, index\n");
}

void NSGAIIAlgorithm::deleteProject()
{
  if(m_NSGAIIProject)
  {
    deallocate_memory_project(m_NSGAIIProject);
    m_NSGAIIProject = NULL;
  }
}

void NSGAIIAlgorithm::setCurrentPopulation(population *currentPop)
{
  if(numIndividuals() != m_NSGAIIProject->popsize)
  {
    initializeIndividuals(m_NSGAIIProject->popsize);
  }

  for(int i = 0 ; i < m_NSGAIIProject->popsize; i++)
  {
    Individual *indA = m_individuals[i];
    individual &indB = currentPop->ind[i];
    indB.index = i;

    double *indAVars = indA->variables();
    double *indAObjs = indA->objectives();
    double *indACons = indA->constraints();

    for(int j = 0; j < m_NSGAIIProject->nreal; j++)
    {
      indAVars[j] = indB.xreal[j];
    }

    for(int j = 0; j < m_NSGAIIProject->nobj; j++)
    {
      indAObjs[j]  = 0.0;
    }

    for(int j = 0; j < m_NSGAIIProject->ncon; j++)
    {
      indACons[j] = 0.0;
    }
  }
}

void NSGAIIAlgorithm::readObjectives(population *currentPop)
{

  for(int i = 0 ; i < m_NSGAIIProject->popsize; i++)
  {
    Individual *indA = m_individuals[i];
    individual &indB = currentPop->ind[i];

    double *indAObjs = indA->objectives();
    double *indACons = indA->constraints();

    for(int j = 0; j < m_NSGAIIProject->nobj; j++)
    {
      double value = indAObjs[j];
      indB.obj[j] = value;
    }

    indB.constr_violation = 0;

    for(int j = 0; j < m_NSGAIIProject->ncon; j++)
    {
      double constr = indACons[j];
      indB.constr[j] = constr;

      if(constr < 0.0)
      {
        indB.constr_violation += constr;
      }
    }
  }
}

const std::unordered_map<std::string, int> NSGAIIAlgorithm::m_inputFileFlags({
                                                                               {"[OPTIONS]", 1},
                                                                               {"[VARIABLES]", 2},
                                                                               {"[OBJECTIVES]", 3},
                                                                               {"[CONSTRAINTS]", 4},
                                                                               {"[VARIABLE_GEOMETRIES]", 5},
                                                                               {"[OBJECTIVE_GEOMETRIES]", 6},
                                                                               {"[CONSTRAINT_GEOMETRIES]", 7},
                                                                               {"[OUTPUT_FILES]", 8},
                                                                             });
const std::unordered_map<std::string, int> NSGAIIAlgorithm::m_optionsFlags({
                                                                             {"RANDOM_SEED", 1},
                                                                             {"POPULATION_SIZE", 2},
                                                                             {"NUM_GENERATIONS", 3},
                                                                             {"NUM_OBJECTIVES", 4},
                                                                             {"NUM_CONSTRAINTS", 5},
                                                                             {"NUM_VARIABLES", 6},
                                                                             {"MUTATION_PROBABILITY", 7},
                                                                             {"CROSSOVER_PROBABILITY", 8},
                                                                             {"MUTATION_DISTRIBUTION_INDEX", 9},
                                                                             {"CROSSOVER_DISTRIBUTION_INDEX", 10},
                                                                           });

const std::unordered_map<std::string, int> NSGAIIAlgorithm::m_outputFileFlags({
                                                                                {"INITIAL_POPULATION", 1},
                                                                                {"FINAL_POPULATION", 2},
                                                                                {"FEASIBLE_POPULATION", 3},
                                                                                {"ALL_GENERATIONS", 4},
                                                                                {"PARAMETERS", 5},
                                                                              });
