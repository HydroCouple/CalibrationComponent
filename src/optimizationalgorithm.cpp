#include "stdafx.h"
#include "optimizationalgorithm.h"
#include "progresschecker.h"
#include "spatial/geometry.h"


using namespace HydroCouple::Spatial;

OptimizationAlgorithm::OptimizationAlgorithm(QObject *parent)
  : QObject(parent)
{
  m_progressChecker = new ProgressChecker(0,100,100,this);
}

OptimizationAlgorithm::~OptimizationAlgorithm()
{
  deleteAllIndividuals();
}

QList<QSharedPointer<HCGeometry>> OptimizationAlgorithm::variableGeometries(int index)
{
  return QList<QSharedPointer<HCGeometry>>();
}

QStringList OptimizationAlgorithm::variableIdentifiers(int index)
{
  return QStringList();
}

QList<QSharedPointer<HCGeometry>> OptimizationAlgorithm::objectiveGeometries(int index)
{
  return QList<QSharedPointer<HCGeometry>>();
}

QStringList OptimizationAlgorithm::objectiveIdentifiers(int index)
{
  return QStringList();
}

QList<QSharedPointer<HCGeometry> > OptimizationAlgorithm::constraintGeometries(int index)
{
  return QList<QSharedPointer<HCGeometry>>();
}

QStringList OptimizationAlgorithm::constraintIdentifiers(int index)
{
  return QStringList();
}

int OptimizationAlgorithm::numIndividuals() const
{
  return (int)m_individuals.size();
}

std::vector<Individual*> OptimizationAlgorithm::individuals() const
{
  return m_individuals;
}

Individual *OptimizationAlgorithm::getIndividual(int index) const
{
  return m_individuals[index];
}

ProgressChecker *OptimizationAlgorithm::progress() const
{
  return m_progressChecker;
}

void OptimizationAlgorithm::setReferenceDir(const QDir &referenceDir)
{
  m_referenceDir = referenceDir;
}

QDir OptimizationAlgorithm::referenceDir() const
{
  return m_referenceDir;
}

QFileInfo OptimizationAlgorithm::getAbsoluteFilePath(const QString &filePath) const
{
  QFileInfo outFile(filePath.trimmed());

  if(outFile.isRelative())
  {
    outFile = m_referenceDir.absoluteFilePath(filePath.trimmed());
  }

  return outFile;
}

QFileInfo OptimizationAlgorithm::getRelativeFilePath(const QString &filePath) const
{
  QFileInfo outFile(filePath.trimmed());

  if(outFile.isAbsolute())
  {
    outFile = m_referenceDir.relativeFilePath(filePath.trimmed());
  }

  return outFile;
}

void OptimizationAlgorithm::initializeIndividuals(int size)
{
  deleteAllIndividuals();

  m_individuals.reserve(size);

  for(int i = 0; i < size; i++)
  {
    Individual *ind = new Individual(this);
    m_individuals.push_back(ind);
  }
}

void OptimizationAlgorithm::deleteAllIndividuals()
{
  for(size_t i = 0; i < m_individuals.size(); i++)
  {
    delete m_individuals[i];
  }

  m_individuals.clear();
}

Individual::Individual(OptimizationAlgorithm *algorithm)
 : QObject(nullptr),
   m_variables(nullptr),
   m_objectives(nullptr),
   m_constraints(nullptr),
   m_algorithm(algorithm)

{
  if(algorithm->numVariables())
    m_variables = new double[algorithm->numVariables()]();

  if(algorithm->numObjectives())
    m_objectives = new double[algorithm->numObjectives()]();

  if(algorithm->numConstraints())
    m_constraints = new double[algorithm->numConstraints()]();
}

Individual::~Individual()
{
  if(m_variables)
    delete[] m_variables;

  if(m_objectives)
    delete[] m_objectives;

  if(m_constraints)
    delete[] m_constraints;
}

double *Individual::variables() const
{
  return m_variables;
}

double *Individual::objectives() const
{
  return m_objectives;
}

double *Individual::constraints() const
{
  return m_constraints;
}
