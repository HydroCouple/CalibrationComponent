#ifndef VARIABLEOUTPUT_H
#define VARIABLEOUTPUT_H

#include "calibrationcomponent_global.h"
#include "spatial/geometryexchangeitems.h"
#include "core/idbasedinputs.h"

class CalibrationComponent;
class Quantity;


class CALIBRATIONCOMPONENT_EXPORT SpatialVariableOutput:
    public GeometryOutputDouble
{
    Q_OBJECT

  public:

    SpatialVariableOutput(const QString &variableId,
                          HydroCouple::Spatial::IGeometry::GeometryType geometryType,
                          Dimension *geometryDimension,
                          Quantity *quantity,
                          CalibrationComponent *calibrationComponent);

    void updateValues(HydroCouple::IInput *querySpecifier) override;

    void updateValues() override;

  private:

    int m_variableIndex;
    QString m_variableId;
    CalibrationComponent *m_calibrationComponent;
    CalibrationComponent *m_parentCalibrationComponent;

};

#endif // VARIABLEOUTPUT_H
