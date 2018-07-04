#include "stdafx.h"
#include "triggercomponentinput.h"
#include "calibrationcomponent.h"
#include "core/valuedefinition.h"
#include "core/unit.h"

using namespace HydroCouple;

TriggerComponentInput::TriggerComponentInput(const QString &id, CalibrationComponent *modelComponent)
  : AbstractInput(id, QList<Dimension*>(), new Quantity(QVariant::Double, Unit::unitlessCoefficient(modelComponent), modelComponent), modelComponent),
    m_calibrationComponent(modelComponent)
{

}

TriggerComponentInput::~TriggerComponentInput()
{

}

int TriggerComponentInput::dimensionLength(const std::vector<int> &dimensionIndexes) const
{
  return 0;
}

void TriggerComponentInput::getValue(const std::vector<int> &dimensionIndexes, void *data) const
{

}

void TriggerComponentInput::setValue(const std::vector<int> &dimensionIndexes, const void *data)
{

}

void TriggerComponentInput::retrieveValuesFromProvider()
{
  while(provider()->modelComponent()->status() == IModelComponent::Updated)
  {
    provider()->updateValues(this);
  }
}

void TriggerComponentInput::applyData()
{

}
