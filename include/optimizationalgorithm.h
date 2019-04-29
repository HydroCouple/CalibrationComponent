#ifndef ALGORITHM_H
#define ALGORITHM_H

#include "calibrationcomponent_global.h"


#include <QObject>
#include <QVector>
#include <QDir>

class Individual;
class ProgressChecker;
class HCGeometry;

class CALIBRATIONCOMPONENT_EXPORT OptimizationAlgorithm : public QObject
{
    Q_OBJECT

  public:

    OptimizationAlgorithm(QObject *parent);

    ~OptimizationAlgorithm();

    virtual int numVariables() const = 0;

    virtual QString variableName(int index) const = 0;

    virtual QString variableDescription(int index) const = 0;

    virtual int getVariableIndex(const QString& variableName) const = 0;

    virtual QList<QSharedPointer<HCGeometry>> variableGeometries(int index);

    virtual QStringList variableIdentifiers(int index);

    virtual int numObjectives() const = 0;

    virtual QString objectiveName(int index) const = 0;

    virtual QString objectiveDescription(int index) const = 0;

    virtual int getObjectiveIndex(const QString& objectiveName) const = 0;

    virtual QList<QSharedPointer<HCGeometry>> objectiveGeometries(int index);

    virtual QStringList objectiveIdentifiers(int index);

    virtual int numConstraints() const = 0;

    virtual QString constraintName(int index) const = 0;

    virtual QString constraintDescription(int index) const = 0;

    virtual int getConstraintIndex(const QString& constraintName) const = 0;

    virtual QList<QSharedPointer<HCGeometry>> constraintGeometries(int index);

    virtual QStringList constraintIdentifiers(int index);

    virtual bool isDone() const = 0;

    int numIndividuals() const;

    std::vector<Individual*> individuals() const;

    Individual *getIndividual(int index) const;

    ProgressChecker *progress() const;

    void initInputFileRead();

    virtual void prepareForInputFileRead() = 0;

    virtual bool processInputFileLine(const QString &currentFlag, const QString &line, QString &error) = 0;

    virtual bool initialize(QStringList &errors) = 0;

    virtual bool prepareForEvaluation(QStringList &errors) = 0;

    virtual bool processEvaluationOutput(QStringList &errors) = 0;

    virtual bool finalize(QStringList &errors) = 0;

    void setReferenceDir(const QDir &referenceDir);

    QDir referenceDir() const;

  protected:

    QFileInfo getAbsoluteFilePath(const QString &filePath) const;

    QFileInfo getRelativeFilePath(const QString &filePath) const;

    std::vector<Individual*> m_individuals;

    void initializeIndividuals(int size);

    void deleteAllIndividuals();

  private:

    ProgressChecker *m_progressChecker;
    QDir m_referenceDir;

};


class CALIBRATIONCOMPONENT_EXPORT Individual : public QObject
{
    Q_OBJECT

  public:

    Individual(OptimizationAlgorithm *algorithm);

    ~Individual();

    double *variables() const;

    double *objectives() const;

    double *constraints() const;

  private:
    double *m_variables, *m_objectives, *m_constraints;
    OptimizationAlgorithm *m_algorithm;
};

#endif // ALGORITHM_H
