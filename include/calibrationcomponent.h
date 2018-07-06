#ifndef CALIBRATIONCOMPONENT_H
#define CALIBRATIONCOMPONENT_H

#include "calibrationcomponent.h"
#include "calibrationcomponent_global.h"
#include "core/abstractmodelcomponent.h"
#include "composition/composition.h"
#include "calibrationcomponentinfo.h"

#include <unordered_map>

class OptimizationAlgorithm;
class IdBasedArgumentString;
class TriggerComponentInput;
class Dimension;
class Quantity;

namespace Composition
{
  class ModelComposition;
}

class CALIBRATIONCOMPONENT_EXPORT CalibrationComponent : public AbstractModelComponent,
    public virtual HydroCouple::ICloneableModelComponent
{

    Q_OBJECT

    Q_INTERFACES(HydroCouple::ICloneableModelComponent)

  public:

    /*!
     * \brief CalibrationComponent
     * \param id
     * \param modelComponentInfo
     */
    CalibrationComponent(const QString &id, CalibrationComponentInfo* modelComponentInfo = nullptr);

    /*!
     * \brief ~CalibrationComponent
     */
    virtual ~CalibrationComponent();

    /*!
     * \brief validate validates this component model instance
     * \return Returns a list of error messages.
     */
    QList<QString> validate() override;

    /*!
     * \brief prepare Prepares the model component instance.
     */
    void prepare() override;

    /*!
     * \brief update
     * \param requiredOutputs
     */
    void update(const QList<HydroCouple::IOutput*> &requiredOutputs = QList<HydroCouple::IOutput*>()) override;

    /*!
     * \brief executeCompositions
     * \param compositions
     * \param requiredOutputs
     */
    void executeCompositions(const std::vector<Composition::ModelComposition*> &compositions, const QList<HydroCouple::IOutput *> &requiredOutputs);

    /*!
     * \brief finish
     */
    void finish() override;

    /*!
     * \brief parent
     * \return
     */
    HydroCouple::ICloneableModelComponent* parent() const override;

    /*!
     * \brief clone
     * \return
     */
    HydroCouple::ICloneableModelComponent* clone() override;

    /*!
     * \brief clones
     * \return
     */
    QList<HydroCouple::ICloneableModelComponent*> clones() const override;

    /*!
     * \brief optimizationAlgorithm
     * \return
     */
    OptimizationAlgorithm *calibrationAlgorithm() const;

    /*!
     * \brief individualIndex
     * \return
     */
    int individualIndex() const;

  protected:

    /*!
     * \brief removeClone
     * \param component
     * \return
     */
    bool removeClone(CalibrationComponent *component);

    /*!
     * \brief intializeFailureCleanUp
     */
    void initializeFailureCleanUp() override;

  private:

    /*!
     * \brief createArguments
     */
    void createArguments() override;

    /*!
     * \brief createInputFileArguments
     */
    void createInputFileArguments();

    /*!
     * \brief createInputs
     */
    void createInputs() override;

    /*!
     * \brief createCalibrationComponentsInput
     */
    void createCalibrationComponentsInput();

    /*!
     * \brief createObjectiveFunctionInputs
     */
    void createObjectiveFunctionInputs();

    /*!
     * \brief createConstraintInputs
     */
    void createConstraintInputs();

    /*!
     * \brief createOutputs
     */
    void createOutputs() override;

    /*!
     * \brief createVariableOutputs
     */
    void createVariableOutputs();

    /*!
     * \brief initializeArguments
     * \param message
     * \return
     */
    bool initializeArguments(QString &message) override;

    /*!
     * \brief initializeInputFilesArguments
     * \param message
     * \return
     */
    bool initializeInputFilesArguments(QString &message);

    /*!
     * \brief resetCalibrationModelComponents
     */
    void resetCalibrationModelComponents();

    /*!
     * \brief findProviderComponents
     * \param provider
     * \param providers
     */
    static void findProviderComponents(HydroCouple::IModelComponent *provider, QSet<HydroCouple::IModelComponent*>& providers);

  private:

    IdBasedArgumentString *m_inputFilesArgument;
    TriggerComponentInput *m_triggerComponentInput;
    int m_individual;
    int m_maxParallelSims;
    CalibrationComponent *m_parent;
    QList<HydroCouple::ICloneableModelComponent*> m_clones;
    OptimizationAlgorithm *m_calibrationAlgorithm;
    Composition::ModelComposition *m_modelComposition;
    QStringList m_errors;
    static const std::unordered_map<std::string, int> m_globalOptionsFlags;
    Dimension *m_geometryDimension;
    Quantity *m_defaultQuantity;
};

#endif // CALIBRATIONCOMPONENT_H
