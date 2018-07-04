#include "stdafx.h"
#include "objectiveinput.h"
#include "calibrationcomponent.h"
#include "core/dimension.h"
#include "optimizationalgorithm.h"
#include "core/valuedefinition.h"
#include "hydrocouplespatial.h"
#include "hydrocouplespatiotemporal.h"

using namespace HydroCouple;
using namespace HydroCouple::Spatial;
using namespace HydroCouple::SpatioTemporal;

SpatialObjectiveInput::SpatialObjectiveInput(const QString &objectiveId,
                                             HydroCouple::Spatial::IGeometry::GeometryType geometryType,
                                             Dimension *geometryDimension,
                                             Quantity *quantity,
                                             CalibrationComponent *calibrationComponent):
  GeometryInputDouble(objectiveId, geometryType, geometryDimension, quantity, calibrationComponent),
  m_objectiveId(objectiveId),
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

  m_objectiveIndex = m_parentCalibrationComponent->calibrationAlgorithm()->getObjectiveIndex(m_objectiveId);
}


bool SpatialObjectiveInput::setProvider(HydroCouple::IOutput *provider)
{
  m_geometryMapping.clear();

  if(AbstractInput::setProvider(provider) && provider)
  {
    ITimeGeometryComponentDataItem *timeGeometryDataItem = nullptr;
    IGeometryComponentDataItem *geometryDataItem = nullptr;

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

    return true;
  }

  return false;
}

bool SpatialObjectiveInput::canConsume(HydroCouple::IOutput *provider, QString &message) const
{
  message = "";

  ITimeGeometryComponentDataItem *timeGeometryDataItem = nullptr;
  IGeometryComponentDataItem *geometryDataItem = nullptr;

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

  return false;
}

void SpatialObjectiveInput::retrieveValuesFromProvider()
{
  while (provider()->modelComponent()->status() == HydroCouple::IModelComponent::Updated)
  {
    provider()->updateValues(this);
  }
}

void SpatialObjectiveInput::applyData()
{
  ITimeGeometryComponentDataItem *timeGeometryDataItem = nullptr;
  IGeometryComponentDataItem *geometryDataItem = nullptr;

  int indIndex = m_calibrationComponent->individualIndex();
  Individual *ind =  m_parentCalibrationComponent->calibrationAlgorithm()->getIndividual(indIndex);

  if((timeGeometryDataItem = dynamic_cast<ITimeGeometryComponentDataItem*>(provider())))
  {
    int time = timeGeometryDataItem->timeCount() - 1;

    for(auto it : m_geometryMapping)
    {
      double value = 0;
      timeGeometryDataItem->getValue(time, it.second, &value);
      ind->objectives()[m_objectiveIndex] = value;
    }
  }
  else if((geometryDataItem = dynamic_cast<IGeometryComponentDataItem*>(provider())))
  {
    for(auto it : m_geometryMapping)
    {
      double value = 0;
      geometryDataItem->getValue(it.second, &value);
      ind->objectives()[m_objectiveIndex] = value;
    }
  }
}

bool SpatialObjectiveInput::equalsGeometry(IGeometry *geom1, IGeometry *geom2, double epsilon)
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
        break;
    }
  }

  return false;
}
