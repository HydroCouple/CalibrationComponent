#ifndef CALIBRATIONCOMPONENTSINPUT_H
#define CALIBRATIONCOMPONENTSINPUT_H

#include "calibrationcomponent_global.h"
#include "core/abstractinput.h"

class CalibrationComponent;

class CALIBRATIONCOMPONENT_EXPORT TriggerComponentInput : public AbstractInput
{
    Q_OBJECT

  public:

    TriggerComponentInput(const QString &id, CalibrationComponent *modelComponent);

    virtual ~TriggerComponentInput() override;

    int dimensionLength(const std::vector<int> &dimensionIndexes) const override;

    void getValue(const std::vector<int> &dimensionIndexes, void *data) const override;

    void setValue(const std::vector<int> &dimensionIndexes, const void *data) override;

    void retrieveValuesFromProvider() override;

    void applyData() override;

  private:

    CalibrationComponent *m_calibrationComponent;
};

#endif // CALIBRATIONCOMPONENTSINPUT_H
