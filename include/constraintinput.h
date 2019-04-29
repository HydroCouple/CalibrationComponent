#ifndef CONSTRAINTINPUT_H
#define CONSTRAINTINPUT_H

#include "calibrationcomponent_global.h"
#include "spatial/geometryexchangeitems.h"
#include "core/idbasedinputs.h"

#include <unordered_map>

class CalibrationComponent;
class Quantity;

class CALIBRATIONCOMPONENT_EXPORT SpatialConstraintInput:
    public GeometryInputDouble
{
    Q_OBJECT

  public:

    SpatialConstraintInput(const QString &constraintId,
                          HydroCouple::Spatial::IGeometry::GeometryType geometryType,
                          Dimension *geometryDimension,
                          Quantity *quantity,
                          CalibrationComponent *calibrationComponent);

    virtual ~SpatialConstraintInput() override;

    bool setProvider(HydroCouple::IOutput *provider) override;

    bool canConsume(HydroCouple::IOutput *provider, QString &message) const override;

    void retrieveValuesFromProvider() override;

    void applyData() override;

  private:

    static bool equalsGeometry(HydroCouple::Spatial::IGeometry *geom1, HydroCouple::Spatial::IGeometry *geom2, double epsilon = 0.00001);

  private:

    int m_constraintIndex;
    QString m_constraintId;
    CalibrationComponent *m_calibrationComponent;
    CalibrationComponent *m_parentCalibrationComponent;
    std::unordered_map<int,int> m_geometryMapping;

};

class CALIBRATIONCOMPONENT_EXPORT IdBasedConstraintInput:
    public IdBasedInputDouble
{
    Q_OBJECT

  public:

    IdBasedConstraintInput(const QString &constraintId,
                          const QStringList& identifiers,
                          Dimension *identifierDimension,
                          Quantity *quantity,
                          CalibrationComponent *calibrationComponent);

    virtual ~IdBasedConstraintInput() override;

    bool setProvider(HydroCouple::IOutput *provider) override;

    bool canConsume(HydroCouple::IOutput *provider, QString &message) const override;

    void retrieveValuesFromProvider() override;

    void applyData() override;

  private:

    int m_constraintIndex;
    QString m_constraintId;
    CalibrationComponent *m_calibrationComponent;
    CalibrationComponent *m_parentCalibrationComponent;
    std::unordered_map<int,int> m_idMapping;

};



#endif // CONSTRAINTINPUT_H
