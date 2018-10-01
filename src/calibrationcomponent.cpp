
#include "stdafx.h"
#include "calibrationcomponent.h"
#include "optimizationalgorithm.h"
#include "progresschecker.h"
#include "core/valuedefinition.h"
#include "core/dimension.h"
#include "nsgaii.h"
#include "core/idbasedargument.h"
#include "triggercomponentinput.h"
#include "core/abstractoutput.h"
#include "composition/modelcomponent.h"
#include "core/dimension.h"
#include "core/unit.h"
#include "variableoutput.h"
#include "objectiveinput.h"
#include "spatial/geometry.h"

#include <QTextStream>

#ifdef USE_OPENMP
#include <omp.h>
#endif

using namespace HydroCouple;
using namespace Composition;

CalibrationComponent::CalibrationComponent(const QString &id, CalibrationComponentInfo *modelComponentInfo)
  : AbstractModelComponent(id, modelComponentInfo),
    m_individual(0),
    m_parent(nullptr),
    m_calibrationAlgorithm(nullptr),
    m_modelComposition(nullptr)
{
  m_geometryDimension = new Dimension("GeometryDimension",this);
  m_defaultQuantity = new Quantity(QVariant::Double, Unit::unitlessCoefficient(this), this);

  createArguments();
}

CalibrationComponent::~CalibrationComponent()
{
  if(m_parent == nullptr)
  {
    if(m_calibrationAlgorithm)
    {
      delete m_calibrationAlgorithm;
    }

    while (m_clones.size())
    {
      CalibrationComponent *clone = dynamic_cast<CalibrationComponent*>(m_clones.first());
      removeClone(clone);
      delete clone;
    }
  }
  else
  {
    m_parent->removeClone(this);
    m_parent = nullptr;
  }
}

QList<QString> CalibrationComponent::validate()
{
  QList<QString> errors;

  if(isInitialized())
  {
    setStatus(IModelComponent::Validating,"Validating...");
    setStatus(IModelComponent::Valid,"");
  }
  else
  {
    //throw has not been initialized yet.
  }

  return errors;
}

void CalibrationComponent::prepare()
{
  if(m_parent == nullptr)
  {
    //Retrieve all calibration components
    if(!isPrepared() && isInitialized() && m_calibrationAlgorithm)
    {
      for(auto output :  outputsInternal())
      {
        for(auto adaptedOutput : output->adaptedOutputs())
        {
          adaptedOutput->initialize();
        }
      }


      if(m_modelComposition)
      {
        delete m_modelComposition;
      }

      m_modelComposition = new ModelComposition(false, QList<IModelComponent*>({this}), this);

      //updateOutputValues(QList<HydroCouple::IOutput*>());
      setStatus(IModelComponent::Updated ,"Finished preparing model");
      setPrepared(true);
    }
    else
    {
      setPrepared(false);
      setStatus(IModelComponent::Failed ,"Error occured when preparing model");
    }
  }
  else
  {
    if(!isPrepared() && isInitialized())
    {
      for(auto output :  outputsInternal())
      {
        for(auto adaptedOutput : output->adaptedOutputs())
        {
          adaptedOutput->initialize();
        }
      }

      updateOutputValues(QList<HydroCouple::IOutput*>());
      setStatus(IModelComponent::Updated ,"Finished preparing model");
      setPrepared(true);
    }
  }
}

void CalibrationComponent::update(const QList<HydroCouple::IOutput *> &requiredOutputs)
{
  if(status() == IModelComponent::Updated)
  {

    if(m_parent != nullptr)
    {
      setStatus(IModelComponent::Updating);

      //update outputs
      updateOutputValues(requiredOutputs);

      //run comsumers until
      applyInputValues();

      setStatus(IModelComponent::Done , "Simulation finished successfully", 100);

    }
    else
    {
      setStatus(IModelComponent::Updating, "Calibration component updating..." , progressChecker()->progress());

      if(!m_calibrationAlgorithm->isDone())
      {
        m_errors.clear();

        if(m_firstSimulation)
        {
          m_firstSimulation = false;
        }
        else
        {
          for(ModelComponent *modelComponent : m_modelComposition->components())
          {
            if(this != modelComponent->modelComponent())
            {
              modelComponent->modelComponent()->finish();
              modelComponent->modelComponent()->initialize();
            }
          }

          m_modelComposition->reestablishConnections();

          for(ModelComponent *modelComponent : m_modelComposition->components())
          {
            if(this != modelComponent->modelComponent())
            {
              modelComponent->modelComponent()->prepare();
            }
          }
        }

        m_calibrationAlgorithm->prepareForEvaluation(m_errors);

        if(m_modelComposition->isCloneable())
        {
          std::vector<ModelComposition*> compositions;
          int remain = m_calibrationAlgorithm->numIndividuals();

          int indIndex = 0;

          while (remain > 0)
          {
            int numClones = remain < m_maxParallelSims ? remain : m_maxParallelSims;

            if(indIndex == 0)
            {
              compositions.push_back(m_modelComposition);
              m_individual = indIndex; indIndex++;
            }

            while((int)compositions.size() < numClones)
            {
              ModelComposition *cloneCompostion = m_modelComposition->clone();

              CalibrationComponent *cloneComponent =  dynamic_cast<CalibrationComponent*>(cloneCompostion->findModelComponent(id())->modelComponent());
              cloneComponent->m_individual = indIndex; indIndex++;

              cloneCompostion->reestablishConnections();

              for(ModelComponent *modelComponent : cloneCompostion->components())
              {
                modelComponent->modelComponent()->prepare();
              }

              cloneCompostion->setDisposeComponentsOnDelete(true);
              compositions.push_back(cloneCompostion);
            }

            executeCompositions(compositions, requiredOutputs);

            for(int i = 0; i < (int)compositions.size(); i++)
            {
              ModelComposition *cloneComposition = compositions[i];

              if(cloneComposition != m_modelComposition)
              {
                for(ModelComponent *component : cloneComposition->components())
                {
                  component->modelComponent()->finish();
                }

                delete cloneComposition;
              }
            }

            compositions.clear();

            remain = remain - numClones;
          }
        }
        else
        {

          m_individual = 0;

          //update outputs
          updateOutputValues(requiredOutputs);

          //run comsumers until
          applyInputValues();

          for(int i = 1; i < m_calibrationAlgorithm->numIndividuals(); i++)
          {
            m_individual = i;

            resetCalibrationModelComponents();

            //update outputs
            updateOutputValues(requiredOutputs);

            //run comsumers until
            applyInputValues();
          }
        }

        //continue with optimization
        m_calibrationAlgorithm->processEvaluationOutput(m_errors);

      }

      if(!m_calibrationAlgorithm->isDone())
      {
        if(progressChecker()->performStep(m_calibrationAlgorithm->progress()->currentValue()))
        {

          QString message = "Calibration component updated | Progress: " + QString::number(progressChecker()->progress()) + " %";
          setStatus(IModelComponent::Updated , message , progressChecker()->progress());
        }
        else
        {
          setStatus(IModelComponent::Updated);
        }
      }
      else
      {
        setStatus(IModelComponent::Done , "Simulation finished successfully", 100);
      }
    }
  }
}

void CalibrationComponent::executeCompositions(const std::vector<ModelComposition*> &compositions, const QList<IOutput *> &requiredOutputs)
{
#ifdef USE_OPENMP
#pragma omp parallel for
#endif
  for(int i = 0; i < (int)compositions.size(); i++)
  {
    ModelComposition *cloneComposition = compositions[i];

    if(cloneComposition == m_modelComposition)
    {
      updateOutputValues(requiredOutputs);
      applyInputValues();
    }
    else
    {
      CalibrationComponent *cloneComponent =  dynamic_cast<CalibrationComponent*>(cloneComposition->findModelComponent(id())->modelComponent());
      while (cloneComponent->status() == IModelComponent::Updated)
      {
        cloneComponent->update();
      }
    }
  }
}

void CalibrationComponent::finish()
{
  if(isPrepared())
  {
    setStatus(IModelComponent::Finishing , "CalibrationComponent with id " + id() + " is being disposed" , 100);

    if(m_modelComposition)
    {
      delete m_modelComposition;
      m_modelComposition = nullptr;
    }

    if(m_parent == nullptr)
    {
      delete m_calibrationAlgorithm;
      m_calibrationAlgorithm = nullptr;
    }

    for(ICloneableModelComponent *component : m_clones)
    {
      component->finish();
    }

    setPrepared(false);
    setInitialized(false);

    setStatus(IModelComponent::Finished, "CalibrationComponent with id " + id() + " has been disposed" , 100);
    setStatus(IModelComponent::Created, "CalibrationComponent with id " + id() + " ran successfully and has been re-created" , 100);
  }
}

ICloneableModelComponent *CalibrationComponent::parent() const
{
  return m_parent;
}

ICloneableModelComponent *CalibrationComponent::clone()
{
  if(isInitialized())
  {
    CalibrationComponent *cloneComponent = dynamic_cast<CalibrationComponent*>(componentInfo()->createComponentInstance());
    cloneComponent->setReferenceDirectory(referenceDirectory());

    IdBasedArgumentString *identifierArg = identifierArgument();

    IdBasedArgumentString *cloneIndentifierArg = cloneComponent->identifierArgument();
    (*cloneIndentifierArg)["Id"] = QString((*identifierArg)["Id"]);
    (*cloneIndentifierArg)["Caption"] = QString((*identifierArg)["Caption"]);
    (*cloneIndentifierArg)["Description"] = QString((*identifierArg)["Description"]);

    cloneComponent->m_parent = this;
    m_clones.append(cloneComponent);
    cloneComponent->initialize();

    emit propertyChanged("Clones");

    return cloneComponent;
  }

  return nullptr;
}

QList<ICloneableModelComponent*> CalibrationComponent::clones() const
{
  return m_clones;
}

OptimizationAlgorithm *CalibrationComponent::calibrationAlgorithm() const
{
  return m_calibrationAlgorithm;
}

int CalibrationComponent::individualIndex() const
{
  return m_individual;
}

bool CalibrationComponent::removeClone(CalibrationComponent *component)
{
  int removed;

#ifdef USE_OPENMP
#pragma omp critical (CalibrationComponent)
#endif
  {
    removed = m_clones.removeAll(component);
  }

  if(removed)
  {
    component->m_parent = nullptr;
    emit propertyChanged("Clones");
  }

  return removed;
}

void CalibrationComponent::initializeFailureCleanUp()
{

}

void CalibrationComponent::createArguments()
{
  createInputFileArguments();
}

void CalibrationComponent::createInputFileArguments()
{
  QStringList fidentifiers;
  fidentifiers.append("Input File");

  Quantity *fquantity = Quantity::unitLessValues("InputFilesQuantity", QVariant::String, this);
  fquantity->setDefaultValue("");
  fquantity->setMissingValue("");

  Dimension *dimension = new Dimension("IdDimension","Dimension for identifiers",this);

  m_inputFilesArgument = new IdBasedArgumentString("InputFiles", fidentifiers, dimension, fquantity, this);
  m_inputFilesArgument->setCaption("Model Input Files");
  m_inputFilesArgument->addFileFilter("Input File (*.inp)");
  m_inputFilesArgument->setMatchIdentifiersWhenReading(true);

  addArgument(m_inputFilesArgument);
}

void CalibrationComponent::createInputs()
{
  createObjectiveFunctionInputs();
}

void CalibrationComponent::createObjectiveFunctionInputs()
{
  if(m_parent == nullptr)
  {
    int numObjectives = m_calibrationAlgorithm->numObjectives();

    for(int i = 0; i < numObjectives; i++)
    {
      QList<QSharedPointer<HCGeometry>> geometries = m_calibrationAlgorithm->objectiveGeometries(i);

      if(geometries.size())
      {
        QString objectiveId = m_calibrationAlgorithm->objectiveName(i);
        SpatialObjectiveInput *spatialVariableInput = new SpatialObjectiveInput(objectiveId, geometries[0]->geometryType(), m_geometryDimension, m_defaultQuantity,this);
        spatialVariableInput->setCaption(objectiveId);
        spatialVariableInput->addGeometries(geometries);
        addInput(spatialVariableInput);
      }
      else
      {

      }
    }
  }
  else
  {

    int numObjectives = m_parent->m_calibrationAlgorithm->numObjectives();

    for(int i = 0; i < numObjectives; i++)
    {
      QList<QSharedPointer<HCGeometry>> geometries = m_parent->m_calibrationAlgorithm->objectiveGeometries(i);

      if(geometries.size())
      {
        QString objectiveId = m_parent->m_calibrationAlgorithm->objectiveName(i);
        SpatialObjectiveInput *spatialVariableInput = new SpatialObjectiveInput(objectiveId, geometries[0]->geometryType(), m_geometryDimension, m_defaultQuantity,this);
        spatialVariableInput->setCaption(objectiveId);
        spatialVariableInput->addGeometries(geometries);
        addInput(spatialVariableInput);
      }
      else
      {

      }
    }
  }
}

void CalibrationComponent::createOutputs()
{
  createVariableOutputs();
}

void CalibrationComponent::createVariableOutputs()
{
  if(m_parent == nullptr)
  {
    int numVariables = m_calibrationAlgorithm->numVariables();

    for(int i = 0; i < numVariables; i++)
    {
      QList<QSharedPointer<HCGeometry>> geometries = m_calibrationAlgorithm->variableGeometries(i);

      if(geometries.size())
      {
        QString variableId = m_calibrationAlgorithm->variableName(i);
        SpatialVariableOutput *spatialVariableOutput = new SpatialVariableOutput(variableId, geometries[0]->geometryType(), m_geometryDimension, m_defaultQuantity,this);
        spatialVariableOutput->setCaption(variableId);
        spatialVariableOutput->addGeometries(geometries);
        addOutput(spatialVariableOutput);
      }
      else
      {

      }
    }
  }
  else
  {

    int numVariables = m_parent->m_calibrationAlgorithm->numVariables();

    for(int i = 0; i < numVariables; i++)
    {
      QList<QSharedPointer<HCGeometry>> geometries = m_parent->m_calibrationAlgorithm->variableGeometries(i);

      if(geometries.size())
      {
        QString variableId = m_parent->m_calibrationAlgorithm->variableName(i);
        SpatialVariableOutput *spatialVariableOutput = new SpatialVariableOutput(variableId, geometries[0]->geometryType(), m_geometryDimension, m_defaultQuantity,this);
        spatialVariableOutput->setCaption(variableId);
        spatialVariableOutput->addGeometries(geometries);
        addOutput(spatialVariableOutput);
      }
      else
      {

      }
    }

  }
}

bool CalibrationComponent::initializeArguments(QString &message)
{
  bool initialized = false;

  if(m_parent == nullptr)
  {
    initialized = initializeInputFilesArguments(message);


  }
  else
  {
    return true;
  }

  return initialized;
}

bool CalibrationComponent::initializeInputFilesArguments(QString &message)
{
  QString inputFilePath = (*m_inputFilesArgument)["Input File"];
  QFileInfo inputFile = getAbsoluteFilePath(inputFilePath);
  m_maxParallelSims = 1;

#ifdef USE_OPENMP
  m_maxParallelSims = std::max(1, omp_get_max_threads() - 1);
#endif

  if(inputFile.exists())
  {
    QFile file(inputFile.absoluteFilePath());

    if (file.open(QIODevice::ReadOnly))
    {
      QTextStream streamReader(&file);
      QString currentFlag ="";
      int currentFlagIndex = -1;
      QRegExp delimiters = QRegExp("(\\,|\\t|\\;|\\s+)");

      int lineCount = 0;

      while (!streamReader.atEnd())
      {
        QString line = streamReader.readLine().trimmed();

        lineCount++;

        if (!line.isEmpty() && !line.isNull())
        {
          if(line[0] == '[' && line[line.size() - 1] == ']')
          {
            currentFlag = line;

            if(currentFlag == "[GLOBAL_OPTIONS]")
            {
              currentFlagIndex = 0;
            }
            else
            {
              currentFlagIndex = -1;
            }
          }
          else if (!QStringRef::compare(QStringRef(&line, 0, 2), ";;"))
          {
            //commment do nothing
          }
          else
          {
            switch (currentFlagIndex)
            {
              case 0:
                {
                  QStringList options = line.split(delimiters, QString::SkipEmptyParts);

                  if(options.size() == 2)
                  {
                    if(options[0] == "ALGORITHM")
                    {
                      if(options[1] == "NSGAII")
                      {
                        m_calibrationAlgorithm = new NSGAIIAlgorithm(nullptr);
                        m_calibrationAlgorithm->setReferenceDir(inputFile.absoluteDir());
                        m_calibrationAlgorithm->prepareForInputFileRead();
                      }
                    }
                    else if(options[0] == "MAX_PARALLEL_SIMS")
                    {
                      bool maxParallelSimsOk;
                      int maxParallelSim = options[1].toInt(& maxParallelSimsOk);

                      if(maxParallelSim > 0)
                        m_maxParallelSims = maxParallelSim;
                    }
                  }
                  else
                  {
                    message = "Line " + QString::number(lineCount) + ": global options error";
                    return false;
                  }
                }
                break;
              default:
                {
                  if(m_calibrationAlgorithm)
                  {
                    bool read = m_calibrationAlgorithm->processInputFileLine(currentFlag, line, message);

                    if(!read)
                    {
                      message = "Line " + QString::number(lineCount) + ": " + message;
                      return false;
                    }
                  }
                }
                break;
            }
          }
        }
      }

      file.close();
    }

    QStringList errors;

    bool initialized = m_calibrationAlgorithm->initialize(errors);

    if(!initialized && errors.size())
    {
      message = QString(errors[0]);

      for(int i = 1; i < errors.size(); i++)
      {
        message = "\n" + QString(errors[i]);
      }
    }
    else
    {
      progressChecker()->reset(m_calibrationAlgorithm->progress()->begin() , m_calibrationAlgorithm->progress()->end());
    }

    return initialized;
  }
  else
  {
    message = "Input file does not exist: " + inputFile.absoluteFilePath();
    return false;
  }

  m_firstSimulation = true;

  return true;
}

void CalibrationComponent::resetCalibrationModelComponents()
{
  for(ModelComponent *modelComponent : m_modelComposition->components())
  {
    if(modelComponent->modelComponent() != this)
    {
      modelComponent->modelComponent()->finish();
      modelComponent->modelComponent()->initialize();
    }
  }

  m_modelComposition->reestablishConnections();

  for(ModelComponent *modelComponent : m_modelComposition->components())
  {
    if(modelComponent->modelComponent() != this)
    {
      modelComponent->modelComponent()->prepare();
    }
  }
}

void CalibrationComponent::findProviderComponents(IModelComponent *provider, QSet<IModelComponent *> &providers)
{

  IMultiInput* mInput = nullptr;

  for(IInput *input : provider->inputs())
  {
    if(input->provider() != nullptr)
    {
      if(!providers.contains(input->provider()->modelComponent()))
      {
        providers.insert(input->provider()->modelComponent());
        findProviderComponents(input->provider()->modelComponent(), providers);
      }
    }
    else if (((mInput = dynamic_cast<IMultiInput*>(input))
              && mInput->providers().length()))
    {
      for(IOutput *mprovider : mInput->providers())
      {
        if(!providers.contains(mprovider->modelComponent()))
        {
          providers.insert(mprovider->modelComponent());
          findProviderComponents(mprovider->modelComponent(), providers);
        }
      }
    }
  }
}
