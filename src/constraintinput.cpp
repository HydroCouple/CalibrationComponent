#include "stdafx.h"
#include "constraintinput.h"
#include "calibrationcomponent.h"
#include "core/dimension.h"
#include "optimizationalgorithm.h"
#include "core/valuedefinition.h"
#include "hydrocouplespatial.h"
#include "hydrocoupletemporal.h"
#include "hydrocouplespatiotemporal.h"

using namespace HydroCouple;
using namespace HydroCouple::Spatial;
using namespace HydroCouple::Temporal;
using namespace HydroCouple::SpatioTemporal;

SpatialConstraintInput::SpatialConstraintInput(const QString &constraintId,
                                               HydroCouple::Spatial::IGeometry::GeometryType geometryType,
                                               Dimension *geometryDimension,
                                               Quantity *quantity,
                                               CalibrationComponent *calibrationComponent):
  GeometryInputDouble(constraintId, geometryType, geometryDimension, quantity, calibrationComponent),
  m_constraintId(constraintId),
  m_calibrationComponent(calibrationComponent),
  m_parentCalibrationComponent(nullptr)
{
  if(m_calibrationComponent->parent() == nullptr)
  {
    m_parentCalibrationComponent = m_calibrationComponent;
  }
  else
  {
    m_parentCalibrationComponent = dynamic_cast<CalibrationComponent*>(m_calibrationComponent->parent());
  }

  m_constraintIndex = m_parentCalibrationComponent->calibrationAlgorithm()->getConstraintIndex(m_constraintId);
}

SpatialConstraintInput::~SpatialConstraintInput()
{

}

bool SpatialConstraintInput::setProvider(HydroCouple::IOutput *provider)
{
  m_geometryMapping.clear();

  if(AbstractInput::setProvider(provider) && provider)
  {
    ITimeGeometryComponentDataItem *timeGeometryDataItem = nullptr;
    IGeometryComponentDataItem *geometryDataItem = nullptr;
    IIdBasedComponentDataItem *idbasedDataItem = nullptr;
    ITimeIdBasedComponentDataItem *timeIdBasedDataItem = nullptr;

    if((timeGeometryDataItem = dynamic_cast<ITimeGeometryComponentDataItem*>(provider)) &&
       timeGeometryDataItem->geometryType() == geometryType())
    {
      if(timeGeometryDataItem->geometryCount())
      {
        for(int i = 0; i < timeGeometryDataItem->geometryCount(); i++)
        {
          IGeometry *providerGeometry = timeGeometryDataItem->geometry(i);

          for(int j = 0 ; j < geometryCount(); j++)
          {
            IGeometry *myGeometry = geometry(j);

            if(equalsGeometry(providerGeometry, myGeometry))
            {
              m_geometryMapping[i] = j;
              break;
            }
          }
        }
      }
    }
    else if((geometryDataItem = dynamic_cast<IGeometryComponentDataItem*>(provider)) &&
            geometryDataItem->geometryType() == geometryType())
    {
      if(geometryDataItem->geometryCount())
      {
        for(int i = 0; i < geometryDataItem->geometryCount(); i++)
        {
          IGeometry *providerGeometry = geometryDataItem->geometry(i);

          for(int j = 0 ; j < geometryCount(); j++)
          {
            IGeometry *myGeometry = geometry(j);

            if(equalsGeometry(providerGeometry, myGeometry))
            {
              m_geometryMapping[i] = j;
              break;
            }
          }
        }
      }
    }
    else if((idbasedDataItem = dynamic_cast<IIdBasedComponentDataItem*>(provider)))
    {
      QStringList pIds = idbasedDataItem->identifiers();

      if(pIds.size())
      {
        for(int i = 0; i < pIds.size(); i++)
        {
          for(int j = 0 ; j < geometryCount(); j++)
          {
            IGeometry *myGeometry = geometry(j);

            if(!QString::compare(pIds[i], myGeometry->id()))
            {
              m_geometryMapping[i] = j;
              break;
            }
          }
        }
      }
    }
    else if((timeIdBasedDataItem = dynamic_cast<ITimeIdBasedComponentDataItem*>(provider)))
    {
      QStringList pIds = timeIdBasedDataItem->identifiers();

      if(pIds.size())
      {
        for(int i = 0; i < pIds.size(); i++)
        {
          for(int j = 0 ; j < geometryCount(); j++)
          {
            IGeometry *myGeometry = geometry(j);

            if(!QString::compare(pIds[i], myGeometry->id()))
            {
              m_geometryMapping[i] = j;
              break;
            }
          }
        }
      }
    }

    return true;
  }

  return false;
}

bool SpatialConstraintInput::canConsume(HydroCouple::IOutput *provider, QString &message) const
{
  message = "";

  ITimeGeometryComponentDataItem *timeGeometryDataItem = nullptr;
  IGeometryComponentDataItem *geometryDataItem = nullptr;
  IIdBasedComponentDataItem *idbasedDataItem = nullptr;
  ITimeIdBasedComponentDataItem *timeIdBasedDataItem = nullptr;


  if((timeGeometryDataItem = dynamic_cast<ITimeGeometryComponentDataItem*>(provider)) &&
     timeGeometryDataItem->geometryType() == geometryType())
  {
    return true;
  }
  else if((geometryDataItem = dynamic_cast<IGeometryComponentDataItem*>(provider)) &&
          geometryDataItem->geometryType() == geometryType())
  {
    return true;
  }
  else if((idbasedDataItem = dynamic_cast<IIdBasedComponentDataItem*>(provider)) ||
          (timeIdBasedDataItem = dynamic_cast<ITimeIdBasedComponentDataItem*>(provider)))
  {
    return true;
  }

  return false;
}

void SpatialConstraintInput::retrieveValuesFromProvider()
{
  while (provider()->modelComponent()->status() == HydroCouple::IModelComponent::Updated)
  {
    provider()->updateValues(this);
  }
}

void SpatialConstraintInput::applyData()
{
  ITimeGeometryComponentDataItem *timeGeometryDataItem = nullptr;
  IGeometryComponentDataItem *geometryDataItem = nullptr;
  IIdBasedComponentDataItem *idbasedDataItem = nullptr;
  ITimeIdBasedComponentDataItem *timeIdBasedDataItem = nullptr;

  int indIndex = m_calibrationComponent->individualIndex();
  Individual *ind =  m_parentCalibrationComponent->calibrationAlgorithm()->getIndividual(indIndex);

  if((timeGeometryDataItem = dynamic_cast<ITimeGeometryComponentDataItem*>(provider())))
  {
    int time = timeGeometryDataItem->timeCount() - 1;

    for(auto it : m_geometryMapping)
    {
      double value = 0;
      timeGeometryDataItem->getValue(time, it.second, &value);
      ind->constraints()[m_constraintIndex] = value;
    }
  }
  else if((geometryDataItem = dynamic_cast<IGeometryComponentDataItem*>(provider())))
  {
    for(auto it : m_geometryMapping)
    {
      double value = 0;
      geometryDataItem->getValue(it.second, &value);
      ind->constraints()[m_constraintIndex] = value;
    }
  }
  else if((idbasedDataItem = dynamic_cast<IIdBasedComponentDataItem*>(provider())))
  {
    for(auto it : m_geometryMapping)
    {
      double value = 0;
      idbasedDataItem->getValue(it.second, &value);
      ind->constraints()[m_constraintIndex] = value;
    }
  }
  else if((timeIdBasedDataItem = dynamic_cast<ITimeIdBasedComponentDataItem*>(provider())))
  {
    int timeIndex = timeIdBasedDataItem->timeCount() - 1;

    for(auto it : m_geometryMapping)
    {
      double value = 0;
      timeIdBasedDataItem->getValue(timeIndex, it.second, &value);
      ind->constraints()[m_constraintIndex] = value;
    }
  }
}

bool SpatialConstraintInput::equalsGeometry(IGeometry *geom1, IGeometry *geom2, double epsilon)
{
  if(geom1->geometryType() == geom2->geometryType())
  {
    switch (geom1->geometryType())
    {
      case IGeometry::LineString:
      case IGeometry::LineStringM:
      case IGeometry::LineStringZ:
      case IGeometry::LineStringZM:
        {
          ILineString *lineString1 = dynamic_cast<ILineString*>(geom1);
          ILineString *lineString2 = dynamic_cast<ILineString*>(geom2);

          if(lineString1 && lineString2 && lineString1->pointCount() == lineString2->pointCount())
          {
            for(int i = 0 ; i < lineString1->pointCount(); i++)
            {
              IPoint *p1 = lineString1->point(i);
              IPoint *p2 = lineString2->point(i);

              double dx = p1->x() - p2->x();
              double dy = p1->y() - p2->y();

              double dist = sqrt(dx * dx + dy * dy);

              if(dist < epsilon)
              {
                return true;
              }
            }
          }
        }
        break;
      default:
        {
          return geom1->equals(geom2);
        }
    }
  }

  return false;
}



IdBasedConstraintInput::IdBasedConstraintInput(const QString &constraintId,
                                               const QStringList& identifiers,
                                               Dimension *identifierDimension,
                                               Quantity *quantity,
                                               CalibrationComponent *calibrationComponent):
  IdBasedInputDouble(constraintId, identifiers, identifierDimension, quantity, calibrationComponent),
  m_constraintId(constraintId),
  m_calibrationComponent(calibrationComponent),
  m_parentCalibrationComponent(nullptr)
{
  if(m_calibrationComponent->parent() == nullptr)
  {
    m_parentCalibrationComponent = m_calibrationComponent;
  }
  else
  {
    m_parentCalibrationComponent = dynamic_cast<CalibrationComponent*>(m_calibrationComponent->parent());
  }

  m_constraintIndex = m_parentCalibrationComponent->calibrationAlgorithm()->getObjectiveIndex(m_constraintId);
}

IdBasedConstraintInput::~IdBasedConstraintInput()
{

}

bool IdBasedConstraintInput::setProvider(HydroCouple::IOutput *provider)
{
  m_idMapping.clear();

  if(AbstractInput::setProvider(provider) && provider)
  {
    ITimeGeometryComponentDataItem *timeGeometryDataItem = nullptr;
    IGeometryComponentDataItem *geometryDataItem = nullptr;
    IIdBasedComponentDataItem *idbasedDataItem = nullptr;
    ITimeIdBasedComponentDataItem *timeIdBasedDataItem = nullptr;

    QStringList ids = identifiers();

    if((timeGeometryDataItem = dynamic_cast<ITimeGeometryComponentDataItem*>(provider)))
    {
      if(timeGeometryDataItem->geometryCount())
      {
        for(int i = 0; i < timeGeometryDataItem->geometryCount(); i++)
        {
          IGeometry *providerGeometry = timeGeometryDataItem->geometry(i);

          for(int j = 0 ; j < ids.size(); j++)
          {
            QString id = ids[j];

            if(!QString::compare(providerGeometry->id(), id))
            {
              m_idMapping[i] = j;
              break;
            }
          }
        }
      }
    }
    else if((geometryDataItem = dynamic_cast<IGeometryComponentDataItem*>(provider)))
    {
      if(geometryDataItem->geometryCount())
      {
        for(int i = 0; i < geometryDataItem->geometryCount(); i++)
        {
          IGeometry *providerGeometry = geometryDataItem->geometry(i);

          for(int j = 0 ; j < ids.size(); j++)
          {
            QString id = ids[j];

            if(!QString::compare(providerGeometry->id(), id))
            {
              m_idMapping[i] = j;
              break;
            }
          }
        }
      }
    }
    else if((idbasedDataItem = dynamic_cast<IIdBasedComponentDataItem*>(provider)))
    {
      QStringList pIds = idbasedDataItem->identifiers();

      if(pIds.size())
      {
        for(int i = 0; i < pIds.size(); i++)
        {
          QString pId = pIds[i];

          for(int j = 0 ; j < ids.size(); j++)
          {
            QString id = ids[j];

            if(!QString::compare(pId, id))
            {
              m_idMapping[i] = j;
              break;
            }
          }
        }
      }
    }
    else if((timeIdBasedDataItem = dynamic_cast<ITimeIdBasedComponentDataItem*>(provider)))
    {
      QStringList pIds = idbasedDataItem->identifiers();

      if(pIds.size())
      {
        for(int i = 0; i < pIds.size(); i++)
        {
          QString pId = pIds[i];

          for(int j = 0 ; j < ids.size(); j++)
          {
            QString id = ids[j];

            if(!QString::compare(pId, id))
            {
              m_idMapping[i] = j;
              break;
            }
          }
        }
      }
    }

    return true;
  }

  return false;
}

bool IdBasedConstraintInput::canConsume(HydroCouple::IOutput *provider, QString &message) const
{
  message = "";

  ITimeGeometryComponentDataItem *timeGeometryDataItem = nullptr;
  IGeometryComponentDataItem *geometryDataItem = nullptr;
  IIdBasedComponentDataItem *idbasedDataItem = nullptr;
  ITimeIdBasedComponentDataItem *timeIdBasedDataItem = nullptr;

  if((timeGeometryDataItem = dynamic_cast<ITimeGeometryComponentDataItem*>(provider)))
  {
    return true;
  }
  else if((geometryDataItem = dynamic_cast<IGeometryComponentDataItem*>(provider)))
  {
    return true;
  }
  else if((idbasedDataItem = dynamic_cast<IIdBasedComponentDataItem*>(provider)))
  {
    return true;
  }
  else if((timeIdBasedDataItem = dynamic_cast<ITimeIdBasedComponentDataItem*>(provider)))
  {
    return true;
  }

  return false;
}

void IdBasedConstraintInput::retrieveValuesFromProvider()
{
  while (provider()->modelComponent()->status() == HydroCouple::IModelComponent::Updated)
  {
    provider()->updateValues(this);
  }
}

void IdBasedConstraintInput::applyData()
{
  ITimeGeometryComponentDataItem *timeGeometryDataItem = nullptr;
  IGeometryComponentDataItem *geometryDataItem = nullptr;
  IIdBasedComponentDataItem *idbasedDataItem = nullptr;
  ITimeIdBasedComponentDataItem *timeIdBasedDataItem = nullptr;

  int indIndex = m_calibrationComponent->individualIndex();
  Individual *ind =  m_parentCalibrationComponent->calibrationAlgorithm()->getIndividual(indIndex);

  if((timeGeometryDataItem = dynamic_cast<ITimeGeometryComponentDataItem*>(provider())))
  {
    int time = timeGeometryDataItem->timeCount() - 1;

    for(auto it : m_idMapping)
    {
      double value = 0;
      timeGeometryDataItem->getValue(time, it.second, &value);
      ind->constraints()[m_constraintIndex] = value;
    }
  }
  else if((geometryDataItem = dynamic_cast<IGeometryComponentDataItem*>(provider())))
  {
    for(auto it : m_idMapping)
    {
      double value = 0;
      geometryDataItem->getValue(it.second, &value);
      ind->constraints()[m_constraintIndex] = value;
    }
  }
  else if((idbasedDataItem = dynamic_cast<IIdBasedComponentDataItem*>(provider())))
  {
    for(auto it : m_idMapping)
    {
      double value = 0;
      idbasedDataItem->getValue(it.second, &value);
      ind->constraints()[m_constraintIndex] = value;
    }
  }
  else if((timeIdBasedDataItem = dynamic_cast<ITimeIdBasedComponentDataItem*>(provider())))
  {
    int time = timeIdBasedDataItem->timeCount() - 1;

    for(auto it : m_idMapping)
    {
      double value = 0;
      timeIdBasedDataItem->getValue(time, it.second, &value);
      ind->constraints()[m_constraintIndex] = value;
    }
  }
}


