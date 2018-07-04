#include "stdafx.h"
#include "variableoutput.h"
#include "core/dimension.h"
#include "calibrationcomponent.h"
#include "optimizationalgorithm.h"
#include "core/valuedefinition.h"

SpatialVariableOutput::SpatialVariableOutput(const QString &variableId,
                                             HydroCouple::Spatial::IGeometry::GeometryType geometryType,
                                             Dimension *geometryDimension,
                                             Quantity *quantity,
                                             CalibrationComponent *calibrationComponent):
  GeometryOutputDouble(variableId, geometryType, geometryDimension, quantity, calibrationComponent),
  m_variableId(variableId),
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

  m_variableIndex = m_parentCalibrationComponent->calibrationAlgorithm()->getVariableIndex(variableId);
}

void SpatialVariableOutput::updateValues(HydroCouple::IInput *querySpecifier)
{
  refreshAdaptedOutputs();
}

void SpatialVariableOutput::updateValues()
{
  int indIndex = m_calibrationComponent->individualIndex();
  Individual *ind =  m_parentCalibrationComponent->calibrationAlgorithm()->getIndividual(indIndex);
  double value =  ind->variables()[m_variableIndex];
  std::vector<double> values(geometryCount(), value);
  setValues(0, geometryCount(), values.data());
}
