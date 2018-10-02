#include "stdafx.h"
#include "calibrationcomponentinfo.h"
#include "spatial/geometryfactory.h"
#include "calibrationcomponent.h"

using namespace HydroCouple;

CalibrationComponentInfo::CalibrationComponentInfo(QObject *parent)
  :AbstractModelComponentInfo(parent)
{
  GeometryFactory::registerGDAL();

  setId("A Model Calibration Component 1.0.0");
  setCaption("Calibration Component");
  setIconFilePath(":/CalibrationComponent/calibrationcomponenticon");
  setDescription("A calibration & optimization component that uses various"
                 " multiobjective evolutionary algorithms");

  setCategory("Parameter Estimation & Uncertainty");
  setCopyright("");
  setVendor("");
  setUrl("www.hydrocouple.org");
  setEmail("caleb.buahin@gmail.com");
  setVersion("1.0.0");

  QStringList documentation;
  documentation << "Several sources";
  setDocumentation(documentation);

}

CalibrationComponentInfo::~CalibrationComponentInfo()
{
}

IModelComponent *CalibrationComponentInfo::createComponentInstance()
{
  QString id =  QUuid::createUuid().toString();
  CalibrationComponent *component = new CalibrationComponent(id, this);
  component->setDescription("Calibration Model Instance");
  return component;
}
