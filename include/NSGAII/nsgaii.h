#ifndef NSGAII_H
#define NSGAII_H

#include "optimizationalgorithm.h"
#include "global.h"

#include <unordered_map>

class CALIBRATIONCOMPONENT_EXPORT NSGAIIAlgorithm : public OptimizationAlgorithm
{
    Q_OBJECT

  public:

    NSGAIIAlgorithm(QObject *parent);

    ~NSGAIIAlgorithm();

    int numVariables() const override;

    QString variableName(int index) const override;

    int getVariableIndex(const QString& variableName) const override;

    QList<QSharedPointer<HCGeometry>> variableGeometries(int index) override;

    int numObjectives() const override;

    QString objectiveName(int index) const override;

    int getObjectiveIndex(const QString& objectiveName) const override;

    QList<QSharedPointer<HCGeometry>> objectiveGeometries(int index) override;

    int numConstraints() const override;

    QString constraintName(int index) const override;

    int getConstraintIndex(const QString& constraintName) const override;

    QList<QSharedPointer<HCGeometry>> constraintGeometries(int index) override;

    bool isDone() const override;

    void prepareForInputFileRead() override;

    bool processInputFileLine(const QString &currentFlag, const QString &line, QString &error) override;

    bool readOptArg(const QString &line, int type,  QString &error);

    bool readOptArgGeometry(const QString &line, int type, QString &error);

    virtual bool initialize(QStringList &errors) override;

    virtual bool prepareForEvaluation(QStringList &errors) override;

    virtual bool processEvaluationOutput(QStringList &errors) override;

    virtual bool finalize(QStringList &errors) override;

  private:

    bool initializeOutputFiles(QStringList &errors);

    void printCustomHeader(FILE *file);

    void deleteProject();

    void setCurrentPopulation(population *currentPop);

    void readObjectives(population *currentPop);

  private:

    NSGAIIProject *m_NSGAIIProject;
    int nreal = 0;
    int nbin = 0;
    int nobj = 0;
    int ncon = 0;
    int popsize = 0;
    double pcross_real;
    double pcross_bin;
    double pmut_real;
    double pmut_bin;
    double eta_c;
    double eta_m;
    int ngen = 0;
    int nbinmut = 0;
    int nrealmut = 0;
    int nbincross = 0;
    int nrealcross = 0;
    std::vector<int> nbits ;
    std::vector<double> min_realvar;
    std::vector<double> max_realvar;
    std::vector<double> min_binvar;
    std::vector<double> max_binvar;
    int bitlength = 0;
    int max_nbits = 0;
    population *parent_pop;
    population *child_pop;
    population *mixed_pop;
    static const std::unordered_map<std::string, int> m_inputFileFlags,
    m_optionsFlags,
    m_outputFileFlags;

    std::vector<QString> m_variableNames,
    m_objectiveNames,
    m_constraintNames;

    std::unordered_map<std::string, QList<QSharedPointer<HCGeometry>>> m_variableGeometries,
    m_objectiveGeometries,
    m_constraintGeometries;

    bool m_isDone;

    QString m_inputFile1, // Initial population
            m_inputFile2, // Final population
            m_inputFile3, // Final feasible population
            m_inputFile4, // All populations
            m_inputFile5; // Model parameters

    QRegExp m_delimiters;

};


#endif // NSGAII_H
