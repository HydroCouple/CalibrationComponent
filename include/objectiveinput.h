#ifndef OBJECTIVEINPUT_H
#define OBJECTIVEINPUT_H

#include "calibrationcomponent_global.h"
#include "spatial/geometryexchangeitems.h"
#include "core/idbasedinputs.h"

#include <unordered_map>

class CalibrationComponent;
class Quantity;

class CALIBRATIONCOMPONENT_EXPORT SpatialObjectiveInput:
    public GeometryInputDouble
{
    Q_OBJECT

  public:

    SpatialObjectiveInput(const QString &objectiveId,
                          HydroCouple::Spatial::IGeometry::GeometryType geometryType,
                          Dimension *geometryDimension,
                          Quantity *quantity,
                          CalibrationComponent *calibrationComponent);

    virtual ~SpatialObjectiveInput() override;

    bool setProvider(HydroCouple::IOutput *provider) override;

    bool canConsume(HydroCouple::IOutput *provider, QString &message) const override;

    void retrieveValuesFromProvider() override;

    void applyData() override;

  private:

    static bool equalsGeometry(HydroCouple::Spatial::IGeometry *geom1, HydroCouple::Spatial::IGeometry *geom2, double epsilon = 0.00001);

  private:

    int m_objectiveIndex;
    QString m_objectiveId;
    CalibrationComponent *m_calibrationComponent;
    CalibrationComponent *m_parentCalibrationComponent;
    std::unordered_map<int,int> m_geometryMapping;

};

class CALIBRATIONCOMPONENT_EXPORT IdBasedObjectiveInput:
    public IdBasedInputDouble
{
    Q_OBJECT

  public:

    IdBasedObjectiveInput(const QString &objectiveId,
                          const QStringList& identifiers,
                          Dimension *identifierDimension,
                          Quantity *quantity,
                          CalibrationComponent *calibrationComponent);

    virtual ~IdBasedObjectiveInput() override;

    bool setProvider(HydroCouple::IOutput *provider) override;

    bool canConsume(HydroCouple::IOutput *provider, QString &message) const override;

    void retrieveValuesFromProvider() override;

    void applyData() override;

  private:

    int m_objectiveIndex;
    QString m_objectiveId;
    CalibrationComponent *m_calibrationComponent;
    CalibrationComponent *m_parentCalibrationComponent;
    std::unordered_map<int,int> m_idMapping;

};


#endif // OBJECTIVEINPUT_H
