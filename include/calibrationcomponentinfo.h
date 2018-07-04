#ifndef CALIBRATIONCOMPONENTINFO_H
#define CALIBRATIONCOMPONENTINFO_H


#include "calibrationcomponent_global.h"
#include "core/abstractmodelcomponentinfo.h"


class CALIBRATIONCOMPONENT_EXPORT CalibrationComponentInfo : public AbstractModelComponentInfo
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "CalibrationComponentInfo")

  public:

    CalibrationComponentInfo(QObject *parent = nullptr);

    virtual ~CalibrationComponentInfo();

    HydroCouple::IModelComponent* createComponentInstance() override;
};

Q_DECLARE_METATYPE(CalibrationComponentInfo*)


#endif // CALIBRATIONCOMPONENTINFO_H
